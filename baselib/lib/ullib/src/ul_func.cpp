//**********************************************************
//			Wrap system normal function Utility 1.0
//
//  Description:
//    This utility defines all normal functions of ANSI C which were wraped. 
//**********************************************************

/* include area */
#include "ul_func.h"
#include "ul_log.h"

///////////////////////////////////////////////////////////////////////////////////
//                            memory operation                                  //
///////////////////////////////////////////////////////////////////////////////////

//*********************************************************************
// allocates memory for an array of nmemb elements of size bytes each and returns a
//       pointer to the allocated memory.  The memory is set to zero.
//	wrap include the LOG function.
//**********************************************************************
void  *ul_calloc(size_t n, size_t size)
{
	void *ptr = NULL;
	if (n <= 0 || size <= 0)
		ul_writelog(UL_LOG_WARNING, "calloc(%zu,%zu) call failed.Parameter was error.",
				n, size);
	ptr = calloc(n, size);
	if (NULL == ptr)
		ul_writelog(UL_LOG_WARNING, "calloc(%zu,%zu) call failed.error[%d] info is '%s'.",
				n, size, errno, strerror(errno));
	return(ptr);
}

//*******************************************
// allocates size bytes and returns a pointer to the allocated memory.  The memory  is
//       not cleared.
// wrap include the LOG function,and the memory will be set to zero
//*******************************************
void *ul_malloc(size_t size)
{
	void *ptr = malloc(size);
	if (NULL == ptr)
		ul_writelog(UL_LOG_WARNING, "malloc(%zu) call failed.error[%d] info is '%s'.",
				size, errno, strerror(errno));
	else {
		memset(ptr, '\0', size);
	}
	return(ptr);
}

//*********************************************
// realloc() returns a pointer to the newly allocated memory, which is suitably aligned for any
// kind  of variable and may be different from ptr, or NULL if the request fails or if size was
// equal to 0.
//*********************************************
void *ul_realloc(void *ptr, size_t size)
{
	void *ptr0 = realloc(ptr,size);
	if (NULL == ptr0)
		ul_writelog(UL_LOG_WARNING, "realloc(%zu) failed.error[%d] info is '%s'.",
				size, errno, strerror(errno));
	return	ptr0;
}

//*********************************************
// frees the memory space pointed to by ptr, which must have been returned by a previous
//       call to ul_Malloc(), ul_Calloc() or ul_Realloc().  
// wrap include the LOG function and parameter check
//*********************************************
void ul_free(void *ptr)
{
	if (NULL == ptr)
		return;
	free(ptr);
}

//************************************************
//	ul_open() open one file.	
//  This is the wrap function of open() system call	
//************************************************
int ul_open(const char *pathname, int oflag, mode_t mode)
{
	int	fd = open(pathname, oflag, mode);
	if (-1 == fd)
		ul_writelog(UL_LOG_WARNING,"open(%s,%d,%d) call failed.error[%d] info is '%s'.",
				pathname, oflag, mode, errno, strerror(errno));
	return fd;
}

//************************************************
// ul_read() attempts  to read up to count bytes from file descriptor fd into the buffer starting
//       at buf
// This function return the data size which it read really.
//************************************************
ssize_t ul_read(int fd, void *ptr, size_t nbytes)
{
	ssize_t	n = read(fd, ptr, nbytes);
	if (-1 == n)
		ul_writelog(UL_LOG_WARNING,"read(%d,%zu) call failed.error[%d] info is '%s'.",
				fd, nbytes, errno, strerror(errno));
	return n;
}

//*******************************************************
// ul_write() writes  up  to  count bytes to the file referenced by the file descriptor fd from the
//       buffer starting at buf.
// This function return the data size which it write really.
//*******************************************************
ssize_t ul_write(int fd, void *ptr, size_t nbytes)
{
	ssize_t	bytes = write(fd, ptr, nbytes);
	if (-1 == bytes)
		ul_writelog(UL_LOG_WARNING, "write(%d,%zu) call failed.error[%d] info is '%s'.", 
				fd, nbytes, errno, strerror(errno));
	return bytes;
}

