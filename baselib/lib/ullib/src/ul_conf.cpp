#include "ul_def.h"
#include "ul_conf.h"
#include "ul_log.h"
#include "ul_func.h"
#include "ul_error.h"
#include "ul_string.h"

static const int LIMIT_CONF_ITEM_NUM = 1024;

enum {
    ULTYPE_INT,
    ULTYPE_UINT,
    ULTYPE_INT64,
    ULTYPE_UINT64,
    ULTYPE_STRING,
    ULTYPE_FLOAT,
};

int readconf_no_dir(const char *fullpath, ul_confdata_t * pd_conf, int start);

ul_confdata_t *
ul_initconf(int num) {
    ul_confdata_t *pc = NULL;

    pc = (ul_confdata_t *) ul_calloc(1, sizeof(ul_confdata_t));

    if (pc == NULL) {
        ul_seterrno(UL_ALLOC_ERR);
        return NULL;
    }

    pc->size = num;
    if (pc->size < LIMIT_CONF_ITEM_NUM) {
        pc->size = LIMIT_CONF_ITEM_NUM;
    }

    pc->item = (ul_confitem_t *) ul_calloc((size_t)pc->size, sizeof(ul_confitem_t));

    if (pc->item == NULL) {
        ul_seterrno(UL_ALLOC_ERR);
        free(pc);
        return NULL;
    }

    return pc;
}

int
ul_freeconf(ul_confdata_t * pd_conf) {
    if (pd_conf == NULL) {
        //ul_seterrno(UL_INVALID_PARAM);
        return 1;
    }
    if (pd_conf->item) {
        ul_free(pd_conf->item);
    }
    ul_free(pd_conf);
    return 1;
}

int escape_ignore(const char *line, int pos) {
    while (line[pos] != 0 && line[pos] == ' ') {
        ++ pos;
    }
    return pos;
}

bool abs_path(const char *file) {
    return (file[0] == '/') || (strncmp(file, "~/", 2) == 0);
}

#define INCLUDE_FLAG "$include"
#define INC_START '"'
#define INC_END '"'

int
ul_readconf_include(const char *path, ul_confdata_t *pd_conf) {
    int ret = 1;

    char incfile[LINE_SIZE];
    int incptr = 0;
    char inconf[PATH_SIZE * 2];

    int itemnum = pd_conf->num;

    for (int i=0; i<itemnum; ++i) {
        if (strcasecmp(pd_conf->item[i].name, INCLUDE_FLAG) == 0) {
            char *line = pd_conf->item[i].value;
            int pos = 0;
            pos = escape_ignore(line, pos);
            if (line[pos] == INC_START) {
                ++ pos;
                pos = escape_ignore(line, pos);
                incptr = 0;
                //line大小限制导致incfile数组足够存内容，因此不需要判断它的上下界
                while (line[pos] != 0 && line[pos] != INC_END) {
                    incfile[incptr++] = line[pos++];
                }
                incfile[incptr] = 0;
                if (line[pos] == INC_END) {
                    if (abs_path(incfile)) {
                        //绝对路径
                        if (readconf_no_dir(incfile, pd_conf, pd_conf->num) != 1) {
                            ul_writelog(UL_LOG_WARNING, "[readconf] error: can't read conf[%s]", incfile);
                        }
                    } else {    //代表相对路径
                        snprintf(inconf, sizeof(inconf), "%s/%s", path, incfile);
                        if (readconf_no_dir(inconf, pd_conf, pd_conf->num) != 1) {
                            ul_writelog(UL_LOG_WARNING, "[readconf] error: can't read conf[%s]", inconf);
                        }
                    }
                } else {
                }
            } else {
                continue;
            }
        }
    }

    return ret;
}

int
ul_readconf_no_dir(const char *fullpath, ul_confdata_t *pd_conf) {
    return readconf_no_dir(fullpath, pd_conf, 0);
}

