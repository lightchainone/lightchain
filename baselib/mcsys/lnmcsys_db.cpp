#include "lnmcsys_db.h"
#include "pthread.h"

namespace lnsys
{
    pthread_once_t random_is_initialized = PTHREAD_ONCE_INIT;
    pthread_key_t LnmcsysDb::_mysql_buf_key;
    pthread_key_t LnmcsysDb::_mysql_res_key;
    pthread_key_t LnmcsysDb::_mysql_escape_buf_key;

    
    
    
    void LnmcsysDb::lnmcsys_db_init_key()
    {
        ASSERT_SYS(0 == pthread_once(&random_is_initialized, &_lnmcsys_db_init_key), 
                "init lnmcsys db keys failed [error_info: %m]");
    }
    
    
    
    
    void LnmcsysDb::lnmcsys_db_destroy_key()
    {
        pthread_key_delete(_mysql_buf_key);
        pthread_key_delete(_mysql_res_key);
        pthread_key_delete(_mysql_escape_buf_key);
        TRACE("%s db key succ.", __FUNCTION__);
    }
    
    
    
    
    LnmcsysDb::LnmcsysDb(LNSYSMySQLWorker *mysql_worker, unsigned long long id, 
                    unsigned int conn_to, unsigned long long db_num, unsigned long long tb_num)
    {
        _clean_member();
        
        lnmcsys_db_init_key();  
        ASSERT_SYS(NULL != mysql_worker, "invalid param for LnmcsysDb constructor [mysql_worker: %p]", mysql_worker);
        _mysql_conn = mysql_worker->get_connection_by_id(id, conn_to);
        ASSERT_MYSQL(NULL != _mysql_conn, "construct LnmcsysDb failed, "
                        "get connection failed [id: %llu, timeout: %u]", id, conn_to);
        _mysql_worker = mysql_worker;
        _db_type = LNMCSYS_DB_TYPE_ID;
        _id = id;
        _sql_buf = _get_mysql_buf();
        _escape_buf = _get_mysql_escape_buf();
        _res = _get_mysql_res();
        _res->free();
        _conn_to = conn_to;
        _db_num = db_num;
        _tb_num = tb_num;
        ASSERT_SYS(0 != _db_num && 0 != _tb_num, "invalid conf for msyql_num and mysql_partion_num "
                    "[msyql_num: %llu, mysql_partion_num: %llu]", _db_num, _tb_num);
        _table_no = _get_table_no();
        DEBUG("construct LnmcsysDb by ID succ [mysql_worker: %p, mysql_conn: %p, id: %llu, conn_to: %u, db_num: %llu, "
                    "tb_num: %llu, table_no: %llu].", _mysql_worker, _mysql_conn, _id, _conn_to, _db_num, _tb_num, _table_no);
    }
    
    
    
    
    LnmcsysDb::LnmcsysDb(LNSYSMySQLWorker *mysql_worker, const char *tag, 
                        unsigned int conn_to, unsigned long long db_num, unsigned long long tb_num)
    {
        _clean_member();
        
        lnmcsys_db_init_key();
        ASSERT_SYS(NULL != mysql_worker, "invalid param for LnmcsysDb constructor [mysql_worker: %p]", mysql_worker);
        _mysql_conn = mysql_worker->get_connection(tag, conn_to);
        ASSERT_MYSQL(NULL != _mysql_conn, "construct LnmcsysDb failed, "
                    "get connection failed [tag: %s, timeout: %u]", tag, conn_to);
        _mysql_worker = mysql_worker;
        _db_type = LNMCSYS_DB_TYPE_TAG;
        SNPRINTF(_tag, "%s", tag);
        _sql_buf = _get_mysql_buf();
        _escape_buf = _get_mysql_escape_buf();
        _res = _get_mysql_res();
        _res->free();
        _conn_to = conn_to;
        _db_num = db_num;
        _tb_num = tb_num;
        ASSERT_SYS(0 != _db_num && 0 != _tb_num, "invalid conf for msyql_num and mysql_partion_num "
                    "[msyql_num: %llu, mysql_partion_num: %llu]", _db_num, _tb_num);
        _table_no = _get_table_no();
        DEBUG("construct LnmcsysDb by TAG succ [mysql_worker: %p, mysql_conn: %p, tag: %s, conn_to: %u, db_num: %llu, "
                    "tb_num: %llu, table_no: %llu].", _mysql_worker, _mysql_conn, _tag, _conn_to, _db_num, _tb_num, _table_no);
    }

