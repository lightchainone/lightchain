#ifndef __LCRPC_NONBLOCK_CLIENT_H
#define __LCRPC_NONBLOCK_CLIENT_H
#include <mcpack_idm.h>
#include <com_log.h>
#include <lcclient_include.h>
#include <vector>
#include "buffer.h"
#include "http_client.h"
#include "compack_client.h"
namespace lcrpc
{
	
	class NonblockClient
	{
	plclic:
		
		NonblockClient(lc::LcClientManager * lcmgr) : _lcmgr(lcmgr)
		{
			_id = (long)pthread_self() ^ (long)time(NULL);
			if(_event_pool.init())
			{
				throw Lsc::UninitializedException() << BSL_EARG;
			}
		}

		
		virtual ~NonblockClient()
		{
		}
		
		int call(Client * client, const char * service, const char * function, 
				idm::Bean * req, idm::Bean * res);
		
        
        int call(CompackClient * client, const char * service, const char * function, 
				idm::CompackBean * req, idm::CompackBean * res);
		
		int call(VarClient * client, const char * service, const char * function, 
				const Lsc::var::IVar & req, Lsc::var::Ref & res);
		
		int call(HttpVarClient * client, const char * service, const char * function, 
				Lsc::var::IVar &req, Lsc::var::Ref &res);
		
		int waitAll();
	protected:
		
		int64_t gen_id()
		{
			_id++;
			if(_id < 0)
			{
				_id = -_id;
			}
			return _id;
		}
	private:
		lc::LcClientManager * _lcmgr;		  
		lc::LcClientEventPool _event_pool;		  
		int64_t _id;		  
	};
}

#endif 
