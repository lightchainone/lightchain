#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <Configure.h>

#include "ul_func.h"
#include "ul_file.h"

#include "lc_log.h"
#include "lc_conf.h"

#define LC_CONF_MAIL_REGEX "^[_.0-9a-zA-Z-]*@([_0-9a-zA-Z-]*.)*[_0-9a-zA-Z_]{2,5}$"
#define LC_CONF_IP_REGEX   "([0-9]|[0-9][0-9]|1[0-9][0-9]|2[0-4]"\
    "[0-9]|25[0-5])\\.([0-9]|[0-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\."\
"([0-9]|[0-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.([0-9]|[0-9][0-9]"\
"|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$"

static int g_lc_conf_version = LC_CONF_VERSION_1;		  


int lc_conf_setversion(int version) {
	int old = g_lc_conf_version;
	g_lc_conf_version = version;
	return old;
}

static Ul_confdata *get_rangefile(const char *path, const char *filename)
{
    Ul_confdata *range;
    
    if (!ul_fexist(path, filename)) {
        LC_LOG_WARNING("no found range file [path:%s filename:%s]", path, filename);
        return NULL;
    }
    range = ul_initconf(LC_CONF_DEFAULT_ITEMNUM);
    if (range != NULL) {
        if (ul_readconf_ex(path, filename, range) == -1) {
            ul_freeconf(range);
            LC_LOG_WARNING("couldn't load [%s]", filename);
            range = NULL;
        }
    } else {
        LC_LOG_WARNING("couldn't open [%s]", filename);
    }
    return range;
}


lc_conf_data_t *lc_conf_init(const char *path, const char *filename,
        const int build, const int num, const int version)
{
    lc_conf_data_t *bf;
    size_t ret;
    char range_filename[WORD_SIZE];
	int reg_ver = version;
	if (LC_CONF_VERSION_DEFAULT == reg_ver) {
		reg_ver = g_lc_conf_version;
	}
	if ((LC_CONF_VERSION_1 != reg_ver) && (LC_CONF_VERSION_2 != reg_ver)){
		LC_LOG_WARNING("unrecognized version: %d, set version to old config\n", reg_ver);
		reg_ver = LC_CONF_VERSION_1;
	}

    if (NULL == path || NULL == filename)
      return NULL;
    bf = (lc_conf_data_t *)ul_calloc(1, sizeof(lc_conf_data_t));
    if (NULL == bf)
        return NULL;
    bf->conf_file = NULL;
    if (build != LC_CONF_READCONF) {
        ret = snprintf(bf->filename, sizeof(bf->filename), "%s/%s.sample", path, filename);
        if (ret >= sizeof(bf->filename)) {
            LC_LOG_FATAL("[lc_conf_init] path:%s filename:%s to long", path, filename);
            free(bf);
            return NULL;
        }
		
		
				bf->conf_file = ul_fopen(bf->filename, "w");
				if (NULL == bf->conf_file) {
					LC_LOG_FATAL("[lc_conf_init] %s write open error", bf->filename);      
					free(bf);
					return NULL;
				}
		
		
		
		
		
		
        
    } else {
        ret = snprintf(bf->filename, sizeof(bf->filename), "%s/%s", path, filename);
        if (ret >= sizeof(bf->filename)) {
            LC_LOG_FATAL("[lc_conf_init] path:%s filename:%s to long", path, filename);
            free(bf);
            return NULL;
        }
    }

    ret = snprintf(range_filename, sizeof(range_filename), "%s.range", filename);
    if (ret >= sizeof(range_filename)) {
        LC_LOG_FATAL("[lc_conf_init] path:%s filename:%s to long", path, filename);
        free(bf);
        return NULL;
    }

	bf->conf_new = NULL;
	switch(reg_ver) {
		case LC_CONF_VERSION_1:
			bf->option = ul_initconf(num);
			if (NULL == bf->option) {
				if (bf->conf_file != NULL) {
					ul_fclose(bf->conf_file);
				}
				ul_free(bf);
				return NULL;
			}
			break;
		case LC_CONF_VERSION_2:
			try {
				bf->conf_new = new comcfg::Configure();
			} catch (...) {
			
					ul_free(bf);
					return NULL;
			
			}
			break;
		default:
			LC_LOG_FATAL("unrecognized version when processing: %d", reg_ver);
			break;
	}
    
    bf->build = build;
    bf->range = NULL;

    if (build != LC_CONF_READCONF)
        return bf;
	bf->range = get_rangefile(path, range_filename);
	switch(reg_ver) {
		case LC_CONF_VERSION_1:

			if (ul_readconf_ex(path, filename, bf->option) == -1) {
				ul_freeconf(bf->option);
				if (bf->range != NULL)
					ul_freeconf(bf->range);
				if (bf->conf_file != NULL)
					ul_fclose(bf->conf_file);
				ul_free (bf);
				return NULL;
			}
			break;
		case LC_CONF_VERSION_2:
			{
			int confres = bf->conf_new->load(path, filename, NULL);
			if (0 != confres){
				delete bf->conf_new;
				ul_free(bf);
				return NULL;
			}
			}
			break;
		default:
			LC_LOG_FATAL("unrecognized version when processing: %d", reg_ver);
			break;
	}
    return bf;
}


int lc_conf_close(lc_conf_data_t *pd_conf)
{
    if (NULL == pd_conf)
        return LC_CONF_SUCCESS;

    if (pd_conf->option != NULL) {
        ul_freeconf(pd_conf->option);
        pd_conf->option = NULL;
    }
    if (pd_conf->range != NULL) {
        ul_freeconf(pd_conf->range);
        pd_conf->range = NULL;
    }
    if (pd_conf->conf_file != NULL) {
        ul_fclose(pd_conf->conf_file);
        pd_conf->conf_file = NULL;
    }
	if (NULL != pd_conf->conf_new) {
		delete pd_conf->conf_new;
		pd_conf->conf_new = NULL;
	}
    ul_free (pd_conf);
    return LC_CONF_SUCCESS;
}



static int write_comment(FILE *fp, const char *comment)
{
    const char *q;
    size_t wn;
    if (NULL == fp)
        return LC_CONF_NULL;
    fputc('\n', fp);
    if (NULL == comment)
        return LC_CONF_SUCCESS;

    while (*comment) {
        while (*comment == '\n') {
          fputc('\n', fp);
          comment++;
        }
        if ('\0' == *comment)
          break;
        fputc('#', fp); 
        q = strchr(comment, '\n'); 
        if (NULL == q) 
            break;
        wn = (size_t)(q - comment);
        fwrite(comment, sizeof(char), wn, fp);  
        comment = q; 
    } 
    fputs(comment, fp); 
    fputc('\n', fp);
    fflush(fp);
    return LC_CONF_SUCCESS;
}


int is_blank_str(const char *src, const size_t n)
{
    size_t i;
    if (NULL == src) {
        return 1;
    }
    for (i = 0; src[i] != '\0' && i < n; i++) {
        if (src[i] != '\0' && src[i] != TAB && src[i] != SPACE)
            return 0;
    }
    return 1;
}

 

