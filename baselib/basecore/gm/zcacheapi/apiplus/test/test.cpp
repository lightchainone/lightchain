
#include<stdlib.h>
#include "lc.h"
#include "zcacheapiplus.h"
#include "zcacheapiplus_struct.h"

int test_apiplus_normal()
{
	printf("/********以下测试是正常的功能回归：insert/update/delete/select****/");
	zcacheplus_t zcacheplus_handel;
	const char * master_service_addrs = "10.65.23.163:44444";
	const char * slave_service_addrs = "10.23.249.12:44444";
	const char * product_name = "comment";
	const char * token = "comment";

	
	if (0 != init_zcache(&zcacheplus_handel, master_service_addrs, slave_service_addrs,
			product_name, token, 200, 200, 200, 5000, 3000, 2, 1, 1))
	{
		printf("create api+ handle fail!\n");
		LC_LOG_DEBUG("create api+ handle fail!");
		return -1;
	}
	printf("create api+ handle ok\n");
	LC_LOG_DEBUG("create api+ handle ok!");

	
	zcache_item_t key;
	zcache_item_t value;
	char out_value [128];
	zcache_item_t value_out;
	zcache_err_t err_out;
	char * key1 = "wanghao";
	char * value1 = "wanghao";

	key.item = key1;
	key.len = strlen(key1) + 1;

	value.item = value1;
	value.len = strlen(value1) + 1;

	if (0 != update_zcache(&zcacheplus_handel, &key, NULL, &value, 120000, &err_out, 1,
			200))
	{
		printf("update fail!\n");
		LC_LOG_DEBUG("update fail!");
		return -1;
	}
	printf("update 1 key:[%s] value:[%s] ok\n", key.item, value.item);
	LC_LOG_DEBUG("update 1 ok!");

	char * key2 = "wanghao's email";
	char * value2 = "wanghao01@ixintui.com";
	key.item = key2;
	key.len = strlen(key2) + 1;
	value.item = value2;
	value.len = strlen(value2) + 1;
	if (0 != update_zcache(&zcacheplus_handel, &key, NULL, &value, 120000, &err_out, 1,
			200))
	{
		printf("update fail!\n");
		LC_LOG_DEBUG("update fail!");
		return -1;
	}
	printf("update 2 key:[%s] value:[%s] ok\n", key.item, value.item);
	LC_LOG_DEBUG("update 2 ok!");

	
	value_out.item = out_value;
	value_out.len = sizeof(out_value);
	if (0 != search_zcache(&zcacheplus_handel, &key, NULL, &value_out, &err_out, 3, 200,
			NULL))
	{
		printf("search  fail!\n");
		LC_LOG_DEBUG("search fail!");
		return -1;
	}
	printf("search  key:[%s] value:[%s] \n", key.item, value_out.item);
	LC_LOG_DEBUG("search  value: %s\n",value_out.item);

	
	if (0 != delete_zcache(&zcacheplus_handel, &key, NULL, 0, &err_out, 4, 200))
	{
		printf("delete  fail!\n");
		LC_LOG_DEBUG("delete fail!");
		return -1;
	}
	printf("delete key:[%s] ok\n", key.item);
	
	if (0 != insert_zcache(&zcacheplus_handel, &key, NULL, &value, 120000, &err_out, 1,
			200))
	{
		printf("insert fail!\n");
		LC_LOG_DEBUG("insert fail!");
		return -1;
	}
	printf("insert key:[%s] value:[%s] ok\n", key.item, value.item);
	LC_LOG_DEBUG("insert  ok!");
	destroy_zcache(&zcacheplus_handel);
	printf("/********以上测试是正常的功能回归：insert/update/delete/select****/ \n");
	return 0;
}

