
#include <lcclient_manager.h>
#include <gtest/gtest.h>
#include "lcclient_connectserver.h"




int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

class test_lc_client_galileo_suite : plclic ::testing::Test{
    protected:
        test_lc_client_galileo_suite(){};
        virtual ~test_lc_client_galileo_suite(){};
        virtual void SetUp() {
            
        };
        virtual void TearDown() {
            
        };
};



static void check_galileo_result(lc::LcClientManager *lcmgr, 
		const char *service_name,
		const char *ip, int port, int max_connect)
{
	printf("enter check_galileo_result\n");
	if(service_name == NULL) {
		lc::LcClientPool *pool = (lc::LcClientPool*)lcmgr->getServiceByName(service_name);
		ASSERT_EQ(pool, (lc::LcClientPool*)NULL);
		return;
	}

	
	lc::LcClientPool *pool = (lc::LcClientPool*)lcmgr->getServiceByName(service_name);
	ASSERT_NE(pool, (lc::LcClientPool*)NULL);
	lc::LcClientConnectManager *connmgr = (lc::LcClientConnectManager*)(pool->getManager());
	ASSERT_NE(connmgr, (lc::LcClientConnectManager*)NULL);

	ASSERT_EQ(pool->getRealServerCount(), 1);

	
	int server_count = pool->getServerCount();
	int errcode;
	lc::LcClientServer *server;
	for(int i = 0; i < server_count; i++) {
		server = pool->getServerByID(i);
		if(server->isEnable())
			break;
	}

	
	ASSERT_STREQ(server->getIP(),    ip);
	ASSERT_EQ(server->getPort(),     port);
	ASSERT_EQ(server->getCapacity(), max_connect);

	
	bsl::ResourcePool rp;
	bsl::var::IVar &conf = pool->getConf(&rp, &errcode);
	ASSERT_EQ(errcode, 0);
	

	ASSERT_STREQ(conf["Server"][0]["IP"].c_str(),               ip);
	ASSERT_EQ(atoi(conf["Server"][0]["MaxConnect"].c_str()), max_connect);
	ASSERT_EQ(atoi(conf["Server"][0]["Port"].c_str()),       port);
}


static void check_localbackup(const char *service_name,
		const char *ip, int port, int max_connect)
{
	
	printf("enter check_localbackup\n");
	comcfg::Configure local_backup_conf;
	ASSERT_EQ(local_backup_conf.load("./conf", "galileo-back.conf"), 0);

	int errcode;
	bsl::string local_name = local_backup_conf["LcClient"]["Service"][0]["Name"].to_bsl_string(&errcode);
	ASSERT_EQ(errcode, 0);
	ASSERT_STREQ(local_name.c_str(), service_name);

	bsl::string local_ip = local_backup_conf["LcClient"]["Service"][0]["Server"][0]["IP"].to_bsl_string(&errcode);
	ASSERT_EQ(errcode, 0);
	ASSERT_STREQ(local_ip.c_str(), ip);

	int local_max_connect = local_backup_conf["LcClient"]["Service"][0]["Server"][0]["MaxConnect"].to_int32(&errcode);
	ASSERT_TRUE(errcode == 0 && local_max_connect == max_connect);

	int local_port = local_backup_conf["LcClient"]["Service"][0]["Server"][0]["Port"].to_int32(&errcode);
	ASSERT_TRUE(errcode == 0 && local_port == port);
}


static void test_main(bool new_version)
{
	lc::LcClientManager *lcmgr = new lc::LcClientManager();

	
	system("rm -f ./conf/galileo-back.conf");

	const char *cfgfile = "galileo_oldversion.conf";
	if(new_version)
		cfgfile = "galileo_newversion.conf";
	
	
	lcmgr->init("./conf", cfgfile);

	const char *galileo_host = "10.81.11.84:2182";
	const char *galileo_path = "/yifei/echo";

	
	GalileoCluster *cluster = GalileoManager::get_instance()->get_cluster(galileo_host, false);
	ASSERT_NE(cluster, (GalileoCluster*)NULL);

	
	const char *service_name = "echo";
	const char *ip = "127.0.0.1";
	int port = 50001;
	int max_connect = 500;
	char resdata[512];
	int tag;

	sprintf(resdata, 
		"{\"name\":\"%s\", \"ip\":\"%s\", \"Port\":%d, \"Linger\" : 1, \"MaxConnect\":%d}",
		service_name, ip, port, max_connect);
	ASSERT_EQ(cluster->register_resource(galileo_path, resdata, &tag), 0);
	
	sleep(3);

	printf("check register.\n");
	check_galileo_result(lcmgr, service_name, ip, port, max_connect);
	check_localbackup(service_name, ip, port, max_connect);

	
	ASSERT_EQ(cluster->delete_resource(galileo_path, tag), 0);
	sleep(3);
	
	printf("check unregister.\n");
	check_galileo_result(lcmgr, NULL, ip, port, max_connect);

	
	service_name = "echo";
	ip = "192.168.1.1";
	port = 60000;
	max_connect = 600;	
	sprintf(resdata, 
		"{\"name\":\"%s\", \"ip\":\"%s\", \"Port\":%d, \"Linger\" : 1, \"MaxConnect\":%d}",
		service_name, ip, port, max_connect);
	ASSERT_EQ(cluster->register_resource(galileo_path, resdata, &tag), 0);
	
	sleep(3);

	printf("check re-register.\n");
	check_galileo_result(lcmgr, service_name, ip, port, max_connect);
	check_localbackup(service_name, ip, port, max_connect);

	printf("test reload....\n");
	
	
	
	
	
	const char *galileo_path2 = "/yifei/echo2";
	service_name = "echo2";
	ip = "192.168.1.2";
	port = 60001;
	max_connect = 601;	
	sprintf(resdata, 
		"{\"name\":\"%s\", \"ip\":\"%s\", \"Port\":%d, \"Linger\" : 1, \"MaxConnect\":%d}",
		service_name, ip, port, max_connect);
	int tag2;
	ASSERT_EQ(cluster->register_resource(galileo_path2, resdata, &tag2), 0);
	
	sleep(3);

	
	comcfg::Configure newconf;
	ASSERT_EQ(newconf.load("./conf", "galileo_reload.conf"), 0);
	int ret = lcmgr->resourceReload(newconf);
	printf("ret is %d\n", ret);

	
	check_galileo_result(lcmgr, service_name, ip, port, max_connect);
	check_localbackup(service_name, ip, port, max_connect);

	
	cluster->delete_watch(galileo_path);
	cluster->delete_watch(galileo_path2);
	ASSERT_EQ(cluster->delete_resource(galileo_path, tag), 0);
	ASSERT_EQ(cluster->delete_resource(galileo_path2, tag2), 0);
	lcmgr->close();
	delete lcmgr;
	GalileoManager::destroy_instance();
}



