#include "ul_log.h"
#include "comlog/comlog.h"
#include "comlog/logswitcher.h"
//extern int __log_switcher__;

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#ifndef __NR_gettid
#define	__NR_gettid 	224
#endif

#define gettid() syscall(__NR_gettid)

static int g_file_size;
static char g_log_path[MAX_FILENAME_LEN+1] = "./";
static char g_proc_name[MAX_FILENAME_LEN+1] = "";

static pthread_key_t g_log_fd = PTHREAD_KEYS_MAX;
static pthread_once_t g_log_unit_once = PTHREAD_ONCE_INIT;
static ul_file_t g_file_array[LOG_FILE_NUM];
static ul_file_t g_file_stderr = {stderr, 0, 1, 0, PTHREAD_MUTEX_INITIALIZER, "/dev/stderr"}; 
static ul_log_t  g_log_stderr = {1, &g_file_stderr, &g_file_stderr, 0, UL_LOG_ALL, 0, 0, {NULL}};
static pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;

static ul_logstat_t g_default_logstate = {UL_LOG_ALL, UL_LOG_FATAL, UL_LOGTTY};


static int namenum_inc = 0;
static Ul_log_item  *item_str = NULL;
static int item_loc = 0;
static Ul_log_ctrl *ctrl_ptr = NULL;
static Ul_log_data *data_ptr = NULL;
static Ul_log_ctrl *ctrl_str = NULL;
static Ul_log_data *data_str = NULL;

static int   col_excur = -1;
static Ul_log_ctrl *ctrl_col_ptr = NULL;
static Ul_log_data *data_col_ptr = NULL;

static int g_global_level = 16;		  /**< 全局记录的日志等级       */


#define TIME_SIZE 20		  /**< 时间buff的长度        */

#define STRING_FATAL	"FATAL: "
#define STRING_WARNING	"WARNING: "
#define STRING_NOTICE	"NOTICE: "
#define STRING_TRACE	"TRACE: "
#define STRING_DEBUG	"DEBUG: "



