#include <stdio.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <time.h>

//#include <ul_log.h>
#include <Lsc/map.h>
#include <Lsc/containers/list/Lsc_list.h>

//#include <Lsc/var/CompackSerializer.h>

#include <Lsc/var/implement.h>
#include <Lsc/ResourcePool.h>

#include <Lsc/ResourcePool.h>
#include <Lsc/var/String.h>
#include <Lsc/var/Array.h>
#include <Lsc/pool.h>
#include <Lsc/var/implement.h>

static char buf[100000];  

int main(void)
{
        Lsc::xnofreepool pool;
        pool.create(buf, 100000);
        Lsc::ResourcePool rp(pool);
        
        Lsc::var::Dict dict = rp.create<Lsc::var::Dict>(rp.get_allocator());
        Lsc::var::Dict dict2 = rp.create<Lsc::var::Dict>(1024,rp.get_allocator());
        
        Lsc::var::String string = rp.create<Lsc::var::String>(rp.get_allocator());
        Lsc::var::String string2 = rp.create<Lsc::var::String>("ibase",rp.get_allocator());
        Lsc::var::String string3 = rp.create<Lsc::var::String>("com-ibase-123",9,rp.get_allocator());
        printf("%s\n%s\n",string2.c_str(),string3.c_str()); // OUTPUT: "ibase" "com-ibase"
        
        Lsc::var::Array array = rp.create<Lsc::var::Array>(rp.get_allocator());

        Lsc::string haha("haha");
        my_hash_func h;
        printf("hahs's hash vale is %llu\n", h(haha));
        
        return 0;
}