static int check_str_regex(const char *conf_value, const char *check_str)
{
    regex_t re;  
    regmatch_t  pmatch[2];
    if (NULL == conf_value)
        return LC_CONF_CHECKFAIL;
    
    if (NULL == check_str)
        return LC_CONF_CHECKSUCCESS;

    if (regcomp(&re,check_str,REG_EXTENDED) != 0) {
        return LC_CONF_CHECKSUCCESS;
    }
    if (regexec(&re, conf_value, 2, pmatch, REG_EXTENDED) == 0) {

        if (pmatch[0].rm_so == 0 && conf_value[pmatch[0].rm_eo] == '\0') {
            regfree(&re);
            return LC_CONF_CHECKSUCCESS;
        }
    }
    regfree(&re);
    return LC_CONF_CHECKFAIL;
}


 
static int check_str_mail(const char *conf_value)
{
    return check_str_regex(conf_value, LC_CONF_MAIL_REGEX);
}

 
static int check_str_ipv4(const char *conf_value)
{
    return check_str_regex(conf_value, LC_CONF_IP_REGEX);
}

 

static int get_regex_str(const char *src, char *reg, size_t max_len)
{
    if (NULL == src || NULL == reg)
        return LC_CONF_NULL;
    const char *left, *right; 
    left = strchr(src, '[');
    if (NULL == left)
        return LC_CONF_NULL;
    right = strrchr(left, ']');
    if (NULL == right)
        return LC_CONF_NULL;
    if (right > left + max_len)
        return LC_CONF_NULL;
    strncpy(reg, left + 1, right - left - 1UL);
    reg[right - left - 1] = '\0';
    return LC_CONF_SUCCESS;
}


 
static int check_str(const lc_conf_data_t *conf, const char *conf_name, const char *conf_value)
{
    char range_str[WORD_SIZE];
    char range_check_str[WORD_SIZE];
    int ret;
    if (NULL == conf || NULL == conf_name || NULL == conf_value)
        return LC_CONF_NULL;
    
    if (NULL == conf->range)
        return LC_CONF_CHECKSUCCESS;

    ret = ul_getconfnstr(conf->range, conf_name, range_str, sizeof(range_str));
    if (0 == ret)
        return LC_CONF_CHECKSUCCESS;
    
    
    if (is_blank_str(range_str, sizeof(range_str))) {
      return LC_CONF_CHECKSUCCESS;
    }
    range_check_str[0] = '\0';
    if (sscanf(range_str, " %s ", range_check_str) != 1) {
      LC_LOG_WARNING("string [%s : %s] range file [%s] format invalid", conf_name, conf_value, range_str);  
      return LC_CONF_CHECKFAIL;
    }
    if (strncasecmp(range_check_str, "ip", 2) == 0) {
        ret = check_str_ipv4(conf_value);
        if (ret != LC_CONF_CHECKSUCCESS) {
          LC_LOG_WARNING("[%s : %s] invalid ip format", conf_name, conf_value);
        }
        return ret;
    
    } else if (strncasecmp(range_check_str, "mail", 4) == 0) {

        ret = check_str_mail(conf_value);
        if (ret != LC_CONF_CHECKSUCCESS) {
          LC_LOG_WARNING("[%s : %s] invaild mail format", conf_name, conf_value);
        }
        return ret;
    
    } else if (strncasecmp(range_check_str, "regex", 5) == 0) {
        if (get_regex_str(range_str, range_check_str, sizeof(range_check_str)) == LC_CONF_SUCCESS) {
          ret = check_str_regex(conf_value, range_check_str);
          if (ret != LC_CONF_CHECKSUCCESS) {
            LC_LOG_WARNING("check [%s : %s] with regex [%s] error", conf_name, conf_value, range_check_str);
          }
          return ret;
        }
    } 

    LC_LOG_WARNING("string [%s : %s] range file [%s] format invalid", conf_name, conf_value, range_str);
    return LC_CONF_CHECKFAIL;
}


#define CHECK_RANGE(num, lmin, rmax) (((num)>=(lmin) && (num)<=(rmax))?0:1)

 
static int check_int_range(const lc_conf_data_t *conf, const char *conf_name, int num)
{
    char range_str[WORD_SIZE];
    int ret;
    int rmin, rmax;
    if (NULL == conf || NULL == conf_name)
        return LC_CONF_NULL;
    if (NULL == conf->range) {
        LC_LOG_WARNING("no found configure range file, no check range[%s]", conf_name);
        return LC_CONF_CHECKSUCCESS;
    }

    ret = ul_getconfnstr(conf->range, conf_name, range_str, sizeof(range_str));
    if (0 == ret) {
        LC_LOG_WARNING("no found [%s] range check item", conf_name);
        return LC_CONF_CHECKSUCCESS;
    }
    if (sscanf(range_str,"range [ %d %d ] ", &rmin, &rmax) == 2) {
        if (CHECK_RANGE(num, rmin, rmax) == 0) {
            return LC_CONF_CHECKSUCCESS;
        } else {
            LC_LOG_WARNING("int [%s] load error, [%d] overflow range [%d %d]", conf_name, num, rmin, rmax);
            return LC_CONF_CHECKFAIL;
        }
    }
    LC_LOG_WARNING("[%s : %d] int_range [%s] invalid format", conf_name, num, range_str);
    return LC_CONF_CHECKFAIL;
}


static int check_uint_range(const lc_conf_data_t *conf, const char *conf_name, u_int num)
{
    char range_str[WORD_SIZE];
    int ret;
    u_int rmin, rmax;
    if (NULL == conf || NULL == conf_name)
        return LC_CONF_NULL;
    if (NULL == conf->range) {
        
        return LC_CONF_CHECKSUCCESS;
    }

    ret = ul_getconfnstr(conf->range, conf_name, range_str, sizeof(range_str));
    if (0 == ret) {
        LC_LOG_WARNING("no found [%s] range check item", conf_name);
        return LC_CONF_CHECKSUCCESS;
    }

    if (sscanf(range_str,"range [ %u %u ] ", &rmin, &rmax) == 2) {
        if (CHECK_RANGE(num, rmin, rmax) == 0) {
            return LC_CONF_CHECKSUCCESS;
        } else {
            LC_LOG_WARNING("uint [%s] load error, [%u] overflow range [%u %u]", conf_name, num, rmin, rmax);
            return LC_CONF_CHECKFAIL;
        }
    }
    LC_LOG_WARNING("[%s : %u] uint_range [%s] invalid format", conf_name, num, range_str);
    return LC_CONF_CHECKFAIL;
}



static int check_int64_range(const lc_conf_data_t *conf, const char *conf_name, long long num)
{
    char range_str[WORD_SIZE];
    int ret;
    long long rmin, rmax;
    if (NULL == conf || NULL == conf_name)
        return LC_CONF_NULL;

    if (NULL == conf->range) {
        LC_LOG_WARNING("no found configure range file, no check range[%s]", conf_name);
        return LC_CONF_CHECKSUCCESS;
    }
    ret = ul_getconfnstr(conf->range, conf_name, range_str, sizeof(range_str));
    if (0 == ret) {
        LC_LOG_WARNING("no found [%s] range check item", conf_name);
        return LC_CONF_CHECKSUCCESS;
    }

    if (sscanf(range_str,"range [ %lld %lld ] ", &rmin, &rmax) == 2) {
        if (CHECK_RANGE(num, rmin, rmax) == 0) {
            return LC_CONF_CHECKSUCCESS;
        } else {
            LC_LOG_WARNING("int64 [%s] load error, [%lld] overflow range [%lld %lld]", conf_name, num, rmin, rmax);
            return LC_CONF_CHECKFAIL;
        }
    }
    LC_LOG_WARNING("[%s : %lld] int64_range [%s] invalid format", conf_name, num, range_str);
    return LC_CONF_CHECKFAIL;
}