int
readconf_no_dir(const char *fullpath, ul_confdata_t * pd_conf, int start) {
    struct stat info;
    FILE *pf;
    char inf_str[LINE_SIZE] = "";
    int item_num = start;

    int line_num = 0;
    int ret = -1;

    /* check if path can read */
    if (0 != ul_stat(fullpath, &info) ||
        !S_ISREG(info.st_mode)) {
        ul_writelog(UL_LOG_FATAL, "[readconf] file (%s) not found or not linked to a regular file", fullpath);
        return -1;
    }

    /* open file */
    pf = ul_fopen(fullpath, "r");
    if (pf == NULL) {
        ul_writelog(UL_LOG_FATAL, "[readconf] can't open file (%s)", fullpath);
        return -1;
    }

    /* read conf item */
    char ch = 0;
    int status = 0;
    int sep_sign = 0;
    int i = 0;
    int j = 0;
    int slclen = 0;
    char str_tmp1[WORD_SIZE] = "";
    char str_tmp2[WORD_SIZE] = "";

    while (fgets(inf_str, LINE_SIZE - 1, pf) != NULL) {
        if (item_num >= pd_conf->size) {
            ret = -1;
            goto end;
        }
        line_num++;

        /* explain line */
        if (inf_str[0] == '#') {
            continue;
        }

        /* analysis line */
        slclen = (int)strlen(inf_str);
        if (slclen <= 0) {
            continue;
        }

        status = 0;
        sep_sign = 0;
        for (i = 0; i <= slclen && status != 4 && status != 5; i++) {
            ch = inf_str[i];
            switch (status) {
                case 0: {       //去第一个字段的前空格
                    if (ch == SPACE || ch == TAB) {
                        break;  //空格继续
                    } else if (ch == '\r' || ch == '\n') {
                        status = 6; //空行
                        break;
                    } else if (ch == ':') {
                        status = 5; //错误退出
                        break;
                    }
                    j = 0;
                    str_tmp1[j++] = ch;
                    status = 1;
                    break;
                }
                case 1: {       //接收name
                    if (ch == SPACE || ch == TAB || ch == ':') { //遇到name后面第一个无效字符
                        if (ch == ':') {
                            sep_sign = 1;
                        }
                        str_tmp1[j] = '\0';
                        status = 2;
                        break;
                    } else if (ch == '\r' || ch == '\n') {  //非法的结束
                        status = 5;
                        break;
                    }
                    str_tmp1[j++] = ch;
                    if(j>=WORD_SIZE){
                        goto end;
                    }
                    break;
                }
                case 2: {       //去掉value前面的多余字符
                    if (ch == SPACE || ch == TAB || ch == ':') {
                        if (ch == ':' && sep_sign == 0) {
                            sep_sign = 1;
                        } else if (ch == ':') { //第二次遇到冒号，认为是内容的一部分
                            j = 0;
                            status = 3;
                        }

                        break;
                    } else if (ch == '\r' || ch == '\n') {
                        str_tmp2[0] = '\0';
                        status = 4;
                        break;
                    }
                    j = 0;
                    status = 3;
                    break;
                }
                case 3: {
                    /*不再进行对冒号的检查
                     * if(NULL != strchr( &inf_str[i], ':'))//value中的冒号检测
                     * {
                     * status = 5;
                     * break;
                     * } */

                    j = slclen - 1; //去除尾部无用字段

                    while ( (j>=0) && (inf_str[j] == SPACE || inf_str[j] == TAB || inf_str[j] == '\r'
                                       || inf_str[j] == '\n' || inf_str[j] == '\0') ) {
                        inf_str[j--] = '\0';
                    }
                    if (i>=1 && (j >= i - 1) && (j - i + 2 < WORD_SIZE)) {
                        strncpy(str_tmp2, &inf_str[i - 1], WORD_SIZE - 1);
                        str_tmp2[WORD_SIZE - 1] = '\0';
                        status = 4;
                        break;
                    }
                    status = 5;
                    break;
                }
                default: {
                    break;
                }
            }
        }
        if (status == 4 && sep_sign == 1) {

            snprintf(pd_conf->item[item_num].name, sizeof(pd_conf->item[item_num].name), "%s", str_tmp1);
            snprintf(pd_conf->item[item_num].value, sizeof(pd_conf->item[item_num].value), "%s", str_tmp2);
            item_num++;
            continue;
        }
        if (status == 6) {
            continue;           //空行继续
        }
        /* error line */
        ul_writelog(UL_LOG_FATAL, "[readconf] line (%d) in conf file (%s) error", line_num, fullpath);
    }

    /* give the real number of conf */
    pd_conf->num = item_num;
    ret = 1;

    /* close conf file */
end:
    ul_fclose(pf);
    return ret;
}

