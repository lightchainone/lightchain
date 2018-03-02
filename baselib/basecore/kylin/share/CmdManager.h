#ifndef _CMD_MANAGER_H_
#define _CMD_MANAGER_H_

#include <map>
#include "utils.h"

class CCmdMgr
{
    typedef int (*DO_CUI_COMMAND)(int argc, char* argv[]);
    struct CmdProc {
        DO_CUI_COMMAND f;
        const char* pUsage;
    };
    typedef std::map<char*, CmdProc, CCaseCmp> CmdMap;
    CmdMap m_CmdMap;

plclic:
    CCmdMgr() {
    }
    ~CCmdMgr() {}

    void Register(char* pName, DO_CUI_COMMAND f, const char *pUsage) {
        CmdProc cp = { f, pUsage };
        m_CmdMap.insert(CmdMap::value_type(pName, cp));
    }

    int Invoke(char* pName, int argc, char* argv[]) {
        DO_CUI_COMMAND f;
        char *p, *p2;
        const char *pUsage;

        CmdMap::iterator it = m_CmdMap.lower_bound(pName);
        if (it != m_CmdMap.end()) {
            p = it->first;
            f = it->second.f;
            pUsage = it->second.pUsage;

            if (strstr(p, pName) == p) {
                if (strcmp(p, pName) != 0) {
                    fprintf(stderr, "%s\n", p);

                    it ++;
                    if (it != m_CmdMap.end()) {
                        p2 = it->first;
                        if (strstr(p2, pName) == p2) {
                            fprintf(stderr, "%s?\n", p2);
                            return EINVAL;                            ;
                        }
                    }
                }
                int ret = f(argc, argv);
                if (EINVAL == ret) {
                    fprintf(stderr, "Usage: %s\n", pUsage);
                }
                return ret;
            }
        }
        fprintf(stderr, "Error: unknown command\n");
        return EINVAL;
    }

    void PrintAll() {
        CmdMap::iterator it;
        for (it=m_CmdMap.begin(); it!=m_CmdMap.end(); it++) {
            printf("%8s: %s\n", it->first, it->second.pUsage);
        }
    }

    void RunInteractive() {
        char cmd[4096], *p[64];
        int n;
        printf("Please input command:\n");
        for (;;) {
            printf("> ");
            fgets(cmd, 4096, stdin);

            n = SplitString(cmd, " \t\n", p, 64, true);
            if (n >= 1) {
                Invoke(p[0], n-1, &p[1]);
            }
            else {
                printf("unknown command, try help\n");
            }
        }
    }
};

#endif
