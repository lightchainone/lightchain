#include "ul_conf.h"

int
ul_readnum_oneuint(const char *work_path, const char *fname, char *vname, u_int *pvalue)
{
	int ret;
	char fnum[PATH_SIZE];
	ul_confdata_t *tconf;
	sprintf(fnum, "%s.n", fname);
	tconf = ul_initconf(1024);
	if (ul_readconf(work_path, fnum, tconf) == -1) {
		ul_freeconf(tconf);
		return 0;
	}
	ret = ul_getconfuint(tconf, vname, pvalue);
	ul_freeconf(tconf);
	return ret;
}


int
ul_readnum_oneint64(const char *work_path, const char *fname, char *vname, long long *pvalue)
{
	int ret;
	char fnum[PATH_SIZE];
	ul_confdata_t *tconf;
	sprintf(fnum, "%s.n", fname);
	tconf = ul_initconf(1024);
	if (ul_readconf(work_path, fnum, tconf) == -1) {
		ul_freeconf(tconf);
		return 0;
	}
	ret = ul_getconfint64(tconf, vname, pvalue);
	ul_freeconf(tconf);
	return ret;
}


int
ul_readnum_oneint(const char *work_path, const char *fname, char *vname, int *pvalue)
{
	int ret;
	char fnum[PATH_SIZE];
	ul_confdata_t *tconf;

	sprintf(fnum, "%s.n", fname);

	tconf = ul_initconf(1024);

	if (ul_readconf(work_path, fnum, tconf) == -1) {
		ul_freeconf(tconf);
		return 0;
	}

	ret = ul_getconfint(tconf, vname, pvalue);
	ul_freeconf(tconf);

	return ret;
}





int
ul_readnum_oneuint64(const char *work_path, const char *fname, char *vname, unsigned long long *pvalue)
{
	int ret;
	char fnum[PATH_SIZE];
	ul_confdata_t *tconf;
	sprintf(fnum, "%s.n", fname);
	tconf = ul_initconf(1024);
	if (ul_readconf(work_path, fnum, tconf) == -1) {
		ul_freeconf(tconf);
		return 0;
	}
	ret = ul_getconfuint64(tconf, vname, pvalue);
	ul_freeconf(tconf);
	return ret;
}

int
ul_getconfstr(ul_confdata_t * pd_conf, char *c_name, char *c_value)
{
	if (pd_conf == NULL || c_name == NULL || c_value == NULL) {
		return 0;
	}
	c_value[0] = '\0';

	for (int i = 0; i < pd_conf->num; i++) {
		if (strcmp((pd_conf->item[i]).name, c_name) == 0) {
			strcpy(c_value, (pd_conf->item[i]).value);
			return 1;
		}
	}

	return 0;
}






/* vim: set ts=4 sw=4 sts=4 tw=100 */
