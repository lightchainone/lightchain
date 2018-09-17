#include <string.h>
#include <dirent.h>

#include "utils.h"
#include "chain_info_retriever.h"

AppInfoRetriever::AppInfoRetriever(const char *chain_info_file_dir) 
{
    if (NULL == chain_info_file_dir)
    {
        WARNING("%s App info file directory is NULL", __FUNCTION__);
        return;
    }

    if (0 == strlen(chain_info_file_dir))
    {
        WARNING("%s App info file directory is empty", __FUNCTION__);
        return;
    }

    bzero(_chain_info_file_dir, sizeof(_chain_info_file_dir));

    strncpy(_chain_info_file_dir, chain_info_file_dir, sizeof(_chain_info_file_dir));

    
    size_t len = strlen(_chain_info_file_dir);
    if ('/' != _chain_info_file_dir[len-1] && (len < sizeof(_chain_info_file_dir) - 1))
    {
        _chain_info_file_dir[len] = '/';
        
    }
}

int AppInfoRetriever::get_chain_info_file_count()
{
    int matched_file_count = 0;
    
    if (-1 == enumerate_chain_info_file(count_chain_info_file, &matched_file_count))  { return -1; }

    return matched_file_count;
}

int AppInfoRetriever::get_long_link_count(uint chainid)
{
    get_long_link_count_from_info_file_data_t info_data;

    info_data.chainid = chainid;
    if (-1 == enumerate_chain_info_file(get_long_link_count_from_info_file, &info_data))  { return -1; }

    return info_data.long_link_count;
}

int AppInfoRetriever::enumerate_chain_info_file(enumerate_chain_info_file_cb_t cb, void *data)
{
    if (0 == strlen(_chain_info_file_dir))
    {
        WARNING("%s App info file directory is empty", __FUNCTION__);
        return -1;
    }

    DIR *dirp = opendir(_chain_info_file_dir);
    if (NULL == dirp)
    {
        WARNING("%s Open directory[%s] error: %s", __FUNCTION__, _chain_info_file_dir, strerror(errno));
        return -1;
    }

    size_t buf_size = offsetof(struct dirent, d_name) + NAME_MAX + 1;
    struct dirent *dp = (struct dirent *)malloc(buf_size);
    if (NULL == dp)
    {
        WARNING("%s Allocate memory error", __FUNCTION__);
        closedir(dirp);

        return -1;
    }

    while(1)
    {
        struct dirent *dp_ret = NULL;
        readdir_r(dirp, dp, &dp_ret); 

        if (NULL == dp_ret) { break; }

        if ((0 == strcmp(dp->d_name, ".")) || (0 == strcmp(dp->d_name, "..")))
        {
            continue;
        }

        Lsc::string filen_name(dp->d_name);
        if (filen_name.length() < strlen(AIF_APP_INFO_FILE_PREFIX))
        {
            continue;
        }

        Lsc::string left_str = filen_name.slcstr(0, strlen(AIF_APP_INFO_FILE_PREFIX));
        if (left_str == AIF_APP_INFO_FILE_PREFIX)
        {
            Lsc::string file_name_with_path(_chain_info_file_dir);
            
            file_name_with_path.chainend(dp->d_name);

            struct stat stat_buf;
            if (stat(file_name_with_path.c_str(), &stat_buf))
            {
                WARNING("%s Get file(%s) status error", __FUNCTION__, file_name_with_path.c_str());
                break;
            } 
            if (S_ISDIR(stat_buf.st_mode)) 
            {
                continue; 
            }
            if (time(NULL) - stat_buf.st_mtime > APP_INFO_FILE_TIME_THRESHOLD_IN_SECONDS)
            {
                continue; 
            }
            
            if (!cb(file_name_with_path.c_str(), data)) { break;}
        }
    } 

    free(dp);
    closedir(dirp);

    return 0;
}

bool AppInfoRetriever::count_chain_info_file(const char *chain_info_file_name, void *data)
{
    UNUSED(chain_info_file_name);

    int *counter = (int *)data;
	
    *counter = *counter + 1;
	
    return true; 
}

bool AppInfoRetriever::get_long_link_count_from_info_file(const char *chain_info_file_name, void *data)
{
    get_long_link_count_from_info_file_data_t *info_data =(get_long_link_count_from_info_file_data_t *)data;
    AppInfoFile chain_info_file;

    if (-1 == chain_info_file.open(chain_info_file_name, O_RDONLY))
    {
        WARNING("%s Open chain info file[%s] error: %s", __FUNCTION__, chain_info_file_name, strerror(errno));
        return true; 
    }

    chain_info_t chain_info;

    bzero(&chain_info, sizeof(chain_info));
    if (-1 == chain_info_file.read(info_data->chainid, &chain_info))
    {
        WARNING("%s Read chain info file[%s] error: %s", __FUNCTION__, chain_info_file_name, strerror(errno));
        return true; 
    }

    info_data->long_link_count = chain_info.long_link_count;

    return false; 
}