static int check_uint64_range(const lc_conf_data_t *conf, const char *conf_name, unsigned long long num)
{
    char range_str[WORD_SIZE];
    int ret;
    unsigned long long rmin, rmax;
    if (NULL == conf || NULL == conf_name)
        return LC_CONF_NULL;
    if (NULL == conf->range) {
        LC_LOG_WARNING("no found configure range file, no check range[%s]", conf_name);
        return LC_CONF_CHECKSUCCESS;
    }

    ret = ul_getconfnstr(conf->range, conf_name, range_str, sizeof(range_str));
    if (0 == ret) {
        LC_LOG_WARNING("no found [%s] range check itemn", conf_name);
        return LC_CONF_CHECKSUCCESS;
    }

    if (sscanf(range_str,"range [ %llu %llu ] ", &rmin, &rmax) == 2) {
        if (CHECK_RANGE(num, rmin, rmax) == 0) {
            return LC_CONF_CHECKSUCCESS;
        } else {
            LC_LOG_WARNING("uint64 [%s] load error, [%llu] overflow range [%llu %llu]", conf_name, num, rmin, rmax);
            return LC_CONF_CHECKFAIL;
        }
    }
    LC_LOG_WARNING("[%s : %llu] uint64_range [%s] invalid format ", conf_name, num, range_str);
    return LC_CONF_CHECKFAIL;
}



int load_str(const lc_conf_data_t *conf, const char *conf_name, char *conf_value, const size_t n)
{

    int ret = LC_CONF_LOST;
    int i;
    if (NULL == conf || NULL == conf_name || NULL == conf_value || 0 == n)
        return LC_CONF_NULL;
    
    conf_value[0] = '\0';
	if (NULL != (conf->option)) {
		for (i = 0; i < conf->option->num; i++) {
			if (strncmp((conf->option->item[i]).name, conf_name, 
						sizeof((conf->option->item[i]).name)) == 0) {
				if (LC_CONF_SUCCESS == ret) {
					LC_LOG_WARNING("more than one configure name is [%s], use frist value [%s]", conf_name, conf_value);
					return LC_CONF_SETMULTIPLE;
				}
				snprintf(conf_value, n, "%s", (conf->option->item[i]).value);
				ret = LC_CONF_SUCCESS;
			}
		}
	} else if (NULL != (conf->conf_new)) {
		bsl::string bstr;
		int ec = (*(conf->conf_new))[conf_name].get_bsl_string(&bstr);
		if (0 != ec) {
			return LC_CONF_LOST;
		}
		snprintf(conf_value, n, "%s", bstr.c_str());
		ret = LC_CONF_SUCCESS;
	}
    
    return ret;
}


int lc_conf_getnstr(const lc_conf_data_t *conf, const char *conf_name,
        char *conf_value,  const size_t n, const char *comment,  const char * default_value) 
{
    int ret;
    if (NULL == conf_name || NULL == conf)
        return LC_CONF_NULL;

    if (conf->build != LC_CONF_READCONF) {
        if (write_comment(conf->conf_file, comment) == LC_CONF_SUCCESS) {
            if (default_value != NULL) {
                fprintf(conf->conf_file, "#[默认配置(字符串),  %s : %s]\n%s : %s", 
                        conf_name, default_value, conf_name, default_value);
            } else {
                fprintf(conf->conf_file, "%s : ", conf_name);
            }
            return LC_CONF_SUCCESS;
        }
        return LC_CONF_NULL;
    }

    if (NULL == conf_value || 0 == n)
        return LC_CONF_NULL;
    
    ret = load_str(conf, conf_name, conf_value, n);
    if (LC_CONF_LOST == ret) {
        if (default_value != NULL) {
            snprintf(conf_value, n, "%s", default_value);
            LC_LOG_WARNING("load string [%s] fail, use default value [%s]", conf_name , default_value);
            return LC_CONF_DEFAULT;
        } 
        LC_LOG_WARNING("load string fail, no found [%s]", conf_name);
        return LC_CONF_LOST;
    }
    if (check_str(conf, conf_name, conf_value) != LC_CONF_CHECKSUCCESS)
        return LC_CONF_CHECKFAIL;
    LC_LOG_NOTICE("get string value [%s : %s]", conf_name, conf_value);
    return ret;
} 

int lc_conf_getint(const lc_conf_data_t *conf, const char *conf_name, int *conf_value,  
        const char *comment,  const int * default_value)
{
    long num;
    char *pts;
    char conf_value_str[WORD_SIZE];
    int ret;

    if (NULL == conf || NULL == conf_name)
        return LC_CONF_NULL;


    if (conf->build != LC_CONF_READCONF) {
        if (write_comment(conf->conf_file, comment) == LC_CONF_SUCCESS) {
            if (default_value != NULL) {
                fprintf(conf->conf_file, "#[默认配置(int),  %s : %d]\n%s : %d", 
                        conf_name, *default_value, conf_name, *default_value);
            } else {
                fprintf(conf->conf_file, "%s : ", conf_name);
            }
            return LC_CONF_SUCCESS;
        }
        return LC_CONF_NULL;
    }

    if (NULL == conf_value)
        return LC_CONF_NULL;
    *conf_value = 0;
    ret = load_str(conf, conf_name, conf_value_str, sizeof(conf_value_str));
    if (LC_CONF_LOST == ret) {
        if (default_value != NULL) {
            *conf_value = *default_value;
            LC_LOG_WARNING("int [%s] no found, use default value [%d]", conf_name , *default_value);
            return LC_CONF_DEFAULT;
        } 
        LC_LOG_WARNING("load int fail, no found [%s]", conf_name);
        return LC_CONF_LOST;
    }

    if (is_blank_str(conf_value_str, sizeof(conf_value_str))) {
      LC_LOG_WARNING("int [%s] is empty", conf_name);
      return LC_CONF_CHECKFAIL;
    }
   
    errno = 0;
    num = strtol(conf_value_str, &pts, 0);
    if (errno == ERANGE || num < INT_MIN || num > INT_MAX) {     
        LC_LOG_WARNING("int [%s] load error, [%s] overflow", conf_name, conf_value_str);
        return LC_CONF_OVERFLOW;
    }
    if (!is_blank_str(pts, sizeof(conf_value_str))) {
        LC_LOG_WARNING("int [%s] load error, [%s] has invalid character", conf_name, conf_value_str);
        return LC_CONF_INVALID_CHARACTER;
    }

    if (check_int_range(conf, conf_name, num) != LC_CONF_CHECKSUCCESS) {
        return LC_CONF_OVERFLOW;
    }
    *conf_value = (int)num;
    LC_LOG_NOTICE("get int value [%s : %d]", conf_name, *conf_value);
    return ret;
}

