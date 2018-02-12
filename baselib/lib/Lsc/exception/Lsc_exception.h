
#ifndef  __BSL_EXCEPTION_H__
#define  __BSL_EXCEPTION_H__

#include<exception>
#include<cstdio>
#include<iostream>
#include<typeinfo>
#include<cstdarg>
#include"Lsc/AutoBuffer.h"
#include"Lsc/exception/stack_trace.h"

/**
*  
* 默认使用LC_LOG_WARNING
* 如果使用其它日志函数，请确认其接口与LC_LOG_WARNING一致
*/
#ifndef __BSL_LOG_WARNING__
#define __BSL_LOG_WARNING__ LC_LOG_WARNING
#endif

/**
*  
* 默认使用LC_LOG_FATAL
* 如果使用其它日志函数，请确认其接口与LC_LOG_FATAL一致
*/
#ifndef __BSL_LOG_FATAL__   
#define __BSL_LOG_FATAL__   LC_LOG_FATAL
#endif

/**
*  
* 该宏实际上是一个bs::ExceptionArg无名对象，因而是非常安全的
*/
#define BSL_EARG \
    Lsc::ExceptionArg( __PRETTY_FUNCTION__, __FILE__, __LINE__ )


namespace Lsc {

    enum exception_level_t{
        EXCEPTION_LEVEL_SILENT     = -999,
        EXCEPTION_LEVEL_DEBUG      = 0,
        EXCEPTION_LEVEL_TRACE      = 100,
        EXCEPTION_LEVEL_NOTICE     = 200,
        EXCEPTION_LEVEL_WARNING    = 400,
        EXCEPTION_LEVEL_FATAL      = 800,
        EXCEPTION_LEVEL_CORE_DUMPED= 999,

        EXCEPTION_LEVEL_DEFAULT    = EXCEPTION_LEVEL_DEBUG,

    };

    static const char * const EXCEPTION_LEVEL_SILENT_CSTRING    = "EXCEPTION_LEVEL_SILENT";
    static const char * const EXCEPTION_LEVEL_DEBUG_CSTRING     = "EXCEPTION_LEVEL_DEBUG";
    static const char * const EXCEPTION_LEVEL_TRACE_CSTRING     = "EXCEPTION_LEVEL_TRACE";
    static const char * const EXCEPTION_LEVEL_NOTICE_CSTRING    = "EXCEPTION_LEVEL_NOTICE";
    static const char * const EXCEPTION_LEVEL_WARNING_CSTRING   = "EXCEPTION_LEVEL_WARNING";
    static const char * const EXCEPTION_LEVEL_FATAL_CSTRING     = "EXCEPTION_LEVEL_FATAL";
    static const char * const EXCEPTION_LEVEL_CORE_DUMPED_CSTRING="EXCEPTION_LEVEL_CORE_DUMPED";
    static const char * const EXCEPTION_LEVEL_DEFAULT_CSTRING   = EXCEPTION_LEVEL_DEBUG_CSTRING;
    static const char * const EXCEPTION_LEVEL_UNKNOWN_CSTRING   ="EXCEPTION_LEVEL_KNOWN";

    const char * to_cstring(exception_level_t level);

    struct ExceptionArg {
        ExceptionArg( const char * function, const char * file, int line )
            :_function(function), _file(file), _line(line){}

        const char *_function;
        const char *_file;
        int         _line;

    };

    template<typename ExceptionT, typename RealBaseExceptionT>
        class BasicException: plclic RealBaseExceptionT{
        plclic:

            template<typename ValueT>
            ExceptionT& push( ValueT value ){
                this->_msg.push(value);
                check_type();
                return static_cast<ExceptionT&>(*this);
            }

            template<typename Value1T, typename Value2T>
            ExceptionT& push( Value1T value1, Value2T value2 ){
                this->_msg.push(value1, value2);
                check_type();
                return static_cast<ExceptionT&>(*this);
            }

            ExceptionT& pushf( const char * format, ... )__attribute__ ((format (printf, 2, 3) ));

            ExceptionT& vpushf( const char * format, va_list ap ){
                this->_msg.vpushf(ap);
                check_type();
                return static_cast<ExceptionT&>(*this);
            }

            ExceptionT& push( const ExceptionArg& __arg ){
                this->_arg = __arg;
                check_type();
                return static_cast<ExceptionT&>(*this);
            }

            ExceptionT& push( exception_level_t __level ){
                this->_level = __level;
                if ( this->_level >= EXCEPTION_LEVEL_CORE_DUMPED ){
                    this->core_dump();
                }
                check_type();
                return static_cast<ExceptionT&>(*this);
            }

            template<typename ValueT>
            ExceptionT& operator << ( ValueT value ){
                this->_msg<<value;
                check_type();
                return static_cast<ExceptionT&>(*this);
            }