int
ul_readconf_ex(const char *work_path, const char *fname, ul_confdata_t * pd_conf) {

    if (work_path == NULL || fname == NULL || pd_conf == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return -1;
    }

    char totalname[LINE_SIZE];
    snprintf(totalname, sizeof(totalname), "%s/%s", work_path, fname);
    int ret = ul_readconf_no_dir(totalname, pd_conf);
    if (ret != 1) {
        return ret;
    }
    return ul_readconf_include(work_path, pd_conf);
}

int
ul_readconf(const char *work_path, const char *fname, ul_confdata_t * pd_conf) {

    if (work_path == NULL || fname == NULL || pd_conf == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return -1;
    }

    char totalname[LINE_SIZE];
    snprintf(totalname, sizeof(totalname), "%s/%s", work_path, fname);
    return ul_readconf_no_dir(totalname, pd_conf);
}

int
ul_writeconf(const char *work_path, const char *fname, ul_confdata_t * pd_conf) {
    FILE *pf = NULL;
    int i = 0;
    char tstra[WORD_SIZE], tstrb[WORD_SIZE];
    char totalname[LINE_SIZE];

    /* check pd_conf */
    if (work_path == NULL || fname == NULL || pd_conf == NULL) {
        //ul_seterrno(UL_INVALID_PARAM);
        ul_writelog(UL_LOG_FATAL, "[writeconf] invalid param");
        return -1;
    }
    if ((pd_conf->num > pd_conf->size) || (pd_conf->num <= 0)) {
        ul_writelog(UL_LOG_FATAL, "[writeconf] var's num (%d) size(%d) error", pd_conf->num, pd_conf->size);
        return -1;
    }

    /* open conf file */
    snprintf(totalname, sizeof(totalname), "%s/%s", work_path, fname);
    pf = ul_fopen(totalname, "w");
    if (pf == NULL) {
        ul_writelog(UL_LOG_FATAL, "[writeconf] fopen %s fail", totalname);
        //ul_seterrno(UL_OPEN_FILE_ERR);
        return -1;
    }

    /* write */
    for (i = 0; i < pd_conf->num; i++) {
        if (sscanf((pd_conf->item[i]).name, "%s %s", tstra, tstrb) != 1) {
            ul_writelog(UL_LOG_FATAL, "[writeconf] var's name (%s) error", (pd_conf->item[i]).name);
            continue;
        }
        fprintf(pf, "%s : %s\n", (pd_conf->item[i]).name, (pd_conf->item[i]).value);
    }

    /* close file */
    ul_fclose(pf);
    return 1;
}

int
ul_getconfnstr(ul_confdata_t * pd_conf, const char *c_name, char *c_value, const size_t n) {
    if (c_value == NULL || c_name == NULL || pd_conf == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return 0;
    }
    c_value[0] = '\0';

    for (int i = 0; i < pd_conf->num; i++) {
        if (strcmp((pd_conf->item[i]).name, c_name) == 0) {
            ul_strlcpy(c_value, (pd_conf->item[i]).value, n);
            return 1;
        }
    }

    return 0;

}

int
ul_modifyconfstr(ul_confdata_t * pd_conf, char *c_name, char *c_value) {

    if (pd_conf == NULL || c_name == NULL || c_value == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return -1;
    }
    if (strlen(c_value) >= WORD_SIZE) {
        ul_seterrno(UL_INVALID_PARAM);
        return -3;
    }

    for (int i = 0; i < pd_conf->num; i++) {
        if (strcmp((pd_conf->item[i]).name, c_name) == 0) {
            strncpy((pd_conf->item[i]).value, c_value, WORD_SIZE);
            return 1;
        }
    }

    return 0;
}

