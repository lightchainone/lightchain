#include <Lsc/list.h>
#include <Lsc/containers/list/Lsc_list.h>
#include <Lsc/containers/list/Lsc_list.h>
#include <Lsc/containers/slist/Lsc_slist.h>
#include <Lsc/containers/slist/Lsc_slist.h>
#include <Lsc/containers/string/Lsc_string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <time.h>
#include <cxxtest/TestSuite.h>

#include <Lsc/archive/Lsc_serialization.h>
#include <Lsc/archive/Lsc_filestream.h>
#include <Lsc/archive/Lsc_binarchive.h>
class p
{
plclic:
	bool operator()(int q)
	{
		return q==50; 
	}
};
class tt{
	plclic:
		int i;
		tt(int t):i(t){}
};

std::string rand_string()
{
    std::string str;
    //srand(time(NULL));
    int n = rand()%10 + 1;
    for (int i = 0; i < n; ++i) {
        str += 'a' + rand()%26;
    }
    return str;
}

class Lsc_test_list : plclic CxxTest::TestSuite
{
plclic:
    void test_operator() 
    {
        {
            Lsc::list<int> l1;
            Lsc::list<int> l2;
            l1.create();
            l2.create();
            l1.insert( l1.begin(), l2.begin(), l2.begin() );

            TS_ASSERT(0 == l1.size());
            TS_ASSERT(0 == l2.size());
            
            TS_ASSERT( 0 != l1.erase( l1.begin() ) );
            TS_ASSERT( l1.begin() == l1.end() );
            TS_ASSERT( l2.begin() == l2.end() );

            l1.insert( l1.begin(), l1.begin(), l1.end() );
            l1.insert( l1.begin(), 0, 1 );
            TS_ASSERT(0 == l1.size());
        }
        {
            Lsc::list<int> l1;
            Lsc::list<int> l2;
            l1.assign(l2.begin(),l2.end());
        }
        {
            Lsc::list<int> t1;
            for (int i = 0; i < 100; i ++) {
                t1.push_back(i);
            }
            TS_ASSERT( 100 == t1.size() );
            Lsc::list<int> t2;
            t2 = t1;
            TS_ASSERT( 100 == t1.size() );
            TS_ASSERT( 100 == t2.size() );
            int i = 0;
            for (Lsc::list<int>::iterator Iter = t2.begin(); Iter != t2.end(); ++ Iter) {
                TS_ASSERT( *Iter == i ++ );
            }
            i = 0;
            t1 = t1;
            TS_ASSERT( 100 == t1.size() );
            for (Lsc::list<int>::iterator Iter = t1.begin(); Iter != t1.end(); ++ Iter) {
                TS_ASSERT( *Iter == i ++ );
            }

            Lsc::list<int> t3;
            t3.push_back(1);
            TS_ASSERT( 1 == *t3.begin() );
            Lsc::list<int> t5;

            Lsc::list<int> t4;
            t4 = t3;
            TS_ASSERT( 1 == t4.size() );
            TS_ASSERT( 1 == t3.size() );

            t3.pop_back();
            t3 = t5;
            TS_ASSERT( 0 == t3.size() );

            t1 = t3;
            TS_ASSERT( 0 == t1.size() );
            TS_ASSERT( 0 == t3.size() );   
        }
        {
            Lsc::list<std::string> l1;
            Lsc::list<std::string> l2;
            for (int i = 0; i < 10; i ++) {
                l1.push_front( rand_string() );
            }
            TS_ASSERT( 10 == l1.size() );
            for (int i = 0; i < 100; i ++) {
                l2 = l1;
            }
            TS_ASSERT( 10 == l1.size() );
            TS_ASSERT( 10 == l2.size() );
            Lsc::list<std::string>::iterator iter = l1.begin();
            Lsc::list<std::string>::iterator iter2 = l2.begin(); 
            for( ; iter != l1.end(); ++ iter, ++ iter2) {
                TS_ASSERT( (*iter) == (*iter2) );
            }
        }
        {
            Lsc::list<int> l1;
            for (int i = 0; i < 100; i ++) {
                l1.push_back(i);
            }
            TS_ASSERT( 100 == l1.size() );
            Lsc::list<int> l2;
            for (int i = 10; i > 0; i --) {
                l2.push_back(i);
            }
            TS_ASSERT( 10 == l2.size() );
            l2 = l1;
            TS_ASSERT( 100 == l1.size() );
            TS_ASSERT( 100 == l2.size() );
            int i = 0;
            for (Lsc::list<int>::iterator iter = l2.begin(); iter != l2.end(); ++ iter, i ++) {
                TS_ASSERT( *iter == i );
            }
        }
    }
    void test_create() 
    {
        Lsc::list<int> l1;
        int i;
        for (i = 0; i < 10000; i ++) {
            l1.create();
        }
        for (i = 0; i < 10000; i ++) {
            l1.create();
        }
        Lsc::list<int> l2;
        TS_ASSERT(0 == l2.size());
        for (i = 0; i < 10000; i ++) {
            l2.create();
        }
        TS_ASSERT( 0 == l2.size());
        for (i = 0; i < 100; i ++) {
            l2.push_back(i);
        }
        TS_ASSERT( 100 == l2.size() );
    }
	void test_string()
	{
		Lsc::list<std::string> t1;
		TS_ASSERT(!t1.create());
		TS_ASSERT(!t1.push_front(std::string("123")));
		TS_ASSERT(!t1.push_front(std::string("abc")));
		TS_ASSERT(!t1.push_front(std::string("ABC")));
        TS_ASSERT(3 == t1.size());
		TS_ASSERT(!t1.destroy());
	}
	void test_push_front()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		TS_ASSERT_EQUALS(t1.size() , 100);
		Lsc::list<int>::iterator i = t1.begin();
		for(int v = 99; v >= 1; v--)
		{
			TS_ASSERT_EQUALS(*i , v);
			i++;
		}
		TS_ASSERT(!t1.destroy());
	}
	
	void test_push_front1()
	{
		Lsc::list<tt> t1;
		TS_ASSERT(!t1.create());
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(tt(i)));
		}
		TS_ASSERT_EQUALS(t1.size() , 100);
		Lsc::list<tt>::iterator i = t1.begin();
		for(int v = 99; v >= 1; v--)
		{
			TS_ASSERT_EQUALS(i->i , v);
			i++;
		}
		TS_ASSERT(!t1.destroy());
	}

	void test_push_back()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_back(i));
		}
		TS_ASSERT_EQUALS(t1.size() , 100);
		Lsc::list<int>::iterator i = t1.begin();
		for(int v=0; v<100; v++)
		{
			TS_ASSERT_EQUALS(*i , v);
			i++;
		}
		TS_ASSERT(!t1.destroy());
	}

	void test_assign1()
	{
		Lsc::list<int> t1, t2;
		TS_ASSERT(!t1.create());
		for (int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
        TS_ASSERT(100 == t1.size());
		TS_ASSERT(!t2.create());
		t2.assign(t1);
        TS_ASSERT(100 == t2.size());
		Lsc::list<int>::iterator i = t2.begin();
		for(int v=99; v>=1; v--)
		{
			TS_ASSERT_EQUALS(*i , v);
			i++;
		}
		TS_ASSERT(!t1.destroy());
		TS_ASSERT(!t2.destroy());
	}
	void test_assign()
	{
		Lsc::list<int> t1, t2;
		TS_ASSERT(!t1.create());
		for (int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		TS_ASSERT(!t2.create());
        TS_ASSERT(0 == t2.size());
		t2.assign(t1.begin(), t1.end());
        TS_ASSERT(100 == t2.size());
		Lsc::list<int>::iterator i = t2.begin();
		for(int v=99; v>=1; v--)
		{
			TS_ASSERT_EQUALS(*i , v);
			i++;
		}
		TS_ASSERT(!t1.destroy());
		TS_ASSERT(!t2.destroy());
	}

	void test_pop_front()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
        TS_ASSERT(0 == t1.size());
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
        TS_ASSERT(100 == t1.size());
		for(int i=99; i>=0; i--)
		{
			TS_ASSERT_EQUALS(i ,t1.front());
			t1.pop_front();
		}
        TS_ASSERT(0 == t1.size());
		TS_ASSERT(!t1.destroy());
	}


	void test_pop_back()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
        TS_ASSERT(0 == t1.size());
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
        TS_ASSERT(100 == t1.size());
		for(int i=0; i<100; i++)
		{
			//		printf("%d\n", t1.back());
			TS_ASSERT_EQUALS(i , t1.back());
			t1.pop_back();
		}
        TS_ASSERT(0 == t1.size());
		TS_ASSERT(!t1.destroy());
	}

	void test_sort()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
		for(int i=100; i>0; i--)
		{
			TS_ASSERT(!t1.push_front(rand()));
		//	TS_ASSERT(!t1.push_front(i));
		}
        TS_ASSERT(100 == t1.size());
		t1.sort();
		TS_ASSERT_EQUALS(t1.size() , 100);
		//int tmp = 0x7fffffff;
		for(Lsc::list<int>::iterator i1 = t1.begin(); i1!=t1.end(); ++i1)
		{
		//	TS_ASSERT(tmp > *i1);
			//	printf("%d\n", *i1);
			/*	if( *i1 != v)
				{
				printf("%d, %d",*i1, v);
				ERROR();
				}
				v--;*/
		}
        TS_ASSERT(100 == t1.size());
		TS_ASSERT(!t1.destroy());
	}

	void test_remove()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
		for(int i=100; i>0; i--)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		t1.remove(50);
		TS_ASSERT_EQUALS(t1.size(), 99);
		TS_ASSERT(!t1.destroy());
	}

	void test_remove1()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
		for(int i=100; i>0; i--)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		TS_ASSERT(!t1.remove_if(p()));
		TS_ASSERT_EQUALS(t1.size(), 99);
		TS_ASSERT(!t1.destroy());
	}


	void test_const()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());				
		for(int i=100; i>0; i--)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		const Lsc::list<int> & t2 = t1;
		int p = 1;
		TS_ASSERT_EQUALS(t2.back() , 100);
		TS_ASSERT_EQUALS(t2.front() , 1);

        p = 1;
		for(Lsc::list<int>::const_iterator i1 = t2.begin(); i1!=t2.end(); ++i1)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p++;
		}
        p = 1;
		for(Lsc::list<int>::const_iterator i1 = t1.begin(); i1!=t1.end(); i1++)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p++;
		}

		TS_ASSERT(!t1.destroy());
		p = 100;
		for(Lsc::list<int>::const_reverse_iterator i1 = t2.rbegin(); i1!=t2.rend(); ++i1)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p--;
		}
		p = 100;
		for(Lsc::list<int>::const_reverse_iterator i1 = t1.rbegin(); i1!=t1.rend(); i1++)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p--;
		}

	}
	void test_const2()
	{
		Lsc::list<int>::const_iterator i = Lsc::list<int>::iterator();
		Lsc::list<int>::const_reverse_iterator p = Lsc::list<int>::reverse_iterator();
	}

	void test_forward_iterator()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());				
		for(int i=100; i>0; i--)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		TS_ASSERT(t1.begin() != t1.end());
		TS_ASSERT(t1.begin() == t1.begin());
		TS_ASSERT(t1.end() == t1.end());
		int p = 1;
		for(Lsc::list<int>::iterator i1 = t1.begin(); i1!=t1.end(); i1++)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p++;
		}
		p = 1;
		for(Lsc::list<int>::iterator i1 = t1.begin(); i1!=t1.end(); ++i1)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p++;
		}
	
		p = 100;
		Lsc::list<int>::iterator i1; 
		for(i1 = --(t1.end()); i1!=t1.begin(); i1--)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p--;
		}
		p = 100;
		for(i1 = --(t1.end()); i1!=t1.begin(); --i1)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p--;
		}
		TS_ASSERT(!t1.destroy());

	}
	void test_reverse_iterator()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());				
		for(int i=100; i>0; i--)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		TS_ASSERT(t1.rbegin() != t1.rend());
		TS_ASSERT(t1.rbegin() == t1.rbegin());
		TS_ASSERT(t1.rend() == t1.rend());
		int p = 100;
		for(Lsc::list<int>::reverse_iterator i1 = t1.rbegin(); i1!=t1.rend(); i1++)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p--;
		}
        p = 100;
		for(Lsc::list<int>::reverse_iterator i1 = t1.rbegin(); i1!=t1.rend(); ++i1)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p--;
		}
		p = 1;
		Lsc::list<int>::reverse_iterator i1;
		for( i1 = --(t1.rend()); i1!=t1.rbegin(); i1--)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p++;
		}
		p = 1;
		for( i1 = --(t1.rend()); i1!=t1.rbegin(); --i1)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p++;
		}

		TS_ASSERT(!t1.destroy());

	}

	void test_reverse()
	{	
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());				
		for(int i=100; i>0; i--)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		t1.reverse();
		TS_ASSERT_EQUALS(t1.size() , 100);
		int p = 100;
		for(Lsc::list<int>::iterator i1 = t1.begin(); i1!=t1.end(); ++i1)
		{
			TS_ASSERT_EQUALS(p , *i1);
			p--;
		}
        TS_ASSERT(100 == t1.size());
		TS_ASSERT(!t1.destroy());

	}

	void test_swap()
	{
		//Lsc::list<int, Lsc::Lsc_alloc<int> > t1;
		//Lsc::list<int, Lsc::Lsc_alloc<int> > t1;
		Lsc::list<int> t1;
		Lsc::list<int> t2;
		TS_ASSERT(!t1.create());				
		TS_ASSERT(!t2.create());				
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		t2.swap(t1);
		TS_ASSERT_EQUALS(t2.size() , 100);
		TS_ASSERT_EQUALS(t1.size() , 0);
		t1.swap(t2);
		TS_ASSERT_EQUALS(t2.size() , 0);
		TS_ASSERT_EQUALS(t1.size() , 100);
		for(int i=0; i<50; i++)
		{
			TS_ASSERT(!t2.push_front(3 * i));
		}
		t2.swap(t1);
		TS_ASSERT_EQUALS(t1.size() , 50);
		TS_ASSERT_EQUALS(t2.size() , 100);
		//Lsc::list<int>::iterator i = t1.begin();
		Lsc::list<int>::iterator i = t1.begin();
		//Lsc::list<int>::iterator p = t2.begin();
		Lsc::list<int>::iterator p = t2.begin();
		/*for(int v = 50; v >= 1; v--)
		{
			TS_ASSERT_EQUALS(*i , 3 * v);
			i++;
			TS_ASSERT_EQUALS(*p , v);
			p++;
		}*/
		TS_ASSERT(!t1.destroy());
		TS_ASSERT(!t2.destroy());
	}
	void test_merge()
	{
		//Lsc::list<int, Lsc::Lsc_alloc<int> > t1;
		//Lsc::list<int, Lsc::Lsc_alloc<int> > t1;
		Lsc::list<int> t1;
		Lsc::list<int> t2;
		TS_ASSERT(!t1.create());				
		TS_ASSERT(!t2.create());				
		t2.merge(t1);
		TS_ASSERT_EQUALS(t2.empty() , true);
		for(int i=49; i>=0; i--)
		{
			TS_ASSERT(!t1.push_front(2*i));
		}
		TS_ASSERT_EQUALS(t2.size() , 0);
		t2.merge(t1);
		TS_ASSERT_EQUALS(t2.size() , 50);
		for(int i=49; i>=0; i--)
		{
			TS_ASSERT(!t1.push_front(2*i+1));
		}
		t2.merge(t1);
		TS_ASSERT_EQUALS(t1.empty() , true);
		TS_ASSERT_EQUALS(t2.size() , 100);
		TS_ASSERT(!t1.destroy());
		Lsc::list<int>::iterator p = t2.begin();
		for(int v = 0; v < 100; v++)
		{
			TS_ASSERT_EQUALS(*p , v);
			p++;
		}
		TS_ASSERT(!t2.destroy());
	}

	void test_insert()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());				
		TS_ASSERT(!t1.insert(t1.begin(),1));
		TS_ASSERT(!t1.insert(t1.end(),2));
		TS_ASSERT_EQUALS(t1.size() , 2);
		TS_ASSERT_EQUALS(t1.front() , 1);
		TS_ASSERT_EQUALS(t1.back() , 2);
     
        Lsc::list<int> t2;
        TS_ASSERT(!t2.insert(t1.begin(), 0)); 
   		TS_ASSERT_EQUALS(t2.size() , 0);
        TS_ASSERT_EQUALS(t1.size() , 3);
        TS_ASSERT_EQUALS(t1.front() , 0);
		TS_ASSERT_EQUALS(t1.back() , 2);
        
		TS_ASSERT(!t1.destroy());
        TS_ASSERT(!t2.destroy());
	}

	void test_insert2()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());				
		TS_ASSERT(!t1.insert(t1.begin(),2, 1));
		TS_ASSERT(!t1.insert(t1.end(),2, 2));
		//printf("%d\n", t1.size());
		TS_ASSERT_EQUALS(t1.size() , 4);
		TS_ASSERT_EQUALS(t1.front() , 1);
		TS_ASSERT_EQUALS(t1.back() , 2);
		TS_ASSERT(!t1.destroy());
	}

	void test_insert3()
	{
		Lsc::list<int> t1;
		Lsc::list<int> t2;
		TS_ASSERT(!t1.create());				
		TS_ASSERT(!t2.create());				
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));
			TS_ASSERT(!t2.push_front(2 * i));
		}
		t1.insert(t1.end(),t2.begin(), t2.end());
		TS_ASSERT_EQUALS(t1.size() , 200);
		
        t2.insert(t1.end(),t2.begin(), t2.end()); 
		TS_ASSERT_EQUALS(t1.size() , 300);

        TS_ASSERT(!t1.destroy());
		TS_ASSERT(!t2.destroy());
	}
	void test_size()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
        const size_t N = 1000000;
		for(size_t i=0; i<N; i++)
		{
			TS_ASSERT(!t1.push_front(i));
            TS_ASSERT((i + 1) == t1.size());
        }
        TS_ASSERT(N == t1.size());
		TS_ASSERT(t1.size()<t1.max_size());
		TS_ASSERT(!t1.destroy());
	}
	void test_resize()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());	
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));

		}
		TS_ASSERT(!t1.resize(200, 10));
		TS_ASSERT_EQUALS(t1.size() , 200);
		TS_ASSERT_EQUALS(t1.back() , 10);
		TS_ASSERT(!t1.resize(100));
		TS_ASSERT_EQUALS(t1.size() , 100);
		TS_ASSERT_EQUALS(t1.back(), 0);
	}
	void test_splice()
	{
		Lsc::list<int> t1;
		Lsc::list<int> t2;
		TS_ASSERT(!t1.create());
		TS_ASSERT(!t2.create());
		for(int i=0; i<30; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		for(int i=30; i<50; i++)
		{
			TS_ASSERT(!t2.push_front(i));
		}    
		t1.splice(t1.begin(), t2);
        for(int i=50; i<100; i++)
		{
			TS_ASSERT(!t2.push_front(i));
		}    
		t2.splice(t1.begin(), t2);
	
        TS_ASSERT_EQUALS(t1.size(), 100);
		TS_ASSERT_EQUALS(t2.size(), 0);
		for(int i=99; i>=0; i--)
		{
			TS_ASSERT_EQUALS(i ,t1.front());
			t1.pop_front();
		}    
		TS_ASSERT(!t1.destroy());
	}
#if 0
	void 1test_splice1()
	{
		Lsc::list<int> t1;
		Lsc::list<int> t2;
		TS_ASSERT(!t1.create());
		TS_ASSERT(!t2.create());
		for(int i=0; i<50; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		for(int i=50; i<100; i++)
		{
			TS_ASSERT(!t2.push_front(i));
		}
		t1.splice(t1.begin(), t2, t2.begin());
		TS_ASSERT_EQUALS(t1.size(), 51);
		TS_ASSERT_EQUALS(t2.size(), 49);
		TS_ASSERT(!t1.destroy());
	
	}
	void 1test_splice2()
	{
		Lsc::list<int> t1;
		Lsc::list<int> t2;
		TS_ASSERT(!t1.create());
		TS_ASSERT(!t2.create());
		for(int i=0; i<50; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		for(int i=50; i<100; i++)
		{
			TS_ASSERT(!t2.push_front(i));
		}
		t1.splice(t1.begin(), t2, t2.begin(), t2.end());
		TS_ASSERT_EQUALS(t1.size(), 100);
		TS_ASSERT_EQUALS(t2.size(), 0);
		for(int i=99; i>=0; i--)
		{
			TS_ASSERT_EQUALS(i ,t1.front());
			t1.pop_front();
		}
		TS_ASSERT(!t1.destroy());
	
	}
#endif
	void test_erase()
	{
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
		t1.erase(t1.begin());
		TS_ASSERT_EQUALS(t1.size(), 99);
		for(int i=98; i>=0; i--)
		{
			TS_ASSERT_EQUALS(i ,t1.front());
			t1.pop_front();
		}
		TS_ASSERT(!t1.destroy());
	}

	void test_erase1()
	{
        Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}
        Lsc::list<int> t2=t1;
        TS_ASSERT(100 == t2.size());
        t1.erase(t2.begin(), t2.end());
        TS_ASSERT(100 == t1.size());
        TS_ASSERT(0 == t2.size());

		t1.erase(t1.begin(), t1.end());
		TS_ASSERT_EQUALS(t1.size(), 0);
		TS_ASSERT(!t1.destroy());
	}
	void test_serialization()
	{
		
		Lsc::list<int> t1;
		TS_ASSERT(!t1.create());
		for(int i=0; i<100; i++)
		{
			TS_ASSERT(!t1.push_front(i));
		}

		Lsc::filestream fd;
		fd.open("data", "w");
		fd.setwbuffer(1<<20);
		Lsc::binarchive ar(fd);
		TS_ASSERT (ar.write(t1) == 0);
		fd.close();
	
		t1.clear();
		TS_ASSERT_EQUALS(t1.size(), 0);
		std::string str;
		fd.open("data", "r");
		{
			Lsc::binarchive ar(fd);
			TS_ASSERT (ar.read(&t1) == 0);
			fd.close();
		}
		TS_ASSERT_EQUALS(t1.size(), 100);
		for(int i=99; i>=0; i--)
		{
			TS_ASSERT_EQUALS(i ,t1.front());
			t1.pop_front();
		}
	}
    void test_iterator(){
        {
            //non-const
            Lsc::list<int> li;

            Lsc::list<int>::iterator it1 = li.begin();
            Lsc::list<int>::iterator it2 = li.end();
            TS_ASSERT(it1 == it2);
            
            Lsc::list<int>::reverse_iterator rit1 = li.rbegin();
            Lsc::list<int>::reverse_iterator rit2 = li.rend();
            TS_ASSERT(rit1 == rit2);
            
            Lsc::list<int>::const_iterator cit1 = li.begin();
            Lsc::list<int>::const_iterator cit2 = li.end();
            TS_ASSERT(cit1 == cit2);
        
            Lsc::list<int>::const_reverse_iterator rcit1 = li.rbegin();
            Lsc::list<int>::const_reverse_iterator rcit2 = li.rend();
            TS_ASSERT(rcit1 == rcit2);    
        }
        {
            //const
            const Lsc::list<int> li;
            
            Lsc::list<int>::const_iterator cit1 = li.begin();
            Lsc::list<int>::const_iterator cit2 = li.end();
            TS_ASSERT(cit1 == cit2);
        
            Lsc::list<int>::const_reverse_iterator rcit1 = li.rbegin();
            Lsc::list<int>::const_reverse_iterator rcit2 = li.rend();
            TS_ASSERT(rcit1 == rcit2);    
        }
    }
};