int lc_conf_getuint(const lc_conf_data_t *conf, const char *conf_name, 
        u_int *conf_value,  const char *comment, const u_int *default_value) 
{
    unsigned long num;
    char *pts;
    char conf_value_str[WORD_SIZE];
    int ret;

    if (NULL == conf || NULL == conf_name)
        return LC_CONF_NULL;


    if (conf->build != LC_CONF_READCONF) {
        if (write_comment(conf->conf_file, comment) == LC_CONF_SUCCESS) {
            if (default_value != NULL) {
                fprintf(conf->conf_file, "#[默认配置(uint),  %s : %u]\n%s : %u", 
                        conf_name, *default_value, conf_name, *default_value);
            } else {
                fprintf(conf->conf_file, "%s : ", conf_name);
            }
            return LC_CONF_SUCCESS;
        }
        return LC_CONF_NULL;
    }

    if (NULL == conf_value)
        return LC_CONF_NULL;
    *conf_value = 0;
    ret = load_str(conf, conf_name, conf_value_str, sizeof(conf_value_str));
 
    if (LC_CONF_LOST == ret) {
        if (default_value != NULL) {
            *conf_value = *default_value;
            LC_LOG_WARNING("load uint [%s] fail, use default value [%u]", conf_name , *default_value);
            return LC_CONF_DEFAULT;
        } 
        LC_LOG_WARNING("load uint fail, no found [%s]", conf_name);
        return LC_CONF_LOST;
    }

    if (is_blank_str(conf_value_str, sizeof(conf_value_str))) {
      LC_LOG_WARNING("uint [%s] is empty", conf_name);
      return LC_CONF_CHECKFAIL;
    }

    if (conf_value_str[0] == '-') {
      LC_LOG_WARNING("uint [%s] load error, [%s] overflow", conf_name, conf_value_str);
      return LC_CONF_OVERFLOW;
    }

    errno = 0;
    num = strtoul(conf_value_str, &pts, 0);
    if (errno == ERANGE || num > UINT_MAX) {     
        LC_LOG_WARNING("uint [%s] load error, [%s] overflow", conf_name, conf_value_str);
        return LC_CONF_OVERFLOW;
    }

    if (!is_blank_str(pts, sizeof(conf_value_str))) {
        LC_LOG_WARNING("uint [%s] load error, [%s] has invalid character", conf_name, conf_value_str);
        return LC_CONF_INVALID_CHARACTER;
    }

    if (check_uint_range(conf, conf_name, num) != LC_CONF_CHECKSUCCESS) {
        return LC_CONF_OVERFLOW;
    }
    *conf_value = (u_int)num;
    LC_LOG_NOTICE("get uint value [%s : %u]", conf_name, *conf_value);
    return ret;
}

int lc_conf_getint64(const lc_conf_data_t *conf, const char *conf_name, 
        long long *conf_value,  const char *comment, const long long *default_value)
{ 
    long long num;
    char *pts;
    char conf_value_str[WORD_SIZE];
    int ret;

    if (NULL == conf || NULL == conf_name)
        return LC_CONF_NULL;


    if (conf->build != LC_CONF_READCONF) {
        if (write_comment(conf->conf_file, comment) == LC_CONF_SUCCESS) {
            if (default_value != NULL) {
                fprintf(conf->conf_file, "#[默认配置(int64),  %s : %lld]\n%s : %lld", 
                        conf_name, *default_value, conf_name, *default_value);
            } else {
                fprintf(conf->conf_file, "%s : ", conf_name);
            }
            return LC_CONF_SUCCESS;
        }
        return LC_CONF_NULL;
    }

    if (NULL == conf_value)
        return LC_CONF_NULL;
    *conf_value = 0;
    ret = load_str(conf, conf_name, conf_value_str, sizeof(conf_value_str));
    if (LC_CONF_LOST == ret) {
        if (default_value != NULL) {
            *conf_value = *default_value;
            LC_LOG_WARNING("load int64 [%s] fail, use default value [%lld]", conf_name , *default_value);
            return LC_CONF_DEFAULT;
        } 
        LC_LOG_WARNING("load int64 fail, no found [%s]", conf_name);
        return LC_CONF_LOST;
    }

    if (is_blank_str(conf_value_str, sizeof(conf_value_str))) {
      LC_LOG_WARNING("int64 [%s] is empty", conf_name);
      return LC_CONF_CHECKFAIL;
    }
 
    errno = 0;
    num = strtoll(conf_value_str, &pts, 0);
    if (errno == ERANGE) {     
        LC_LOG_WARNING("int64 [%s] load error, [%s] overflow", conf_name, conf_value_str);
        return LC_CONF_OVERFLOW;
    }

    if (!is_blank_str(pts, sizeof(conf_value_str))) {
        LC_LOG_WARNING("int64 [%s] load error, [%s] has invalid character", conf_name, conf_value_str);
        return LC_CONF_INVALID_CHARACTER;
    }

    if (check_int64_range(conf, conf_name, num) != LC_CONF_CHECKSUCCESS) {
        return LC_CONF_OVERFLOW;
    }
    *conf_value = num;
    LC_LOG_NOTICE("get int64 value [%s : %lld]", conf_name, *conf_value);
    return ret;
}

int lc_conf_getuint64(const lc_conf_data_t *conf, const char *conf_name, 
        unsigned long long *conf_value, const char *comment, const unsigned long long *default_value)
{  
    unsigned long long num;
    char *pts;
    char conf_value_str[WORD_SIZE];
    int ret;

    if (NULL == conf || NULL == conf_name)
        return LC_CONF_NULL;

    if (conf->build != LC_CONF_READCONF) {
        if (write_comment(conf->conf_file, comment) == LC_CONF_SUCCESS) {
            if (default_value != NULL) {
                fprintf(conf->conf_file, "#[默认配置(uint64),  %s : %llu]\n%s : %llu", 
                        conf_name, *default_value, conf_name, *default_value);
            } else {
                fprintf(conf->conf_file, "%s : ", conf_name);
            }
            return LC_CONF_SUCCESS;
        }
        return LC_CONF_NULL;
    }

    if (NULL == conf_value)
        return LC_CONF_NULL;
    *conf_value = 0;
    ret = load_str(conf, conf_name, conf_value_str, sizeof(conf_value_str));
    if (LC_CONF_LOST == ret) {
        if (default_value != NULL) {
            *conf_value = *default_value;
            LC_LOG_WARNING("load uint64 [%s] fail, use default value [%llu]", conf_name , *default_value);
            return LC_CONF_DEFAULT;
        } 
        LC_LOG_WARNING("load uint64 fail, no found [%s]", conf_name);
        return LC_CONF_LOST;
    }

    if (is_blank_str(conf_value_str, sizeof(conf_value_str))) {
      LC_LOG_WARNING("uint64 [%s] is empty", conf_name);
      return LC_CONF_CHECKFAIL;
    }
 
    if (conf_value_str[0] == '-') {
      LC_LOG_WARNING("uint64 [%s] load error, [%s] overflow", conf_name, conf_value_str);
      return LC_CONF_OVERFLOW;
    }

    errno = 0;
    num = strtoull(conf_value_str, &pts, 0);
    if (errno == ERANGE) {     
        LC_LOG_WARNING("uint64 [%s] load error, [%s] overflow", conf_name, conf_value_str);
        return LC_CONF_OVERFLOW;
    }

    if (!is_blank_str(pts, sizeof(conf_value_str))) {
        LC_LOG_WARNING("uint64 [%s] load error, [%s] has invalid character", conf_name, conf_value_str);
        return LC_CONF_INVALID_CHARACTER;
    }

    if (check_uint64_range(conf, conf_name, num) != LC_CONF_CHECKSUCCESS) {
        return LC_CONF_OVERFLOW;
    }
    *conf_value = num;
    LC_LOG_NOTICE("get uint64 value [%s : %llu]", conf_name, *conf_value);
    return ret;
} 
int lc_conf_getfloat(const lc_conf_data_t *conf, const char *conf_name, 
        float *conf_value,  const char *comment, const float *default_value)
{ 
    float num;
    char *pts;
    char conf_value_str[WORD_SIZE];
    int ret;

    if (NULL == conf || NULL == conf_name)
        return LC_CONF_NULL;


    if (conf->build != LC_CONF_READCONF) {
        if (write_comment(conf->conf_file, comment) == LC_CONF_SUCCESS) {
            if (default_value != NULL) {
                fprintf(conf->conf_file, "#[默认配置(float),  %s : %f]\n%s : %f", 
                        conf_name, *default_value, conf_name, *default_value);
            } else {
                fprintf(conf->conf_file, "%s : ", conf_name);
            }
            return LC_CONF_SUCCESS;
        }
        return LC_CONF_NULL;
    }

    if (NULL == conf_value)
        return LC_CONF_NULL;
    *conf_value = 0.0f;
    ret = load_str(conf, conf_name, conf_value_str, sizeof(conf_value_str));
    if (LC_CONF_LOST == ret) {
        if (default_value != NULL) {
            *conf_value = *default_value;
            LC_LOG_WARNING("load float [%s] fail, use default value [%f]", conf_name , *default_value);
            return LC_CONF_DEFAULT;
        } 
        LC_LOG_WARNING("load float fail, no found [%s]", conf_name);
        return LC_CONF_LOST;
    }

    if (is_blank_str(conf_value_str, sizeof(conf_value_str))) {
      LC_LOG_WARNING("float [%s] is empty", conf_name);
      return LC_CONF_CHECKFAIL;
    }
 
    errno = 0;
    num = strtod(conf_value_str, &pts);
    if (errno == ERANGE) {     
        LC_LOG_WARNING("float [%s] load error, [%s] overflow", conf_name, conf_value_str);
        return LC_CONF_OVERFLOW;
    }

    if (!is_blank_str(pts, sizeof(conf_value_str))) {
        LC_LOG_WARNING("float [%s] load error, [%s] has invalid character", conf_name, conf_value_str);
        return LC_CONF_INVALID_CHARACTER;
    }

    *conf_value = num;
    LC_LOG_NOTICE("get float value [%s : %f]", conf_name, *conf_value);
    return ret;
}



