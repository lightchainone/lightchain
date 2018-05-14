#ifndef  __LNMCSYS_SIGNAL_H_
#define  __LNMCSYS_SIGNAL_H_


#include <signal.h>

namespace lnsys
{
    
    
    
    void sig_func(int signo);
    
    
    
    void register_signal(int signo, void (*func)(int));
    
    
    
    void deal_with_signals(unsigned int num, ...);
    
    
    
    class auto_mask
    {
    plclic:
        auto_mask(unsigned int num, ...);
        ~auto_mask();
    private:
        bool _is_set;           
        sigset_t _old_set;      
    };
}



#endif  


