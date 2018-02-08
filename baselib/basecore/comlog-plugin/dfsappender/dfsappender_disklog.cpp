#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include "dfsappender_disklog.h"

namespace comspace {
namespace comlogplugin {
namespace dfsappender {

const int BAK_NUM = 1;

int DfsAppenderDiskLog::reopen_write(uint64_t id) 
{
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s.%llu", _path, _filename,
            (unsigned long long)id);
    int fd = ::open(fullpath, O_CREAT | O_WRONLY | O_APPEND, 0666);
    if (fd < 0) {
        return -1;
    }

    int bkfd = _writefd;
    _writefd = fd;
    if (bkfd != -1) {
        ::close(bkfd);
    }
    _last_id = id;
    return 0;
}

int DfsAppenderDiskLog::reopen_read(struct idx_pos_t *pos)
{
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s.%llu", _path, _filename,
            (unsigned long long)pos->idx);
    FILE *fp = fopen(fullpath, "r");
    if (NULL == fp) {
        return -1;
    }
    if (_buffer == NULL) {
        _buffer = (char*)malloc(DISK_BUFFER);
    }
    if (_buffer != NULL) {
        ::setbuffer(fp, _buffer, DISK_BUFFER);
    }
    this->write_pos(pos);
    FILE *bkfd = _readfd;
    _readfd = fp;
    if (bkfd != NULL) {
        fclose(bkfd);
    }
    return 0;
}

int DfsAppenderDiskLog::checklimit(size_t buf_size)
{
    if (_writefd < 0) {
        return -1;
    }
    struct stat stbuf;
    fstat(_writefd, &stbuf);
    if (stbuf.st_size + (ssize_t)buf_size > this->_limit_size) {
        this->reopen_write(_last_id+1);
    }
    return 0;
}

int DfsAppenderDiskLog::open(const char *path, const char *filename,
        int64_t limit_size, uint32_t remove, const char *txt)
{

    int len = strlen(filename);
    char *ptr = NULL;
    uint64_t cid = 0;
    uint64_t maxid = 0;

    snprintf(_path, sizeof(_path), "%s", path);
    snprintf(_filename, sizeof(_filename), "%s", filename);
    if (NULL == txt) {
        _txt[0] = '\0';
    } else {
        snprintf(_txt, sizeof(_txt), "%s", txt);
    }
    DIR *dp = opendir(path);
    if (NULL == dp) {
        return -1;
    }
    struct dirent entry;
    struct dirent *entryPtr;
    
    
    
    while (readdir_r(dp, &entry, &entryPtr) == 0) {
        if (NULL == entryPtr) {
            break;
        }
        if (strcmp(entryPtr->d_name, ".") == 0 ||
                strcmp(entryPtr->d_name, "..") == 0) {
            continue;
        }
        if (strncmp(entryPtr->d_name, filename, len) == 0) {
            if (entryPtr->d_name[len] == '.') {
                errno = 0;
                
                cid = strtoull(entryPtr->d_name+len+1, &ptr, 10);
                if (0 == errno) {
                    if (cid > maxid) {
                        maxid = cid;
                    }
                }
            }
        }
    }
    closedir(dp);
    _limit_size = limit_size;
    char last_file[1024];
    snprintf(last_file, sizeof(last_file), "%s/%s.%llu", _path, _filename,
            (unsigned long long)maxid);
    
    
    struct stat last_stat;
    if (stat(last_file, &last_stat) < 0) {
        
        last_stat.st_size = 0;
    }

    
    if (last_stat.st_size != 0) {
        _last_id = maxid + 1;
    } else {
        _last_id = maxid;
    }
 
    if (this->reopen_write(_last_id) < 0) {
        return -1;
    }

    
    snprintf(this->_idx_filename, sizeof(this->_idx_filename),
                "%s/%s.idx", path, filename);

    if (read_last_pos(&_read_pos) < 0) {
        return -1;
    }
    if (this->reopen_read(&_read_pos) < 0) {
        return -1;
    }
    pthread_mutex_init(&_lock, NULL);
    ::fseek(_readfd, _read_pos.offset, SEEK_SET);
    this->_remove = remove;
    return 0;
}

int DfsAppenderDiskLog::close()
{
    if (_writefd != -1) {
        ::close(_writefd);
        _writefd = -1;
    }
    if (_readfd != NULL) {
        fclose(_readfd);
        _readfd = NULL;
    }
    if (_posfd != -1) {
        ::close(_posfd);
        _posfd = -1;
    }
    if (_buffer != NULL) {
        free(_buffer);
    }
    return 0;
}

int DfsAppenderDiskLog::read_last_pos(struct idx_pos_t * read_pos)
{
    if (_posfd < 0) {
        bool newfile = false;
        
        if (access(this->_idx_filename, F_OK) < 0) {
            
            newfile = true;
        }
        _posfd = ::open(this->_idx_filename, O_RDWR|O_CREAT, 0666);
        if (_posfd < 0) {
            return -1;
        }
        if (newfile) {
            char buff[128];
            memset(buff, ' ', sizeof(buff));
            
            ::write(_posfd, buff, sizeof(buff));
            if (_txt[0] != '\0') {
                
                ::write(_posfd, _txt, strlen(_txt));
            }
        }
    }
    if (this->read_pos(&_read_pos) < 0) {
        read_pos->idx = 0;
        read_pos->offset = 0;
        this->write_pos(&_read_pos);
         
    }
    return 0;
}
ssize_t DfsAppenderDiskLog::write(const void *buf, size_t buf_size)
{
    if (_writefd < 0) {
        return -1;
    }

    
    pthread_mutex_lock(&_lock);
    checklimit(buf_size);
    ssize_t ret = 0;
    ret = ::write(_writefd, buf, buf_size);
    pthread_mutex_unlock(&_lock);
    return ret;
}
ssize_t DfsAppenderDiskLog::writev(const struct iovec *vec, size_t num)
{
    if (_writefd < 0) {
        return -1;
    }
    
    pthread_mutex_lock(&_lock);
    size_t body_len = 0;
    for (size_t i = 0; i < num; ++i) {
        body_len += vec[i].iov_len;
    }
    checklimit(body_len);
    ssize_t ret = 0;
    ret = ::writev(_writefd, vec, num);
    pthread_mutex_unlock(&_lock);
    return ret;
}

ssize_t DfsAppenderDiskLog::read(void *buf, size_t buf_size)
{
    if (NULL == _readfd) {
        return -1;
    }
    size_t ret = fread(buf, 1, buf_size, _readfd);
    return ret;
}

int DfsAppenderDiskLog::read_pos(struct idx_pos_t *pos)
{
    char buffer[64];
    long long idx;
    unsigned long long offset;
    if (::read(_posfd, buffer, sizeof(buffer)) > 0) {
        sscanf(buffer, "%lld %llu ", &idx, &offset);
        pos->idx = idx;
        pos->offset = offset;
        return 0;
    } 
    return -1; 
}

int DfsAppenderDiskLog::write_pos(struct idx_pos_t *pos)
{
    if (::lseek(_posfd, 0, SEEK_SET) < 0) {
        return -1;
    }
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "%-25lld %-25llu ",
            (long long)pos->idx,
            (unsigned long long)pos->offset);
    if (::write(_posfd, buffer, len) < 0) {
        return -1;
    }
    return 0;
}

