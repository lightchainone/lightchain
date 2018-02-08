



#ifndef  __BAKLOG_H_
#define  __BAKLOG_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

namespace comspace {
namespace comlogplugin {
namespace dfsappender {

struct disk_log_conf_t
{
    char path[1024];
    char file[1024];
    uint32_t queue_size;
    int64_t max_size;
    uint32_t remove;
};

class DfsAppenderDiskLog {
plclic:
    static const int DISK_BUFFER = 512*1024;
    DfsAppenderDiskLog()
    {
        _path[0] = '\0';
        _filename[0] = '\0';
        _writefd = -1;
        _readfd = NULL;
        _posfd = -1;
        _read_pos.idx = 0;
        _read_pos.offset = 0;
        _last_id = 0;
        _limit_size = 0;
        _remove = 0;
        _update = false;
        _buffer = NULL;
    
    };
    ~DfsAppenderDiskLog()
    {
       this->close(); 
    };

    struct idx_pos_t {
        uint64_t idx;
        int64_t offset;
    };

    int reopen_write(uint64_t id);
    int reopen_read(struct idx_pos_t *pos);
    int checklimit(size_t buf_size = 0);
    int open(const char *path,
            const char *filename,
            int64_t limit_size,
            uint32_t remove = 0,
            const char *txt = NULL);
    int close();
    int getpos(size_t len, uint64_t *idx, int64_t *offset);
    int read_last_pos(struct idx_pos_t * read_pos);
    ssize_t write(const void *buf, size_t buf_size);
    ssize_t read(void *buf, size_t buf_size);
    ssize_t writev(const struct  iovec *vec, size_t num);
    int commit_read(size_t buf_size);
    int read_pos(struct idx_pos_t *pos);
    int write_pos(struct idx_pos_t *pos);

private:
    char _path[1024];
    char _filename[1024];
    char _idx_filename[1024];
    char _txt[10240];
    int _remove;
    int _writefd;
    int _posfd;
    FILE *_readfd;
    char *_buffer;
    uint64_t _last_id;
    int64_t _limit_size;
    bool _update;
    struct idx_pos_t _read_pos;
    pthread_mutex_t _lock;
        
};

} 
} 
} 


#endif  