int paste_iplist(const char *svr_name, const char *src, lc_svr_ip_t svr_ips[], u_int *num)
{
    u_int i, j, l, r;
    if (NULL == src || NULL == num)
      return LC_CONF_NULL;
    
    i = 0;
    *num = 0;
    while (*src) {
        if (i >= LC_CONF_IPMAX) {
            LC_LOG_WARNING("[%s] ip too much , must <= %u", svr_name, LC_CONF_IPMAX);
            return LC_CONF_CHECKFAIL;
        }

        l = strspn(src, " \t");
        src += l;
        j = 0;svr_ips[i].num = 0;
        while (*src) {
            if (j >= LC_CONF_MAX_CLONE) {
                LC_LOG_WARNING("[%s] clone ips too much, must <= %u", svr_name, LC_CONF_MAX_CLONE);
                return LC_CONF_CHECKFAIL;
            }
            r = strcspn(src, " \t/");
            if (r >= LC_CONF_IPLENMAX) {
                LC_LOG_WARNING("[%s] ip %*s too long", svr_name, r,src);
                return LC_CONF_CHECKFAIL;
            }
            strncpy(svr_ips[i].name[j], src, r);
            svr_ips[i].name[j][r] = '\0';
            if (check_str_ipv4(svr_ips[i].name[j]) 
                    != LC_CONF_CHECKSUCCESS) {
            	LC_LOG_WARNING("[%s] ip %s format error", svr_name, svr_ips[i].name[j]);
                return LC_CONF_CHECKFAIL;
            }
            src += r;
            j++;
            if (*src != '/') break;
            src++;
        }
        svr_ips[i].num = j;
        ++i;
    }
    *num = i;
    return LC_CONF_SUCCESS;
}


int lc_conf_getsvr(const lc_conf_data_t *conf, const char *product_name, 
    const char *module_name, lc_svr_t *conf_value, const char *comment) 
{
    char svr_name[WORD_SIZE];
    char str_tmp[WORD_SIZE];
    char conf_name[WORD_SIZE];
    int ret, item_ret;
    u_int tmp, tmp_default;


    if (NULL == conf)
        return LC_CONF_NULL;

    if (NULL == conf_value && LC_CONF_READCONF == conf->build)
        return LC_CONF_NULL;

    if (NULL == module_name) {
      LC_LOG_WARNING("[lc_conf_getsvr] module name is NULL");
      return LC_CONF_NULL;
    }

    if (LC_CONF_READCONF != conf->build) {
        if (write_comment(conf->conf_file, "") != LC_CONF_SUCCESS) {
            return LC_CONF_NULL;
        }
        if (write_comment(conf->conf_file, comment) != LC_CONF_SUCCESS) {
            return LC_CONF_NULL;
        }
    }

    
    ret = LC_CONF_SUCCESS;
    
    if (NULL == product_name) {
      snprintf(conf_name, sizeof(conf_name), "%s", module_name);
    } else {
      snprintf(conf_name, sizeof(conf_name), "%s_%s", product_name, module_name);
    }
   
    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_name", conf_name);
    item_ret = lc_conf_getnstr(conf, svr_name, str_tmp, sizeof(str_tmp), 
            "服务名");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF) {
        snprintf(conf_value->svr_name, sizeof(conf_value->svr_name), "%s", str_tmp); 
    }

    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_port", conf_name); 
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "启动服务的端口");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    } else {
        if (conf_value != NULL && conf->build == LC_CONF_READCONF && tmp > USHRT_MAX) {
            LC_LOG_WARNING("[%s] port [%d] more than %d", svr_name, tmp, USHRT_MAX);
            ret = LC_CONF_CHECKFAIL;
        }
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->port = tmp;

    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_readtimeout", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务的读超时(单位:ms)");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->read_timeout = tmp;
    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_writetimeout", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务的写超时(单位:ms)");
    if (item_ret != LC_CONF_SUCCESS) {
        return ret = item_ret; 
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->write_timeout = tmp;

    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_threadnum", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务启动的线程数");
    if (item_ret != LC_CONF_SUCCESS) {
        return ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->thread_num = tmp;

    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_connecttype", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务连接的类型(0:短连接, 1:长连接)"
            "example: 0 (use short connect)" );
    if (item_ret != LC_CONF_SUCCESS)
        ret = item_ret;
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->connect_type = tmp;

    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_servertype", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务使用的pool类型(0:XPOOL, 1:CPOOL, 2:EPOOL)");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->server_type = tmp;

    
    tmp_default = 100;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_quenesize", conf_name);
    lc_conf_getuint(conf, svr_name, &tmp, "CPOOL的socksize当使用CPOOL时设置有效", 
        &tmp_default);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->queue_size = tmp;
 
    
    tmp_default = 500;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_socksize", conf_name);
    lc_conf_getuint(conf, svr_name, &tmp, "CPOOL的socksize当使用CPOOL时设置有效"
        , &tmp_default);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->sock_size = tmp;
    
    
    if (LC_CONF_READCONF != conf->build) {
      if (fputc('\n', conf->conf_file) == EOF)
        return LC_CONF_NULL;
    }

    return ret;
}

