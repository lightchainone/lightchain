#include "myclient_include.h"
#include "lc.h"

void fail_handler(MysqlConnection *conn, const char *sql)
{
	printf("query error: errno[%d], error[%s], sql[%s]\n", conn->getMysqlErrno(),
			conn->getMysqlError(), sql);
	exit(1);
}


int test_connection()
{
	
	MyclientServerConf test_pool_conf;
	
	
	MysqlConnection* test_connection = new MysqlConnection();
	printf("init connection ok !\n");
	test_connection->connect2(&test_pool_conf);
	printf("connect connection ok !\n");
	MyclientRes res;
	char query [] = "show tables";
	test_connection->query(query, &res, true);
	res.__print();
  unsigned int err=test_connection->getLastMysqlErrorno();
  printf("getMysqlError ok!,err:%u",err);
	test_connection->disconnect();
	return 0;

}


int test_pool()
{
	
	MyclientServerConf test_pool_conf [6];
	char temp_name [100];

	for (int i = 0; i < 6; i++)
	 {
		test_pool_conf [i].port = 9800;
		test_pool_conf [i].read_timeout = 2000;
		test_pool_conf [i].write_timeout = 2000;
		test_pool_conf [i].connect_timeout = 2000;
		test_pool_conf [i].min_connection = 3;
		test_pool_conf [i].max_connection = 5;
        if (i == 0)
        {
            strcpy(test_pool_conf [i].ip, "10.32.11.129");
        }
        else
        {
		    strcpy(test_pool_conf [i].ip, "10.32.11.130");
        }
		strcpy(test_pool_conf [i].username, "root");
		strcpy(test_pool_conf [i].password, "12345");
		
        snprintf(temp_name, sizeof(temp_name), "%s", "ddbs" );
        strcpy(test_pool_conf [i].dbname, temp_name);
		strcpy(test_pool_conf [i].charset, "latin1");
	}
	strcpy(test_pool_conf [0].tag, MYCLIENT_MASTER_HOST_TYPE);
	strcpy(test_pool_conf [1].tag, MYCLIENT_SLAVE_LOCAL_HOST_TYPE);
	strcpy(test_pool_conf [2].tag, MYCLIENT_SLAVE_LOCAL_HOST_TYPE);
	strcpy(test_pool_conf [3].tag, MYCLIENT_SLAVE_REMOTE_HOST_TYPE);
	strcpy(test_pool_conf [4].tag, MYCLIENT_AVERAGE_HOST_TYPE);
	strcpy(test_pool_conf [5].tag, MYCLIENT_AVERAGE_HOST_TYPE);

	int ret = 0;
	MyclientPool * test_pool = new MyclientPool();
	if (test_pool == NULL)
	{
		printf("errror in init\n");
	}
	ret = test_pool->init(false);
    ret = test_pool->addServer("./conf", "queryf.conf");
	
	if (ret != 0)
	{
		printf("[%d]error in add server:%d", __LINE__, ret);
	}
    else
    {
        printf("successful!\n");
    }
	
	
    MyclientRes res;
    int err = 0;
    MysqlConnection *test_connection = test_pool->fetchConnection(MYCLIENT_MASTER_HOST_TYPE, &err, 0);
    if (NULL == test_connection)
    {
        printf("####error!\n");
    }
    else
    {
        printf("successful!\n");
        test_connection->query("use ddbs;");
        
	    
	        printf("use ddbs ok!\n");
    }
    printf("========================over!\n");
    getchar();
    test_pool->putBackConnection(test_connection, true);
    test_connection = test_pool->fetchConnection(MYCLIENT_MASTER_HOST_TYPE, &err, 0);
    if (NULL == test_connection){
        printf("Error!Cant' get another connection.\n");
    }else{
        printf("Success!\n");
         test_connection->query("use ddbs;");
    }
    test_pool->putBackConnection(test_connection, true);
    
    exit(0);
	for (int i = 0; i < 2; i++)
	{
		int err = 0;
		MysqlConnection* test_connection = NULL;
		
		
		
		
		
		{
			

			{
				printf("test average host \n");
				test_connection = test_pool->fetchConnection(MYCLIENT_AVERAGE_HOST_TYPE,
						&err, 0);
				if (test_connection == NULL)
				{
					printf("error in fetch:%d", err);
					exit(0);
				}
				test_connection->setFailHandler(fail_handler);
				printf("get connection ok!!!\n");
				test_connection->query(
						"CREATE TABLE IF NOT EXISTS `tblBook` (`id` int NOT NULL auto_increment, `name` varchar(100) NOT NULL, `author` varchar(100) NOT NULL, `price` int(11) default NULL, PRIMARY KEY  (`id`)) ENGINE=InnoDB");
				printf("create table ok!\n");
				test_connection->query(
						"insert into tblBook (name, author, price) values('AAA', 'zhangdongjin', 15)");
				printf("insert table value 1 ok!\n");

				const char
						* query_multi [] =
						{
								"delete from tblBook",
								"insert into tblBook (name, author, price) values('AAA', 'zhangdongjin', 15)",
								"insert into tblBook (name, author, price) values('BBB', 'zhangdongjin', 9)",
								"insert into tblBook (name, author, price) values('CCC', 'wanghao', 20)",
								"insert into tblBook (name, author, price) values('DDD', 'wanghao', 8)" };
				test_connection->queryInTransaction(query_multi, 5);

				printf("insert value ok!!!\n");
				ret = test_pool->putBackConnection(test_connection, false);
				if (ret != 0)
				{
					printf("putback connection  eror :%d", ret);
				}
				printf("put back connection ok!\n");
			}
		}
		
		{
			test_connection = test_pool->fetchConnection(MYCLIENT_AVERAGE_HOST_TYPE,
					&err, 0);
			if (test_connection == NULL)
			{
				printf("error in fetch:%d", err);
				exit(0);
			}
			test_connection->setFailHandler(fail_handler);
			printf("get connection ok!!!\n");
			char query [] = "select name, author from tblBook where price > 10";
			test_connection->query(query, &res, true);

			unsigned rows_count = res.getRowsCount();

			for (unsigned i = 0; i != rows_count; i++)
			{
				const char *name = res [i] [0];
				const char *author = res [i] [1];

				printf("%u:\t%s\t%s\n", i, name, author);
			}
			ret = test_pool->putBackConnection(test_connection, false);
			if (ret != 0)
			{
				printf("putback connection  eror :%d", ret);
			}
			printf("put back connection ok!\n");
		}
		
		{
			test_connection = test_pool->fetchConnection(MYCLIENT_AVERAGE_HOST_TYPE,
					&err, 0);
			if (test_connection == NULL)
			{
				printf("error in fetch:%d", err);
				exit(0);
			}
			test_connection->setFailHandler(fail_handler);
			printf("get connection ok!!!\n");
			const char *fmt =
					"select id, name from tblBook where price > %u and author = '%s'";
			test_connection->queryf(&res, true, fmt, 10, "zhangdongjin");
			res.__print();
			res.free();

			ret = test_pool->putBackConnection(test_connection, false);
			if (ret != 0)
			{
				printf("putback connection  eror :%d", ret);
			}
			printf("put back connection ok \n");
		}
	}
	test_pool->destroy();
	printf("test ok!!!\n");
	return 0;
}

int main()
{
	lc_log_init("./", "test.", 1500, 16);
	
	test_pool();
}