static int
addconf(ul_confdata_t * pd_conf, char *c_name, void *c_value, int type) {
    if (pd_conf == NULL || c_name == NULL || c_value == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return -4;
    }

    if (strlen(c_name) >= WORD_SIZE) {
        ul_seterrno(UL_INVALID_PARAM);
        return -1;
    }
    if (pd_conf->num >= pd_conf->size) {
        ul_seterrno(UL_DETAIL_ERR);
        ul_seterrbuf("can't add any more");
        return -2;
    }

    for (int i = 0; i < pd_conf->num; i++) {
        if (strcmp((pd_conf->item[i]).name, c_name) == 0) {
            return 0;
        }
    }

    strncpy((pd_conf->item[pd_conf->num]).name, c_name, WORD_SIZE);
    switch (type) {
        case ULTYPE_STRING:
            if (strlen((char *) c_value) >= WORD_SIZE) {
                ul_seterrno(UL_INVALID_PARAM);
                return -3;
            }
            strncpy((pd_conf->item[pd_conf->num]).value, (char *) c_value, WORD_SIZE);
            break;
        case ULTYPE_INT:
            snprintf((pd_conf->item[pd_conf->num]).value, WORD_SIZE, "%d", *((int *) c_value));
            break;
        case ULTYPE_UINT:
            snprintf((pd_conf->item[pd_conf->num]).value, WORD_SIZE, "%u", *((u_int *) c_value));
            break;
        case ULTYPE_INT64:
            snprintf((pd_conf->item[pd_conf->num]).value, WORD_SIZE, "%lld", *((long long *) c_value));
            break;
        case ULTYPE_UINT64:
            snprintf((pd_conf->item[pd_conf->num]).value, WORD_SIZE, "%llu", *((unsigned long long *) c_value));
            break;
        case ULTYPE_FLOAT:
            snprintf((pd_conf->item[pd_conf->num]).value, WORD_SIZE, "%f", *((float *) c_value));
            break;
        default:
            (pd_conf->item[pd_conf->num]).value[0] = 0;
            break;
    }

    pd_conf->num++;

    return 1;
}

int
ul_addconfstr(ul_confdata_t * pd_conf, char *c_name, char *c_value) {
    return addconf(pd_conf, c_name, c_value, ULTYPE_STRING);
}

int
ul_getconfint(ul_confdata_t * pd_conf, char *c_name, int *c_value) {
    char tstr[WORD_SIZE];
    char *pstr = NULL;
    int reti, num;

    reti = ul_getconfnstr(pd_conf, c_name, tstr, sizeof(tstr));
    if (reti == 0)
        return 0;

    if ((tstr[0] == '0') && (tstr[1] == 0)) {
        *c_value = 0;
        return 1;
    }

    if (tstr[0] != '0') {
        reti = sscanf(tstr, "%d", &num);
    } else if (tstr[1] != 'x') {
        pstr = tstr + 1;
        reti = sscanf(pstr, "%o", &num);
    } else {
        pstr = tstr + 2;
        reti = sscanf(pstr, "%x", &num);
    }

    if (reti <= 0) {
        return 0;
    }

    *c_value = num;

    return 1;
}


int
ul_modifyconfint(ul_confdata_t * pd_conf, char *c_name, int c_value) {
    if (pd_conf == NULL || c_name == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return 0;
    }
    for (int i = 0; i < pd_conf->num; i++) {
        if (strcmp((pd_conf->item[i]).name, c_name) == 0) {
            snprintf((pd_conf->item[i]).value, WORD_SIZE, "%d", c_value);
            return 1;
        }
    }

    return 0;
}

int
ul_addconfint(ul_confdata_t * pd_conf, char *c_name, int c_value) {
    return addconf(pd_conf, c_name, &c_value, ULTYPE_INT);
}


int
ul_getconffloat(ul_confdata_t * pd_conf, char *c_name, float *c_value) {
    char tstr[WORD_SIZE];
    int reti;

    reti = ul_getconfnstr(pd_conf, c_name, tstr, sizeof(tstr));
    if (reti == 0) {
        return 0;
    }

    *c_value = (float)atof(tstr);

    return 1;
}


int
ul_modifyconffloat(ul_confdata_t * pd_conf, char *c_name, float c_value) {
    if (pd_conf == NULL || c_name == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return 0;
    }
    for (int i = 0; i < pd_conf->num; i++) {
        if (strcmp((pd_conf->item[i]).name, c_name) == 0) {
            snprintf((pd_conf->item[i]).value, WORD_SIZE, "%f", c_value);
            return 1;
        }
    }

    return 0;
}

int
ul_addconffloat(ul_confdata_t * pd_conf, char *c_name, float c_value) {
    return addconf(pd_conf, c_name, &c_value, ULTYPE_FLOAT);
}

