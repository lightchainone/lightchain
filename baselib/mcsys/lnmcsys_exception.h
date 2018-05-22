#ifndef  __LNMCSYS_EXCEPTION_H_
#define  __LNMCSYS_EXCEPTION_H_

#include "Lsc/exception.h"
#include "utils.h"
#include "def.h"

namespace lnsys {
    enum lnsys_e_temporary {
        LNSYS_E_NOT_GROUP = 2006,
    };
}



#define THROW_BREAK(fmt...) THROW(lnsys::LNSYS_E_OK, ##fmt) 
#define THROW_SYS(fmt...) THROW(lnsys::LNSYS_E_SYS, ##fmt)
#define THROW_PARAM(fmt...) THROW(lnsys::LNSYS_E_PARAM, ##fmt)
#define THROW_ZCACHE(fmt...) THROW(lnsys::LNSYS_E_ZCACHE, ##fmt)
#define THROW_MYSQL(fmt...) THROW(lnsys::LNSYS_E_MYSQL, ##fmt)
#define THROW_AUTH(fmt...) THROW(lnsys::LNSYS_E_AUTH, ##fmt)
#define THROW_QUOTA(fmt...) THROW(lnsys::LNSYS_E_QUOTA, ##fmt)
#define THROW_MINUTE_QUOTA(fmt...) THROW(lnsys::LNSYS_E_MINUTE_QUOTA, ##fmt);
#define THROW_NOT_FOUND(fmt...) THROW(lnsys::LNSYS_E_NOT_FOUND, ##fmt)
#define THROW_TOKEN_EXPIRED(fmt...) THROW(lnsys::LNSYS_E_TOKEN_EXPIRED, ##fmt)
#define THROW_NOT_BIND(fmt...) THROW(lnsys::LNSYS_E_NOT_BIND, ##fmt)
#define THROW_DUPLICATE(fmt...) THROW(lnsys::LNSYS_E_DUPLICATE, ##fmt)
#define THROW_REDIS(fmt...) THROW(lnsys::LNSYS_E_REDIS, ##fmt)
#define THROW_VALUE_FORMAT(fmt...) THROW(lnsys::LNSYS_VALUE_FORMAT, ##fmt)
#define THROW_NO_BINDINFO(fmt...) THROW(lnsys::LNSYS_REDIS_NO_BINDINFO, ##fmt)
#define THROW_BIND_REPEATED(fmt...) THROW(lnsys::LNSYS_BIND_REPEATED, ##fmt)
#define THROW_NOT_GROUP(fmt...) THROW(lnsys::LNSYS_E_NOT_GROUP, ##fmt)





