#include <stdio.h>
#include <stdlib.h>
#include "dep/dep_file.h"

int ul_appfile(const char *path, const char *fsrc, const char *fdest);

ssize_t ul_readfile(const char *path, const char *fname, void *buf, int size);


ssize_t ul_writefile(const char *path, const char *fname, void *buf, int size);

int ul_exchangedir(const char *path, const char *sname, const char *dname);


int ul_cpfile(const char *path, const char *fsrc, const char *fdest);

int ul_mvfile(const char *path, const char *fsrc, const char *fdest);

int ul_showgo(char *remark, int goint)
{
	char back_str[]="\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
	printf("%s", back_str);
	printf("%s : %d", remark, goint);
	return fflush(stdout);
}

int ul_showgocmp(char *remark, int base, int goint)
{
	char out_str[512];

	char back_str[]="\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
	printf("%s", back_str);
	if (base==-1){
		printf("%s : %d/unknown", remark, goint);
		snprintf(out_str, 511, "%s : %d/unknown\n", remark, goint);
	}
	else{
		printf("%s : %d/%d", remark, goint, base);
		snprintf(out_str, 511, "%s : %d/%d\n", remark, goint, base);
	}
	return fflush(stdout);

}

int ul_showgoend(char *remark, int base, int goint)
{
	ul_showgocmp(remark, base, goint);
	printf("\n");
	return 0;
}

int ul_appfile(char *path, char *fsrc, char *fdest)
{
	return ul_appfile((const char*)path, (const char*)fsrc, (const char*)fdest);
}

ssize_t ul_readfile(char *path, char *fname, void *buf, int size)
{
	return ul_readfile((const char *)path, (const char *)fname, buf, size);
}

ssize_t ul_writefile(char *path, char *fname, void *buf, int size)
{
	return ul_writefile((const char *)path, (const char *)fname, buf, size);
}

int ul_exchangedir(char *path, char *sname, char *dname)
{
	return ul_exchangedir((const char *)path, (const char *)sname, (const char *)dname);
}

int ul_cpfile(char *path, char *fsrc, char *fdest)
{
	return ul_cpfile((const char *)path, (const char *)fsrc, (const char *)fdest);
}

int ul_mvfile(char *path, char *fsrc, char *fdest)
{
	return ul_mvfile((const char *)path, (const char *)fsrc, (const char *)fdest);
}
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
