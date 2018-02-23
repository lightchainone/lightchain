

 #ifndef  __BSL_TEST_FUN_
 #define  __BSL_TEST_FUN_
 #include <cxxtest/TestSuite.h>
#include <string>
#include <Lsc/containers/hash/Lsc_readset.h>
#include <Lsc/archive/Lsc_binarchive.h>
#include <Lsc/archive/Lsc_filestream.h>
#include <Lsc/archive/Lsc_serialization.h>
#include <stdlib.h>
#include <time.h>
#include <set>

char s[][128]={"yingxiang", "yufan", "wangjiping", "xiaowei", "yujianjia", "maran", "baonenghui",
	"gonglei", "wangyue", "changxinglong", "chenxiaoming", "guoxingrong", "kuangyuheng"	
};
char t[][128]={"AAAA", "qsbbs", "yufa", "yingxian", "f*ck", "hello world!", "ixintui"};

const int N = sizeof(s) / sizeof(s[0]);
const int M = sizeof(t) / sizeof(t[0]);

class Lsc_test_fun : plclic CxxTest::TestSuite
{
	plclic:
	typedef std::string key;
	std::set <key> st;
	plclic:
		class hash_func{
		plclic:
			static const int BASE = 33;
			size_t operator()(const std::string& s) const {
				size_t i, t = 0;
				for(i = 0; i < s.size(); i++){
					t *= BASE;
					t += s[i];
				}
				return t;
			}
		};
		void test_operator() {
			{
				Lsc::readset<key> rst;
				init();
				rst.assign(st.begin(),st.end());
				Lsc::readset<key> rst2;
				rst2 = rst;
				TS_ASSERT(rst2.size() == rst.size());
				TS_ASSERT(rst.size() == st.size());
			}
			{
				std::set<key> st;
				Lsc::readset<key> rst;
				rst.assign(st.begin(),st.end());
			}
			{
				Lsc::readset<key> rst;
				init();
				rst.assign(st.begin(),st.end());
				Lsc::readset<key> rst2(rst);
				TS_ASSERT(rst2.size() == rst.size());
				TS_ASSERT(rst.size() == st.size());
			}
			{
				Lsc::readset<key,hash_func> rst;
				init();
				rst.assign(st.begin(),st.end());
				int i;
				Lsc::readset<key,hash_func> rst2(rst);
				for (i = 0; i < N; i ++) {
					int ret = rst.get(key(s[i]));
					TS_ASSERT( ret == Lsc::HASH_EXIST );
					ret = rst2.get(key(s[i]));
					TS_ASSERT(ret == Lsc::HASH_EXIST);
				}
			}
			{
				try {
					Lsc::readset<key> rst(0);
					Lsc::readset<key> rst2(rst);
				} catch (Lsc::Exception& e) {
					printf("\n==\n%s\n==\n",e.all());
				}
			}
		}
		void test_create() {
			Lsc::readset<key> rst;
			for (int i = 0; i < 100; i ++) {
				rst.create();
			}
			Lsc::readset<key> rst2;
			for (int i = 0; i < 10; i ++) {
				rst = rst2;
			}
		}
		void test_norsal_1(void) 
		{   
			int val = 0;
			TSM_ASSERT(val, val == 0);
		}

		void test_rs1(void){
			Lsc::readset<key> rst;
			int ret = rst.create();
			TSM_ASSERT(ret, ret == 0);
		}

		void test_rs2(void){
			Lsc::readset<key, hash_func> rst;
			int ret = rst.create();
			TSM_ASSERT(ret, ret == 0);
		}

		void test_rs3(void){
			Lsc::readset<key, hash_func> rst;
			int ret = rst.create();
			TSM_ASSERT(ret, ret == 0);
			init();
			ret = rst.assign(st.begin(), st.end());
			TSM_ASSERT(ret, ret == 0);
			TSM_ASSERT(ret, rst.size() == st.size());
		}

		void test_rs4(void){
			Lsc::readset<key, hash_func> rst;
			int ret = rst.create();
			TSM_ASSERT(ret, ret == 0);
			init();
			ret = rst.assign(st.begin(), st.end());
			int i;
			for(i = 0; i < 100; i++){
				key k = rnd_item();
				ret = rst.get(k);

				if(st.find(k) != st.end()){
					TSM_ASSERT(ret, ret == Lsc::HASH_EXIST);
				}
				else{
					TSM_ASSERT(ret, ret = Lsc::HASH_NOEXIST);
				}
			}
		}   