static void test_local_backup(bool new_version)
{
	lc::LcClientManager *lcmgr = new lc::LcClientManager();

	const char *cfgfile = "galileo_oldversion.conf";
	if(new_version)
		cfgfile = "galileo_newversion.conf";
	
	
	system("rm -f ./conf/galileo-back.conf");
	
	lcmgr->init("./conf", cfgfile);

	const char *galileo_host = "10.81.11.84:2182";
	const char *galileo_path = "/yifei/echo";

	
	GalileoCluster *cluster = GalileoManager::get_instance()->get_cluster(galileo_host, false);
	ASSERT_NE(cluster, (GalileoCluster*)NULL);

	const char *service_name = "echo";
	const char *ip = "127.0.0.1";
	int port = 60000;
	int max_connect = 500;
	char resdata[512];
	int tag;

	sprintf(resdata, 
		"{\"name\":\"%s\", \"ip\":\"%s\", \"Port\":%d, \"Linger\" : 1, \"MaxConnect\":%d}",
		service_name, ip, port, max_connect);
	ASSERT_EQ(cluster->register_resource(galileo_path, resdata, &tag), 0);
	
	sleep(3);

	check_galileo_result(lcmgr, service_name, ip, port, max_connect);
	check_localbackup(service_name, ip, port, max_connect);

	cluster->delete_watch(galileo_path);
	ASSERT_EQ(cluster->delete_resource(galileo_path, tag), 0);
	lcmgr->close();
	delete lcmgr;
	GalileoManager::destroy_instance();

	
	lcmgr = new lc::LcClientManager();

	const char *wrong_cfgfile = "galileo_oldversion_wrong.conf";
	if(new_version)
		wrong_cfgfile = "galileo_newversion_wrong.conf";

	
	lcmgr->init("./conf", wrong_cfgfile); 

	galileo_host = "10.81.11.84:3000";

	
	cluster = GalileoManager::get_instance()->get_cluster(galileo_host, false);
	ASSERT_EQ(cluster, (GalileoCluster*)NULL);

	check_galileo_result(lcmgr, service_name, ip, port, max_connect);
	check_localbackup(service_name, ip, port, max_connect);

	lcmgr->close();
	delete lcmgr;
	GalileoManager::destroy_instance();
}




static void test_local_config(bool new_version)
{
	lc::LcClientManager *lcmgr = new lc::LcClientManager();

	
	system("rm -f ./conf/galileo-back.conf");
	
	const char *wrong_cfgfile = "galileo_oldversion_wrong.conf";
	if(new_version)
		wrong_cfgfile = "galileo_newversion_wrong.conf";
	lcmgr->init("./conf", wrong_cfgfile);

	const char *galileo_host = "10.81.11.84:3000";

	
	GalileoCluster *cluster = GalileoManager::get_instance()->get_cluster(galileo_host, false);
	ASSERT_EQ(cluster, (GalileoCluster*)NULL);

	const char *service_name = "localname";
	const char *ip = "127.0.0.1";
	int port = 70000;
	int max_connect = 700;
	
	check_galileo_result(lcmgr, service_name, ip, port, max_connect);

	lcmgr->close();
	delete lcmgr;
	GalileoManager::destroy_instance();
}




TEST_F(test_lc_client_galileo_suite, case_oldversion)
{
	printf("runnint testcase 'oldverison'\n");
	test_main(false);
}

TEST_F(test_lc_client_galileo_suite, case_oldversion_local_backup)
{
	printf("runnint testcase 'oldverison_local_backup'\n");
	test_local_backup(false);
}

TEST_F(test_lc_client_galileo_suite, case_oldversion_local_config)
{
	printf("runnint testcase 'oldverison_local_config'\n");
	test_local_config(false);
}

 

TEST_F(test_lc_client_galileo_suite, case_newversion)
{
	printf("runnint testcase 'newverison'\n");
	test_main(true);
}

TEST_F(test_lc_client_galileo_suite, case_newversion_local_backup)
{
	printf("runnint testcase 'newverison_local_backup'\n");
	test_local_backup(true);
}

TEST_F(test_lc_client_galileo_suite, case_newversion_local_config)
{
	printf("runnint testcase 'newverison_local_config'\n");
	test_local_config(true);
}