int
ul_getconfuint(ul_confdata_t * pd_conf, char *c_name, u_int *c_value) {
    char tstr[WORD_SIZE];
    char *pstr = NULL;
    int reti = 0;
    u_int num = 0;

    reti = ul_getconfnstr(pd_conf, c_name, tstr, sizeof(tstr));
    if (reti == 0)
        return 0;

    if ((tstr[0] == '0') && (tstr[1] == '\0')) {
        *c_value = 0;
        return 1;
    }

    if (tstr[0] != '0') {
        reti = sscanf(tstr, "%u", &num);
    } else if (tstr[1] != 'x') {
        pstr = tstr + 1;
        reti = sscanf(pstr, "%o", &num);
    } else {
        pstr = tstr + 2;
        reti = sscanf(pstr, "%x", &num);
    }

    if (reti <= 0) {
        return 0;
    }

    *c_value = num;

    return 1;
}


int
ul_modifyconfuint(ul_confdata_t * pd_conf, char *c_name, u_int c_value) {

    if (pd_conf == NULL || c_name == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return 0;
    }

    for (int i = 0; i < pd_conf->num; i++) {
        if (strcmp((pd_conf->item[i]).name, c_name) == 0) {
            snprintf((pd_conf->item[i]).value, WORD_SIZE, "%u", c_value);
            return 1;
        }
    }

    return 0;
}

int
ul_addconfuint(ul_confdata_t * pd_conf, char *c_name, u_int c_value) {
    return addconf(pd_conf, c_name, &c_value, ULTYPE_UINT);
}

int
ul_getconfint64(ul_confdata_t * pd_conf, char *c_name, long long *c_value) {
    char tstr[WORD_SIZE];
    char *pstr = NULL;
    int reti = 0;
    long long num = 0;
    reti = ul_getconfnstr(pd_conf, c_name, tstr, sizeof(tstr));
    if (reti == 0)
        return 0;

    if ((tstr[0] == '0') && (tstr[1] == '\0')) {
        *c_value = 0;
        return 1;
    }

    if (tstr[0] != '0') {
        reti = sscanf(tstr, "%lld", &num);
    } else if (tstr[1] != 'x') {
        pstr = tstr + 1;
        reti = sscanf(pstr, "%llo", &num);
    } else {
        pstr = tstr + 2;
        reti = sscanf(pstr, "%llx", &num);
    }

    if (reti <= 0) {
        return 0;
    }

    *c_value = num;

    return 1;
}


int
ul_modifyconfint64(ul_confdata_t * pd_conf, char *c_name, long long c_value) {
    if (pd_conf == NULL || c_name == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return 0;
    }

    for (int i = 0; i < pd_conf->num; i++) {
        if (strcmp((pd_conf->item[i]).name, c_name) == 0) {
            snprintf((pd_conf->item[i]).value, WORD_SIZE, "%lld", c_value);
            return 1;
        }
    }

    return 0;
}

int
ul_addconfint64(ul_confdata_t * pd_conf, char *c_name, long long c_value) {
    return addconf(pd_conf, c_name, &c_value, ULTYPE_INT64);
}


int
ul_getconfuint64(ul_confdata_t * pd_conf, char *c_name, unsigned long long *c_value) {
    char tstr[WORD_SIZE];
    char *pstr;
    int reti;
    unsigned long long num;
    reti = ul_getconfnstr(pd_conf, c_name, tstr, sizeof(tstr));
    if (reti == 0)
        return 0;

    if ((tstr[0] == '0') && (tstr[1] == '\0')) {
        *c_value = 0;
        return 1;
    }

    if (tstr[0] != '0') {
        reti = sscanf(tstr, "%llu", &num);
    } else if (tstr[1] != 'x') {
        pstr = tstr + 1;
        reti = sscanf(pstr, "%llo", &num);
    } else {
        pstr = tstr + 2;
        reti = sscanf(pstr, "%llx", &num);
    }

    if (reti <= 0) {
        return 0;
    }

    *c_value = num;

    return 1;
}


int
ul_modifyconfuint64(ul_confdata_t * pd_conf, char *c_name, unsigned long long c_value) {
    if (pd_conf == NULL || c_name == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return 0;
    }

    for (int i = 0; i < pd_conf->num; i++) {
        if (strcmp((pd_conf->item[i]).name, c_name) == 0) {
            snprintf((pd_conf->item[i]).value, WORD_SIZE, "%llu", c_value);
            return 1;
        }
    }

    return 0;
}