		void test_rs5(void){
			Lsc::readset<key, hash_func> rst;
			int ret = rst.create();
			TSM_ASSERT(ret, ret == 0);
			init();
			ret = rst.assign(st.begin(), st.end());
			int i;
			for(i = 0; i < N; i++){
				ret = rst.get(key(s[i]));
				TSM_ASSERT(ret, ret = Lsc::HASH_EXIST);
			}
		}

		void test_rs6(void){
			Lsc::readset<key, hash_func> rst;
			int ret = rst.create();
			TSM_ASSERT(ret, ret == 0);
			init();
			ret = rst.assign(st.begin(), st.end());
			int i;
			for(i = 0; i < M; i++){
				ret = rst.get(key(t[i]));
				TSM_ASSERT(ret, ret = Lsc::HASH_NOEXIST);
			}
		}

		void test_rs7(void){
			Lsc::readset<key, hash_func> rst;
			int ret = rst.create(12345);
			TSM_ASSERT(ret, ret == 0);
			key p[N];
			int i;
			key k;
			for(i = 0; i < N; i++) {
				p[i] = s[i];
			}
			ret = rst.assign(p, p+N);
			TSM_ASSERT(ret, ret == 0);
			for(i = 0; i < N; i++){
				ret = rst.get(key(s[i]));
				TSM_ASSERT(ret, ret == Lsc::HASH_EXIST);
			}
			TS_ASSERT(rst.size() == (size_t)N);
			Lsc::readset<key, hash_func> rst2;
			TS_ASSERT(rst2.create() == 0);
			TS_ASSERT(rst2.assign(rst.begin(), rst.end()) == 0);
			TS_ASSERT(rst2.size() == (size_t)N);
			rst.clear();
			TS_ASSERT(rst.size() == 0); 
			TS_ASSERT(rst.bucket_size() == 12345);
			rst.destroy();
			TS_ASSERT(rst.bucket_size() == 0); 
		}

		void test_rs8(void){
			Lsc::readset<key, hash_func> rst;
			int ret = rst.create();
			TSM_ASSERT(ret, ret == 0);
			key p[N];
			int i;
			key k;
			for(i = 0; i < N; i++) {
				p[i] = key(s[i]);
			}
			ret = rst.assign(p, p+N);
			TSM_ASSERT(ret, ret == 0);
			for(i = 0; i < N; i++){
				ret = rst.get(key(s[i]));
				TSM_ASSERT(ret, ret == Lsc::HASH_EXIST);
			}
			Lsc::filestream fs; 
			ret = fs.open("hashset.dat", "w");
			TSM_ASSERT(ret, ret == 0); 
			Lsc::binarchive ar(fs);
			ret = ar.write(rst);
			TSM_ASSERT(ret, ret == 0); 
			fs.close();
		}

		void test_rs9(){
			test_rs8();
			Lsc::readset<key, hash_func> rst;
			int ret = rst.create();
			TS_ASSERT(ret == 0);

			Lsc::filestream fs;
			ret = fs.open("hashset.dat", "r");
			TSM_ASSERT(ret, ret == 0);

			Lsc::binarchive ar(fs);
			ret = ar.read(&rst);
			TSM_ASSERT(ret, ret == 0);
			fs.close();

			int i;
			for(i = 0; i < N; i++){
				ret = rst.get(key(s[i]));
				TSM_ASSERT(ret, ret == Lsc::HASH_EXIST);
			}
			ret = get_const(&rst);
			TSM_ASSERT(ret, 0 == ret);
		}

		void test_rs10(){
			test_rs8();
			Lsc::readset<key, hash_func> rst;
			int ret = rst.create();
			FILE * fp = fopen("failtest", "w");
			fclose(fp);
			TS_ASSERT(ret == 0);

			Lsc::filestream fs; 
			ret = fs.open("failtest", "r");
			TSM_ASSERT(ret, ret == 0); 

			Lsc::binarchive ar(fs);
			ret = ar.read(&rst);
			TSM_ASSERT(ret, ret != 0); 
			fs.close();
		}
	private:
		key rnd_item(){
			srand(time(NULL));
			return (key)(s[rand() % N]);
		}
		void init(){
			int i;
			for(i = 0; i < N; i++){
				st.insert((key)s[i]);
			}
		}

		int get_const(const Lsc::readset<key, hash_func> * rst){
			for(int i = 0; i < N; i++){
				int ret = rst->get(key(s[i]));
				TSM_ASSERT(ret, ret == Lsc::HASH_EXIST);
			}
			return 0;
		}


};


#endif
















/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
