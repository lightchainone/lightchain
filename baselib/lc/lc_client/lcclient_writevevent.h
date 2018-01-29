


#ifndef  __LCCLIENT_WRITEVEVENT_H_
#define  __LCCLIENT_WRITEVEVENT_H_

#include <sys/uio.h>
#include <nshead.h>
#include <bsl/deque.h>

#include "lcclient_nshead.h"

namespace lc
{

    class LcClientNsheadWritevEvent : plclic LcClientNsheadEvent
    {
        plclic:
            LcClientNsheadWritevEvent():_iov(NULL), _iov_count(0), _orig_iov(NULL), _orig_iov_count(0)
            {
                _orig_item_len_deque.create();
                _orig_item_base_deque.create();
            }

            virtual ~LcClientNsheadWritevEvent(){}

            int set_iov(iovec *iov, size_t count) {
                if (NULL == iov) {
                    return -1;
                }

                _iov = iov;
                _iov_count = count;
                
                return 0;
            }

            
                
            
        plclic:
            
            virtual void session_begin();

            
            

            
            virtual void write_done_callback();

        protected:
            int parse_iov(int ret);

            int store_iov(iovec *iov, size_t count) {
                if (NULL == iov) {
                    return -1;
                }

                _orig_iov = iov;
                _orig_iov_count = count;

                _orig_item_len_deque.clear();
                _orig_item_base_deque.clear();
                for (size_t i=0; i<count; i++) {
                    _orig_item_len_deque.push_back(iov[i].iov_len);
                    _orig_item_base_deque.push_back(iov[i].iov_base);
                }
                return 0;

            }

            void reset_iov() {
                _iov = _orig_iov;
                _iov_count = _orig_iov_count;

                for (size_t i=0; i<_orig_iov_count; i++) {
                    _iov[i].iov_len = _orig_item_len_deque[i];
                    _iov[i].iov_base = _orig_item_base_deque[i];
                }

                
                _orig_iov = NULL;
                _orig_iov_count = 0;
                _orig_item_len_deque.clear();
                _orig_item_base_deque.clear();
            }

            void process_error(unsigned int st) {
                set_sock_status(st);
                reset_iov();
                session_done();
            }
        protected:
            iovec *_iov;
            size_t _iov_count;
            

            iovec *_orig_iov;
            size_t _orig_iov_count;
            bsl::deque<size_t> _orig_item_len_deque;
            bsl::deque<void *> _orig_item_base_deque;
    };

}












#endif  