int lc_conf_getreqsvr(const lc_conf_data_t *conf, const char *product_name, 
        const char *module_name, lc_request_svr_t *conf_value, const char *comment) 
{
    char svr_name[WORD_SIZE];
    char str_tmp[WORD_SIZE];
    char conf_name[WORD_SIZE];
    int ret, item_ret;
    u_int tmp;
    u_int tmp_default;
    if (NULL == conf)
        return LC_CONF_NULL;

    if (NULL == conf_value && LC_CONF_READCONF == conf->build)
        return LC_CONF_NULL;

    if (NULL == module_name) {
      LC_LOG_WARNING("[lc_conf_getsvr] module name is NULL");
      return LC_CONF_NULL;
    }

    if (LC_CONF_READCONF != conf->build) {
        if (write_comment(conf->conf_file, "") != LC_CONF_SUCCESS) {
            return LC_CONF_NULL;
        }
        if (write_comment(conf->conf_file, comment) != LC_CONF_SUCCESS) {
            return LC_CONF_NULL;
        }
    }

    
    ret = LC_CONF_SUCCESS;
    
    if (NULL == product_name) {
      snprintf(conf_name, sizeof(conf_name), "%s", module_name);
    } else {
      snprintf(conf_name, sizeof(conf_name), "%s_%s", product_name, module_name);
    }

    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_name", conf_name);
    item_ret = lc_conf_getnstr(conf, svr_name, str_tmp, sizeof(str_tmp), 
            "请求服务名");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF) {
        snprintf(conf_value->svr_name, sizeof(conf_value->svr_name), "%s", str_tmp); 
    }

    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_ip", conf_name);
    item_ret = lc_conf_getnstr(conf, svr_name, str_tmp, sizeof(str_tmp), 
            "请求服务的IP地址，可以设置多个IP, 在对应的rang文件中可以不设置规则，\n"
            "默认情况会对ip进行格式检查");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    } else {
        if (conf_value != NULL && conf->build == LC_CONF_READCONF) {
            item_ret = paste_iplist(svr_name, str_tmp, conf_value->ip_list, &(conf_value->num));
            if (item_ret != LC_CONF_SUCCESS) {
                ret = item_ret;
            }
        }
    }

    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_port", conf_name); 
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务的端口");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    } else {
        if (conf_value != NULL && conf->build == LC_CONF_READCONF && tmp > USHRT_MAX) {
            LC_LOG_WARNING("[%s] port [%d] more than %d", svr_name, tmp, USHRT_MAX);
            ret = LC_CONF_CHECKFAIL;
        }
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->port = tmp;
    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_readtimemout", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务的读超时(单位:ms)");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->read_timeout = tmp;
    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_writetimeout", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务的写超时(单位:ms)");
    if (item_ret != LC_CONF_SUCCESS) {
        return ret = item_ret; 
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->write_timeout = tmp;

    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_connecttimeout", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务的连接超时(单位:ms)");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->connect_timeout = tmp;

    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_maxconnect", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务时最大连接数");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->max_connect = tmp;


    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_retry", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务的重试次数");
    if (item_ret != LC_CONF_SUCCESS) {
        return ret = item_ret; 
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->retry = tmp;


    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_connecttype", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务连接的类型(0:短连接, 1:长连接)");
    if (item_ret != LC_CONF_SUCCESS)
        ret = item_ret;
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->connect_type = tmp;
   
    
    tmp_default = 0;
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_linger", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "连接是否使用linger方式\n"
            "使用linger方式可以避免出现TIME_WAIT的情况，但压力大时可能会出错\n"
            "默认不使用(0不使用， 非0使用)", &tmp_default);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->linger = tmp;
 
    
    if (LC_CONF_READCONF != conf->build) {
      if (fputc('\n', conf->conf_file) == EOF)
        return LC_CONF_NULL;
    }

    return ret;
}


int lc_conf_getonenstr(const char *path, const char *filename, 
        const char *conf_name, char *conf_value, const size_t n, const char *default_value)
{
    lc_conf_data_t *conf;
    int ret;
    if (NULL == conf_value || NULL == conf_name)
        return LC_CONF_NULL;
    conf = lc_conf_init(path, filename);
    if (NULL == conf)
        return LC_CONF_LOADFAIL;
    ret = lc_conf_getnstr(conf, conf_name, conf_value, n, NULL, default_value);
    lc_conf_close(conf);
    return ret;
}


int lc_conf_getoneint(const char *path, const char *filename, 
        const char *conf_name, int *conf_value, const int *default_value)
{
    lc_conf_data_t *conf;
    int ret;
    if (NULL == conf_value || NULL == conf_name)
        return LC_CONF_NULL;
    conf = lc_conf_init(path, filename);
    if (NULL == conf)
        return LC_CONF_LOADFAIL;
    ret = lc_conf_getint(conf, conf_name, conf_value, NULL, default_value);
    lc_conf_close(conf);
    return ret;
}


int lc_conf_getoneuint(const char *path, const char *filename, 
        const char *conf_name, u_int *conf_value,  const u_int *default_value)
{
    lc_conf_data_t *conf;
    int ret;
    if (NULL == conf_value || NULL == conf_name)
        return LC_CONF_NULL;
    conf = lc_conf_init(path, filename);
    if (NULL == conf)
        return LC_CONF_LOADFAIL;
    ret = lc_conf_getuint(conf, conf_name, conf_value, NULL, default_value);
    lc_conf_close(conf);
    return ret;
}


int lc_conf_getoneint64(const char *path, const char *filename, 
        const char *conf_name, long long *conf_value, const long long *default_value)
{
    lc_conf_data_t *conf;
    int ret;
    if (NULL == conf_value || NULL == conf_name)
        return LC_CONF_NULL;
    conf = lc_conf_init(path, filename);
    if (NULL == conf)
        return LC_CONF_LOADFAIL;
    ret = lc_conf_getint64(conf, conf_name, conf_value, NULL, default_value);
    lc_conf_close(conf);
    return ret;
}


int lc_conf_getoneuint64(const char *path, const char *filename, 
        const char *conf_name, unsigned long long *conf_value, const unsigned long long *default_value)
{
    lc_conf_data_t *conf;
    int ret;
    if (NULL == conf_value || NULL == conf_name)
        return LC_CONF_NULL;
    conf = lc_conf_init(path, filename);
    if (NULL == conf)
        return LC_CONF_LOADFAIL;
    ret = lc_conf_getuint64(conf, conf_name, conf_value, NULL, default_value);
    lc_conf_close(conf);
    return ret;
}

int lc_conf_getonefloat(const char *path, const char *filename, 
        const char *conf_name, float *conf_value, const float *default_value)
{
    lc_conf_data_t *conf;
    int ret;
    if (NULL == conf_value || NULL == conf_name)
        return LC_CONF_NULL;
    conf = lc_conf_init(path, filename);
    if (NULL == conf)
        return LC_CONF_LOADFAIL;
    ret = lc_conf_getfloat(conf, conf_name, conf_value, NULL, default_value);
    lc_conf_close(conf);
    return ret;
}



int lc_conf_getonesvr(const char *path, const char *filename, const char *product_name, 
        const char *module_name, lc_svr_t *conf_value)
{
    lc_conf_data_t *conf;
    int ret;
    if (NULL == conf_value)
        return LC_CONF_NULL;

    conf = lc_conf_init(path, filename);
    if (NULL == conf)
        return LC_CONF_LOADFAIL;
    ret = lc_conf_getsvr(conf, product_name, module_name, conf_value, NULL);
    lc_conf_close(conf);
    return ret;
}