int
ul_addconfuint64(ul_confdata_t * pd_conf, char *c_name, unsigned long long c_value) {
    return addconf(pd_conf, c_name, &c_value, ULTYPE_UINT64);
}


static int
kreadnum_one(const char *work_path, const char *fname, char *vname, void *pvalue, int type) {
    char fnum[PATH_SIZE];
    int reti = 0;
    ul_confdata_t *tconf;

    snprintf(fnum, sizeof(fnum), "%s.n", fname);

    tconf = ul_initconf(LIMIT_CONF_ITEM_NUM);

    if (ul_readconf(work_path, fnum, tconf) == -1) {
        ul_freeconf(tconf);
        return -1;
    }

    switch (type) {
        case ULTYPE_INT:
            reti = ul_getconfint(tconf, vname, (int *) pvalue);
            break;
        case ULTYPE_UINT:
            reti = ul_getconfuint(tconf, vname, (u_int *) pvalue);
            break;
        case ULTYPE_INT64:
            reti = ul_getconfint64(tconf, vname, (long long *) pvalue);
            break;
        case ULTYPE_UINT64:
            reti = ul_getconfuint64(tconf, vname, (unsigned long long *) pvalue);
            break;
        case ULTYPE_FLOAT:
            reti = ul_getconffloat(tconf, vname, (float *) pvalue);
            break;
    }

    ul_freeconf(tconf);

    if (reti) {
        return reti;
    } else {
        ul_writelog(UL_LOG_FATAL, "[kreadnum_one] have no (%s).", vname);
        return -1;
    }
}


int
ul_kreadnum_oneint(const char *work_path, const char *fname, char *vname, int *pvalue) {
    return kreadnum_one(work_path, fname, vname, (void *) pvalue, ULTYPE_INT);
}


int
ul_kreadnum_onefloat(const char *work_path, const char *fname, char *vname, float *pvalue) {
    return kreadnum_one(work_path, fname, vname, (void *) pvalue, ULTYPE_FLOAT);
}


int
ul_writenum_init(const char *work_path, const char *fname) {
    FILE *pf;
    char totalname[LINE_SIZE];

    snprintf(totalname, sizeof(totalname), "%s/%s.n", work_path, fname);
    pf = ul_fopen(totalname, "w");
    if (pf == NULL) {
        return -1;
    }
    ul_fclose(pf);
    return 1;
}

static int
writenum_one(const char *work_path, const char *fname, const char *vname, void *value, int type) {
    FILE *pf = NULL;
    char tstra[WORD_SIZE], tstrb[WORD_SIZE];
    char totalname[LINE_SIZE];

    if (work_path == NULL || fname == NULL || vname == NULL || value == NULL) {
        ul_seterrno(UL_INVALID_PARAM);
        return -3;
    }
    /* open num file */
    snprintf(totalname, sizeof(totalname), "%s/%s.n", work_path, fname);
    pf = ul_fopen(totalname, "a");
    if (pf == NULL) {
        ul_seterrno(UL_OPEN_FILE_ERR);
        return -1;
    }

    /* write */
    if (sscanf(vname, "%s %s", tstra, tstrb) != 1) {
        ul_writelog(UL_LOG_FATAL, "[writeconf] var's name (%s) error", vname);
        return -2;
    }
    switch (type) {
        case ULTYPE_INT:
            fprintf(pf, "%s : %d\n", vname, *((int *) value));
            break;
        case ULTYPE_UINT:
            fprintf(pf, "%s : %u\n", vname, *((u_int *) value));
            break;
        case ULTYPE_INT64:
            fprintf(pf, "%s : %lld\n", vname, *((long long *) value));
            break;
        case ULTYPE_UINT64:
            fprintf(pf, "%s : %llu\n", vname, *((unsigned long long *) value));
            break;
        case ULTYPE_FLOAT:
            fprintf(pf, "%s : %f\n", vname, *((float *) value));
            break;
        default:
            break;
    }

    /* close file */
    ul_fclose(pf);
    return 1;
}