//*******************************************************
// ul_close() will close a file descriptor.
//*******************************************************
int ul_close(int fd)
{
	int val = close(fd);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "close(%d) call failed.error[%d] info is '%s'.",
				fd, errno, strerror(errno));
	return val;
}

//******************************************
// performs one of various miscellaneous operations on fd.  The operation in question is
//      determined by cmd<see fcntl()>
//******************************************
int ul_fcntl(int fd, int cmd, int arg)
{
	int	n = fcntl(fd, cmd, arg);

	if (-1 == n)
		ul_writelog(UL_LOG_WARNING, "fcntl(%d,%d,%d) call failed.error[%d] info is '%s'.",
				fd, cmd, arg, errno, strerror(errno));
	return n;
}

//*****************************************************
// ul_dup() create a copy of the file descriptor oldfd.
// It uses the lowest-numbered unused descriptor for the new descriptor.
// Return: return the new descriptor if success.otherwize return -1
//*****************************************************
int	ul_dup(int oldfd)
{
	int	fd = dup(oldfd);
	if (-1 == fd)
		ul_writelog(UL_LOG_WARNING, "dup(%d) call failed.error[%d] info is '%s'.",
				oldfd, errno, strerror(errno));
	return fd;
}


//*****************************************************
// ul_dup2() create a copy of the file descriptor oldfd.
// makes newfd be the copy of oldfd, closing newfd first if necessary.
// Return:return the new descriptor if success.otherwize return -1 
//*****************************************************
int	ul_dup2(int oldfd, int newfd)
{
	int fd = dup2(oldfd, newfd);
	if (-1 == fd)
		ul_writelog(UL_LOG_WARNING, "dup2(%d) call failed.error[%d] info is '%s'.",
				oldfd, errno, strerror(errno));
	return fd;
}

//**************************************************
// ul_lseek() function repositions the offset of the file descriptor fildes to the argument off
//       set according to the directive whence as follows:
//		SEEK_SET(from 0 position)//SEEK_CUR(from current position)//SEEK_END(from tail)
// Return: if success,it returns the esulting offset.otherwize returns -1
//**************************************************
off_t ul_lseek(int fildes, off_t offset, int whence)
{
	off_t pos = lseek(fildes,offset,whence);
	if (-1 == pos)
		ul_writelog(UL_LOG_WARNING, "lseek(%d,%ld,%d) call failed.error(%d) info is %s.",
				fildes, offset, whence, errno, strerror(errno));
	return pos;
}

//**************************************************************
// ul_Pipe  creates  a  pair of file descriptors, pointing to a pipe inode, and places them in the
// 	array pointed to by filedes.  filedes[0] is for reading, filedes[1] is for writing.
// Return: 0 success, -1 failed.
//**************************************************************
int ul_pipe(int fds[2])
{
	int	val = pipe(fds);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "pipe() call failed.error(%d) info is %s.",
				errno, strerror(errno));
	return val;
}

//*******************************************************************
// ul_stat() return  information  about the specified file named by file name.  
// The stat of a file was be set in a data struct <struct stat>
// Return: 0 means success,and -1 meads failed.
//*******************************************************************
int ul_stat(const char *file_name, struct stat *buf)
{
	int	val = stat(file_name,buf);
	if(-1 == val)
		ul_writelog(UL_LOG_WARNING, "stat(%s) call failed.error[%d] info is '%s'.",
				file_name, errno, strerror(errno));
	return val;
}

//*******************************************************************
// ul_fstat() return  information  about the specified file named by file descripter .  
// The stat of a file was be set in a data struct <struct stat>
// Return: 0 means success,and -1 meads failed.
//*******************************************************************
int ul_fstat(int filedes, struct stat *buf)
{
	int	val = fstat(filedes,buf);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "fstat(%d) call failed.error[%d] info is '%s'.",
				filedes, errno, strerror(errno));
	return val;
}

