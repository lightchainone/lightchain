


#ifndef  __SORTSVR_H_
#define  __SORTSVR_H_

#include <algorithm>
#include <vector>
#include "sort.idm.qsort.h"
	using namespace std;
class QSortServlet : plclic idm::qsort_servlet_trunk
{
plclic:
	virtual int sort(const idm::qsort_sort_params &in, idm::qsort_sort_response &out) {
		
		const idm::vector<uint32_t> & indata = in.indata();	
		std::vector<uint32_t> vec;
		for (int i=0; i<(int)indata.size(); ++i) {
			cout << indata[i] << " ";
			vec.push_back(indata[i]);
		}
		cout << endl;
		std::sort(vec.begin(), vec.end());	
		for (int i=0; i<(int)vec.size(); ++i) {	
			out.m_result_params()->set_outdata(i, vec[i]);
		}
		return 0;
	}

	virtual int echo(const idm::qsort_echo_params &in, idm::qsort_echo_response &out) 
	{
		const char * instr = in.instr();
		out.m_result_params()->set_outstr(instr); 
		return 0;
	}
};


class QSortServer : plclic lcrpc::LcRpcServer
{
	protected:
		virtual lcrpc::Servlet * createServlet()
		{
			return new QSortServlet;
		}
};












#endif  


