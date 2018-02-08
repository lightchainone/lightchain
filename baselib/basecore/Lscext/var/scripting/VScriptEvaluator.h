#ifndef  __BSL_VAR_VSCRIPTEVALUATOR_H__
#define  __BSL_VAR_VSCRIPTEVALUATOR_H__
#include <vector>
#include <Lsc/var/IVar.h>
#include <Lsc/var/implement.h>
#include <Lsc/var/VScriptOperator.h>

namespace Lsc{
namespace var{
    
    class VScriptEvaluator{
    plclic:
        
        VScriptEvaluator(ResourcePool& rp)
            :_rp(rp), _script(NULL), _cur(NULL), _has_slc_query(false), _has_call_query(false){}

        
        IVar& eval( const char * script, IVar& data );
        
        
        int current_pos() const {
            return _cur - _script;
        }

        
        int current_line() const {
            return _line;
        }
        
        
        int current_column() const {
            return _cur - _line_begin + 1;
        }

        
        bool has_slc_query() const {
            return _has_slc_query;
        }

         
        bool has_call_query() const {
            return _has_call_query;
        }

    private:
        
        typedef IVar::string_type   string_type;
        
        
        typedef long long           number_type;
        
        
        typedef unsigned char _key_t;
        
        
        
        
        enum token_t{
            END             = '\0',
            WHITE_SPACE     = ' ',
            LBRACKET        = '[',
            RBRACKET        = ']',
            LPAREN          = '(',
            RPAREN          = ')',
            PLUS            = '+',
            MINUS           = '-',
            DIGIT           = '0',
            STRING_LITERAL  = '\"',
            IDENTIFIER      = '_',
            COMMA           = ',',
            DOT             = '.',
            SEMI            = ';',
            STAR            = '*',
            VIRGULE         = '/',
            PERCENT         = '%',
            LESS            = '<',
            GREATER           = '>',
            EQUAL           = '=',
            EXCLAMATION     = '!',
            AND             = '&',
            OR              = '|',
        };
      
        
        typedef Lsc::var::VScriptOperator::operator_t operator_t;
        
        
        struct operator_attr_t{  
            
            bool is_left_associated;    
            
            bool is_write;
            
            int operand_num;  
            
            int priority;
            
            const char *literal;
                 
            int len;
            
            const char *name;         
        }; 
        
       
        IVar& eval_expr(IVar& var);


        
        token_t look_ahead(bool is_skip_white = true);

       
        operator_t eval_operator(bool is_expect_operand);

        
        void reduce(operator_t op, 
                std::vector<int>& operator_pos, std::vector<operator_t>& operator_stack, 
                std::vector<int>& operand_pos, std::vector<IVar*>& operand_stack);

        
        IVar& eval_path_expr(IVar& var);

        
        IVar& eval_call_expr(IVar& var, IVar& parent);

        
        IVar& eval_field_expr(IVar& var );

        
        void eval_identifier();

        
        void eval_string_literal();

        
        number_type eval_number_literal();

        
        void skip_white(){
            while(_s_token_map[_key_t(*_cur)] == WHITE_SPACE ){
                if ( *_cur == '\n' ){
                    ++ _line;
                    _line_begin = ++ _cur;
                }else{
                    ++ _cur;
                }
            }
        }
        
        
        ResourcePool&   _rp;
        
        
        IVar *          _pdata;
        
        
        IVar::string_type _str;
        
        
        const char *    _script;
        
        
        const char *    _cur;
        
        
        const char *    _line_begin;
        
        
        int             _line;
        
        
        bool            _has_slc_query;
        
        
        bool            _has_call_query;   
        
        
        static bool     _s_init();
        
         
        static void     _s_init_token_map();

        
        static void     _s_init_unescape_map();
        
        
        static void     _s_init_char2operator_map();
        
        
        static void     _s_init_operator_attr();
	    
        
        static token_t  _s_token_map[0x100];

        
        static char     _s_unescape_map[0x100];

        
        static bool     _s_is_operator[0x100];
        
        
        static operator_t _s_char_pair_operator_map[0x100][0x100];

        
        static bool     _s_is_expect_operand[VScriptOperator::OPERATOR_TYPE_NUM];

        
        static operator_attr_t _s_operators[VScriptOperator::OPERATOR_TYPE_NUM];
        
        
        static bool     _s_static_initer;
    };
}}
#endif  