//**************************************************************
// ul_Unlink() will delete on file.But if any  processes
//      still have the file open the file will remain in existence 
// Return: if success, 0 returns;otherwize -1 returned.
//**************************************************************
int	ul_unlink(const char *pathname)
{
	int	val = unlink(pathname);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "unlink(%s) call failed.error[%d] info is '%s'.",
				pathname, errno, strerror(errno));
	return val;
}

//****************************************
// wrap function about system call fchmod().
// on success,return value is 0, on failed,return -1
// Note: fchmod() was safety than chmod()
//****************************************
int ul_fchmod(int fildes, mode_t mode)
{
	int	val = fchmod(fildes,mode);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "fchmod(%d,%d) call failed,error[%d] info is '%s'.",
				fildes, mode, errno, strerror(errno));
	return val;
}

//*******************************************************
// ul_Chdir changes the current directory to that specified in path
// On success, zero is returned.  On error, -1 is returned
//*******************************************************
int	ul_chdir(const char *path)
{
	int	val = chdir(path);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "chdir(%s) call failed.error[%d] info is '%s'.",
				path, errno, strerror(errno));
	return val;
}

//****************************************
// wrap function about system call fchdir().The directory is given as an open file descriptor
// on success,return value is 0, on failed,return -1
// Note: fchdir() was safety than chdir()
//****************************************
int ul_fchdir(int fildes)
{
	int	val = fchdir(fildes);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "fchdir(%d) call failed,error[%d] info is '%s'.",
				fildes, errno, strerror(errno));
	return val;
}

//****************************************
// wrap function about system call fchown().
// on success,return value is 0, on failed,return -1
// Note: fchown() was safety than chown()
//     : If  the owner or group is specified as -1, then that ID is not changed.
//****************************************
int ul_fchown(int fildes, uid_t owner,gid_t group)
{
	int	val = fchown(fildes,owner,group);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "fchown(%d,%d,%d) call failed,error[%d] info is '%s'.",
				fildes, owner, group, errno, strerror(errno));
	return val;
}

//******************************************************************
// Truncate causes the file named by path to be truncated to at most length bytes in size.  
// Return: 0 -- success, -1 -- failed.
//******************************************************************
int	ul_truncate(const char *path, off_t length)
{
	int	val = truncate(path,length);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING,"truncate(%s,%ld) call failed.error[%d] info is '%s'.",
				path, length, errno, strerror(errno));
	return val;
}
//******************************************************************
// ul_ftruncate causes the file named by referenced by fd to be truncated to at most length bytes in size.  
// Return: 0 -- success, -1 -- failed.
//******************************************************************
int	ul_ftruncate(int fd, off_t length)
{
	int	val = ftruncate(fd,length);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "ftruncate(%d,%ld) call failed.error[%d] info is '%s'.",
				fd, length, errno, strerror(errno));
	return val;
}

//*****************************************************************************
// Wrap function about fpathconf().This function gets  a  value  for  the configuration option name for the
// open file descriptor filedes.
// Note:on failed,return -1.If the system does not have a limit,-100 is returned.
//*****************************************************************************
long ul_fpathconf(int filedes, int name)
{
	int   error = errno;
	long  val = fpathconf(filedes,name);
	if (-1 == val) {
		if (error != errno)
			ul_writelog(UL_LOG_WARNING, "fpathconf(%d,%d) call failed.error[%d] info is '%s'.",
					filedes,name,errno,strerror(errno));
		else {
			val = -100;
		}
	}
	return val;
}

//***********************************************************************
// Wrap function about pathconf().
// Note:on failed,return -1.If the system does not have a limit,-100 is returned.
//***********************************************************************
long ul_pathconf(char *path,int name)
{
	int	error = errno;
	long val = pathconf(path,name);
	if (-1 == val) {
		if (error != errno)
			ul_writelog(UL_LOG_WARNING, "pathconf(%s,%d) call failed.error[%d] info is '%s'.",
					path, name, errno, strerror(errno));
		else {
			val = -100;
		}
	}
	return val;
}

//////////////////////////////////////////////////////////////////////////////
//                                 mmap file                                //
//////////////////////////////////////////////////////////////////////////////

