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

    
    
    
    char *LnmcsysDb::escape_strings(unsigned int num, ...)
    {
        _escape_buf[0] = 0x00;
        va_list ap;
        va_start(ap, num);
        char *start = _escape_buf;
        long long left = LNMCSYS_MAX_LEN_MYSQL_ESCAPE_BUF;
        for(unsigned int i = 0; i < num; i++)
        {
            const char *str = va_arg(ap, const char *);
            long long now_len = strlen(str);
            ASSERT_PARAM(left >= static_cast<long long>(now_len * 2 + 1), 
                    "invalid param too long, escape buffer was used up [now_str: %s, now_str_len: %lld, "
                    "escape_buffer_left: %lu]", str, now_len, left);
            int ret = _mysql_conn->escapeString(start, str, static_cast<unsigned long>(left), now_len);
            ASSERT_MYSQL(ret >= 0, "escape string failed [mysql_conn: %p, str: %s, ret: %d]", _mysql_conn, str, ret);
            DEBUG("%s succ [str: %s, result: %s]", __FUNCTION__, str, start);
            long long offset = strlen(start) + 1;
            left = left - offset;
            start = start + offset;
        }
        va_end(ap);
        return _escape_buf;
    }

    char *LnmcsysDb::escape_binary(const void *src, const uint32_t size)
    {
        _escape_buf[0] = 0x00;
        char *start = _escape_buf;
        
        ASSERT_PARAM(LNMCSYS_MAX_LEN_MYSQL_ESCAPE_BUF >= static_cast<long long>(size * 2 + 1), "invalid param too long, escape buffer was used up[now_src_len: %u, escape_buffer_len: %lld]", size, LNMCSYS_MAX_LEN_MYSQL_ESCAPE_BUF);
        
        int ret = _mysql_conn->escapeString(start, static_cast<const char *>(src), LNMCSYS_MAX_LEN_MYSQL_ESCAPE_BUF, size);

        ASSERT_MYSQL(ret >= 0, "escape string failed [mysql_conn: %p, ret: %d]", _mysql_conn, ret);
        DEBUG("%s succ [result: %s]", __FUNCTION__, start);

        
        return _escape_buf;
    }

    
    
    
    int LnmcsysDb::_pre_process_sql(std::string& sql, const std::string& tb_no_pattern, unsigned long long table_no)
    {
        int count = 0;
        char table_no_buf[21];
        snprintf(table_no_buf, sizeof(table_no_buf), "%llu", table_no);
        std::string tb_no = table_no_buf;
        const size_t nsize = tb_no.size();
        const size_t psize = tb_no_pattern.size();
        for(size_t pos = sql.find(tb_no_pattern, 0); pos != std::string::npos; pos = sql.find(tb_no_pattern, pos + nsize))
        {   
            sql.replace(pos, psize, tb_no);
            count++;
        }
        return count;
    }

    MyclientRes &LnmcsysDb::_query(int *affect_row)
    {
        _res->free();
        int ret = _mysql_worker->query(_res, _mysql_conn, _sql_buf);
        if(ret < 0)
        {
            _re_conn = true; 
            _sql_buf[0] = 0x00;
            THROW_MYSQL("execute sql failed [ret: %d]", ret);
        }
        DO_IF(NULL != affect_row, *affect_row = ret);
        TRACE("%s succ [affect_row: %d].", __FUNCTION__, ret);
        _sql_buf[0] = 0x00;
        return *_res;
    }

    
    
    
    MyclientRes &LnmcsysDb::_query_nothrow(int *affect_row)
    {
        _res->free();
        int ret = _mysql_worker->query(_res, _mysql_conn, _sql_buf);
        DO_IF(0 > ret, WARNING("execute sql failed, will reset LnmcsysDb [ret: %d]", ret), _sql_buf[0] = 0x00, _reset(true, false));
        DO_IF(NULL != affect_row, *affect_row = ret);
        DO_IF(0 <= ret, TRACE("%s succ [affect_row: %d].", __FUNCTION__, ret), _sql_buf[0] = 0x00);
        return *_res;
    }

    
    
    
    unsigned long long LnmcsysDb::_get_table_no()
    {
        unsigned long long table_no = 0;
        if(LNMCSYS_DB_TYPE_ID == _db_type)
        {
            table_no = _id % (_tb_num * _db_num) % _tb_num;
            return table_no;
        }
        if(LNMCSYS_DB_TYPE_TAG == _db_type)
        {
            table_no = sign64(_tag, strlen(_tag)) % (_tb_num * _db_num) % _tb_num;
            return table_no;
        }
        if ( LNMCSYS_DB_TYPE_DBID == _db_type) 
        {
            
            return 0;
        }
        THROW_SYS("invalid database type [db_type: %u]", _db_type);
        return 0; 
    }

}























