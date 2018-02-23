

#include <stdlib.h>
#include <containers/list/Lsc_list.h>
#include <yperfbench.h>
#include <list>
int main(int argc, char ** argv )
{

	pb::init_opt(argc, argv);
	int len = pb::getopt<int>("len");
	Lsc::list<int> s;
	std::list<int> t;
	s.create();
	for(int i=0; i < len; i++)
	{
		int p = rand();
		t.push_front(p);
		s.push_front(p);
	}
	pb::timer time;
	s.sort();
	time.check();
#if 0
	t.sort();
	Lsc::list<int>::iterator i1 = s.begin();
	for(std::list<int>::iterator i2 = t.begin();i2 != t.end();i2++)
	{
		if(*i1 != *i2)
		{
			printf("%d,%d\n", *i1, *i2);
			exit(1);
		}
		i1++;
	}
#endif
}









/* vim: set ts=4 sw=4 sts=4 tw=100 */