int
ul_writenum_oneint(const char *work_path, const char *fname, const char *vname, int value) {
    return writenum_one(work_path, fname, vname, &value, ULTYPE_INT);
}

int
ul_writenum_onefloat(const char *work_path, const char *fname, const char *vname, float value) {
    return writenum_one(work_path, fname, vname, &value, ULTYPE_FLOAT);
}


int
ul_kreadnum_oneuint(const char *work_path, const char *fname, char *vname, u_int *pvalue) {
    return kreadnum_one(work_path, fname, vname, pvalue, ULTYPE_UINT);
}

int
ul_writenum_oneuint(const char *work_path, const char *fname, const char *vname, u_int value) {
    return writenum_one(work_path, fname, vname, &value, ULTYPE_UINT);
}


int
ul_kreadnum_oneint64(const char *work_path, const char *fname, char *vname, long long *pvalue) {
    return kreadnum_one(work_path, fname, vname, pvalue, ULTYPE_INT64);
}

int
ul_writenum_oneint64(const char *work_path, const char *fname, const char *vname, long long value) {
    return writenum_one(work_path, fname, vname, &value, ULTYPE_INT64);
}



int
ul_kreadnum_oneuint64(const char *work_path, const char *fname, char *vname,
                      unsigned long long *pvalue) {
    return kreadnum_one(work_path, fname, vname, pvalue, ULTYPE_UINT64);
}

int
ul_writenum_oneuint64(const char *work_path, const char *fname, const char *vname,
                      unsigned long long value) {
    return writenum_one(work_path, fname, vname, &value, ULTYPE_UINT64);
}


/*
** command argument
*/
ul_argu_stru_t *
argument_init() {
    ul_argu_stru_t *argu_struct = NULL;
    argu_struct = (ul_argu_stru_t *) calloc(1, sizeof(ul_argu_stru_t));
    return argu_struct;
}

void
argument_destroy(ul_argu_stru_t * argument_struct) {
    free(argument_struct);
}

int
read_argument(ul_argu_stru_t * argument_struct, int argc, char **argv) {
    int option = 0;
    int m_option = 0;
    int opt_num = 0;
    int arg_num = 0;
    int mopt_num = 0;
    char ch;
    while (--argc) {
        if (**(++argv) == '-') {
            m_option = 0;
            option = 0;
            while ((ch = *(++*argv)) != 0) {
                if (ch == '-') {
                    strncpy(argument_struct->m_option[mopt_num++].option, *argv + 1, MAX_MUTI_OPT);
                    if (strlen(argument_struct->m_option[mopt_num - 1].option) == 0)
                        m_option = 0;
                    else {
                        argument_struct->mopt_num++;
                        m_option++;
                    }
                    break;
                } else {
                    option++;
                    argument_struct->opt_num++;
                    argument_struct->option[opt_num++].option = *(*argv);
                    if (opt_num >= MAX_OPTION_NUM) {
                        fprintf(stdout,
                                "ARG_ERROR: two many option parameters %d have been inputed.\n",
                                opt_num);
                        return -1;
                    }
                }
            }
        } else {
            if (m_option == 1) {
                if (mopt_num > 0) {
                    ul_strlcpy(argument_struct->m_option[mopt_num - 1].opt_cont, (*argv)++,
                               MAX_OPT_CONTENT);
                }
                m_option = 0;
            } else {
                if (option == 1) {
                    if (opt_num > 0) {
                        ul_strlcpy(argument_struct->option[opt_num - 1].opt_cont, (*argv)++,
                                   MAX_OPT_CONTENT);
                    } else {
                        //基本参数
                        argument_struct->arg_num++;
                        strncpy(argument_struct->b_arg[arg_num++].b_argument, (*argv)++,
                                MAX_BASE_OPTLEN);
                        if (arg_num >= MAX_BASE_OPTNUM) {
                            fprintf(stdout,
                                    "ARG_ERROR: two many basic parameters %d have been inputed.\n",
                                    arg_num);
                            return -1;
                        }
                    }
                    option = 0;
                }
            }
        }
    }

#ifdef CHK_CONF_HOME
    int in_config = 0;
    int in_homepath = 0;

    for (int i = 0; i < argument_struct->opt_num; i++) {
        if ((argument_struct->option[i].option == 'c')
            && strlen(argument_struct->option[i].opt_cont))
            in_config = 1;
        else if ((argument_struct->option[i].option == 'd')
                 && strlen(argument_struct->option[i].opt_cont))
            in_homepath = 1;
    }
    if (in_config == 0) {
        fprintf(stdout, "ARG_ERROR: -c <configure> argument should be specified.\n");
        return -1;
    }
    if (in_homepath == 0) {
        fprintf(stdout, "ARG_ERROR: -d < home path > argument should be specified.\n");
        return -1;
    }
#endif

    return 0;
}

