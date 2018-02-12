#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "mempool.h"

#if 0
int main()
{
	comspace::MemPool mempool;
	mempool.create();
	char *str = (char *)mempool.alloc(1024);
	snprintf(str, SIZEOFPTR(str), "%s", "hello world");

	printf("len=%d str=%s\n", SIZEOFPTR(str), str);

	for (int i=0; i<100000; ++i) {
		//int size = rand()%(1<<24) + 1;
		int size = 1<<19;
		fprintf(stderr, "%d[%d]\n", i, size);
		void *ptr = mempool.alloc(size);
		if (ptr == NULL) {
			fprintf(stderr, "faint");
			continue;
		}
		memset(ptr, 0, size);
		if(rand()%2)
		mempool.dealloc(ptr);
		//sleep(1);
	}

	return 0;
}
#endif

#include <vector>

comspace::MemPool g_pool;
std::vector<void *> g_vec;
pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
int g_ptr = 0;

#define LOOP (1<<20)

void * _pid1_(void *)
{
	for (int i=0; i<LOOP; ++i) {
		pthread_mutex_lock(&g_lock);
		void *ptr = g_pool.alloc(102400);
		if (ptr != NULL) g_vec.push_back(ptr);
		pthread_mutex_unlock(&g_lock);
		//fprintf(stderr, "%d ", g_vec.size() - g_ptr);
	}
	return NULL;
}

void * _pid2_ (void *)
{
	for (int i=0; i<LOOP; ++i) {
		pthread_mutex_lock(&g_lock);
		if (g_ptr < (int)g_vec.size()) {
			void *ptr = g_vec[g_ptr];
			++g_ptr;
			g_pool.dealloc(ptr);
		}
		pthread_mutex_unlock(&g_lock);
	}
	return NULL;
}


int main()
{
	g_pool.create();
	pthread_t pid1, pid2;

	pthread_create(&pid1, NULL, _pid1_, NULL);
	pthread_create(&pid2, NULL, _pid2_, NULL);

	pthread_join(pid1, NULL);
	pthread_join(pid2, NULL);
	return 0;
}















/* vim: set ts=4 sw=4 sts=4 tw=100 */
