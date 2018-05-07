



#include "sort.idm.qsort.h"
namespace idm
{
	const char * const qsort_servlet_trunk::ERROR_MSG []  = {
	};
	int qsort_servlet_trunk::setError(unsigned int err_no, const char * data)
	{
		if (err_no == 0)
		{
			return 0;
		}
		if ((size_t)err_no < sizeof(lcrpc::ERROR_MSG)/sizeof(char *))
		{
			setErrorMsgPtr(ERROR_MSG[err_no], data);
			return 0;
		}
		else
		{
			return -1;
		}
	}
	int qsort_servlet_trunk::dispatch(int64_t, const char * function, const mc_pack_t * param, compack::buffer::Object * result)
	{
		if (0 == strcmp(function, "sort"))
		{
			qsort_sort_params param_idm(getMempool());
			if (param != 0)
			{
				param_idm.load(mc_pack_get_buffer(param), mc_pack_get_size(param));
			}
			qsort_sort_response response_idm(getMempool());
			if (sort(param_idm, response_idm))
			{
				CWARNING_LOG("%s", "user function error");
				return -1;
			}
			if (isError())
			{
				CWARNING_LOG("%s", "user error set");
				return writeError(result);
			}
			else
			{
				response_idm.save(result);
				return 0;
			}
		}
		if (0 == strcmp(function, "echo"))
		{
			qsort_echo_params param_idm(getMempool());
			if (param != 0)
			{
				param_idm.load(mc_pack_get_buffer(param), mc_pack_get_size(param));
			}
			qsort_echo_response response_idm(getMempool());
			if (echo(param_idm, response_idm))
			{
				CWARNING_LOG("%s", "user function error");
				return -1;
			}
			if (isError())
			{
				CWARNING_LOG("%s", "user error set");
				return writeError(result);
			}
			else
			{
				response_idm.save(result);
				return 0;
			}
		}
		CWARNING_LOG("%s", "method not found");
		return -1;
	}
}