#ifdef _POSIX_MAPPED_FILES
//**********************************************************************
// ul_Mmap() function asks to map length bytes starting at offset from the file
// (or other object) specified by fd into memory, preferably at address start.
// On success,returns a pointer to the mapped  area.On error,returns -1
// Note: You can execute 'man mmap' to now how to use this command
//**********************************************************************
char *ul_mmap(void *start, size_t length, int prot , int flags, int fd, off_t offset)
{
	void *ptr = mmap(start, length, prot, flags, fd, offset);
	if ((void *)-1 == ptr)
		ul_writelog(UL_LOG_WARNING, "mmap(%p,%zu,%d,%d,%d,%ld) call failed.error[%d] info is '%s'.",
				start, length, prot, flags, fd, offset, errno, strerror(errno));
	return ((char *)ptr);
}

//*************************************************************************
// wrap about function munmap
// Note: here,the parameter 'start' was the point which returned by call mmap()
// on success, munmap returns 0, on failure -1
//*************************************************************************
int ul_munmap(void *start, size_t length)
{
	int val = munmap(start, length);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "munmap(%p,%zu) call filed.error[%d] info is '%s'.",
				start, length, errno, strerror(errno));
	return val;
}
#endif

////////////////////////////////////////////////////////////////////////////////
//          The next function was wraped functioni about stdio               //
////////////////////////////////////////////////////////////////////////////////

//************************************************
// wrap function about fopen()
// Return: success,a FILE point will be returned.Otherwise,NULL was returned.
//************************************************
FILE *ul_fopen(const char *filename, const char *mode)
{
	FILE *fp = fopen(filename, mode);
	if (NULL == fp) {
		ul_writelog(UL_LOG_WARNING, "fopen(\"%s\", \"%s\") failed.error[%d] info is '%s'.",
				filename, mode, errno, strerror(errno));
	}

	return fp;
}

