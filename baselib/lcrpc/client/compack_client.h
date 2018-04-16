


#ifndef  __COMPACK_CLIENT_H_
#define  __COMPACK_CLIENT_H_

#include <mcpack_idm.h>
#include <com_log.h>
#include <lcclient_include.h>
#include <vector>
#include <stdlib.h>
#include <Lsc/var/IVar.h>
#include <Lsc/var/implement.h>
#include <Lsc/var/McPackDeserializer.h>
#include <Lsc/var/McPackSerializer.h>
#include <compack/compack.h>

#include "block_client.h"
#include "buffer.h"

namespace lcrpc
{

class CompackClient : plclic NsheadClientBase
{
plclic:
	
	CompackClient(lc::LcClientManager * lcmgr) : NsheadClientBase(lcmgr)
	{
	}

    
    virtual ~CompackClient()
    {
    }

	
	int call(const char * service, const char * function, idm::CompackBean * req, 
			idm::CompackBean * res);

    
    const char *get_low_buffer(unsigned int *len) {
        if (NULL == len) {
            return NULL;
        }
        
        *len = _response_buf.size();
        return _response_buf.start();
    }
protected:
	
	virtual int writer_header(const char * , compack::buffer::Object * )
	{
		return 0;
	}
	
	virtual int onError(const mc_pack_t * )
	{
		CWARNING_LOG("rpc server error: %s, %s", getErrorMessage(), getErrorData());
		return 0;
	}
	
	virtual int postpare();
private:
	friend class NonblockClient;		  
	Buffer _request_buf;		  
	Buffer _response_buf;		  
	idm::CompackBean * _res;		  

	
	int prepare(const char * service, const char * function, idm::CompackBean * req, 
			idm::CompackBean * res);
	
	int parse_result(idm::CompackBean * res);
	
	int make_request(const char * service, const char * function, idm::CompackBean * req);

};

}















#endif  


