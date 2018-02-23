

#include <stdlib.h>
#include <list>
#include "yperfbench.h"

int main(int argc, char ** argv )
{

	pb::init_opt(argc, argv);
	int len = pb::getopt<int>("len");
	std::list<int> s;
	for(int i=0; i < len; i++)
		s.push_front(rand());
	pb::timer t;
	s.sort();
	t.check();
}









/* vim: set ts=4 sw=4 sts=4 tw=100 */