//************************************************
// wrap function about fclose()
// Return: success,0 was returned.Otherwise,-1 was returned.
//************************************************
int ul_fclose(FILE *fp)
{
	int val = fclose(fp);
	if (val != 0) {
		val = -1;
		ul_writelog(UL_LOG_WARNING, "fclose() call was failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	}
	return  val;
}

//*******************************************************************
// wrap function about call fdopen().
// fdopen() associates a stream with the existing file descriptor
// on error,returns NULL
//*******************************************************************
FILE *ul_fdopen(int fd, const char *mode)
{   
	FILE *fp = fdopen(fd, mode);
	if (NULL == fp)
		ul_writelog(UL_LOG_WARNING, "fdopen(%d,%s) call was failed.error[%d] info is '%s'.",
				fd, mode, errno, strerror(errno));
	return fp;
}

//**************************************************
// Wrap function about freopen()
//**************************************************
FILE *ul_freopen(const char *path, const char *mode, FILE *stream)
{
	FILE *fp = freopen(path,mode,stream);
	if (NULL == fp)
		ul_writelog(UL_LOG_WARNING, "freopen(%s,%s) call failed.error[%d] info is '%s'.",
				path, mode, errno, strerror(errno));
	return	fp;
}

//*******************************************************************
// wrap function about call fgets().
// on error,returns NULL
//*******************************************************************
char *ul_fgets(char *ptr, int n, FILE *stream)
{
	char *rptr = fgets(ptr, n, stream);
	if (NULL == rptr && ferror(stream))
		ul_writelog(UL_LOG_WARNING,"fgets(%d) call failed.error[%d] info is '%s'.",
				n, errno, strerror(errno));
	return rptr;
}

//*******************************************************************
// wrap function about call fputs().
// return a non - negative number on success, or EOF on error
//*******************************************************************
int ul_fputs(const char *ptr, FILE *stream)
{
	int	val = EOF;
	if (NULL == ptr) {
		ul_writelog(UL_LOG_WARNING, "Parameter 'ptr' was NULL.");
		return EOF;
	}
	if ((val = fputs(ptr,stream)) == EOF)
		ul_writelog(UL_LOG_WARNING, "fputs(len=%zu) call failed.error[%d] info is '%s'.",\
				strlen(ptr), errno, strerror(errno));
	return val;
}

//*******************************
// Wrap function about fseek
//*******************************
int ul_fseek( FILE *stream, long offset, int whence)
{
	int	val = fseek(stream,offset,whence);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "fseek(%ld,%d) call failed.error[%d] info is '%s'.",
				offset, whence, errno, strerror(errno));
	return val;
}

//*******************************
// Wrap function about ftell
//*******************************
long ul_ftell(FILE *stream)
{
	long val = ftell(stream);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "ftell() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

//*******************************
// Wrap function about rewind
//*******************************
void ul_rewind(FILE *stream)
{
	rewind(stream);	
}

//*******************************
// Wrap function about fgetpos
//*******************************
int ul_fgetpos(FILE *stream, fpos_t *pos)
{
	int	val = fgetpos(stream,pos);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "fgetpos() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}
//*******************************
// Wrap function about fsetpos
//*******************************
int ul_fsetpos(FILE *stream, fpos_t *pos)
{
	int	val = fsetpos(stream,pos);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "fsetpos call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

//****************************************************************
// Wrap fuction about remove()
//****************************************************************
int ul_remove(const char *pathname)
{
	int	val = remove(pathname);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "remove(%s) call failed.error[%d] info is '%s'.",
				pathname, errno, strerror(errno));
	return val;
}

//*****************************************************************
// Wrap function about rename()
//*****************************************************************
int ul_rename(const char *oldpath, const char *newpath)
{
	int	val = -1;
	if (!strlen(oldpath)||!strlen(newpath))
		return -1;
	val = rename(oldpath,newpath);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "rename(%s,%s) call failed.error[%d] info is '%s'.",
				oldpath, newpath, errno, strerror(errno));
	return val;
}


//*****************************************************************
// Wrap function about tmpfile()
//*****************************************************************
FILE *ul_tmpfile (void)
{
	FILE *fp = tmpfile();

	if (NULL == fp)
		ul_writelog(UL_LOG_WARNING, "tmpfile() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return fp;
}

//*******************************************************************
// Wrap function about tmpnam()
//*******************************************************************
/* comment by Chen Jingkai at 2002:04:05:11:00:00 
   char *ul_tmpnam(char *s)
   {
   return(tmpnam(s));
   }
 */
//*******************************************************
// Wrap function about popen()
//*******************************************************
FILE *ul_popen(const char *command, const char *type)
{
	FILE *fp = NULL;
	if ((NULL == command || !strlen(command)) ||
		(NULL == type || !strlen(type)))
		return	NULL;
	fp = popen(command,type);
	if (NULL == fp)
		ul_writelog(UL_LOG_WARNING, "popen(%s,%s) call failed.error[%d] info is '%s'.",
				command, type, errno, strerror(errno));
	return fp;
}

//*******************************************************
// Wrap function about pclose()
//*******************************************************
int ul_pclose(FILE *stream)
{
	int	val = -1;
	if (stream == NULL )
		return -1;
	val = pclose(stream);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "pclose() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

/////////////////////////////////////////////////////////////////////////////
//                           directiry functions                           //
/////////////////////////////////////////////////////////////////////////////

//*******************************************
// Wrap function about opendir()
// on error,return NULL
//*******************************************
DIR *ul_opendir(const char *path)
{
	DIR *dirp = opendir(path);
	if (NULL == dirp)
		ul_writelog(UL_LOG_WARNING, "opendir(%s) call failed.error[%d] info is '%s'.",
				path, errno, strerror(errno));
	return dirp;
}

//******************************************************
// Wrap function about readdir()
// on error, NULL was returned.
//******************************************************
struct dirent *ul_readdir(DIR *dir)
{
	struct dirent *d = readdir(dir);
	if (NULL == d)
		ul_writelog(UL_LOG_WARNING, "readdir() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return	d;
}

//****************************************************
// Wrap function about closedir()
// on success,0 was returned.Otherwise, -1 returned.
//****************************************************
int	ul_closedir(DIR *dirp)
{
	int	val = -1;
	if (dirp == NULL || (val = closedir(dirp))== -1 )
		ul_writelog(UL_LOG_WARNING, "closedir() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

/////////////////////////////////////////////////////////////////////////////////////////
//                               Signal fucntion                                       //
/////////////////////////////////////////////////////////////////////////////////////////

//******************************************************
// Wrap function about kill()
// On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
//******************************************************
int ul_kill(pid_t pid, int sig)
{
	int	val = kill(pid,sig);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "kill(%d,%d) call failed.error[%d] info is '%s'.",
				pid, sig, errno, strerror(errno));
	if (val == 0 && pid == -1 )
		ul_writelog(UL_LOG_WARNING, "kill process(pid == -1 ) executed successfully.");
	return val;
}

//***************************************************************
// wrap function about raise()
//***************************************************************
int ul_raise(int sig)
{
	int val = raise(sig);
	if (val != 0)
		ul_writelog(UL_LOG_WARNING, "raise(%d) call failed.error[%d] info is '%s'.",
				sig, errno, strerror(errno));
	return val;
}

//******************************************************
// Wrap function about alarm()
//******************************************************
unsigned int ul_alarm(unsigned int secs)
{
	return alarm(secs);
}

//******************************************************
// Wrap function about sigemptyset()
// return 0 on success and -1 on error.
//******************************************************
int ul_sigemptyset(sigset_t *set)
{
	int val = sigemptyset(set);

	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "sigemptyset() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

//******************************************************
// Wrap function about sigfillset()
// return 0 on success and -1 on error.
//******************************************************
int ul_sigfillset(sigset_t *set)
{
	int val = sigfillset(set);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "sigfillset() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

//******************************************************
// Wrap fucntion aboout sigaddset()
// return 0 on success and -1 on error.
//******************************************************
int ul_sigaddset(sigset_t *set, int signum)
{
	int val = sigaddset(set,signum);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "sigaddset(%d) call failed.error[%d] info is '%s'.",
				signum, errno, strerror(errno));
	return val;
}

//******************************************************
// wrap function about sigdelset()
// return 0 on success and -1 on error.
//******************************************************
int ul_sigdelset(sigset_t *set, int signum)
{
	int	val = sigdelset(set,signum);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "sigdelset(%d) call failed.error[%d] info is '%s'.",
				signum, errno, strerror(errno));
	return	val;
}

//******************************************************
// Wrap function about sigismember()
// returns 1 if signum is a member of set,0 if signum is not a member,and -1 on error.
//******************************************************
int ul_sigismember(const sigset_t *set, int signum)
{
	int	val = sigismember(set,signum);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "sigismember(%d) call failed.error[%d] info is '%s'.",
				signum, errno, strerror(errno));
	return val;	
}

//******************************************************************
// Wrap function about sigaction()
// return 0 on success and -1 on error.
//******************************************************************
int ul_sigaction(int signo,const struct sigaction *act,struct sigaction *oact)
{
	int val = sigaction(signo, act, oact);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "sigaction(%d) call failed.error[%d] info is '%s'.",
				signo, errno, strerror(errno));
	return val;
}

//******************************************************************
// Wrap function about sigprocmask()
// return 0 on success and -1 on error.
//******************************************************************
int ul_sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	int val = sigprocmask(how,set,oldset);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "sigprocmask(%d) call failed.error[%d] info is '%s'.",
				how, errno, strerror(errno));
	return val;
}

//******************************************************************
// Wrap function about sigpending()
// return 0 on success and -1 on error.
//******************************************************************
int ul_sigpending(sigset_t *set)
{
	int val = sigpending(set);
	if (-1 == val )
		ul_writelog(UL_LOG_WARNING, "sigpending() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

//******************************************************************
// Wrap function about sigsuspend()
// return 0 on success and -1 on error.
//******************************************************************
int ul_sigsuspend(const sigset_t *mask)
{
	int	val = sigsuspend(mask);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "sigsuspend() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

//*************************************************************
// Wrap function about pause()
// pause always returns -1, and errno is set to ERESTARTNOHAND.
//*************************************************************
int ul_pause(void)
{
	return pause() ;
}

////////////////////////////////////////////////////////////////////////////////////
//                          Create process function                              //
////////////////////////////////////////////////////////////////////////////////////

//************************************************
// Wrap function about fork()
//************************************************
pid_t ul_fork(void)
{
	pid_t val = fork();
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "fork() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;	
}

//************************************************
// Wrap function about exec
//************************************************
int ul_execve(const char *filename, char *const argv [], char *const envp[])
{
	int	val = execve(filename, argv, envp);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "execve(%s) call failed.error[%d] info is '%s'.",
				filename, errno, strerror(errno));
	return val;
}

//**************************************
//  Wrap function about wait()
//**************************************
pid_t ul_wait(int *status)
{
	int	val = wait(status);
	if (val <= 0)
		ul_writelog(UL_LOG_WARNING, "wait() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

//**************************************
//  Wrap function about waitpid()
//**************************************
pid_t ul_waitpid(pid_t pid, int *status, int options)
{
	int	val = waitpid(pid,status,options);
	if (val <= 0 )
		ul_writelog(UL_LOG_WARNING, "waitpid(%d,%d) call failed.error[%d] info is '%s'.",
				pid, options, errno, strerror(errno));
	return val;
}

//*********************************************
// Wrap function about getpid()
//*********************************************
pid_t	ul_getpid()
{
	return getpid();
}

//*********************************************
// Wrap function about setpgid()
//*********************************************
int ul_setpgid(pid_t pid, pid_t pgid)
{
	int	val = setpgid(pid,pgid);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "setpgid(%d,%d) call failed.error[%d] info is '%s'.",
				pid, pgid, errno, strerror(errno));
	return val;
}

//*********************************************
// Wrap function about getpgid()
//*********************************************
/* comment by Chen Jingkai at 2002:04:05:11:00:00 
   pid_t ul_getpgid(pid_t pid)
   {
   pid_t	val;
   if((val=getpgid(pid)) == -1 )
   ul_writelog(UL_LOG_WARNING,"getpgid(%d) call failed.error[%d] info is '%s'.",pid,errno,strerror(errno));
   return	val;
   }
 */
//*********************************************
// Wrap function about setpgrp()
//*********************************************
int ul_setpgrp(void)
{
	int	val = setpgrp();
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "setpgrp() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

//*********************************************
// Wrap function about getpgrp()
//*********************************************
pid_t ul_getpgrp()
{
	return getpgrp();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                        IPC functions                                 //
/////////////////////////////////////////////////////////////////////////////////////////////

//*******************************************************************
//	Wrap function about mkfifo()
//******************************************************************* 
int ul_mkfifo(const char *pathname, mode_t mode)
{
	int	val = -1;
	if (strlen(pathname) == 0)
		return -1;
	val = mkfifo(pathname,mode);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "mkfifo(%s,%d) call failed.error[%d] info is '%s'.",
				pathname, mode, errno, strerror(errno));
	return val;
}

//*******************************************************************
// Wrap function about ftok()
//******************************************************************* 
key_t ul_ftok(char *pathname, char proj)
{
	key_t key = ftok(pathname,proj);
	if (-1 == key)
		ul_writelog(UL_LOG_WARNING, "ftok(%s,%c) call failed.error[%d] info is '%s'.",
				pathname, proj, errno, strerror(errno));
	return key;
}

//*******************************************************************
// Wrap function about shmget()
//******************************************************************* 
int ul_shmget(key_t key, size_t size, int shmflg) //man 里第2个参数是int 可真正的头文件里却是size_t，这里使用size_t
{
	int	id = -1;

	if (key < 0)
		return -1;
	id = shmget(key, size, shmflg);
	if (-1 == id)
		ul_writelog(UL_LOG_WARNING, "shmget(%d,%zu,%d) call failed.error[%d] info is '%s'.",
				key, size, shmflg, errno, strerror(errno));
	return id;
}

//*******************************************************************
// Wrap function about shmat()
// Return value: return	-1 if failed.
//******************************************************************* 
void *ul_shmat(int shmid, const void *shmaddr, int shmflg )
{
	void *ptr = shmat(shmid,shmaddr,shmflg);
	if ((void *)(-1) == ptr)
		ul_writelog(UL_LOG_WARNING, "shmat(%d,%d) call failed.error[%d] info is '%s'.",
				shmid, shmflg, errno, strerror(errno));
	return ptr;
}

//*******************************************************************
//	Wrap function about shmdt()
//******************************************************************* 
int	ul_shmdt(const void *shmaddr)
{
	int	val = shmdt(shmaddr);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "shmdt() call failed.error[%d] info is '%s'.",
				errno, strerror(errno));
	return val;
}

//*******************************************************************
//	Wrap function about shmctl()
//******************************************************************* 
int ul_shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
	int	val = shmctl(shmid, cmd, buf);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "shmctl(%d,%d) call failed.error[%d] info is '%s'.",
				shmid, cmd, errno, strerror(errno));
	return val;
}

//*******************************************************************
//	Wrap function about semget()
//******************************************************************* 
int ul_semget(key_t key, int nsems, int semflg )
{
	int	id = semget(key,nsems,semflg);
	if (-1 == id)
		ul_writelog(UL_LOG_WARNING, "semget(%d,%d,%d) call failed.error[%d] info is '%s'.",
				key, nsems, semflg, errno, strerror(errno));
	return	id;
}

//*******************************************************************
//	Wrap function about semctl()
//******************************************************************* 
int ul_semctl (int semid, int semnum, int cmd, union semun arg)
{
	int	val = semctl(semid, semnum, cmd, arg);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "semctl(%d,%d,%d) call failed.error[%d] info is '%s'.",
				semid, semnum, cmd, errno, strerror(errno));
	return val;
}

//*******************************************************************
//	Wrap function about semop()
//******************************************************************* 
int ul_semop(int semid, struct sembuf *sops, unsigned nsops )
{
	int	val = -1;
	while ((val = semop(semid,sops,nsops)) == -1) {
		if (EINTR == errno)	{
			continue;
		}
		ul_writelog(UL_LOG_WARNING, "semop(%d) call failed.error[%d] info is '%s'.",
				semid, errno, strerror(errno));
		break;
	}
	return val;
}

//*******************************************************************
//	Wrap function about msgget()
//******************************************************************* 
int ul_msgget(key_t key, int msgflg)
{
	int	id = msgget(key,msgflg);
	if (-1 == id)
		ul_writelog(UL_LOG_WARNING, "msgget(%d,%d) call failed.error[%d] info is '%s'.",
				key, msgflg, errno, strerror(errno));
	return id;
}

//*******************************************************************
//	Wrap function about msgctl()
//******************************************************************* 
int ul_msgctl(int msqid, int  cmd, struct msqid_ds *buf)
{
	int	val = msgctl(msqid,cmd,buf);
	if (-1 == val)
		ul_writelog(UL_LOG_WARNING, "msgctl(%d,%d) call failed.error[%d] info is '%s'.",
				msqid, cmd, errno, strerror(errno));
	return val;
}

//*******************************************************************
//	Wrap function about msgsnd()
//******************************************************************* 
int ul_msgsnd(int msqid, struct msgbuf *msgp, size_t msgsz, int msgflg)
{
	int	nbyte = msgsnd(msqid, msgp, msgsz, msgflg);
	if (-1 == nbyte)
		ul_writelog(UL_LOG_WARNING, "msgsnd(%d,%zu,%d) call failed.error[%d] info is '%s'.",
				msqid, msgsz, msgflg, errno, strerror(errno));
	return nbyte;
}

//*******************************************************************
//	Wrap function about msgrcv()
//******************************************************************* 
ssize_t ul_msgrcv(int msqid, struct msgbuf *msgp, size_t msgsz, long msgtyp, int msgflg)
{
	ssize_t	nbyte = msgrcv(msqid, msgp, msgsz, msgtyp, msgflg);
	if (-1 == nbyte)
		ul_writelog(UL_LOG_WARNING, "msgrcv(%d,%zu,%ld,%d) call failed.error[%d] info is '%s'.",
				msqid, msgsz, msgtyp, msgflg, errno, strerror(errno));
	return nbyte;
}




/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