int test_apiplus_plus()
{
	printf("/********以下测试是plus功能***********/");
	zcacheplus_t zcacheplus_handel;
	const char * master_service_addrs = "10.65.23.163:44444";
	const char * slave_service_addrs = "10.23.249.12:44444";
	const char * product_name = "comment";
	const char * token = "comment";

	
	if (0 != init_zcache(&zcacheplus_handel, master_service_addrs, slave_service_addrs,
			product_name, token, 200, 200, 200, 5000, 3000, 2, 1, 1))
	{
		printf("create api+ handle fail!\n");
		LC_LOG_DEBUG("create api+ handle fail!");
		return -1;
	}
	printf("create api+ handle ok\n");
	LC_LOG_DEBUG("create api+ handle ok!");

	
	zcache_item_t key;
	zcache_item_t value;
	char out_value [128];
	zcache_item_t value_out;
	zcache_err_t err_out;
	char * key1 = "where is wanghao from";
	char * value1 = "nanjing/jiangsu";
	key.item = key1;
	key.len = strlen(key1) + 1;

	value.item = value1;
	value.len = strlen(value1) + 1;
	if (0 != update_zcache(&zcacheplus_handel, &key, NULL, &value, 120000, &err_out, 10,
			200))
	{
		printf("update fail!\n");
		LC_LOG_DEBUG("update fail!");
		return -1;
	}
	printf("update master key:[%s] value:[%s]  ok\n", key.item, value.item);
	LC_LOG_DEBUG("update master ok!");

	char * key2 = "where is wanghao from";
	char * value2 = "Nanjing University";
	key.item = key2;
	key.len = strlen(key2) + 1;
	value.item = value2;
	value.len = strlen(value2) + 1;
	
	if (0 != zcache_update((zcacheplus_handel.slave_zc_handle), &key, NULL, &value,
			120000, &err_out, 11, 200))
	{
		printf("update fail!\n");
		LC_LOG_DEBUG("update fail!");
		return -1;
	}
	printf("update slave key:[%s] value:[%s]  ok\n", key.item, value.item);
	LC_LOG_DEBUG("update 2 ok!");

	printf("同学，现在可以去挂掉master的cacheserver了，ok(y) \n");
	int temp_int = 0;
	int temp_int1 = 0;
	scanf("%d", &temp_int);
	printf("请注意select是否来自slave zcache \n");

	
	value_out.item = out_value;
	value_out.len = sizeof(out_value);
	if (0 != search_zcache(&zcacheplus_handel, &key, NULL, &value_out, &err_out, 12, 200,
			NULL))
	{
		printf("search  fail!\n");
		LC_LOG_DEBUG("search fail!");
		
	}
	printf("search  value: %s\n", value_out.item);
	LC_LOG_DEBUG("search  value: %s\n",value_out.item);

	printf("同学，现在去恢复master的cacheserver,ok(y)\n");
	scanf("%d", &temp_int1);

	sleep(2);
	
	value_out.item = out_value;
	value_out.len = sizeof(out_value);
	if (0 != search_zcache(&zcacheplus_handel, &key, NULL, &value_out, &err_out, 13, 200,
			NULL))
	{
		printf("search  fail!\n");
		LC_LOG_DEBUG("search fail!");
		
	}
	printf("search  value: %s\n", value_out.item);
	LC_LOG_DEBUG("search  value: %s\n",value_out.item);

	
	value_out.item = out_value;
	value_out.len = sizeof(out_value);
	if (0 != search_zcache(&zcacheplus_handel, &key, NULL, &value_out, &err_out, 13, 200,
			NULL))
	{
		printf("search  fail!\n");
		LC_LOG_DEBUG("search fail!");
		
	}
	printf("search  value: %s\n", value_out.item);
	LC_LOG_DEBUG("search  value: %s\n",value_out.item);

	printf("请注意select是否来自master zcache,ok(y)\n");
	scanf("%d", &temp_int);

	printf("/********以上测试是plus功能***********/ \n");
	return 0;
}

