#include <pthread.h>
#include <stdarg.h>

#include "lnmcsys_signal.h"
#include "lnmcsys_mgr.h"
#include "lnmcsys_exception.h"

namespace lnsys
{
    
    
    
    void sig_func(int signo)
    {
        if(SIGHUP == signo || SIGINT == signo ||
            SIGQUIT == signo || SIGTERM == signo)
        {
            TRACE("signal %s caught, program is going to stop.", strsignal(signo));
            lnsys::LnmcsysManager::get_lnmcsys_chain()->stop();
        }
        else
        {
            TRACE("signal %s caught, program do nothing as designed.", strsignal(signo));
        }
    }

    
    
    
    void lnmcsys_signal(int signo, void (*func)(int))
    {
        struct sigaction sa;
        memset(&sa, 0x00, sizeof(struct sigaction));
        sa.sa_handler = func;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags |= SA_RESTART;
        if(0 != sigaction(signo, &sa, NULL))
        {
            THROW_SYS("set signal %s's resonse funciton failed [%m].", strsignal(signo));
        }
        TRACE("set signal %s's resonse funciton succ.", strsignal(signo)); 
    }

    
    
    
    void deal_with_signals(unsigned int num, ...)
    {
        va_list p;
        va_start(p, num);
        for(unsigned int i = 0; i < num; i++)
        {
            int signo = va_arg(p, int);
            if(SIGPIPE == signo)
            {
                lnmcsys_signal(signo, SIG_IGN);
                continue;
            }
            lnmcsys_signal(signo, sig_func);
        }
        va_end(p);
    }

    
    
    
    auto_mask::auto_mask(unsigned int num, ...)
    {
        va_list p;
        va_start(p, num);
        _is_set = false;
        sigset_t set;
        sigemptyset(&set);
        for(unsigned int i = 0; i < num ; i++)
        {
            int signo = va_arg(p, int);
            if(0 != sigaddset(&set, signo))
            {
                
                WARNING("add signo to set failed [signal: %s, error: %s].", strsignal(signo), strerror(errno));
            }
        }

        if(0 != pthread_sigmask(SIG_BLOCK, &set, &_old_set))
        {
            THROW_SYS("set thread mask failed [error: %m].");
        }
        _is_set = true;
        TRACE("%s", "set thread mask succ.");
        va_end(p);
    }

    
    
    
    auto_mask::~auto_mask()
    {
        do
        {
            if(_is_set)
            {
                if(0 != pthread_sigmask(SIG_SETMASK, &_old_set, NULL))
                {
                    WARNING("restore thread mask failed [error: %s].", strerror(errno));
                    break;
                }
                _is_set = false;
                TRACE("%s", "restore thread mask succ.");
            }
        }while(false);
    }
}
