static char *ul_ctime(char *t_ime, size_t t_ime_size)
{
	time_t tt;
	struct tm vtm;

	time(&tt);
	localtime_r(&tt, &vtm);
	snprintf(t_ime, t_ime_size, "%02d-%02d %02d:%02d:%02d",
			vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
	return t_ime;
}


static ul_log_t *ul_alloc_log_unit()
{
	ul_log_t   *log_fd;

	log_fd = (ul_log_t *)calloc(1, sizeof(ul_log_t));
	if (NULL == log_fd)
		return NULL;
//	log_fd->tid = pthread_self();
	log_fd->tid = gettid();

	if (pthread_setspecific(g_log_fd, log_fd) != 0) {
		free(log_fd);
		return NULL;
	}


	return log_fd;
}



static ul_log_t *ul_get_log_unit()
{
	return (ul_log_t *)pthread_getspecific(g_log_fd);
}

static void ul_free_log_unit()
{
	ul_log_t *log_fd;

	log_fd = ul_get_log_unit();
	if (log_fd != NULL) {
		pthread_setspecific(g_log_fd, NULL);
		free(log_fd);
	}
}


static FILE *ul_open_file(const char *name, char *mode)
{
	size_t path_len;
	FILE   *fp;
	const char* path_end;
	char   path[MAX_FILENAME_LEN+1];

	fp = fopen(name, mode);
	if (fp != NULL)
		return fp;

	path_end = strrchr(name, '/');
	if (path_end != NULL) {
		path_len = (path_end>name+MAX_FILENAME_LEN) ? MAX_FILENAME_LEN : (size_t)(path_end-name);
	} else {
		path_len = strlen(name)>MAX_FILENAME_LEN ? MAX_FILENAME_LEN : strlen(name);
	}
	strncpy(path, name, path_len);
	path[path_len] = '\0';

	mkdir(path, 0700);

	return fopen(name, mode);
}

static ul_file_t *ul_open_file_unit(const char *file_name, int flag, int max_size)
{
	int i;
	ul_file_t *file_free = NULL;

	pthread_mutex_lock(&file_lock);
	for (i=0; i<LOG_FILE_NUM; i++) {
		if (NULL == g_file_array[i].fp) {
			if (NULL == file_free) {
				file_free = &g_file_array[i];
			}
			continue;	
		}
		if (!strcmp(g_file_array[i].file_name, file_name)) {
			g_file_array[i].ref_cnt++;
			pthread_mutex_unlock(&file_lock);
			return &g_file_array[i];
		}

	}

	if (file_free != NULL) {
		file_free->fp = ul_open_file(file_name, "a");
		if (NULL == file_free->fp) {
			pthread_mutex_unlock(&file_lock);
			return (ul_file_t *)ERR_OPENFILE;
		}
		pthread_mutex_init(&(file_free->file_lock), NULL);
		file_free->flag = flag;
		file_free->ref_cnt = 1;
		file_free->max_size = max_size;
		snprintf(file_free->file_name, MAX_FILENAME_LEN, "%s", file_name);
	}

	pthread_mutex_unlock(&file_lock);

	return file_free;
}

static int ul_check(ul_file_t *file_fd, char *tmp_filename, size_t tmp_filename_size,
		int split_file)
{
	int ret = 0, stat_ret;
	struct stat st;
	if (NULL == file_fd || NULL == file_fd->fp) {
		return -1;
	}

	if (0 == file_fd->max_size) {
		return 0;
	}

	if (stderr == file_fd->fp) {
		return -1;
	}
	
	stat_ret = stat(file_fd->file_name, &st);
	
	if (-1 == stat_ret) {
		fclose(file_fd->fp);
		file_fd->fp = ul_open_file(file_fd->file_name, "a");
		if (NULL == file_fd->fp) {
			return -1;
		}
	}


	if ((file_fd->flag & UL_FILE_TRUNCATE) && st.st_size + LOG_BUFF_SIZE_EX >= file_fd->max_size) {
		if (!split_file) {
			//先将旧日志 mv 成 .filename.tmp,
			//在确认新的日志文件已经被建立后删除.这样可以防止意外造成文件丢失
			char * p = NULL;
			p = strrchr(file_fd->file_name, '/');
			if (NULL == p) {
				snprintf(tmp_filename, tmp_filename_size, ".%s.tmp", file_fd->file_name);
			} else {
				*p = '\0';
				snprintf(tmp_filename, tmp_filename_size, "%s/.%s.tmp",file_fd->file_name, p+1);
				*p = '/';
			}
		} else {
			int suff_num = 0;
			time_t tt;
			struct tm vtm;
			time(&tt);
			localtime_r(&tt, &vtm);
			int retl = snprintf(tmp_filename, tmp_filename_size, "%s.%04d%02d%02d%02d%02d%02d",
					file_fd->file_name, vtm.tm_year+1900, vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour, 
					vtm.tm_min, vtm.tm_sec);

			struct stat statbuf;
			while (stat(tmp_filename, &statbuf) == 0) {
				suff_num++;
				snprintf(tmp_filename + retl , tmp_filename_size - retl, ".%d", suff_num); 
			}
		}
		rename(file_fd->file_name, tmp_filename);
		fclose(file_fd->fp);
		file_fd->fp = ul_open_file(file_fd->file_name, "a");
		if (NULL == file_fd->fp) {
			return -1;
		}
		ret = 1;
	} else {
		if(st.st_size>=file_fd->max_size) {
			file_fd->flag |= UL_FILE_FULL;
		}
	}
	return ret;	    
}		    

int ul_set_namenum(int namenum)
{
	if(namenum < NAMENUM_INDATA) {
		namenum_inc = 0;
	} else {
		namenum_inc = namenum-NAMENUM_INDATA;
	}
	return namenum_inc+NAMENUM_INDATA;
}

int ul_openshm(const char *proc_name)
{
	int  i, j;
	int  rc;
	int  shmid;
	int  semid;
	int  excur = -1;
	int  key = 988988;
	int  sem_key = 899899; 
	struct sembuf sembuf;

	if ((shmid=shmget(key, 0, SVSHM_MODE)) == -1) {
		//ul_writelog(UL_LOG_NONE, "can't open shmid for shm\n");
		return(0);
	}

	if ((ctrl_str = (Ul_log_ctrl *)shmat(shmid, NULL, 0)) == (Ul_log_ctrl *) -1) {
		//fprintf(stderr, "in log: shared memory segments attached failed\n");
		shmctl(shmid, IPC_RMID, NULL);
		return(0);
	}
	data_str = (Ul_log_data *)(ctrl_str+MAX_ATTACH_NUM);


	if ((semid=semget(sem_key, 1, SVSEM_MODE)) < 0) {
		//fprintf(stderr, "in log: semget get error \n");
		return(0);
	}

	sembuf.sem_num = 0;
	sembuf.sem_op  = -1;
	sembuf.sem_flg = 0 | SEM_UNDO;
	while (1) {
		rc = semop(semid, &sembuf, 1);
		if (-1 == rc && EINTR == errno) {
			continue;
		} else if (-1 == rc) {
			//fprintf(stderr, "in log:get sem error %s\n",strerror(errno));
			return(0);
		} else {
			break;
		}
	}


	for (i = 0; i < MAX_ATTACH_NUM; i++) {
		if(!strcmp(proc_name, ctrl_str[i].processname)) {
			excur = i;
			break;
		} 
	} 

	if (excur != -1) {
		//fprintf(stderr, "the processi %s not normally exit\n", proc_name);
		ctrl_ptr = &ctrl_str[excur];
		data_ptr = (Ul_log_data *)((char *)data_str+(sizeof(Ul_log_data)+namenum_inc*sizeof(Ul_log_item))*excur);
		if(data_ptr != NULL) {
			snprintf((char *)((char *)data_ptr+(long)(((Ul_log_data *)NULL)->buff)+\
						sizeof(Ul_log_item)*namenum_inc), 10, "%s", STRING_NULL);
			for (j = 0; j < (NAMENUM_INDATA+namenum_inc); j++) {
				snprintf(data_ptr->item_str[j].name, sizeof(data_ptr->item_str[j].name), "%s", STRING_NULL);
				data_ptr->item_str[j].value = -1;
			}
		}
	} else { 
		for (i = 0; i < MAX_ATTACH_NUM; i++) {
			if (!strcmp(STRING_NULL, ctrl_str[i].processname)) {
				excur = i;
				break;
			} 
		} 
		if (-1 == excur) {
			//fprintf(stderr, "in log: no space in shm\n");
			return(0);
		}  

		ctrl_ptr = &ctrl_str[excur];
		data_ptr = (Ul_log_data *)((char *)data_str+(sizeof(Ul_log_data)+namenum_inc*sizeof(Ul_log_item))*excur);
	}

	if (ctrl_ptr != NULL) { 
		snprintf(ctrl_ptr->processname, PRO_NAME_LEN, "%s", proc_name);
		ctrl_ptr->processid = getpid();
	}

	sembuf.sem_op  = 1;
	if (semop(semid, &sembuf, 1) == -1) {
		//fprintf(stderr, "in log :get sem error %s\n",strerror(errno));
		return(0);
	}

	return 0;
}

int ul_closeshm(void)
{
	int  i;
	int  rc;
	int  semid;
	int  sem_key = 899899;
	struct sembuf sembuf;


	if ((semid = semget(sem_key, 1, SVSEM_MODE)) < 0) {
		return(-1);
	}


	sembuf.sem_num = 0;
	sembuf.sem_op  = -1;
	sembuf.sem_flg = 0|SEM_UNDO;
	while (1) {
		rc = semop(semid, &sembuf, 1);
		if (-1 == rc && EINTR == errno) {
			continue;
		} else if (-1 == rc) {
			//fprintf(stderr,"in log:get sem error %s\n",strerror(errno));
			return(-1);
		} else {
			break;
		}
	}



	if (ctrl_ptr != NULL) { 
		ctrl_ptr->processid = -1;
		snprintf(ctrl_ptr->processname, PRO_NAME_LEN, "%s", STRING_NULL);
		snprintf(ctrl_ptr->start_time, TIME_LEN, "%s", STRING_NULL);  
		ctrl_ptr=NULL;
	}
	
	if (data_ptr != NULL) {
		snprintf((char *)((char *)data_ptr+(long)(((Ul_log_data *)NULL)->buff)+\
					sizeof(Ul_log_item)*namenum_inc), 10, "%s", STRING_NULL); 
		for (i = 0; i < (NAMENUM_INDATA+namenum_inc); i++) {
			data_ptr->item_str[i].value = -1;
			snprintf(data_ptr->item_str[i].name, 10, "%s", STRING_NULL);
		}
		data_ptr = NULL;
	} 

	sembuf.sem_op  = 1;
	if (semop(semid, &sembuf, 1) == -1) {
		//fprintf(stderr, "in log :get sem error %s\n",strerror(errno));
		return(-1);
	}

	return 0;
}
void ul_closefile(ul_file_t *file_fd)
{
	pthread_mutex_lock(&file_lock);
	file_fd->ref_cnt--;

	if(file_fd->ref_cnt <= 0) {
		if (file_fd->fp != NULL) {
			fclose(file_fd->fp);
		}
		memset(file_fd, 0, sizeof(ul_file_t));
	}
	pthread_mutex_unlock(&file_lock);
}


int ul_openlog_ex(ul_log_t *log_fd, const char *file_name, int mask, int flag,
		int maxlen, ul_log_self_t * self = NULL)
{
	char tmp_name[MAX_FILENAME_LEN];

	log_fd->mask = mask;
	g_log_stderr.mask = mask;

	snprintf(tmp_name, MAX_FILENAME_LEN, "%slog", file_name);
	log_fd->pf = ul_open_file_unit(tmp_name, flag, maxlen);
	if (NULL == log_fd->pf) {
		return ERR_NOSPACE;
	} else if ((ul_file_t *)ERR_OPENFILE == log_fd->pf) {
		return ERR_OPENFILE;
	}

	snprintf(tmp_name, MAX_FILENAME_LEN, "%slog.wf", file_name);
	log_fd->pf_wf = ul_open_file_unit(tmp_name, flag, maxlen);
	if (NULL == log_fd->pf_wf) {
		ul_closefile(log_fd->pf);
		return ERR_NOSPACE;
	} else if ((ul_file_t *)ERR_OPENFILE == log_fd->pf_wf) {
		ul_closefile(log_fd->pf);
		return ERR_OPENFILE;
	}
	if (self != NULL) {
		for (int i = 0;i < self->log_number; i++) {
			if (strlen(self->name[i])!=0 && self->flags[i]) {
				snprintf(tmp_name,MAX_FILENAME_LEN, "%s%s.sdf.log", file_name,self->name[i]);
				log_fd->spf[i] = ul_open_file_unit(tmp_name,flag,maxlen);
				if (NULL == log_fd->spf[i]  || (ul_file_t *)ERR_OPENFILE == log_fd->spf[i]) {
					for (int j = i - 1;j >= 0;j--) {
						if (self->flags[j]) {
							ul_closefile(log_fd->spf[j]);
						}
					}
					ul_closefile(log_fd->pf);
					ul_closefile(log_fd->pf_wf);
					return (NULL == log_fd->spf[i])?ERR_NOSPACE:ERR_OPENFILE;
				}
			}
		}
	}
	return 0;
}

/**
 *
**/
static int ul_vwritelog_buff(ul_file_t * file_fd, const char *buff, const int split_file)
{
	int check_flag = 0;	
	char tmp_filename[PATH_SIZE];		  /**< 临时文件名，日志文件回滚使用       */
	
	if (NULL == file_fd) {
		return -1;
	}
	
	pthread_mutex_lock(&(file_fd->file_lock));
	check_flag = ul_check(file_fd, tmp_filename, sizeof(tmp_filename),
			split_file);

	if (check_flag >= 0) {
		fprintf(file_fd->fp, "%s\n", buff);
		fflush(file_fd->fp);
	}
	pthread_mutex_unlock(&(file_fd->file_lock));


	if (1 == check_flag && 0 == split_file) {
		remove(tmp_filename);
	}

	return 0;
}


static int ul_vwritelog_ex(ul_log_t *log_fd, int event, const char *fmt, va_list args)
{
	size_t  bpos = 0;
	char buff[LOG_BUFF_SIZE_EX];
	char now[TIME_SIZE];
	ul_file_t *file_fd;
	buff[0] = '\0';
	file_fd = log_fd->pf;
	if (log_fd->mask < event) {
		return ERR_EVENT;
	}
	switch (event) {
		case UL_LOG_START:
			break;

		case UL_LOG_WFSTART:
			file_fd = log_fd->pf_wf;
			break;

		case UL_LOG_NONE:
			break;
		case UL_LOG_FATAL:
			memcpy(&buff[bpos], STRING_FATAL, sizeof(STRING_FATAL));
			file_fd = log_fd->pf_wf;
			break;

		case UL_LOG_WARNING:
			memcpy(&buff[bpos], STRING_WARNING, sizeof(STRING_WARNING));
			file_fd = log_fd->pf_wf;
			break;

		case UL_LOG_NOTICE:
			memcpy(&buff[bpos], STRING_NOTICE, sizeof(STRING_NOTICE));
			break;

		case UL_LOG_TRACE:
			memcpy(&buff[bpos], STRING_TRACE, sizeof(STRING_TRACE));
			break;

		case UL_LOG_DEBUG:
			memcpy(&buff[bpos], STRING_DEBUG, sizeof(STRING_DEBUG));
			break;

		default:
			break;
	}

	if (file_fd->flag&UL_FILE_FULL) {
		return ERR_FILEFULL;
	}
	
	ul_ctime(now, sizeof(now));
	bpos += strlen(&buff[bpos]);
	bpos += snprintf(&buff[bpos], LOG_BUFF_SIZE_EX-bpos, "%s:  %s * %lu ",
			now, g_proc_name, log_fd->tid);
	
	vsnprintf(&buff[bpos], LOG_BUFF_SIZE_EX-bpos, fmt, args);

	if (log_fd->log_syslog&event) {
		int	priority;

		switch (event) {
			case UL_LOG_FATAL:
				priority = LOG_ERR;
				break; 

			case UL_LOG_WARNING:
				priority = LOG_WARNING;
				break; 

			case UL_LOG_NOTICE:
				priority = LOG_INFO;
				break;

			case UL_LOG_TRACE:
			case UL_LOG_DEBUG:
				priority = LOG_DEBUG;
				break;

			default:
				priority = LOG_NOTICE;
				break;
		}

		(void) syslog (priority, "%s\n", buff);
	}	
	
	return ul_vwritelog_buff(file_fd, buff, (log_fd->log_spec&UL_LOGSIZESPLIT));	
}

static int ul_vwritelog_ex_self(ul_log_t *log_fd, int event, const char *fmt, va_list args)
{
	size_t  bpos = 0;
	char buff[LOG_BUFF_SIZE_EX];
	char now[TIME_SIZE];
	ul_file_t *file_fd;

	file_fd = log_fd->spf[event];

	if (file_fd->flag & UL_FILE_FULL) {
		return ERR_FILEFULL;
	}

	ul_ctime(now, sizeof(now));
	bpos += snprintf(&buff[bpos], LOG_BUFF_SIZE_EX-bpos, "SDF_LOG: LEVEL:%d %s: %s * %lu",
			event, now, g_proc_name, log_fd->tid);
	vsnprintf(&buff[bpos], LOG_BUFF_SIZE_EX-bpos, fmt, args);

	return ul_vwritelog_buff(file_fd, buff, (log_fd->log_spec&UL_LOGSIZESPLIT));
}

static int ul_writelog_ex(ul_log_t *log_fd, int event, const char *fmt, ...)
{
	int ret = 0;
	va_list args;

	va_start(args, fmt);
	ret = ul_vwritelog_ex(log_fd, event, fmt, args);
	va_end(args);

	return ret; 
}




static int ul_closelog_fd(ul_log_t *log_fd)
{

	if (NULL == log_fd) {
		return -1;
	}

	if (log_fd->pf != NULL) {
		ul_closefile(log_fd->pf);
	}
	if (log_fd->pf_wf != NULL) {
		ul_closefile(log_fd->pf_wf);
	}
	for (int i = 0; i < MAX_SELF_DEF_LOG; i++) {
		if(log_fd->spf[i] != NULL) {
			ul_closefile(log_fd->spf[i]);
		}
	}

	return 1;
}

static void log_fd_init()
{
	pthread_key_create(&g_log_fd, NULL);
}

int  ul_threadlog_sup()
{
	pthread_once(&g_log_unit_once, log_fd_init);
	return 0;
} 

/**
 *
**/
static int ul_openlog_self(const ul_log_self_t *self)
{
	//check self define log
	if (self != NULL) {
		if (self->log_number > MAX_SELF_DEF_LOG || self->log_number < 0) {
			fprintf(stderr,"in ul_log.cpp :self define log_number error!log_number = %d\n",self->log_number);
			fprintf(stderr,"in ul_log.cpp:open error!\n");
			return -1;
		}
		for (int i =0 ;i < self->log_number; i++) {
			if (strlen(self->name[i]) == 0 && self->flags[i] != 0) {
				fprintf(stderr,"in ul_log.cpp :self define log error![%d]\n",i);
				fprintf(stderr,"in ul_log.cpp:open error!\n");
				return -1;
			}
		}
	}
	return 0;
}

int  ul_openlog_r(const char *thread_name, ul_logstat_t  *log_state, ul_log_self_t * self)
{
	if (comspace::com_get_log_switcher() == comspace::__USING_COMLOG__){
		return com_openlog_r();
	}
	pthread_t tid;
	ul_log_t *log_fd = NULL;
	char file_name[MAX_FILENAME_LEN];

	if (ul_openlog_self(self) == -1) {
		return -1;
	}
	
//	tid = pthread_self();
	tid = gettid();

	if (NULL == log_state) {
		log_state = &g_default_logstate;
	}
	if ((log_state->spec&UL_LOGNEWFILE) && thread_name!=NULL && thread_name[0]!='\0') {
		snprintf(file_name, MAX_FILENAME_LEN, "%s/%s_%s_%lu_", g_log_path, g_proc_name, thread_name, tid);
	} else if(log_state->spec&UL_LOGNEWFILE) {
		snprintf(file_name, MAX_FILENAME_LEN, "%s/%s_null_%lu_", g_log_path, g_proc_name, tid);
	} else {
		snprintf(file_name, MAX_FILENAME_LEN, "%s/%s", g_log_path, g_proc_name);
	}
	log_fd = ul_alloc_log_unit();
	if (NULL == log_fd) {
		fprintf(stderr,"in ul_log.cpp :no space!\n");
		fprintf(stderr,"in ul_log.cpp:open error!\n");
		return -1;
	}

	if (ul_openlog_ex(log_fd, file_name, log_state->events, UL_FILE_TRUNCATE, g_file_size,self) != 0) {
		if(log_state->spec&UL_LOGTTY) {
			fprintf(stderr, "in ul_log.cpp:Can't open log file : %slog, exit!\n", thread_name);
		}
		ul_free_log_unit();
		return -1;
	}

	log_fd->log_spec = log_state->spec;
	log_fd->log_syslog = log_state->to_syslog;

	if (log_state->spec&UL_LOGTTY) {
		fprintf(stderr, "Open log file %slog success!\n", thread_name);
	}
	ul_writelog_ex(log_fd, UL_LOG_START, "* Open thread log by----%s:%s\n============="
			"===================================", g_proc_name, thread_name);
	ul_writelog_ex(log_fd, UL_LOG_WFSTART, "* Open thread log by----%s:%s for wf\n======"
			"==========================================", g_proc_name, thread_name);

	return 0;
}

int ul_openlog(const char *log_path, const char *proc_name, ul_logstat_t  *log_state,
		int max_size, ul_log_self_t *self)
{
	if(comspace::com_get_log_switcher() != comspace::__LOG_NOT_DEFINED__){
		fprintf(stderr, "[ul_openlog]Log is already open....");
		return -1;
	}
	char *end;
	ul_log_t *log_fd;
	char file_name[MAX_FILENAME_LEN+1];

	ul_threadlog_sup();
	
	if (ul_openlog_self(self) == -1) {
		return -1;
	}
	
	if (NULL == log_state) {
		log_state = &g_default_logstate;
	}

	if (max_size <= 0 || max_size >= MAX_FILE_SIZE) {
		g_file_size = (MAX_FILE_SIZE<<20);
	} else {
		g_file_size = (max_size<<20);
	}

	if (NULL == log_path || '\0' == log_path[0]) {
		g_log_path[0] = '.';
		g_log_path[1] = '\0';
	} else {
		strncpy(g_log_path, log_path, MAX_FILENAME_LEN);
		g_log_path[MAX_FILENAME_LEN] = '\0';
	}

	if (NULL == proc_name || '\0' == proc_name[0]) {
		snprintf(g_proc_name, sizeof(g_proc_name), "%s", "null");
	} else {
		strncpy(g_proc_name, proc_name, MAX_FILENAME_LEN);
		g_proc_name[MAX_FILENAME_LEN] = '\0';
		end = strchr(g_proc_name, '_');
		if(end != NULL) {
			*end = '\0';
		}
	}

	snprintf(file_name, MAX_FILENAME_LEN, "%s/%s", g_log_path, g_proc_name);

	log_fd = ul_alloc_log_unit();
	if (NULL == log_fd) {
		fprintf(stderr,"in ul_log.cpp :no space!\n");
		fprintf(stderr,"in ul_log.cpp:open error!\n");
		return -1;
	}

	if (ul_openlog_ex(log_fd, file_name, log_state->events, UL_FILE_TRUNCATE, g_file_size,self) != 0) {
		if(log_state->spec&UL_LOGTTY)
			fprintf(stderr, "in ul_log.cpp:Can't open log file : %slog, exit!\n", proc_name);
		ul_free_log_unit();
		return -1;
	}

	g_global_level = log_state->events;
	log_fd->log_spec = log_state->spec;
	log_fd->log_syslog = log_state->to_syslog;

	if (log_state->spec & UL_LOGTTY) {
		fprintf(stderr, "Open log file %slog success!\n", proc_name);
	}
	ul_writelog_ex(log_fd, UL_LOG_START, "* Open process log by----%s\n============="
			"====================================", proc_name);
	ul_writelog_ex(log_fd, UL_LOG_WFSTART, "* Open process log by----%s for wf\n======"
			"==================================================", proc_name);

	ul_openshm(proc_name);

//	comspace::__log_switcher__ = comspace::__USING_ULLOG__;
	com_set_log_switcher(comspace::__USING_ULLOG__);

	return 0;
}

extern int com_writelog_args(int events, const char *fmt, va_list args);
int ul_writelog(const int event, const char* fmt, ...)
{
	int ret;
	va_list args;
	va_start(args, fmt);

	if(comspace::com_get_log_switcher() == comspace::__USING_COMLOG__){
		ret = com_writelog_args(event, fmt, args);
		va_end(args);
		return ret;
	}

	ul_log_t *log_fd;

	log_fd = ul_get_log_unit();
	if (NULL == log_fd) {
		log_fd = &g_log_stderr;
	}

	int self_log_id = event&UL_LOG_SELF_MASK;
	if (event >= UL_LOG_SELF_BEGIN && event <= UL_LOG_SELF_END && log_fd->spf[self_log_id] != NULL) {
		//写自定义log
		ret = ul_vwritelog_ex_self(log_fd, self_log_id, fmt, args);
	} else if (log_fd->mask < event) {
		ret = ERR_EVENT;
	} else {
		ret = ul_vwritelog_ex(log_fd, event, fmt, args);
	}
	va_end(args);
	
	if (log_fd->log_spec & UL_LOGTTY) {
		va_start(args,fmt);
		ret = ul_vwritelog_ex(&g_log_stderr, event, fmt, args);
		va_end(args);
	}
	return ret;
}

int ul_writelog_args(const int event, const char* fmt, va_list args)
{
	int ret;

	if(comspace::com_get_log_switcher() == comspace::__USING_COMLOG__){
		ret = com_writelog_args(event, fmt, args);
		return ret;
	}

	ul_log_t *log_fd;
    va_list bkargs;    //backup args for ullib tty output
    va_copy(bkargs, args);

	log_fd = ul_get_log_unit();
	if (NULL == log_fd) {
		log_fd = &g_log_stderr;
	}

	int self_log_id = event&UL_LOG_SELF_MASK;
	if (event >= UL_LOG_SELF_BEGIN && event <= UL_LOG_SELF_END && log_fd->spf[self_log_id] != NULL) {
		//写自定义log
		ret = ul_vwritelog_ex_self(log_fd, self_log_id, fmt, args);
	} else if (log_fd->mask < event) {
		ret = ERR_EVENT;
	} else {
		ret = ul_vwritelog_ex(log_fd, event, fmt, args);
	}
	
	if (log_fd->log_spec & UL_LOGTTY) {
		ret = ul_vwritelog_ex(&g_log_stderr, event, fmt, bkargs);
	}
    va_end(bkargs);
	return ret;
}

static int ul_closelog_ex(int iserr, const char * close_info)
{
	ul_log_t  *log_fd;

	log_fd = ul_get_log_unit();
	
	if (NULL == log_fd) {
		return -1;
	}


	if (iserr) {
		ul_writelog_ex(log_fd, UL_LOG_END, 
				"< ! > Abnormally end %s\n================================================",
				close_info);
		ul_writelog_ex(log_fd, UL_LOG_WFEND, 
				"< ! > Abnormally end %s\n================================================",
				close_info);
	} else {
		ul_writelog_ex(log_fd, UL_LOG_END, 
				"< - > Normally end %s\n================================================",
				close_info);
		ul_writelog_ex(log_fd, UL_LOG_WFEND, 
				"< - > Normally end %s\n================================================",
				close_info);
	}

	ul_closelog_fd(log_fd);

	if (log_fd->log_spec & UL_LOGTTY) {
		fprintf(stderr,"Close log successed.\n");
	}
	ul_free_log_unit();


	return 0;
}

int ul_closelog_r(int iserr)
{
	if(comspace::com_get_log_switcher() == comspace::__USING_COMLOG__){
		return com_closelog_r();
	}
	return ul_closelog_ex(iserr, "thread");
}



int ul_closelog(int iserr)
{
	if(comspace::com_get_log_switcher() == comspace::__USING_COMLOG__){
		return com_closelog(iserr);
	}
	ul_closeshm();

	int ret = ul_closelog_ex(iserr, "process");
//	comspace::__log_switcher__ = comspace::__LOG_NOT_DEFINED__;
	com_set_log_switcher(comspace::__LOG_NOT_DEFINED__);
	return ret;
}

int ul_log_assign(char *name, int ini_val)
{
	if (NULL == data_ptr) {
		fprintf(stderr, "in log :maybe no apply block from shm\n");
		return(-1);
	}

	if (NULL == name) {
		fprintf(stderr, "in log :name is NULL\n");
		return(-1);
	} 

	if (strlen(name) >= NAMELEN_INDATA) {
		fprintf(stderr, "in log : name is tooo long %s\n", name);  
		return(-1);
	} 

	if (item_loc >= (NAMENUM_INDATA+namenum_inc)) {
		fprintf( stderr, "in log : no space for new name,for cur_loc is  %d\n", item_loc);
		return(-1);
	}


	snprintf(data_ptr->item_str[item_loc].name, NAMELEN_INDATA, "%s", name);  
	data_ptr->item_str[item_loc].value = ini_val;

	if (NULL == item_str) {
		item_str = (Ul_log_item *)calloc(sizeof(Ul_log_item), NAMENUM_INDATA+namenum_inc);
	}
	snprintf(item_str[item_loc].name, NAMELEN_INDATA, "%s", name);  
	item_str[item_loc].value = ini_val;

	item_loc++;

	return(0);
}         


int ul_log_getid(char *name)
{
	int i;
	int cur_loc = -1;

	for (i = 0; i < (NAMENUM_INDATA+namenum_inc) && item_str != NULL; i++) {
		if (!strcmp(name, item_str[i].name)) {
			cur_loc = i;
			break;
		}
	}

	if (-1 == cur_loc) {
		fprintf(stderr, "in log : there is no %s in data block\n", name);
		return(-1);
	}

	return(i);
} 


int ul_log_setval( int rel_loc ,int val)
{
	if (rel_loc<0 || rel_loc>=(NAMENUM_INDATA+namenum_inc)) {
		fprintf(stderr, "in log :rel_loc is uneffective %d \n", rel_loc);
		return(-1);
	}

	if (NULL == data_ptr) {
		fprintf(stderr, "in log : maybe no apply block from shm\n");
		return(-1);
	}

	data_ptr->item_str[rel_loc].value = val;

	return(0);
} 


int ul_log_addval( int rel_loc,int val)
{
	if (rel_loc<0 || rel_loc>=(NAMENUM_INDATA+namenum_inc)) {
		fprintf(stderr, "in log :rel_loc is uneffective %d \n", rel_loc);
		return(-1);
	}

	if (NULL == data_ptr) {
		fprintf(stderr, "in log : maybe no apply block from shm\n");
		return(-1);
	} 

	data_ptr->item_str[rel_loc].value += val;

	return(0);
}


int ul_log_monproc(char * processname)
{
	int i,shmid;
	int key = 988988;
	Ul_log_ctrl *ctrl_str = NULL;
	Ul_log_data *data_str = NULL;


	shmid = shmget(key,0,SVSHM_MODE);
	ctrl_str = (Ul_log_ctrl *)shmat(shmid, NULL, SHM_RDONLY);

	if ((Ul_log_ctrl *)-1 == ctrl_str) {
		fprintf(stderr, "in log:  shared memory segments attached failed\n");
		shmctl(shmid, IPC_RMID, NULL);
		exit (1);
	}

	data_str = (Ul_log_data *)(ctrl_str+MAX_ATTACH_NUM);

	for (i = 0; i < MAX_ATTACH_NUM; i++) {
		if (!strcmp(processname, ctrl_str[i].processname)) {
			col_excur = i;
			break;
		}
	}

	if (-1 == col_excur) {
		fprintf(stderr, "in log : no item of process : %s\n", processname);
		return(-1);
	}

	ctrl_col_ptr = &ctrl_str[i];
	data_col_ptr = (Ul_log_data *)((char *)data_str+(sizeof(Ul_log_data)+namenum_inc*sizeof(Ul_log_item))*i);

	return(0);
}        

int ul_log_getval(char *name, int *pval)
{
	int i;
	int cur_loc = -1;

	if (NULL == data_col_ptr || NULL == ctrl_col_ptr) {
		fprintf(stderr, "in log : maybe no  block in shm for process \n");
		return(-1);
	}    

	if (!strcmp(ctrl_col_ptr->processname, STRING_NULL)) {
		fprintf(stderr,"in log : the process has closed\n");
		return(-1);
	}

	for (i = 0; i < (NAMENUM_INDATA+namenum_inc); i++) {
		if (!strcmp(name, data_col_ptr->item_str[i].name)) {
			cur_loc = i;
			break;
		}
	}

	if (cur_loc == -1) {
		fprintf(stderr, "in log:there is no %s in data block\n", name);
		return(-1);
	}

	*pval = data_col_ptr->item_str[cur_loc].value;

	return(0);
}


int ul_log_getbuf(char *buff, int buflen)
{
	if (NULL == data_col_ptr || NULL == ctrl_col_ptr) {
		fprintf(stderr,"in log : maybe no  block in shm for process \n");
		return(-1);
	}


	snprintf(buff, buflen, "%s", (char *)((char *)data_col_ptr+(long)(((Ul_log_data *)NULL)->buff)+\
				sizeof(Ul_log_item)*namenum_inc));

	return(0);
}

int ul_log_createshm(void)
{
	int rc;
	int shm_id = -1;
	int shm_key = 988988;
	int sem_id = -1;
	int sem_key = 899899;
	struct sembuf sembuf;


	shm_id = shmget(shm_key,  (sizeof(Ul_log_ctrl)+sizeof(Ul_log_data)+\
				sizeof(Ul_log_item)*namenum_inc)*MAX_ATTACH_NUM, 0666|IPC_CREAT|IPC_EXCL);
	if (-1 == shm_id) {
		goto FAIL;
	}
	sem_id = semget(sem_key,  1, SVSEM_MODE|IPC_CREAT|IPC_EXCL);
	if (-1 == sem_id) {
		goto FAIL;
	}

	sembuf.sem_num = 0;
	sembuf.sem_op  = 1;
	sembuf.sem_flg = 0;
	while (1) {
		rc = semop(sem_id, &sembuf, 1);
		if (-1 == rc && EINTR == errno) {
			continue;
		} else if (-1 == rc) {
			goto FAIL;
		} else {
			break;
		}
	}

	ctrl_str = (Ul_log_ctrl *)shmat(shm_id, NULL, 0);
	if ((Ul_log_ctrl *)-1 == ctrl_str) {
		goto FAIL;
	}

	data_str = (Ul_log_data *)(ctrl_str+MAX_ATTACH_NUM);

	return 0;

FAIL:

	return -1;
}

int ul_log_attachshm(void)
{
	int shm_id = -1;
	int shm_key = 988988;


	shm_id = shmget(shm_key,  0, SVSHM_MODE);
	if (-1 == shm_id) {
		fprintf(stderr, "in ul_log_attachshm: attach share memory error %s\n",strerror(errno));
		return -1;
	}

	ctrl_str = (Ul_log_ctrl *)shmat(shm_id, NULL, 0);
	if ((Ul_log_ctrl *)-1 == ctrl_str) {
		fprintf(stderr, "in ul_log_attachshm: attach share memory error %s\n",strerror(errno));
		return -1;
	}

	data_str = (Ul_log_data *)(ctrl_str+MAX_ATTACH_NUM);


	return 0;
}

int ul_log_deattachshm(void)
{
	if (ctrl_str != NULL && ctrl_str != (Ul_log_ctrl *)-1) {
		return shmdt(ctrl_str);
	}
	return 0;
}

int ul_log_cleanshm(char *proc_name, char *item_name, bool clean_buff)
{
	int i, j;
	Ul_log_data *cur_data_ptr = NULL;


	if ((Ul_log_ctrl *)-1 == ctrl_str) {
		return -1;
	}

	if (NULL == proc_name) { 
		for (i=0; i < MAX_ATTACH_NUM; i++) {
			ctrl_str[i].processid = -1;
			snprintf(ctrl_str[i].processname, PRO_NAME_LEN, "%s", STRING_NULL);
			snprintf(ctrl_str[i].start_time, TIME_LEN, "%s", STRING_NULL);
		}

		for (i=0; i < MAX_ATTACH_NUM; i++) {
			cur_data_ptr = (Ul_log_data *)((char *)data_str+(sizeof(Ul_log_data)+namenum_inc*sizeof(Ul_log_item))*i);
			snprintf((char *)((char *)cur_data_ptr+(long)(((Ul_log_data *)NULL)->buff)+\
						sizeof(Ul_log_item)*namenum_inc), 10, "%s", STRING_NULL);
			for (j = 0; j < (NAMENUM_INDATA+namenum_inc); j++) {
				cur_data_ptr->item_str[j].value=-1;
				snprintf(cur_data_ptr->item_str[j].name, sizeof(cur_data_ptr->item_str[j].name),
						"%s", STRING_NULL);
			}
		}
	} else if (NULL == item_name && !clean_buff) {
			for (i=0; i < MAX_ATTACH_NUM; i++) {
				if (!strcmp(ctrl_str[i].processname, proc_name))
					break;
			}
			if (i >= MAX_ATTACH_NUM)
				return -1;

			ctrl_str[i].processid = -1;
			snprintf(ctrl_str[i].processname, PRO_NAME_LEN, "%s", STRING_NULL);
			snprintf(ctrl_str[i].start_time, TIME_LEN, "%s", STRING_NULL);

			cur_data_ptr = (Ul_log_data *)((char *)data_str+(sizeof(Ul_log_data)+namenum_inc*sizeof(Ul_log_item))*i);
			for (j = 0; j < (NAMENUM_INDATA+namenum_inc); j++) {
				cur_data_ptr->item_str[j].value = -1;
				snprintf(cur_data_ptr->item_str[j].name, sizeof(cur_data_ptr->item_str[j].name), "%s", STRING_NULL);
			}

			snprintf((char *)((char *)cur_data_ptr+(long)(((Ul_log_data *)NULL)->buff)+\
						sizeof(Ul_log_item)*namenum_inc), 10, "%s", STRING_NULL);

		} else {
			for(i=0; i<MAX_ATTACH_NUM; i++) {
				if (!strcmp(ctrl_str[i].processname, proc_name))
					break;
			}
			if (i >= MAX_ATTACH_NUM) {
				return -1;
			}

			cur_data_ptr = (Ul_log_data *)((char *)data_str+(sizeof(Ul_log_data)+namenum_inc*sizeof(Ul_log_item))*i);

			if (clean_buff) {
				snprintf((char *)((char *)cur_data_ptr+(long)(((Ul_log_data *)NULL)->buff)+\
							sizeof(Ul_log_item)*namenum_inc), 10, "%s", STRING_NULL);
			}

			if (item_name != NULL) {
				for (j = 0; j < (NAMENUM_INDATA+namenum_inc); j++) {
					if (!strcmp(cur_data_ptr->item_str[j].name, item_name)) {
						break;
					}
				}
				if (j >= (NAMENUM_INDATA+namenum_inc)) {
					return -1;
				}

				cur_data_ptr->item_str[j].value=-1;
				snprintf(cur_data_ptr->item_str[j].name, 10, "%s", STRING_NULL);
			}
		}
	return 0;
}

int ul_log_listproc(char *proc_name, Ul_log_ctrl *proc_ctrl)
{
	static int cur_proc = 0;

	if (NULL == proc_ctrl) {
		cur_proc = 0;
		return 0;
	}

	if (NULL == ctrl_str) {
		return -1;
	}

	if (cur_proc >= MAX_ATTACH_NUM) {
		return 0;
	}

	for (; cur_proc<MAX_ATTACH_NUM; cur_proc++) {
		if (proc_name != NULL) {
			if (!strcmp(ctrl_str[cur_proc].processname, proc_name) != 0) {
				break;
			}
		} else {
			if (strcmp(ctrl_str[cur_proc].processname, STRING_NULL) != 0) {
				break;
			}
		}
	}
	if (cur_proc >= MAX_ATTACH_NUM) {
		return 0;
	}

	memcpy(proc_ctrl, &ctrl_str[cur_proc], sizeof(Ul_log_ctrl));
	cur_proc++;

	return 1;

}

int ul_log_listitem(char *proc_name, Ul_log_item *item_str, char *buff, int buff_len)
{
	int i;
	static int cur_item = 0;
	static Ul_log_data *cur_data_ptr = NULL;

	if (NULL == proc_name && NULL == item_str && NULL == buff) {
		cur_item = 0;
		cur_data_ptr = NULL;
		return 0;
	}

	if (NULL == ctrl_str || NULL == data_str) {
		return -1;
	}

	if (NULL == cur_data_ptr && proc_name != NULL) {
		for (i = 0; i < MAX_ATTACH_NUM; i++) {
			if (!strcmp(ctrl_str[i].processname, proc_name)) {
				break;
			}
		}

		if (i >= MAX_ATTACH_NUM) {
			return 0;
		}

		cur_item = 0;

		cur_data_ptr = (Ul_log_data *)((char *)data_str+(sizeof(Ul_log_data)+namenum_inc*sizeof(Ul_log_item))*i);
	} else if (NULL == cur_data_ptr) {
		return -1;
	}


	if (item_str != NULL) {
		for (; cur_item < (NAMENUM_INDATA+namenum_inc); cur_item++) {
			if (strcmp(cur_data_ptr->item_str[cur_item].name, STRING_NULL)) {
				break;
			}
		}
		if (cur_item >= (NAMENUM_INDATA+namenum_inc)) {
			return 0;
		}

		memcpy(item_str, &(cur_data_ptr->item_str[cur_item]), sizeof(Ul_log_item));
		cur_item++;
	}

	if (buff != NULL) {
		snprintf(buff, buff_len, "%s", (char *)((char *)cur_data_ptr+(long)(((Ul_log_data *)NULL)->buff)+\
					sizeof(Ul_log_item)*namenum_inc));
		if (!strcmp(buff, STRING_NULL))
			return 0;
	}


	return 1;
}

int ul_get_log_level() {
	return  g_global_level;
}

void ul_set_log_level(int lv) {
	g_global_level = lv;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