int test_apiplus_plus2()
{
	printf("/********以下测试是plus2功能***********/");
	zcacheplus_t zcacheplus_handel;
	const char * master_service_addrs = "10.65.23.163:44444";
	const char * slave_service_addrs = "10.23.249.12:44444";
	const char * product_name = "comment";
	const char * token = "comment";
	int temp_int = 0;

	
	if (0 != init_zcache(&zcacheplus_handel, master_service_addrs, slave_service_addrs,
			product_name, token, 200, 200, 200, 5000, 3000, 2, 1, 1))
	{
		printf("create api+ handle fail!\n");
		LC_LOG_DEBUG("create api+ handle fail!");
		return -1;
	}
	printf("create api+ handle ok\n");
	LC_LOG_DEBUG("create api+ handle ok!");

	printf("人为中断\n");
	scanf("%d", &temp_int);

	
	zcache_item_t key;
	zcache_item_t value;
	char out_value [128];
	zcache_item_t value_out;
	zcache_err_t err_out;
	char * key1 = "where is wanghao from";
	char * value1 = "nanjing/jiangsu";
	key.item = key1;
	key.len = strlen(key1) + 1;

	value.item = value1;
	value.len = strlen(value1) + 1;
	if (0 != update_zcache(&zcacheplus_handel, &key, NULL, &value, 120000, &err_out, 10,
			200))
	{
		printf("update fail!\n");
		LC_LOG_DEBUG("update fail!");
		
	}
	printf("update master key:[%s] value:[%s]  ok\n", key.item, value.item);
	LC_LOG_DEBUG("update master ok!");

	
	printf("search  10 times");
	for (int i = 0; i < 10; i++)
	{
		char temp_key [128];
		snprintf(temp_key, sizeof(temp_key), "%d", rand());
		key.item = temp_key;
		key.len = strlen(temp_key) + 1;
		value_out.item = out_value;
		value_out.len = sizeof(out_value);
		if (0 != update_zcache(&zcacheplus_handel, &key, NULL, &value_out, 120000,
				&err_out, 10, 200))
		{
			printf("search  fail!\n");
			LC_LOG_DEBUG("search fail!");
			
		}
	}

	printf("人为中断\n");
	scanf("%d", &temp_int);

	
	printf("search  10 times");
	for (int i = 0; i < 10; i++)
	{
		char temp_key [128];
		snprintf(temp_key, sizeof(temp_key), "%d", rand());
		key.item = temp_key;
		key.len = strlen(temp_key) + 1;
		value_out.item = out_value;
		value_out.len = sizeof(out_value);
		if (0 != search_zcache(&zcacheplus_handel, &key, NULL, &value_out, &err_out, 13,
				200, NULL))
		{
			printf("search  fail!\n");
			LC_LOG_DEBUG("search fail!");
			
		}
	}

	printf("人为中断\n");
	scanf("%d", &temp_int);

	
	printf("search  10 times");
	for (int i = 0; i < 10; i++)
	{
		char temp_key [128];
		snprintf(temp_key, sizeof(temp_key), "%d", rand());
		key.item = temp_key;
		key.len = strlen(temp_key) + 1;
		value_out.item = out_value;
		value_out.len = sizeof(out_value);
		if (0 != search_zcache(&zcacheplus_handel, &key, NULL, &value_out, &err_out, 13,
				200, NULL))
		{
			printf("search  fail!\n");
			LC_LOG_DEBUG("search fail!");
			
		}
	}

	printf("人为中断\n");
	scanf("%d", &temp_int);

	printf("/********以上测试是plus2功能***********/ \n");
	return 0;
}

int main()
{
	lc_log_init("./", "test.", 1500, 16);
	if (0 != test_apiplus_normal())
	{
		printf("test fail!\n");
		LC_LOG_DEBUG("test fail!");
		return 0;
	}
	if (0 != test_apiplus_plus())
	{
		printf("test fail!\n");
		LC_LOG_DEBUG("test fail!");
		return 0;
	}

	

	printf("test ok!\n");
	LC_LOG_DEBUG("test ok!");
	return 0;
}
