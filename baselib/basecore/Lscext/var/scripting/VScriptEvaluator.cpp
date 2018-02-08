#include <exception>
#include <climits>
#include <Lsc/var/implement.h>
#include <Lsc/var/smart_deserialize_number.h>
#include <Lsc/var/VScriptEvaluator.h>

namespace Lsc{
namespace var{

    
    IVar& VScriptEvaluator::eval( const char * script, IVar& data ){
        if ( script == NULL ){
            
            throw Lsc::NullPointerException()<<BSL_EARG<<"script can't be NULL!";
        }
        _pdata  = &data;
        _line   = 1;
        _script = _cur = _line_begin = script;
        _has_slc_query = _has_call_query = false;
        try{
            while(true){
                IVar& res = eval_expr(*_pdata);
                skip_white();
                if ( _s_token_map[_key_t(*_cur)] != SEMI ){
                    return res;
                }else{
                    ++ _cur;
                }
            }
        }catch(Lsc::Exception& e){
            const char * begin = _cur - 30 > _script ? _cur - 30 : _script;
            e.push("{VScriptEvaluator line[").push(_line).push("] col[").push(current_column());
            e.push("] vscript[").push( begin, _cur - begin ).pushf("~!~%.20s] }", _cur);
            throw;
        }
    }
    
    
    IVar& VScriptEvaluator::eval_expr(IVar& var){
        std::vector<int> operator_pos;   
        std::vector<int> operand_pos;    
        std::vector<operator_t> operator_stack;     
        std::vector<IVar*> operand_stack;           
        token_t cur_token;          
        operator_t op;              
        bool is_expect_operand = true; 
        bool is_end = false;     
        int paren_flag = 0;         
        while(!is_end){
            skip_white();
            cur_token = _s_token_map[_key_t(*_cur)];
            switch(cur_token){
            case STRING_LITERAL:
                
                operand_pos.push_back(_cur - _script);
                eval_string_literal();      
                operand_stack.push_back(&_rp.create<String>(_str));
                is_expect_operand = false;
                break;
            case IDENTIFIER:
            case DOT:
            case LBRACKET:
                
                operand_pos.push_back(_cur - _script);
                operand_stack.push_back(&eval_path_expr(var));      
                is_expect_operand = false;
                break;
            default:
                 
                if(cur_token == DIGIT || is_expect_operand && (cur_token == MINUS || cur_token == PLUS) && look_ahead() == DIGIT){
                    operand_pos.push_back(_cur - _script);
                    operand_stack.push_back(&smart_deserialize_number(_rp, _cur, &_cur));   
                    is_expect_operand = false;    
                }else{
                    
                    int op_pos = _cur - _script;
                    
                    if( (cur_token == RPAREN) && (paren_flag == 0) 
                        || !_s_is_operator[cur_token]){ 
                        op = VScriptOperator::OP_END;
                    }else if(_s_is_operator[cur_token]){
                        op = eval_operator(is_expect_operand);  
                        is_expect_operand = _s_is_expect_operand[op];
                    }
                    
                    reduce(op, operator_pos, operator_stack, operand_pos, operand_stack); 
                    
                    if(op == VScriptOperator::OP_RPAREN){  
                        
                        if(operator_stack.back() != VScriptOperator::OP_LPAREN){
                            throw Lsc::ParseErrorException()<<BSL_EARG
                                <<"parentheses do not match!";
                        }else{  
                            operator_pos.pop_back();
                            operator_stack.pop_back(); 
                        }
                    }else{
                        operator_pos.push_back(op_pos);
                        operator_stack.push_back(op); 
                    } 
                    
                    if(op == VScriptOperator::OP_LPAREN){
                        ++paren_flag;
                    }else if(op == VScriptOperator::OP_RPAREN){
                        --paren_flag;
                    }
                    
                    is_end = (op == VScriptOperator::OP_END);
                }
                break;
            }
        }
        
        if (!(operator_stack.size() == 1 && operator_stack[0] == VScriptOperator::OP_END && operand_stack.size() <= 1)){
            throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected END found, syntax error.";
        }
        return (operand_stack.size() == 0 ? Lsc::var::Null::null : *(operand_stack[0]));
    }

    
    VScriptEvaluator::token_t VScriptEvaluator::look_ahead(bool is_skip_white ){
        const char *tmp  = _cur;
        if(is_skip_white){
            skip_white();
        }
        token_t res = _s_token_map[_key_t(*(_cur + 1))];
        _cur = tmp;
        return res;
    }

    
    VScriptEvaluator::operator_t VScriptEvaluator::eval_operator(bool is_expect_operand){
        operator_t op = _s_char_pair_operator_map[_key_t(*_cur)][_key_t(*(_cur + 1))];
        
        if(is_expect_operand){
            switch(op){
            case VScriptOperator::OP_ADD: 
                op = VScriptOperator::OP_POSITIVE; 
                break;
            case VScriptOperator::OP_SLC:
                op = VScriptOperator::OP_NEGATIVE;
                break;
            default:
                break;
            }
        }
        _cur += _s_operators[op].len;
        return op;
    }
    
    
    void VScriptEvaluator::reduce(operator_t op, 
            std::vector<int>& operator_pos, std::vector<operator_t>& operator_stack,
            std::vector<int>& operand_pos, std::vector<IVar*>& operand_stack){
        if(op == VScriptOperator::OP_LPAREN){  
            return;
        }

        bool has_left_paren = false;
        while (!operator_stack.empty() && !has_left_paren){
            operator_t prev_op = operator_stack.back();
            if(!(_s_operators[prev_op].is_left_associated && 
                (_s_operators[prev_op].priority >= _s_operators[op].priority) ||
                 (!_s_operators[prev_op].is_left_associated) && 
                 (_s_operators[prev_op].priority > _s_operators[op].priority))){
               break;
            }
            int opr_num = _s_operators[prev_op].operand_num;
            if ( (int)operand_stack.size() < opr_num ){
                throw Lsc::ParseErrorException()<<BSL_EARG<<"insufficient operands: size["
                    <<operand_stack.size()<<"]";
            }
            IVar *operand1 = NULL;
            IVar *operand2 = NULL;
            IVar *result = NULL;
            int pos1;
            int pos2;
            int op_pos;
            switch(opr_num){
            case 0:
                
                
                has_left_paren = true;  
                break;
            case 1:
                
                operand1 = operand_stack[operand_stack.size() - 1];
                operand_stack.pop_back();
                
                result = VScriptOperator::exec_operator(prev_op, operand1, _rp);     
                
                operand_stack.push_back(result);
                
                operator_pos.pop_back();
                operator_stack.pop_back();
                break;
            case 2:
                
                pos1 = operand_pos[operand_pos.size() - 2];
                pos2 = operand_pos[operand_pos.size() - 1];
                op_pos = operator_pos[operator_pos.size() - 1];
                if(_s_operators[prev_op].is_write && !(pos1 < op_pos && op_pos < pos2)){
                    throw Lsc::ParseErrorException()<<BSL_EARG<<
                        "insufficient operands: expect operand in the left of operator "<<_s_operators[prev_op].name;
                }
                
                operand1 = operand_stack[operand_stack.size() - 2];
                operand2 = operand_stack[operand_stack.size() - 1];
                operand_pos.pop_back(); 
                operand_pos.pop_back();
                operand_stack.pop_back();
                operand_stack.pop_back();
                
                result = VScriptOperator::exec_operator(prev_op, operand1, operand2, _rp);     
                
                operand_pos.push_back(pos1);
                operand_stack.push_back(result);
                
                operator_pos.pop_back();
                operator_stack.pop_back();
                break;
            default:
                throw Lsc::ParseErrorException()<<BSL_EARG<<"unknown number of operands: size["
                    <<operand_stack.size()<<"]";
                break;
            }
            
       }
       return;
    }

    
    IVar& VScriptEvaluator::eval_path_expr(IVar& var){
        IVar *pvar = &var;
        IVar *pprnt= _pdata;
        bool empty = true;
        if( _s_token_map[_key_t(*_cur)] == IDENTIFIER ){  
            empty = false;
            eval_identifier();
            pprnt = pvar;
            pvar = &((*pprnt)[_str]);
        }
        while(true){
            skip_white();
            switch( _s_token_map[_key_t(*_cur)] ){
                case DOT:
                    ++ _cur;
                    skip_white();
                    eval_identifier();
                    if ( _str.empty() ){
                        if ( empty ){
                            empty = false;
                            skip_white();
                            if ( _s_token_map[_key_t(*_cur)] == DOT ){    
                                throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected '.'";
                            }
                            
                        }else{
                            
                            throw Lsc::ParseErrorException()<<BSL_EARG<<"expect: <identifier>" ;
                        }
                    }else{
                        empty = false;
                        pprnt = pvar;
                        pvar = &((*pprnt)[_str]);
                    }
                    break;
                case LBRACKET:
                    empty = false;
                    pprnt = pvar;
                    pvar = &eval_field_expr(*pprnt);
                    break;

                case LPAREN:
                    empty = false;
                    pvar = &eval_call_expr(*pvar, *pprnt);
                    pprnt = _pdata;
                    break;
                default:
                    if ( empty ){
                        throw Lsc::ParseErrorException()<<BSL_EARG<<"expect: <field_expr>" ;
                    }
                    return *pvar;
            }
        }
    }

    
    IVar& VScriptEvaluator::eval_call_expr(IVar& var, IVar& parent){
        ++ _cur;  
        _has_call_query = true;
        IVar& args  = _rp.create<Array>();
        skip_white();
        if ( _s_token_map[_key_t(*_cur)] == RPAREN ){ 
            ++ _cur;
            return var(parent, args, _rp);
        }

        for( int size = 0; ; ++ size ){
            args[size]     = eval_expr( *_pdata );
            skip_white();
            switch ( _s_token_map[_key_t(*_cur)] ){
            case RPAREN:
                ++ _cur;
                return var(parent, args, _rp);

            case COMMA:
                ++ _cur;
                break;

            default:
                throw Lsc::ParseErrorException()<<BSL_EARG<<"expect: , or )" ;
            }
        }
    }

    
    IVar& VScriptEvaluator::eval_field_expr(IVar& var ){
        ++ _cur;    
        
        skip_white();
        IVar* pres = NULL;
        number_type idx;
        switch( _s_token_map[_key_t(*_cur)] ){
            case STRING_LITERAL:    
                eval_string_literal();
                pres = &(var[_str]);
                break;
            case DIGIT:             
            case MINUS:
            case PLUS:
                idx = eval_number_literal();    
                if ( idx < 0 ){
                    idx += var.size();
                }

                
                try{
                    pres = &(var[idx]);
                }catch(std::bad_alloc &e){
                    throw Lsc::BadAllocException()<<BSL_EARG<<"Bad alloc exception occured in var array, resizing failed.";
                }catch(std::exception &e){
                    throw Lsc::StdException(e)<<BSL_EARG<<"std exception: "<<e.what();
                }
                break;
            default:                 
                IVar& field = eval_path_expr( *_pdata);
                if ( field.is_string() ){
                    pres = &(var[field.to_string()]);
                }else{
                    
                    try{
                        pres = &(var[field.to_int64()]);
                    }catch(std::bad_alloc &e){
                        throw Lsc::BadAllocException()<<BSL_EARG<<"Bad alloc exception occured in var array, resizing failed.";
                    }catch(std::exception &e){
                        throw Lsc::StdException(e)<<BSL_EARG<<"std exception: "<<e.what();
                    }
                }
                break;

        }
        skip_white();
        if ( _s_token_map[_key_t(*_cur)] != RBRACKET ){
            throw Lsc::ParseErrorException()<<BSL_EARG<<"expect: ]" ;
        }
        ++ _cur; 

        return *pres;

    }

    
    inline void VScriptEvaluator::eval_identifier(){
        skip_white();
        const char * begin_pos = _cur;
        while( _s_token_map[_key_t(*_cur)] == IDENTIFIER
                || _s_token_map[_key_t(*_cur)] == DIGIT ){
            ++ _cur;
        }
        _str.clear();
        _str.append(begin_pos, _cur );
    }

    
    inline void VScriptEvaluator::eval_string_literal(){
        char delim = *_cur; 
        const char *last_pos  = _cur + 1;
        _str.clear();
        for( ++ _cur; *_cur && *_cur != delim; ++ _cur ){
            if ( *_cur == '\\' ){
                _str.append( last_pos, _cur );
                ++ _cur;
                char tmp = _s_unescape_map[_key_t(*_cur)];
                if ( tmp ){
                    _str.push_back(tmp);
                }else{  
                    _str.push_back('\\');
                    _str.push_back(*_cur);
                }
                last_pos = _cur + 1;
            }
        }

        if ( *_cur == delim ){
            
            _str.append( last_pos, _cur );
            ++ _cur;

        }else{
            
            throw Lsc::ParseErrorException()<<BSL_EARG<<"expect:"<<delim<<" get:\\0";
        }
    }

    
    inline VScriptEvaluator::number_type
    VScriptEvaluator::eval_number_literal(){
        bool positive = true;
        skip_white();
        switch(_s_token_map[_key_t(*_cur)]){
            case PLUS:
                ++ _cur;
                break;
            case MINUS:
                positive = false;
                ++ _cur;
                break;
            case DIGIT:
                break;
            default:
                throw Lsc::ParseErrorException()<<BSL_EARG<<"expect: <number literal>" ;
        }
        
        skip_white();
        if ( _s_token_map[_key_t(*_cur)] != DIGIT ){
            throw Lsc::ParseErrorException()<<BSL_EARG<<"expect: <digit>";
        }
        
        const number_type INT_MAX_VAL = (sizeof(void*) == 4 ? INT_MAX : LONG_LONG_MAX);
        const number_type INT_MIN_VAL = (sizeof(void*) == 4 ? INT_MIN : LONG_LONG_MIN); 
        number_type num = 0;
        if(positive){  
            for(; _s_token_map[_key_t(*_cur)] == DIGIT; ++_cur){
                number_type d = *_cur - '0';
                if(num < INT_MAX_VAL / 10 || num == INT_MAX_VAL / 10 && (d <= INT_MAX_VAL % 10)){
                    num = num * 10 + d;
                }else{
                    throw Lsc::ParseErrorException()<<BSL_EARG<<"literal integer exceeds range [INT_MIN, INT_MAX], over-flow.";
                }
            }
        }else{
            for(; _s_token_map[_key_t(*_cur)] == DIGIT; ++_cur){
                number_type d = *_cur - '0';
                if(num > INT_MIN_VAL / 10 || num == INT_MIN_VAL / 10 && (d <= -(INT_MIN_VAL % 10))){
                    num = num * 10 - d;
                }else{
                    throw Lsc::ParseErrorException()<<BSL_EARG<<"literal integer exceeds range [INT_MIN, INT_MAX], under-flow.";
                }
            }
        }
        return num;
    }

    
    bool VScriptEvaluator::_s_init(){
        VScriptEvaluator::_s_init_token_map();
        VScriptEvaluator::_s_init_unescape_map();
        VScriptEvaluator::_s_init_char2operator_map();
        VScriptEvaluator::_s_init_operator_attr();
        return true;   
    }

     
    void VScriptEvaluator::_s_init_token_map(){
        _s_token_map[_key_t('\0')] = END;  

        _s_token_map[_key_t(' ')]  = WHITE_SPACE;
        _s_token_map[_key_t('\t')] = WHITE_SPACE;
        _s_token_map[_key_t('\r')] = WHITE_SPACE;
        _s_token_map[_key_t('\n')] = WHITE_SPACE;

        _s_token_map[_key_t('[')]  = LBRACKET;
        _s_token_map[_key_t(']')]  = RBRACKET;
        _s_token_map[_key_t('(')]  = LPAREN;
        _s_token_map[_key_t(')')]  = RPAREN;
        _s_token_map[_key_t('\"')] = STRING_LITERAL;
        _s_token_map[_key_t('\'')] = STRING_LITERAL;
        _s_token_map[_key_t(',')]  = COMMA;
        _s_token_map[_key_t('.')]  = DOT;
        _s_token_map[_key_t(';')]  = SEMI;
        _s_token_map[_key_t('+')]  = PLUS;
        _s_token_map[_key_t('-')]  = MINUS;
        _s_token_map[_key_t('*')]  = STAR;
        _s_token_map[_key_t('/')]  = VIRGULE;
        _s_token_map[_key_t('%')]  = PERCENT;
        _s_token_map[_key_t('<')]  = LESS;
        _s_token_map[_key_t('>')]  = GREATER;
        _s_token_map[_key_t('=')]  = EQUAL;
        _s_token_map[_key_t('!')]  = EXCLAMATION;
        _s_token_map[_key_t('&')]  = AND;
        _s_token_map[_key_t('|')]  = OR;

        for( char c = '0'; c <= '9'; ++ c ){
            _s_token_map[_key_t(c)] = DIGIT;
        }

        _s_token_map[_key_t('_')] = IDENTIFIER;
        for( char c = 'A'; c <= 'Z'; ++ c ){
            _s_token_map[_key_t(c)] = IDENTIFIER;
        }
        for( char c = 'a'; c <= 'z'; ++ c ){
            _s_token_map[_key_t(c)] = IDENTIFIER;
        }
       
        return;
    }

    
    void VScriptEvaluator::_s_init_unescape_map(){
        _s_unescape_map[_key_t('\"')] = '\"';
        _s_unescape_map[_key_t('\'')] = '\'';
        _s_unescape_map[_key_t('\\')] = '\\';
        _s_unescape_map[_key_t('/' )] = '/';
        _s_unescape_map[_key_t('b')] = '\b';
        _s_unescape_map[_key_t('f')] = '\f';
        _s_unescape_map[_key_t('n')] = '\n';
        _s_unescape_map[_key_t('r')] = '\r';
        _s_unescape_map[_key_t('t')] = '\t';
        
        return;
    }

    
    void VScriptEvaluator::_s_init_char2operator_map(){

        
        _s_is_operator[_key_t('+')] = true;
        _s_is_operator[_key_t('-')] = true;
        _s_is_operator[_key_t('*')] = true;
        _s_is_operator[_key_t('/')] = true;
        _s_is_operator[_key_t('%')] = true;
        _s_is_operator[_key_t('=')] = true;
        _s_is_operator[_key_t('!')] = true;
        _s_is_operator[_key_t('<')] = true;
        _s_is_operator[_key_t('>')] = true;
        _s_is_operator[_key_t('&')] = true;
        _s_is_operator[_key_t('|')] = true;
        _s_is_operator[_key_t('(')] = true;
        _s_is_operator[_key_t(')')] = true;
        
        
        operator_t single_char_operator[0x100];
        for(int i = 0 ;i < 0x100; ++i){
            single_char_operator[i] = VScriptOperator::OP_END;
        }
        
        single_char_operator[_key_t('+')] = VScriptOperator::OP_ADD; 
        single_char_operator[_key_t('-')] = VScriptOperator::OP_SLC; 
        single_char_operator[_key_t('*')] = VScriptOperator::OP_MUL;
        single_char_operator[_key_t('/')] = VScriptOperator::OP_DIV;
        single_char_operator[_key_t('%')] = VScriptOperator::OP_MOD;
        single_char_operator[_key_t('=')] = VScriptOperator::OP_ASSIGN;
        single_char_operator[_key_t('!')] = VScriptOperator::OP_LOGIC_NOT;
        single_char_operator[_key_t('<')] = VScriptOperator::OP_LESS_THAN;
        single_char_operator[_key_t('>')] = VScriptOperator::OP_GREATER_THAN;
        single_char_operator[_key_t('(')] = VScriptOperator::OP_LPAREN;
        single_char_operator[_key_t(')')] = VScriptOperator::OP_RPAREN;
        
        for(int i = 0; i < 0x100; ++i){
            for(int j = 0; j < 0x100; ++j){
                _s_char_pair_operator_map[i][j] = single_char_operator[i];
            }
        }    
        
        _s_char_pair_operator_map[_key_t('+')][_key_t('=')] = VScriptOperator::OP_ADD_ASSIGN;
        _s_char_pair_operator_map[_key_t('-')][_key_t('=')] = VScriptOperator::OP_SLC_ASSIGN;
        _s_char_pair_operator_map[_key_t('*')][_key_t('=')] = VScriptOperator::OP_MUL_ASSIGN;
        _s_char_pair_operator_map[_key_t('/')][_key_t('=')] = VScriptOperator::OP_DIV_ASSIGN;
        _s_char_pair_operator_map[_key_t('%')][_key_t('=')] = VScriptOperator::OP_MOD_ASSIGN;
        _s_char_pair_operator_map[_key_t('=')][_key_t('=')] = VScriptOperator::OP_EQUAL;
        _s_char_pair_operator_map[_key_t('!')][_key_t('=')] = VScriptOperator::OP_NOT_EQUAL;
        _s_char_pair_operator_map[_key_t('<')][_key_t('=')] = VScriptOperator::OP_LESS_EQUAL_THAN;
        _s_char_pair_operator_map[_key_t('>')][_key_t('=')] = VScriptOperator::OP_GREATER_EQUAL_THAN;
        _s_char_pair_operator_map[_key_t('&')][_key_t('&')] = VScriptOperator::OP_LOGIC_AND;
        _s_char_pair_operator_map[_key_t('|')][_key_t('|')] = VScriptOperator::OP_LOGIC_OR;
        
        return;
    }

    
	void VScriptEvaluator::_s_init_operator_attr(){

        
        _s_is_expect_operand[VScriptOperator::OP_END] = true;
        _s_is_expect_operand[VScriptOperator::OP_LPAREN] = true;
        _s_is_expect_operand[VScriptOperator::OP_RPAREN] = false;
		_s_is_expect_operand[VScriptOperator::OP_MOD_ASSIGN] = true;
		_s_is_expect_operand[VScriptOperator::OP_DIV_ASSIGN] = true;
		_s_is_expect_operand[VScriptOperator::OP_MUL_ASSIGN] = true;
		_s_is_expect_operand[VScriptOperator::OP_SLC_ASSIGN] = true;
		_s_is_expect_operand[VScriptOperator::OP_ADD_ASSIGN] = true;
		_s_is_expect_operand[VScriptOperator::OP_ASSIGN] = true;
		_s_is_expect_operand[VScriptOperator::OP_LOGIC_OR] = true;
		_s_is_expect_operand[VScriptOperator::OP_LOGIC_AND] = true;
		_s_is_expect_operand[VScriptOperator::OP_NOT_EQUAL] = true;
		_s_is_expect_operand[VScriptOperator::OP_EQUAL] = true;
		_s_is_expect_operand[VScriptOperator::OP_GREATER_EQUAL_THAN] = true;
		_s_is_expect_operand[VScriptOperator::OP_GREATER_THAN] = true;
		_s_is_expect_operand[VScriptOperator::OP_LESS_EQUAL_THAN] = true;
		_s_is_expect_operand[VScriptOperator::OP_LESS_THAN] = true;
		_s_is_expect_operand[VScriptOperator::OP_SLC] = true;
		_s_is_expect_operand[VScriptOperator::OP_ADD] = true;
		_s_is_expect_operand[VScriptOperator::OP_MOD] = true;
		_s_is_expect_operand[VScriptOperator::OP_DIV] = true;
		_s_is_expect_operand[VScriptOperator::OP_MUL] = true;
		_s_is_expect_operand[VScriptOperator::OP_LOGIC_NOT] = true;
        _s_is_expect_operand[VScriptOperator::OP_NEGATIVE] = true;
        _s_is_expect_operand[VScriptOperator::OP_POSITIVE] = true;

        
        _s_operators[VScriptOperator::OP_END].is_left_associated = true;
        _s_operators[VScriptOperator::OP_LPAREN].is_left_associated = true;
        _s_operators[VScriptOperator::OP_RPAREN].is_left_associated = true;
		_s_operators[VScriptOperator::OP_MOD_ASSIGN].is_left_associated = false;
		_s_operators[VScriptOperator::OP_DIV_ASSIGN].is_left_associated = false;
		_s_operators[VScriptOperator::OP_MUL_ASSIGN].is_left_associated = false;
		_s_operators[VScriptOperator::OP_SLC_ASSIGN].is_left_associated = false;
		_s_operators[VScriptOperator::OP_ADD_ASSIGN].is_left_associated = false;
		_s_operators[VScriptOperator::OP_ASSIGN].is_left_associated = false;
		_s_operators[VScriptOperator::OP_LOGIC_OR].is_left_associated = true;
		_s_operators[VScriptOperator::OP_LOGIC_AND].is_left_associated = true;
		_s_operators[VScriptOperator::OP_NOT_EQUAL].is_left_associated = true;
		_s_operators[VScriptOperator::OP_EQUAL].is_left_associated = true;
		_s_operators[VScriptOperator::OP_GREATER_EQUAL_THAN].is_left_associated = true;
		_s_operators[VScriptOperator::OP_GREATER_THAN].is_left_associated = true;
		_s_operators[VScriptOperator::OP_LESS_EQUAL_THAN].is_left_associated = true;
		_s_operators[VScriptOperator::OP_LESS_THAN].is_left_associated = true;
		_s_operators[VScriptOperator::OP_SLC].is_left_associated = true;
		_s_operators[VScriptOperator::OP_ADD].is_left_associated = true;
		_s_operators[VScriptOperator::OP_MOD].is_left_associated = true;
		_s_operators[VScriptOperator::OP_DIV].is_left_associated = true;
		_s_operators[VScriptOperator::OP_MUL].is_left_associated = true;
		_s_operators[VScriptOperator::OP_LOGIC_NOT].is_left_associated = false;
        _s_operators[VScriptOperator::OP_NEGATIVE].is_left_associated = false;
        _s_operators[VScriptOperator::OP_POSITIVE].is_left_associated = false;

        
        _s_operators[VScriptOperator::OP_END].is_write = false;
        _s_operators[VScriptOperator::OP_LPAREN].is_write = false;
        _s_operators[VScriptOperator::OP_RPAREN].is_write = false;
		_s_operators[VScriptOperator::OP_MOD_ASSIGN].is_write = true;
		_s_operators[VScriptOperator::OP_DIV_ASSIGN].is_write = true;
		_s_operators[VScriptOperator::OP_MUL_ASSIGN].is_write = true;
		_s_operators[VScriptOperator::OP_SLC_ASSIGN].is_write = true;
		_s_operators[VScriptOperator::OP_ADD_ASSIGN].is_write = true;
		_s_operators[VScriptOperator::OP_ASSIGN].is_write = true;
		_s_operators[VScriptOperator::OP_LOGIC_OR].is_write = false;
		_s_operators[VScriptOperator::OP_LOGIC_AND].is_write = false;
		_s_operators[VScriptOperator::OP_NOT_EQUAL].is_write = false;
		_s_operators[VScriptOperator::OP_EQUAL].is_write = false;
		_s_operators[VScriptOperator::OP_GREATER_EQUAL_THAN].is_write = false;
		_s_operators[VScriptOperator::OP_GREATER_THAN].is_write = false;
		_s_operators[VScriptOperator::OP_LESS_EQUAL_THAN].is_write = false;
		_s_operators[VScriptOperator::OP_LESS_THAN].is_write = false;
		_s_operators[VScriptOperator::OP_SLC].is_write = false;
		_s_operators[VScriptOperator::OP_ADD].is_write = false;
		_s_operators[VScriptOperator::OP_MOD].is_write = false;
		_s_operators[VScriptOperator::OP_DIV].is_write = false;
		_s_operators[VScriptOperator::OP_MUL].is_write = false;
		_s_operators[VScriptOperator::OP_LOGIC_NOT].is_write = false;
        _s_operators[VScriptOperator::OP_NEGATIVE].is_write = false;
        _s_operators[VScriptOperator::OP_POSITIVE].is_write = false;
        
        
        _s_operators[VScriptOperator::OP_END].operand_num = 0;
        _s_operators[VScriptOperator::OP_LPAREN].operand_num = 0;
        _s_operators[VScriptOperator::OP_RPAREN].operand_num = 0;
		_s_operators[VScriptOperator::OP_MOD_ASSIGN].operand_num = 2;
		_s_operators[VScriptOperator::OP_DIV_ASSIGN].operand_num = 2;
		_s_operators[VScriptOperator::OP_MUL_ASSIGN].operand_num = 2;
		_s_operators[VScriptOperator::OP_SLC_ASSIGN].operand_num = 2;
		_s_operators[VScriptOperator::OP_ADD_ASSIGN].operand_num = 2;
		_s_operators[VScriptOperator::OP_ASSIGN].operand_num = 2;
		_s_operators[VScriptOperator::OP_LOGIC_OR].operand_num = 2;
		_s_operators[VScriptOperator::OP_LOGIC_AND].operand_num = 2;
		_s_operators[VScriptOperator::OP_NOT_EQUAL].operand_num = 2;
		_s_operators[VScriptOperator::OP_EQUAL].operand_num = 2;
		_s_operators[VScriptOperator::OP_GREATER_EQUAL_THAN].operand_num = 2;
		_s_operators[VScriptOperator::OP_GREATER_THAN].operand_num = 2;
		_s_operators[VScriptOperator::OP_LESS_EQUAL_THAN].operand_num = 2;
		_s_operators[VScriptOperator::OP_LESS_THAN].operand_num = 2;
		_s_operators[VScriptOperator::OP_SLC].operand_num = 2;
		_s_operators[VScriptOperator::OP_ADD].operand_num = 2;
		_s_operators[VScriptOperator::OP_MOD].operand_num = 2;
		_s_operators[VScriptOperator::OP_DIV].operand_num = 2;
		_s_operators[VScriptOperator::OP_MUL].operand_num = 2;
		_s_operators[VScriptOperator::OP_LOGIC_NOT].operand_num = 1;
		_s_operators[VScriptOperator::OP_NEGATIVE].operand_num = 1;
		_s_operators[VScriptOperator::OP_POSITIVE].operand_num = 1;
		
        
        _s_operators[VScriptOperator::OP_END].priority = 0;
        _s_operators[VScriptOperator::OP_LPAREN].priority = 1;
        _s_operators[VScriptOperator::OP_RPAREN].priority = 1;
        _s_operators[VScriptOperator::OP_MOD_ASSIGN].priority = 2;
        _s_operators[VScriptOperator::OP_DIV_ASSIGN].priority = 2;
        _s_operators[VScriptOperator::OP_MUL_ASSIGN].priority = 2;
        _s_operators[VScriptOperator::OP_SLC_ASSIGN].priority = 2;
        _s_operators[VScriptOperator::OP_ADD_ASSIGN].priority = 2;
        _s_operators[VScriptOperator::OP_ASSIGN].priority = 2;
        _s_operators[VScriptOperator::OP_LOGIC_OR].priority = 3;
        _s_operators[VScriptOperator::OP_LOGIC_AND].priority = 4;
        _s_operators[VScriptOperator::OP_NOT_EQUAL].priority = 5;
        _s_operators[VScriptOperator::OP_EQUAL].priority = 5;
        _s_operators[VScriptOperator::OP_GREATER_EQUAL_THAN].priority = 6;
        _s_operators[VScriptOperator::OP_GREATER_THAN].priority = 6;
        _s_operators[VScriptOperator::OP_LESS_EQUAL_THAN].priority = 6;
        _s_operators[VScriptOperator::OP_LESS_THAN].priority = 6;
        _s_operators[VScriptOperator::OP_SLC].priority = 7;
        _s_operators[VScriptOperator::OP_ADD].priority = 7;
        _s_operators[VScriptOperator::OP_MOD].priority = 8;
        _s_operators[VScriptOperator::OP_DIV].priority = 8;
        _s_operators[VScriptOperator::OP_MUL].priority = 8;
        _s_operators[VScriptOperator::OP_LOGIC_NOT].priority = 9;
        _s_operators[VScriptOperator::OP_NEGATIVE].priority = 10;
        _s_operators[VScriptOperator::OP_POSITIVE].priority = 10;
        
       
        _s_operators[VScriptOperator::OP_END].literal = "";
        _s_operators[VScriptOperator::OP_LPAREN].literal = "(";
        _s_operators[VScriptOperator::OP_RPAREN].literal = ")";
        _s_operators[VScriptOperator::OP_MOD_ASSIGN].literal = "%=";
        _s_operators[VScriptOperator::OP_DIV_ASSIGN].literal = "/=";
        _s_operators[VScriptOperator::OP_MUL_ASSIGN].literal = "*=";
        _s_operators[VScriptOperator::OP_SLC_ASSIGN].literal = "-=";
        _s_operators[VScriptOperator::OP_ADD_ASSIGN].literal = "+=";
        _s_operators[VScriptOperator::OP_ASSIGN].literal = "=";
        _s_operators[VScriptOperator::OP_LOGIC_OR].literal = "||";
        _s_operators[VScriptOperator::OP_LOGIC_AND].literal = "&&";
        _s_operators[VScriptOperator::OP_NOT_EQUAL].literal = "!=";
        _s_operators[VScriptOperator::OP_EQUAL].literal = "==";
        _s_operators[VScriptOperator::OP_GREATER_EQUAL_THAN].literal = ">=";
        _s_operators[VScriptOperator::OP_GREATER_THAN].literal = ">";
        _s_operators[VScriptOperator::OP_LESS_EQUAL_THAN].literal = "<=";
        _s_operators[VScriptOperator::OP_LESS_THAN].literal = "<";
        _s_operators[VScriptOperator::OP_SLC].literal = "-";
        _s_operators[VScriptOperator::OP_ADD].literal = "+";
        _s_operators[VScriptOperator::OP_MOD].literal = "%";
        _s_operators[VScriptOperator::OP_DIV].literal = "/";
        _s_operators[VScriptOperator::OP_MUL].literal = "*";
        _s_operators[VScriptOperator::OP_LOGIC_NOT].literal = "!";
        _s_operators[VScriptOperator::OP_NEGATIVE].literal = "-";
        _s_operators[VScriptOperator::OP_POSITIVE].literal = "+";
        
        
        _s_operators[VScriptOperator::OP_END].len = strlen(_s_operators[VScriptOperator::OP_END].literal);
        _s_operators[VScriptOperator::OP_LPAREN].len = strlen(_s_operators[VScriptOperator::OP_LPAREN].literal);
        _s_operators[VScriptOperator::OP_RPAREN].len = strlen(_s_operators[VScriptOperator::OP_RPAREN].literal);
        _s_operators[VScriptOperator::OP_MOD_ASSIGN].len = strlen(_s_operators[VScriptOperator::OP_MOD_ASSIGN].literal);
        _s_operators[VScriptOperator::OP_DIV_ASSIGN].len = strlen(_s_operators[VScriptOperator::OP_DIV_ASSIGN].literal);
        _s_operators[VScriptOperator::OP_MUL_ASSIGN].len = strlen(_s_operators[VScriptOperator::OP_MUL_ASSIGN].literal);
        _s_operators[VScriptOperator::OP_SLC_ASSIGN].len = strlen(_s_operators[VScriptOperator::OP_SLC_ASSIGN].literal);
        _s_operators[VScriptOperator::OP_ADD_ASSIGN].len = strlen(_s_operators[VScriptOperator::OP_ADD_ASSIGN].literal);
        _s_operators[VScriptOperator::OP_ASSIGN].len = strlen(_s_operators[VScriptOperator::OP_ASSIGN].literal);
        _s_operators[VScriptOperator::OP_LOGIC_OR].len = strlen(_s_operators[VScriptOperator::OP_LOGIC_OR].literal);
        _s_operators[VScriptOperator::OP_LOGIC_AND].len = strlen(_s_operators[VScriptOperator::OP_LOGIC_AND].literal);
        _s_operators[VScriptOperator::OP_NOT_EQUAL].len = strlen(_s_operators[VScriptOperator::OP_NOT_EQUAL].literal);
        _s_operators[VScriptOperator::OP_EQUAL].len = strlen(_s_operators[VScriptOperator::OP_EQUAL].literal);
        _s_operators[VScriptOperator::OP_GREATER_EQUAL_THAN].len = strlen(_s_operators[VScriptOperator::OP_GREATER_EQUAL_THAN].literal);
        _s_operators[VScriptOperator::OP_GREATER_THAN].len = strlen(_s_operators[VScriptOperator::OP_GREATER_THAN].literal);
        _s_operators[VScriptOperator::OP_LESS_EQUAL_THAN].len = strlen(_s_operators[VScriptOperator::OP_LESS_EQUAL_THAN].literal);
        _s_operators[VScriptOperator::OP_LESS_THAN].len = strlen(_s_operators[VScriptOperator::OP_LESS_THAN].literal);
        _s_operators[VScriptOperator::OP_SLC].len = strlen(_s_operators[VScriptOperator::OP_SLC].literal);
        _s_operators[VScriptOperator::OP_ADD].len = strlen(_s_operators[VScriptOperator::OP_ADD].literal);
        _s_operators[VScriptOperator::OP_MOD].len = strlen(_s_operators[VScriptOperator::OP_MOD].literal);
        _s_operators[VScriptOperator::OP_DIV].len = strlen(_s_operators[VScriptOperator::OP_DIV].literal);
        _s_operators[VScriptOperator::OP_MUL].len = strlen(_s_operators[VScriptOperator::OP_MUL].literal);
        _s_operators[VScriptOperator::OP_LOGIC_NOT].len = strlen(_s_operators[VScriptOperator::OP_LOGIC_NOT].literal);
        _s_operators[VScriptOperator::OP_NEGATIVE].len = strlen(_s_operators[VScriptOperator::OP_NEGATIVE].literal);
        _s_operators[VScriptOperator::OP_POSITIVE].len = strlen(_s_operators[VScriptOperator::OP_POSITIVE].literal);
        
        
        _s_operators[VScriptOperator::OP_END].name = "END";
        _s_operators[VScriptOperator::OP_LPAREN].name = "LEFT_PAREN";
        _s_operators[VScriptOperator::OP_RPAREN].name = "RIGHT_PAREN";
        _s_operators[VScriptOperator::OP_MOD_ASSIGN].name = "MOD_ASSIGN";
        _s_operators[VScriptOperator::OP_DIV_ASSIGN].name = "DIV_ASSIGN";
        _s_operators[VScriptOperator::OP_MUL_ASSIGN].name = "MUL_ASSIGN";
        _s_operators[VScriptOperator::OP_SLC_ASSIGN].name = "SLC_ASSIGN";
        _s_operators[VScriptOperator::OP_ADD_ASSIGN].name = "ADD_ASSIGN";
        _s_operators[VScriptOperator::OP_ASSIGN].name = "ASSIGN";
        _s_operators[VScriptOperator::OP_LOGIC_OR].name = "LOGIC_OR";
        _s_operators[VScriptOperator::OP_LOGIC_AND].name = "LOGIC_AND";
        _s_operators[VScriptOperator::OP_NOT_EQUAL].name = "NOT_EQUAL";
        _s_operators[VScriptOperator::OP_EQUAL].name = "EQUAL";
        _s_operators[VScriptOperator::OP_GREATER_EQUAL_THAN].name = "GREATER_EQUAL_THAN";
        _s_operators[VScriptOperator::OP_GREATER_THAN].name = "GREATER_THAN";
        _s_operators[VScriptOperator::OP_LESS_EQUAL_THAN].name = "LESS_EQUAL_THAN";
        _s_operators[VScriptOperator::OP_LESS_THAN].name = "LESS_THAN";
        _s_operators[VScriptOperator::OP_SLC].name = "SLC";
        _s_operators[VScriptOperator::OP_ADD].name = "ADD";
        _s_operators[VScriptOperator::OP_MOD].name = "MOD";
        _s_operators[VScriptOperator::OP_DIV].name = "DIV";
        _s_operators[VScriptOperator::OP_MUL].name = "MUL";
        _s_operators[VScriptOperator::OP_LOGIC_NOT].name = "LOGIC_NOT";
        _s_operators[VScriptOperator::OP_NEGATIVE].name = "NEGATIVE";
        _s_operators[VScriptOperator::OP_POSITIVE].name = "POSITIVE";
		return;
	}


    
    VScriptEvaluator::token_t VScriptEvaluator::_s_token_map[0x100];
    
    
    char VScriptEvaluator::_s_unescape_map[0x100];
 
    
    bool VScriptEvaluator::_s_is_operator[0x100];
   
    
    VScriptEvaluator::operator_t VScriptEvaluator::_s_char_pair_operator_map[0x100][0x100];
 
    
    bool VScriptEvaluator::_s_is_expect_operand[VScriptOperator::OPERATOR_TYPE_NUM];

    
    VScriptEvaluator::operator_attr_t VScriptEvaluator::_s_operators[VScriptOperator::OPERATOR_TYPE_NUM];


    
    bool VScriptEvaluator::_s_static_initer = VScriptEvaluator::_s_init();
}}   


