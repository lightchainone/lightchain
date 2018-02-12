
#ifndef __UL_FILE_H
#define __UL_FILE_H

#include <stdlib.h>
#include <stdio.h>
#include "dep/dep_file.h"


/**
 *
**/
int ul_fullpath(const char *path, const char *fname, char *fullpath, size_t size);

/**
 *
 */
int ul_fexist(const char *path,const char *fname);

/**
 *
 */
off_t ul_fsize(const char *path, const char *fname);

/**
 *
 */
int ul_finfo(const char *path, const char *fname,int *nline,int *nword,int *nchar);

/**
 *
 */
int ul_nullfile(const char *path, const char *fname);

/**
 *
 */
int ul_cpfile(const char *path, const char *fsrc, const char *fdest);

/**
 *
 */
int ul_appfile(const char *path, const char *fsrc, const char *fdest);

/**
 *
 */
int ul_mvfile(const char *path, const char *fsrc, const char *fdest);

/**
 *
 */
ssize_t ul_readfile(const char *path, const char *fname, void *buf, int size);

/**
 *
 */
ssize_t ul_writefile(const char *path, const char *fname, void *buf, int size);
 
/**
 *
 */
int ul_exchangedir(const char *path, const char *sname, const char *dname);

#endif // __UL_FILE_H_
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
