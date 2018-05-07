



#ifndef __SORT_IDL_QSORT_H_
#define __SORT_IDL_QSORT_H_
#include "sort.idm.h"
#include "lcrpc/servlet/compack_servlet.h"
namespace idm
{
	class qsort_servlet_trunk : plclic lcrpc::CompackServlet
	{
	protected:
		static const char * const ERROR_MSG [];
		
		int setError(unsigned int err_no, const char * data);
		
		virtual int sort(const qsort_sort_params &, qsort_sort_response &) = 0;
		
		virtual int echo(const qsort_echo_params &, qsort_echo_response &) = 0;
		
		virtual int dispatch(int64_t id, const char * function, const mc_pack_t * param, compack::buffer::Object * response);
	};
}
#endif 