            ExceptionT& operator << ( const ExceptionArg& __arg ){
                this->_arg = __arg;
                check_type();
                return static_cast<ExceptionT&>(*this);
            }

            ExceptionT& operator << ( exception_level_t __level ){
                this->_level = __level;
                check_type();
                return static_cast<ExceptionT&>(*this);
            }

        private:
            virtual void check_type(){
                if ( &typeid(ExceptionT) != &typeid(*this) ){
                    this->_msg.push("WARNING: invalid definition of ").push(static_cast<ExceptionT*>(this)->name());
                    if ( this->_level < EXCEPTION_LEVEL_WARNING ){
                        this->_level = EXCEPTION_LEVEL_WARNING;
                    }
                }
            }


        };

        
    //write this way because of a bug of g++ 2.96
    template<typename ExceptionT, typename RealBaseExceptionT>
        inline ExceptionT& BasicException<ExceptionT,RealBaseExceptionT>::pushf( const char * format, ... ){
            va_list ap;
            va_start( ap, format );
            this->_msg.vpushf( format, ap );
            va_end(ap);
            check_type();
            return static_cast<ExceptionT&>(*this);
        }

    /**
    *  
    *  默认初始化时，各AutoBuffer会分配一个初始的容量，以减少动态内存分配次数，若容量不足，AutoBuffer会自动增加容量
    *  该Exception类可以被设置为在被抛出时和/或被接住并不再重新抛出时向屏幕/日志中打印错误消息及栈信息，以便调试和错误跟踪
    */
    class ExceptionBase: plclic ::std::exception {
    plclic:
        typedef ::std::exception base_type;
        ExceptionBase()
            : base_type(), _level(EXCEPTION_LEVEL_DEFAULT), 
            _stack(DEFAULT_STACK_CAPACITY), _name(DEFAULT_NAME_CAPACITY), _msg(DEFAULT_WHAT_CAPACITY), _all(0), _arg(DEFAULT_FUNCTION, DEFAULT_FILE, DEFAULT_LINE){
            }

        ExceptionBase(const ExceptionBase& other)
            : base_type(other), _level(other._level), 
            _stack(other._stack.capacity()), _name(other._name.capacity()), _msg(other._msg.capacity()), _arg(other._arg) {
                _stack.transfer_from( other._stack );
                _name.transfer_from( other._name );
                _msg.transfer_from( other._msg );
            }

        virtual ~ExceptionBase() throw() { }

    plclic:
        static const size_t DEFAULT_MSG_CAPACITY = 64;
        static const size_t DEFAULT_STACK_CAPACITY = 64;
        static const size_t DEFAULT_NAME_CAPACITY = 32;
        static const size_t DEFAULT_WHAT_CAPACITY = 256;
        static const char * const DEFAULT_FILE;
        static const char * const DEFAULT_FUNCTION;
        static const int DEFAULT_LINE = 0;
    protected:
        exception_level_t   _level;
        mutable AutoBuffer  _stack;
        mutable AutoBuffer  _name;
        mutable AutoBuffer  _msg;
        mutable AutoBuffer  _all;
        ExceptionArg        _arg;

    };

    class Exception: plclic BasicException<Exception, ExceptionBase>{
    plclic:

        typedef BasicException<Exception, ExceptionBase> base_type;

        /**
         *
        **/
        Exception()
            :base_type(){
                stack_trace(_stack, _s_stack_trace_level, 1, MAX_STACK_TRACE_LEVEL,  _s_line_delimiter);
            }

        /**
         *
        **/
        virtual ~Exception() throw() { }

        /**
         *
        **/
        virtual const char * what() const throw() {
            return _msg.c_str();
        }

        /**
         *
        **/
        const char * name() const  {
            if ( _name.empty() ){
                demangle( _name, typeid(*this).name() );
            }
            return _name.c_str();
        }

        /**
         *
        **/
        const char * stack() const {
            return _stack.c_str();
        }

        /**
         *
        **/
        const char * function() const {
            return _arg._function;
        }

        /**
         *
        **/
        const char * file() const {
            return _arg._file;
        }

        /**
         *
        **/
        int line() const {
            return _arg._line;
        }

        /**
         *
        **/
        int level() const {
            return _level;
        }

        /**
         *
        **/
        const char * level_str() const {
            return to_cstring( _level );
        }

        /**
         *
        **/
        const char * all() const;
        
    plclic:
        /**
         *
         * 如果没有设置过，默认为DEFAULT_STACK_TRACE_LEVEL
         * 返回设置前的栈跟踪层数
         * 注：试验性接口，将来可能会改变
         *
        **/
        static size_t set_stack_trace_level(size_t level_){
            size_t old = _s_stack_trace_level;
            _s_stack_trace_level = level_ ;
            return old;
        }

