


#include <containers/slist/Lsc_slist.h>
#include <yperfbench.h>

int main(int argc, char ** argv )
{

	pb::init_opt(argc, argv);
	int c = pb::getopt<int>("time");
	Lsc::slist<int> s;
	pb::timer t;
	for (int i=0; i< c; i++)
	{
		s.push_front(i);
	}
	t.check();

}









/* vim: set ts=4 sw=4 sts=4 tw=100 */