int DfsAppenderDiskLog::getpos(size_t len, uint64_t *idx, int64_t *offset)
{
    
    if (NULL == idx || NULL == offset) {
        return -1;
    }
    *idx = _read_pos.idx;
    *offset = _read_pos.offset + len;
    return -1;
}

int DfsAppenderDiskLog::commit_read(size_t buf_size)
{
    
    if (NULL == _readfd) {
        return -1;
    }
    struct idx_pos_t pos = {_read_pos.idx, _read_pos.offset};
    char fullpath[1024];
    struct stat statbuf;
    size_t le = 0;

    if (fstat(fileno(_readfd), &statbuf) < 0) {
        return -1;
    }

    int ret = -1;

    do {
        
        if (pos.offset >= statbuf.st_size)  {
            
            
            pthread_mutex_lock(&_lock);
            if (pos.idx != _last_id) {
                
                ret = 0;
            }
            pthread_mutex_unlock(&_lock);
            if (0 == ret) {
                snprintf(fullpath, sizeof(fullpath), "%s/%s.%llu", _path, _filename, 
                        pos.idx+1ULL);
                
                if (access(fullpath, F_OK) == 0) {
                    pos.offset = 0;
                    pos.idx++;
                    
                    if (this->reopen_read(&pos) != 0) {
                        return -1;
                    }
                    this->_update = true; 
                }
            } else {
                
                return -1;
            }
        } else {
            le = statbuf.st_size - pos.offset;
        }

        
        if (this->_remove && this->_update) {
            
            this->write_pos(&pos);
            
            snprintf(fullpath, sizeof(fullpath), "%s/%s.%llu",
                    _path, _filename, 
                    (unsigned long long)pos.idx - 1);
            char tmp_file[1024];
            snprintf(tmp_file, sizeof(tmp_file), "%s/.tmp.%s.%llu",
                    _path, _filename, 
                    (unsigned long long)pos.idx%BAK_NUM); 
            rename(fullpath, tmp_file);
            this->_update = false;
        }

        _read_pos = pos;
        
        if (le >= buf_size) {
            _read_pos.offset += buf_size;
            break;
        }
        _read_pos.offset += le;
        buf_size -= le;
        pos = _read_pos;
        le = 0;

    } while (le > 0);

    this->write_pos(&_read_pos);

    if (::fseek(_readfd, _read_pos.offset, SEEK_SET) < 0) {
        return -1;
    }

    if (this->_update) {
        return 1;
    }
    return 0;
}
} 
} 
} 