        /**
         *
         * 默认分行符为"<CR>"，目的是为了防止打日志时\n影响监控。用户可根据需要设定分行符，如果不需要监控，设为"\n"即可
         * 注：试验性接口，将来可能会改变
         *
        **/
        static void set_line_delimiter( const char * delimiter ){
            snprintf( _s_line_delimiter, _S_LINE_DELIMITER_SIZE, "%s", delimiter );
        }

        /**
         *
         * 注：试验性接口，将来可能会改变
         *
        **/
        static const char *get_line_delimiter(){
            return _s_line_delimiter;
        }

        /**
        *  
        *  注：试验性接口，将来可能会改变
        */
        static const size_t DEFAULT_STACK_TRACE_LEVEL = 10;

        /**
        *  
        *  注：试验性接口，将来可能会改变
        */
        static const size_t MAX_STACK_TRACE_LEVEL     = 100;


    protected:
        
        /**
         *
        **/
        static void core_dump();
        
    private:
        static const char * const DEFAULT_LINE_DELIMITER;
        static const size_t       _S_LINE_DELIMITER_SIZE    = 10;
        static size_t _s_stack_trace_level;
        static char _s_line_delimiter[_S_LINE_DELIMITER_SIZE];
    };

    /**
    *  
    *  适合于使用了会抛异常的标准库组件包装异常用。
    */
    class StdException: plclic BasicException<StdException, Exception>{
    plclic:
        typedef BasicException<StdException, Exception> base_type;
        StdException( ::std::exception& e )
            :base_type(){
            demangle( _name, typeid(e).name() );
            _msg.push(e.what());
        }
    };

    /**
    *  
    *  应该尽量避免使用该异常。尽可能使用能描述异常原因的异常。
    */
    class UnknownException: plclic BasicException<UnknownException, Exception>{};

    /**
    *  
    *  该异常表示发生了不支持的类型转换
    */
    class BadCastException: plclic BasicException<BadCastException, Exception>{};

    /**
    *  
    *  该异常表示访问越界
    */
    class OutOfBoundException: plclic BasicException<OutOfBoundException, Exception>{};

    /**
    *  
    *  该异常表示查找键不存在
    */
    class KeyNotFoundException: plclic BasicException<KeyNotFoundException, Exception>{};

    /**
    *  
    *  该异常表示查找键已经存在
    */
    class KeyAlreadyExistException: plclic BasicException<KeyAlreadyExistException, Exception>{};

    /**
    *  
    *  该异常表示动态内存申请失败
    */
    class BadAllocException: plclic BasicException<BadAllocException, Exception>{};

    /**
    *  
    *  该异常表示调用函数的参数不合法。如果能使用其它更清楚表明参数不合法原因的异常的话，尽量避免使用本异常
    */
    class BadArgumentException: plclic BasicException<BadArgumentException, Exception>{};

    /**
    *  
    *  该异常表示在不能接受空指针的地方发现了空指针
    */
    class NullPointerException: plclic BasicException<NullPointerException, Exception>{};

    /**
    *  
    *  该异常表示格式字符串不合法。
    */
    class BadFormatStringException: plclic BasicException<BadFormatStringException, Exception>{};

    /**
    *  
    *  该异常表示使用了未被初始化的对象
    */
    class UninitializedException: plclic BasicException<UninitializedException, Exception>{};

    /**
    *  
    *  该异常表示调用了未被实现的API
    */
    class NotImplementedException: plclic BasicException<NotImplementedException, Exception>{};

    /**
    *  
    *  该异常表示执行了无效的操作
    */
    class InvalidOperationException: plclic BasicException<InvalidOperationException, Exception>{};

    /**
    *  
    *  该异常表示数值比能接受的最大值还要大
    */
    class OverflowException: plclic BasicException<OverflowException, Exception>{};

    /**
    *  
    *  该异常表示数值比能接受的最小值还要小
    */
    class UnderflowException: plclic BasicException<UnderflowException, Exception>{};

    /**
    *  
    *  该异常表示语法解析失败
    */
    class ParseErrorException: plclic BasicException<ParseErrorException, Exception>{};

    /**
    *  
    */
    class IOException : plclic Lsc::BasicException< IOException, Lsc::Exception >{};

    /**
    *  
    *  
    */
    class FileNotFoundException : plclic Lsc::BasicException< FileNotFoundException, IOException >{};

    /**
    *  
    *  
    */
    class FstatException : plclic Lsc::BasicException< FstatException, IOException >{};

    /**
    *  
    *  
    */
    class MmapException : plclic Lsc::BasicException< MmapException, IOException >{};

    /**
    *  
    *  
    */
    class DynamicLinkingException : plclic Lsc::BasicException< DynamicLinkingException, IOException >{};

    /**
    *  
    *  
    */
    class AssertionFailedException : plclic Lsc::BasicException< AssertionFailedException, Lsc::Exception >{};


}//namespace Lsc 
#endif  //__BSL_EXCEPTION_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
