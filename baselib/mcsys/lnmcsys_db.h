#ifndef  __LNMCSYS_DB_H_
#define  __LNMCSYS_DB_H_



#include "def.h"
#include "utils.h"
#include "lnmcsys_exception.h"
#include "dbworker_include.h"
namespace lnsys
{
        const unsigned long long LNMCSYS_MAX_LEN_MYSQL_BUF = 3 * 1024 * 1024;                  
        const unsigned long long LNMCSYS_MAX_LEN_MYSQL_ESCAPE_BUF = 3 * 1024 * 1024;   
        extern pthread_once_t random_is_initialized;                                                                    
        
        
        
        enum lnmcsys_db_type_t
        {
                LNMCSYS_DB_TYPE_ID = 0,                        
                LNMCSYS_DB_TYPE_TAG = 1,                       
                LNMCSYS_DB_TYPE_DBID = 2,
                LNMCSYS_DB_TYPE_INVALID = 3            
        };
        
        
        
        class LnmcsysDb
        {
        plclic:
                
                
                
                LnmcsysDb(LNSYSMySQLWorker *mysql_worker, unsigned long long id, unsigned int conn_to, unsigned long long db_num, unsigned long long tb_num);
                
                
                
                LnmcsysDb(LNSYSMySQLWorker *mysql_worker, const char *tag, unsigned int conn_to, unsigned long long db_num, unsigned long long tb_num);

                
                
                LnmcsysDb(LNSYSMySQLWorker *mysql_worker, unsigned long long dbid, unsigned int conn_to);
                
                
                
                void start_transaction();
                
                
                
                void end_transaction(bool commit = false);
                
                
                
                MyclientRes &query(const char *sql, ...);       
                
                
                
                MyclientRes &query(int &affect_row, const char *sql, ...);      
                
                
                
                MyclientRes &query_nothrow(const char *sql, ...);
                
                
                
                MyclientRes &query_nothrow(int &affect_row, const char *sql, ...);
                
                
                
                char *escape_strings(unsigned int num, ...);

                
                char *escape_binary(const void *src, const uint32_t size);
                
                
                
                ~LnmcsysDb();

        plclic:
                
                
                
                static void lnmcsys_db_init_key();
                
                
                
                static void lnmcsys_db_destroy_key();

                uint64_t get_db_index();

                uint64_t get_table_index();

        private:
                
                
                
                int _pre_process_sql(std::string& sql, const std::string& tb_no_pattern, unsigned long long table_no);
                
                
                
                MyclientRes &_query(int *affect_row = NULL);
                
                
                
                MyclientRes &_query_nothrow(int *affect_row = NULL);
                
                
                
                unsigned long long _get_table_no();
                
                
                
                void _reset(bool re_conn = false, bool commit = false);
                
                
                
                void _clean_member();
                
                
                
                void _release_connection(bool re_conn = false);
                
                
                
                void _destroy(bool re_conn = false, bool commit = false);


        private:
                
                
                
                static void _lnmcsys_db_init_key();
                
                
                
                static void _free(void *lparam);
                
                
                
                static void _free_array(void *lparam);
                
                
                
                char *_get_mysql_buf();
                
                
                
                MyclientRes *_get_mysql_res();
                
                
                
                char *_get_mysql_escape_buf();

                uint64_t _get_server_index(const char* hash_name);

                uint64_t _get_server_index(uint64_t uid);

        private:
                unsigned long long _db_num;                                                                     
                unsigned long long _tb_num;                                                                     
                unsigned long long _table_no;                                                           
                MysqlConnection *_mysql_conn;                                                           
                LNSYSMySQLWorker *_mysql_worker;                                                            
                short _db_type;                                                                                         
                char _tag[LNSYS_MAX_LEN_APPID + LNSYS_MAX_LEN_USER_ID + 2];     
                unsigned long long _id;                                                                         
                MyclientRes *_res;                                                                                      
                char *_sql_buf;                                                                                         
                char *_escape_buf;                                                                                      
                bool _re_conn;                                                                                          
                unsigned int _conn_to;                                                                          
                bool _transaction_on;                                                                           

        private:
                static pthread_key_t _mysql_buf_key;                                            
                static pthread_key_t _mysql_res_key;                                            
                static pthread_key_t _mysql_escape_buf_key;                                     
        };
};

#endif  