#define ASSERT_BREAK(expr, out_put, fmt, ...) \
    if(!expr) \
    { \
        out_put(fmt".", ##__VA_ARGS__); \
        break; \
    }
#define ASSERT_LONG_BREAK(expr, fmt, ...) DO_IF(!(expr), THROW_BREAK(fmt, ##__VA_ARGS__))
#define ASSERT_SYS(expr, fmt, ...) DO_IF(!(expr), THROW_SYS(fmt, ##__VA_ARGS__))
#define ASSERT_PARAM(expr, fmt, ...) DO_IF(!(expr), THROW_PARAM(fmt, ##__VA_ARGS__))
#define ASSERT_ZCACHE(expr, fmt, ...) DO_IF(!(expr), THROW_ZCACHE(fmt, ##__VA_ARGS__))
#define ASSERT_MYSQL(expr, fmt, ...) DO_IF(!(expr), THROW_MYSQL(fmt, ##__VA_ARGS__))
#define ASSERT_AUTH(expr, fmt, ...) DO_IF(!(expr), THROW_AUTH(fmt, ##__VA_ARGS__))
#define ASSERT_QUOTA(expr, fmt, ...) DO_IF(!(expr), THROW_QUOTA(fmt, ##__VA_ARGS__))
#define ASSERT_MINUTE_QUOTA(expr, fmt, ...) DO_IF(!(expr), THROW_MINUTE_QUOTA(fmt, ##__VA_ARGS__))
#define ASSERT_NOT_FOUND(expr, fmt, ...) DO_IF(!(expr), THROW_NOT_FOUND(fmt, ##__VA_ARGS__))
#define ASSERT_TOKEN_EFFECTIVE(expr, fmt, ...) DO_IF(!(expr), THROW_TOKEN_EXPIRED(fmt, ##__VA_ARGS__))
#define ASSERT_NOT_BIND(expr, fmt, ...) DO_IF(!(expr), THROW_NOT_BIND(fmt, ##__VA_ARGS__))
#define ASSERT_DUPLICATE(expr, fmt, ...) DO_IF(!(expr), THROW_DUPLICATE(fmt, ##__VA_ARGS__))
#define ASSERT_REDIS(expr, fmt, ...) DO_IF(!(expr), THROW_REDIS(fmt, ##__VA_ARGS__))
#define ASSERT_NOT_FOUND_GROUP(expr, fmt, ...) DO_IF(!(expr), THROW_NOT_GROUP(fmt, ##__VA_ARGS__))





#define EXCEPTION_STR_MAX_LEN 1024    
namespace lnsys
{
    
    
    
    class lnsys_exception: plclic std::exception
    {
    plclic:
        
        
        
        lnsys_exception()
        {
            set_error(lnsys::LNSYS_E_OK);
            set_file("<unknown-file>");
            set_function("<unknown-function>");
            set_line(0);
            set_what("");
        }
        
        
        
        lnsys_exception(unsigned int line, const char *file, const char *function, const char *fmt, ...)
        {
            set_line(line);
            set_file(file);
            set_function(function);
            va_list p;
            va_start(p, fmt);
            set_what(fmt, p);
            va_end(p);
        }   
        
        
        
        lnsys_exception(const char *fmt, ...)
        {
            va_list p;
            va_start(p, fmt);
            set_what(fmt, p);
            va_end(p);
        }
        
        
        
        void set_what(const char *fmt, ...)
        {
            if(NULL == fmt || 0 == strlen(fmt))
            {
                _errmcinfo[0] = 0x00;
                return ;
            }
            va_list p;
            va_start(p, fmt);
            vsnprintf(_errmcinfo, sizeof(_errmcinfo), fmt, p);
            va_end(p);
        }
        
        
        
        void set_file(const char *file)
        {
            if(NULL != file || 0 < strlen(file))
            {
                snprintf(_file, sizeof(_file), "%s", file);
            }
        }
        
        
        
        void set_function(const char *function)
        {       
            if(NULL != function || 0 < strlen(function))
            {
                snprintf(_function, sizeof(_function), "%s", function);
            }
        }
        
        
        
        void set_line(unsigned int line)
        {
            _line = line;
        }
        
        
        
        void set_error(int error_code)
        {
            _error_code = error_code;
        }
        
        
        
        virtual const char *what() const throw()
        {
            return _errmcinfo;
        }
        
        
        
        virtual const char *file() const throw()
        {
            return _file;
        }
        
        
        
        virtual const char *function() const throw()
        {
            return _function;
        }
        
        
        
        virtual unsigned int line() const throw()
        {
            return _line;
        }
        
        
        
        virtual int get_error() const throw()
        {
            return _error_code;
        }
    protected:
        char _file[PATH_MAX];                   
        char _function[PATH_MAX];               
        unsigned int _line;                     
        int _error_code;                        
        char _errmcinfo[EXCEPTION_STR_MAX_LEN];    
    };
};





#define PREPARE_ERROR_STRING(error_code, right_output, error_output) \
    char buf[EXCEPTION_STR_MAX_LEN]; \
    snprintf(buf, sizeof(buf), "%s", 0 == strcmp(ex.what(), "") ? "an unexpect error occured" : ex.what()); \
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "%s", " BREAK_POINT_INFO["); \
    if(0 != strcmp("<unknown-file>", ex.file())) \
    { \
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "file: %s, ", ex.file()); \
    } \
    if(0 < ex.line()) \
    { \
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "line: %d, ", ex.line()); \
    } \
    if(0 != strcmp("<unknown-function>", ex.function())) \
    { \
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "function: %s, ", ex.function()); \
    } \
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "error_code: %d]", (error_code)); \
    if(lnsys::LNSYS_E_OK == error_code) \
    { \
        right_output("%s.", buf); \
    } \
    else \
    { \
        error_output("%s.", buf); \
    } \
    __func_ret_code_by_inf_iis_not_same_as_other_variable__ = (error_code); \




#define FUNC_START \
    int __func_ret_code_by_inf_iis_not_same_as_other_variable__ = lnsys::LNSYS_E_SYS; \
    try \
    { \
        do \
        {




#define FUNC_END(right_output, error_output) \
        }while(false); \
        __func_ret_code_by_inf_iis_not_same_as_other_variable__ = lnsys::LNSYS_E_OK; \
        right_output("main logic was executed successfully [file: %s, function: %s].", __FILE__, __FUNCTION__); \
    } \
    catch(lnsys::lnsys_exception &ex) \
    { \
        PREPARE_ERROR_STRING(ex.get_error(), right_output, error_output); \
    } \
    catch(Lsc::Exception &ex) \
    { \
        PREPARE_ERROR_STRING(__func_ret_code_by_inf_iis_not_same_as_other_variable__, right_output, error_output); \
    }\
    catch(...) \
    { \
        error_output("an error occured [error_info: unknown, error_code: %d].", \
            __func_ret_code_by_inf_iis_not_same_as_other_variable__); \
    }




#define THROW(error_code, fmt, ...) \
    do \
    { \
        lnsys::lnsys_exception tmp; \
        tmp.set_file(__FILE__); \
        tmp.set_line(__LINE__); \
        tmp.set_error(error_code); \
        tmp.set_function(__FUNCTION__); \
        tmp.set_what(fmt, ##__VA_ARGS__); \
        throw tmp; \
    }while(false)     
#define THROW_BSL(fmt...) \
    do \
    { \
        char buf[EXCEPTION_STR_MAX_LEN]; \
        snprintf(buf, sizeof(buf), ##fmt); \
        throw Lsc::Exception() << buf << BSL_EARG; \
    }while(false)




#define FUNC_GET_ERROR(result) \
    result = __func_ret_code_by_inf_iis_not_same_as_other_variable__;




#define FUNC_RETURN \
    return __func_ret_code_by_inf_iis_not_same_as_other_variable__;   


#endif  


