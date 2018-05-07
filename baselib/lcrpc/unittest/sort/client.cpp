#include <Lsc/var/JsonSerializer.h>
#include <Lsc/var/JsonDeserializer.h>
#include "lcrpc/client/client.h"
#include "sort.idm.h"
using namespace std;
int main()
{
    lc_log_init("./log", "client");

	lc::LcClientManager mgr;
	mgr.init();
	lcrpc::CompackClient client(&mgr);
	lcrpc::CompackClient client1(&mgr);
	lcrpc::NonblockClient nonblock(&mgr);
	Lsc::syspool pool;
	idm::qsort_sort_params in(&pool);
	idm::qsort_sort_response out(&pool);
	idm::qsort_sort_params in1(&pool);
	idm::qsort_sort_response out1(&pool);
	in.set_indata(0, 100);
	in.set_indata(1, 50);
	in.set_indata(3, 150);
	
	
	in1.set_indata(0, 101);
	in1.set_indata(1, 51);
	in1.set_indata(2, 151);

	nonblock.call(&client, "ixintui", "sort", &in, &out);
	nonblock.call(&client1, "ixintui", "sort", &in1, &out1);
	nonblock.waitAll();

	if(client.getError() == 0)
	{
		cout << out.result_params().outdata(0) << endl;
		cout << out.result_params().outdata(1) << endl;
		cout << out.result_params().outdata(2) << endl;
		cout << out.result_params().outdata(3) << endl;
	}
	else
	{
		cerr << client.getErrorMessage() << endl;
	}
	if(client1.getError() == 0)
	{
		cout << out1.result_params().outdata(0) << endl;
		cout << out1.result_params().outdata(1) << endl;
		cout << out1.result_params().outdata(2) << endl;
	}
	else
	{
		cerr << client1.getErrorMessage() << endl;
	}
	in1.set_indata(0, 102);
	in1.set_indata(1, 52);
	in1.set_indata(2, 152);
	if(client1.call("ixintui", "sort", &in1, &out1))
	{
		cerr << __FILE__ << client1.getErrorMessage() << endl;
	}
	else
	{
		cout << out1.result_params().outdata(0) << endl;
		cout << out1.result_params().outdata(1) << endl;
		cout << out1.result_params().outdata(2) << endl;
	}
	cout << "var test:" << endl;
	Lsc::ResourcePool rp;
	lcrpc::VarClient vc(&mgr, rp);
	Lsc::var::JsonDeserializer deser(rp);
	Lsc::var::JsonSerializer ser;
	Lsc::var::Ref res;
	vc.call("ixintui", "sort", deser.deserialize("{\"indata\":[1,3,2]}"), res);
	Lsc::AutoBuffer buf;
	ser.serialize(res, buf);
	cout << buf.c_str() <<endl;


	return 0;
}