int lc_conf_getonereqsvr(const char *path, const char *filename, const char *product_name,
        const char *module_name, lc_request_svr_t *conf_value)
{
    lc_conf_data_t *conf;
    int ret;
    if (NULL == conf_value)
        return LC_CONF_NULL;

    conf = lc_conf_init(path, filename);
    if (NULL == conf)
        return LC_CONF_LOADFAIL;
    ret = lc_conf_getreqsvr(conf, product_name, module_name, conf_value, NULL);
    lc_conf_close(conf);
    return ret;
}


int lc_conf_getsvr_ex(const lc_conf_data_t *conf, const char *product_name, 
    const char *module_name, lc_svr_t *conf_value, const char *comment) 
{
    char svr_name[WORD_SIZE];
    char str_tmp[WORD_SIZE];
    char conf_name[WORD_SIZE];
	char auth_tmp[256];
    int ret, item_ret;
    u_int tmp, tmp_default;
    int tmp1, tmp_default1;

    if (NULL == conf)
        return LC_CONF_NULL;

    if (NULL == conf_value && LC_CONF_READCONF == conf->build)
        return LC_CONF_NULL;

    if (NULL == module_name) {
      LC_LOG_WARNING("[lc_conf_getsvr] module name is NULL");
      return LC_CONF_NULL;
    }

    if (LC_CONF_READCONF != conf->build) {
        if (write_comment(conf->conf_file, "") != LC_CONF_SUCCESS) {
            return LC_CONF_NULL;
        }
        if (write_comment(conf->conf_file, comment) != LC_CONF_SUCCESS) {
            return LC_CONF_NULL;
        }
    }

    
    ret = LC_CONF_SUCCESS;
    
    if (NULL == product_name) {
      snprintf(conf_name, sizeof(conf_name), "%s", module_name);
    } else {
      snprintf(conf_name, sizeof(conf_name), "%s_%s", product_name, module_name);
    }
   
    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_name", conf_name);
    item_ret = lc_conf_getnstr(conf, svr_name, str_tmp, sizeof(str_tmp), 
            "服务名", svr_name);
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF) {
        snprintf(conf_value->svr_name, sizeof(conf_value->svr_name), "%s", str_tmp); 
    }

    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_port", conf_name); 
	tmp_default = (rand()%20000) + 2000;
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "启动服务的端口", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    } else {
        if (conf_value != NULL && conf->build == LC_CONF_READCONF && tmp > USHRT_MAX) {
            LC_LOG_WARNING("[%s] port [%d] more than %d", svr_name, tmp, USHRT_MAX);
            ret = LC_CONF_CHECKFAIL;
        }
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->port = tmp;

	
	
	
	tmp_default = 1<<20;
	snprintf(svr_name, sizeof(svr_name), "_svr_%s_readbufsize", conf_name);
	item_ret = lc_conf_getuint(conf, svr_name, &tmp, "设置服务的读缓冲区，默认为1<<20 bytes",
			&tmp_default);
	if (item_ret != LC_CONF_SUCCESS) {
		ret = item_ret;
	}
	if (conf_value != NULL && conf->build == LC_CONF_READCONF) {
		conf_value->read_buf_size = tmp;
	}

	tmp_default = 1<<20;
	snprintf(svr_name, sizeof(svr_name), "_svr_%s_writebufsize", conf_name);
	item_ret = lc_conf_getuint(conf, svr_name, &tmp, "设置服务的写缓冲区，默认为1<<20 bytes",
			&tmp_default);
	if (item_ret != LC_CONF_SUCCESS) {
		ret = item_ret;
	}
	if (conf_value != NULL && conf->build == LC_CONF_READCONF) {
		conf_value->write_buf_size = tmp;
	}

	tmp_default = 0;
	snprintf(svr_name, sizeof(svr_name), "_svr_%s_userbufsize", conf_name);
	item_ret = lc_conf_getuint(conf, svr_name, &tmp, "设置服务的用户缓冲区，默认为0 bytes",
			&tmp_default);
	if (item_ret != LC_CONF_SUCCESS) {
		ret = item_ret;
	}
	if (conf_value != NULL && conf->build == LC_CONF_READCONF) {
		conf_value->user_buf_size = tmp;
	}

	
	tmp_default = 500;
	snprintf(svr_name, sizeof(svr_name), "_svr_%s_connecttimeout", conf_name);
	item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务的连接超时(单位:ms)", &tmp_default);
	if (item_ret != LC_CONF_SUCCESS) {
		ret = item_ret;
	}
	if (conf_value != NULL && conf->build == LC_CONF_READCONF)
		conf_value->connect_timeout = tmp;

    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_readtimeout", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务的读超时(单位:ms)", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->read_timeout = tmp;

    
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_writetimeout", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务的写超时(单位:ms)", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        return ret = item_ret; 
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->write_timeout = tmp;

    
	tmp_default = 20;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_threadnum", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务启动的线程数", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        return ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->thread_num = tmp;

    
	tmp_default = 0;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_connecttype", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务连接的类型(0:短连接, 1:长连接)"
            "example: 0 (use short connect)", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS)
        ret = item_ret;
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->connect_type = tmp;

    
	tmp_default = 2;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_servertype", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务使用的pool类型(0:XPOOL, 1:CPOOL, 2:EPOOL, 3:APOOL), 使用xpool时，短连接请保证工作线程数大于前端到来的并发请求数，长连接请保证工作线程数大于前端设置的最大连接数"
			, &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->server_type = tmp;

    
    tmp_default = 100;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_quenesize", conf_name);
    lc_conf_getuint(conf, svr_name, &tmp, "CPOOL的socksize当使用CPOOL时设置有效", 
        &tmp_default);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->queue_size = tmp;
 
    
    tmp_default = 500;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_socksize", conf_name);
    lc_conf_getuint(conf, svr_name, &tmp, "CPOOL的socksize当使用CPOOL时设置有效"
        , &tmp_default);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->sock_size = tmp;

    
    tmp_default = 60;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_keepalivetimeout", conf_name);
    lc_conf_getuint(conf, svr_name, &tmp, "连接的keepalive时间 (s)", &tmp_default);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->keepalive_timeout = tmp;

    
    tmp_default = 0;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_stacksize", conf_name);
    lc_conf_getuint(conf, svr_name, &tmp, "工作线程的栈大小 (byte), 0表示不修改栈大小，不能为负数", &tmp_default);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->stack_size = tmp;

    
    tmp_default = 0;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_threadsched", conf_name);
    lc_conf_getuint(conf, svr_name, &tmp, "是否使用线程实时调度策略，需root权限，默认不开启", &tmp_default);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->using_threadsche = tmp;

    
    tmp_default1 = 10;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_listenpriority", conf_name);
    lc_conf_getint(conf, svr_name, &tmp1, "使用实时调度时，监听线程的优先级", &tmp_default1);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->listen_prio = tmp1;

    
    tmp_default1 = 5;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_workerpriority", conf_name);
    lc_conf_getint(conf, svr_name, &tmp1, "使用实时调度时，工作线程的优先级", &tmp_default1);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->worker_prio = tmp1;

    snprintf(svr_name, sizeof(svr_name), "_svr_%s_auth", conf_name);
    item_ret = lc_conf_getnstr(conf, svr_name, auth_tmp, sizeof(auth_tmp), 
            "授权IP文件", "");
    if (item_ret != LC_CONF_SUCCESS) {
		conf_value->auth[0] = 0;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF) {
        snprintf(conf_value->auth, sizeof(conf_value->auth), "%s", auth_tmp); 
    }

    conf_value->galileo_register_enable = 0;
	tmp_default1 = 0;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_galileo_register_enable", conf_name);
    lc_conf_getint(conf, svr_name, &tmp1, "是否向Galileo注册", &tmp_default1);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->galileo_register_enable = tmp1;


	conf_value->galileo_register_resdata[0] = 0;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_galileo_register_resdata", conf_name);
    item_ret = lc_conf_getnstr(conf, svr_name, auth_tmp, sizeof(auth_tmp), 
            "Galileo实体资源", "");
    if (item_ret == LC_CONF_SUCCESS && conf_value != NULL && conf->build == LC_CONF_READCONF) {
        snprintf(conf_value->galileo_register_resdata, sizeof(conf_value->galileo_register_resdata), "%s", auth_tmp); 
    }	

	conf_value->galileo_acl_enable = 0;
	tmp_default1 = 0;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_galileo_acl_enable", conf_name);
    lc_conf_getint(conf, svr_name, &tmp1, "是否启用Galileo ACL机制", &tmp_default1);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->galileo_acl_enable = tmp1;


	conf_value->galileo_svrlist[0] = 0;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_galileo_svrlist", conf_name);
    item_ret = lc_conf_getnstr(conf, svr_name, auth_tmp, sizeof(auth_tmp), 
            "Galileo服务器列表", "");
    if (item_ret == LC_CONF_SUCCESS && conf_value != NULL && conf->build == LC_CONF_READCONF) {
        snprintf(conf_value->galileo_svrlist, sizeof(conf_value->galileo_svrlist), "%s", auth_tmp); 
    }	

	conf_value->galileo_path[0] = 0;
    snprintf(svr_name, sizeof(svr_name), "_svr_%s_galileo_path", conf_name);
    item_ret = lc_conf_getnstr(conf, svr_name, auth_tmp, sizeof(auth_tmp), 
            "Galileo服务路径", "");
    if (item_ret == LC_CONF_SUCCESS && conf_value != NULL && conf->build == LC_CONF_READCONF) {
        snprintf(conf_value->galileo_path, sizeof(conf_value->galileo_path), "%s", auth_tmp); 
    }	

    
    if (LC_CONF_READCONF != conf->build) {
      if (fputc('\n', conf->conf_file) == EOF)
        return LC_CONF_NULL;
    }

	if (ret == LC_CONF_DEFAULT) {
		return LC_CONF_SUCCESS;
	}
    return LC_CONF_SUCCESS;
}