    LnmcsysDb::LnmcsysDb(LNSYSMySQLWorker *mysql_worker, unsigned long long dbid, unsigned int conn_to) {
        _clean_member();
        
        lnmcsys_db_init_key();
        ASSERT_SYS(NULL != mysql_worker, "invalid param for LnmcsysDb constructor [mysql_worker: %p]", mysql_worker);
        _mysql_conn = mysql_worker->get_connection(dbid, conn_to);
        ASSERT_MYSQL(NULL != _mysql_conn, "construct LnmcsysDb failed, "
                    "get connection failed [dbid: %llu, timeout: %u]", dbid, conn_to);
        _mysql_worker = mysql_worker;
        _db_type = LNMCSYS_DB_TYPE_DBID;
        
        _id = dbid;
        _sql_buf = _get_mysql_buf();
        _escape_buf = _get_mysql_escape_buf();
        _res = _get_mysql_res();
        _res->free();
        _conn_to = conn_to;
        _db_num = 1;
        _tb_num = 1;
        
        
        ASSERT_SYS(0 != _db_num && 0 != _tb_num, "invalid conf for msyql_num and mysql_partion_num "
                    "[msyql_num: %llu, mysql_partion_num: %llu]", _db_num, _tb_num);
        _table_no = _get_table_no();
        DEBUG("construct LnmcsysDb by TAG succ [mysql_worker: %p, mysql_conn: %p, dbid: %llu, conn_to: %u, db_num: %llu, "
                    "tb_num: %llu, table_no: %llu].", _mysql_worker, _mysql_conn,  _id, _conn_to, _db_num, _tb_num, _table_no);     
    }

    
    
    
    MyclientRes &LnmcsysDb::query(const char *sql, ...)
    {
        
        va_list p;
        va_start(p, sql);
        vsnprintf(_sql_buf, LNMCSYS_MAX_LEN_MYSQL_BUF, sql, p);
        va_end(p);

        std::string _sql = _sql_buf;
        std::string pattern;
        pattern = "${TABLE_NO}";
        _pre_process_sql(_sql, pattern, _table_no);
        pattern = "$TABLE_NO";
        _pre_process_sql(_sql, pattern, _table_no);
        snprintf(_sql_buf, LNMCSYS_MAX_LEN_MYSQL_BUF, "%s", _sql.c_str());
        return _query(NULL);
    }

    
    
    
    MyclientRes &LnmcsysDb::query(int &affect_row, const char *sql, ...)
    {
        va_list p;
        va_start(p, sql);
        vsnprintf(_sql_buf, LNMCSYS_MAX_LEN_MYSQL_BUF, sql, p);
        va_end(p);

        std::string _sql = _sql_buf;
        std::string pattern;
        pattern = "${TABLE_NO}";
        _pre_process_sql(_sql, pattern, _table_no);
        pattern = "$TABLE_NO";
        _pre_process_sql(_sql, pattern, _table_no);
        snprintf(_sql_buf, LNMCSYS_MAX_LEN_MYSQL_BUF, "%s", _sql.c_str());
        return _query(&affect_row);
    }

    
    
    
    MyclientRes &LnmcsysDb::query_nothrow(const char *sql, ...)
    {
        va_list p;
        va_start(p, sql);
        vsnprintf(_sql_buf, LNMCSYS_MAX_LEN_MYSQL_BUF, sql, p);
        va_end(p);

        std::string _sql = _sql_buf;
        std::string pattern = "${TABLE_NO}";
        _pre_process_sql(_sql, pattern, _table_no);
        pattern = "$TABLE_NO";
        _pre_process_sql(_sql, pattern, _table_no);
        snprintf(_sql_buf, LNMCSYS_MAX_LEN_MYSQL_BUF, "%s", _sql.c_str());
        return _query_nothrow(NULL);
    }

    
    
    
    MyclientRes &LnmcsysDb::query_nothrow(int &affect_row, const char *sql, ...)
    {
        va_list p;
        va_start(p, sql);
        vsnprintf(_sql_buf, LNMCSYS_MAX_LEN_MYSQL_BUF, sql, p);
        va_end(p);

        std::string _sql = _sql_buf;
        std::string pattern = "${TABLE_NO}";
        _pre_process_sql(_sql, pattern, _table_no);
        pattern = "$TABLE_NO";
        _pre_process_sql(_sql, pattern, _table_no);
        snprintf(_sql_buf, LNMCSYS_MAX_LEN_MYSQL_BUF, "%s", _sql.c_str());
        return _query_nothrow(&affect_row);
    }

    
}