// return value :
//     0 : not find the option or buffer was too little;
//     > 0: take out success
int
get_one_option(ul_argu_stru_t * argument_struct, char option, char *opt_value, int *opt_size) {
    int ret_val = 0;
    int len = 0;
    for (int i = 0; i < argument_struct->opt_num; i++) {
        if (argument_struct->option[i].option == option) {
            len = (int)strlen(argument_struct->option[i].opt_cont);
            if (opt_value == NULL || opt_size == NULL || len == 0)
                return 1;
            memset(opt_value, 0, (size_t)*opt_size);
            //如果opt_value的大小不够的话.
            if((len+1)>*opt_size){
                ul_strlcpy(opt_value, argument_struct->option[i].opt_cont, (size_t)*opt_size);
                return 0;
            } else {
                //NOTICE(zhangyan04):这里的参数必须为len+1.这样才能够copy len个字节.
                ul_strlcpy(opt_value, argument_struct->option[i].opt_cont, (size_t)(len+1));
                *opt_size = len;
            }
            if (argument_struct->option[i].t_out != 1) {
                argument_struct->cur_opt++;
                argument_struct->option[i].t_out = 1;
            }
            ret_val = len;
            break;
        }
    }

    if (ret_val == 0) {
        if (opt_size != NULL && opt_value != NULL) {
            memset(opt_value, 0, (size_t)*opt_size);
            *opt_size = 0;
        }
    }
    return ret_val;
}

// return value :
//     0 : not find the option or buffer was too little;
//     > 0: take out success
int
get_one_complex_option(ul_argu_stru_t * argument_struct, char *option, char *opt_value, int *opt_size) {
    int ret_val = 0;
    int len = 0;
    for (int i = 0; i < argument_struct->mopt_num; i++) {
        if (strcmp(argument_struct->m_option[i].option, option) == 0) {
            len = (int)strlen(argument_struct->m_option[i].opt_cont);
            if (opt_value == NULL || opt_size == NULL || len == 0)
                return 1;
            memset(opt_value, 0, (size_t)*opt_size);
            //如果opt_value的大小不够的话.
            if ((len+1) > *opt_size) {
                ul_strlcpy(opt_value, argument_struct->m_option[i].opt_cont, (size_t)*opt_size);
                return 0;
            } else {
                //NOTICE(zhangyan04):这里参数必须为len+1,才能够copy len个字节.
                ul_strlcpy(opt_value, argument_struct->m_option[i].opt_cont, (size_t)(len+1));
                *opt_size = len;
            }
            if (argument_struct->m_option[i].t_out != 1) {
                argument_struct->cur_opt++;
                argument_struct->m_option[i].t_out = 1;
            }
            ret_val = len;
            break;
        }
    }

    if (ret_val == 0) {
        if (opt_size != NULL && opt_value != NULL) {
            memset(opt_value, 0, (size_t)*opt_size);
            *opt_size = 0;
        }
    }

    return ret_val;
}

// take out the base parameter
// input: serial : serial of the base option . 0 means the first parameter ;
// return value :
//    0 :  not found or the buffer was little
//   >0 :  find
int
get_base_argument(ul_argu_stru_t * argument_struct, int serial, char *b_argument, int *size) {
    /* Not used Variable, Recommend by Chen Jingkai at 2002:04:18
     * int ret_value = 0 ;
     */
    int len = 0;
    if (serial < 0 || serial > argument_struct->arg_num)
        return 0;
    else {
        memset(b_argument, 0, (size_t)*size);
        len = (int)strlen(argument_struct->b_arg[serial].b_argument);
        if (len == 0 || len >= *size)
            return 0;
        ul_strlcpy(b_argument, argument_struct->b_arg[serial].b_argument, (size_t)*size);
        *size = len;
    }
    return len;
}