int lc_conf_getreqsvr_ex(const lc_conf_data_t *conf, const char *product_name, 
        const char *module_name, lc_request_svr_t *conf_value, const char *comment) 
{
    char svr_name[WORD_SIZE];
    char str_tmp[WORD_SIZE];
    char conf_name[WORD_SIZE];
    int ret, item_ret;
    u_int tmp;
    u_int tmp_default;
    if (NULL == conf)
        return LC_CONF_NULL;

    if (NULL == conf_value && LC_CONF_READCONF == conf->build)
        return LC_CONF_NULL;

    if (NULL == module_name) {
      LC_LOG_WARNING("[lc_conf_getsvr] module name is NULL");
      return LC_CONF_NULL;
    }

    if (LC_CONF_READCONF != conf->build) {
        if (write_comment(conf->conf_file, "") != LC_CONF_SUCCESS) {
            return LC_CONF_NULL;
        }
        if (write_comment(conf->conf_file, comment) != LC_CONF_SUCCESS) {
            return LC_CONF_NULL;
        }
    }

    
    ret = LC_CONF_SUCCESS;
    
    if (NULL == product_name) {
      snprintf(conf_name, sizeof(conf_name), "%s", module_name);
    } else {
      snprintf(conf_name, sizeof(conf_name), "%s_%s", product_name, module_name);
    }

    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_name", conf_name);
    item_ret = lc_conf_getnstr(conf, svr_name, str_tmp, sizeof(str_tmp), 
            "请求服务名", conf_name);
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF) {
        snprintf(conf_value->svr_name, sizeof(conf_value->svr_name), "%s", str_tmp); 
    }

    
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_ip", conf_name);
    item_ret = lc_conf_getnstr(conf, svr_name, str_tmp, sizeof(str_tmp), 
            "请求服务的IP地址，可以设置多个IP, 在对应的rang文件中可以不设置规则，\n"
            "默认情况会对ip进行格式检查", "127.0.0.1");
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    } else {
        if (conf_value != NULL && conf->build == LC_CONF_READCONF) {
            item_ret = paste_iplist(svr_name, str_tmp, conf_value->ip_list, &(conf_value->num));
            if (item_ret != LC_CONF_SUCCESS) {
                ret = item_ret;
            }
        }
    }

    
	tmp_default = 8020;
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_port", conf_name); 
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务的端口", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    } else {
        if (conf_value != NULL && conf->build == LC_CONF_READCONF && tmp > USHRT_MAX) {
            LC_LOG_WARNING("[%s] port [%d] more than %d", svr_name, tmp, USHRT_MAX);
            ret = LC_CONF_CHECKFAIL;
        }
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->port = tmp;
    
	tmp_default = 500;
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_readtimemout", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务的读超时(单位:ms)", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->read_timeout = tmp;
    
	tmp_default = 500;
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_writetimeout", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务的写超时(单位:ms)", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        return ret = item_ret; 
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->write_timeout = tmp;

    
	tmp_default = 200;
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_connecttimeout", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务的连接超时(单位:ms)", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->connect_timeout = tmp;

    
	tmp_default = 1000;
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_maxconnect", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务时最大连接数", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        ret = item_ret;
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->max_connect = tmp;


    
	tmp_default = 1;
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_retry", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "请求服务的重试次数", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS) {
        return ret = item_ret; 
    }
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->retry = tmp;


    
	tmp_default = 0;
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_connecttype", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "服务连接的类型(0:短连接, 1:长连接)", &tmp_default);
    if (item_ret != LC_CONF_SUCCESS)
        ret = item_ret;
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->connect_type = tmp;
   
    
    tmp_default = 0;
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_linger", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "连接是否使用linger方式\n"
            "使用linger方式可以避免出现TIME_WAIT的情况，但压力大时可能会出错\n"
            "默认不使用(0不使用， 非0使用)", &tmp_default);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->linger = tmp;

	
	tmp_default = 0;
    snprintf(svr_name, sizeof(svr_name), "_reqsvr_%s_no_magic_checker", conf_name);
    item_ret = lc_conf_getuint(conf, svr_name, &tmp, "1表示关闭，0表示开启 数据校验", &tmp_default);
    if (conf_value != NULL && conf->build == LC_CONF_READCONF)
        conf_value->no_magic_checker = tmp;
 
    
    if (LC_CONF_READCONF != conf->build) {
      if (fputc('\n', conf->conf_file) == EOF)
        return LC_CONF_NULL;
    }

	if (ret == LC_CONF_DEFAULT) {
		return LC_CONF_SUCCESS;
	}
    return LC_CONF_SUCCESS;
}
        






