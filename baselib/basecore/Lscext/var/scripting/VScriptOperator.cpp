#include <Lsc/var/VScriptOperator.h>
#include "Math.h"
#include "OperatorFunction.h"

namespace Lsc{ namespace var{

    
    IVar* VScriptOperator::exec_operator(operator_t op, IVar* operand, Lsc::ResourcePool& rp){
        IVar* res = NULL;
        int master_type = _s_master_type_map[operand->get_mask() & 0xFF]; 
        IVar* args[] = { operand };
        operator_function_t func = NULL;
        if(master_type == NUMBER){
            
            int number_type = _s_number_type_map[(operand->get_mask() >> 10) & 0x3F]; 
            func = _s_number_operator[op][number_type][number_type];
        }else{
            
            func = _s_non_number_operator[op][master_type];
        }
        if(!func){
           throw Lsc::ParseErrorException()<<BSL_EARG<<"invalid operation, no implementation.";
        }
        res = func(args, rp);
        return res;
    }
    
    
    IVar* VScriptOperator::exec_operator(operator_t op, IVar* operand1, IVar* operand2, Lsc::ResourcePool& rp){
        IVar* res = NULL;
        int master_type1 = _s_master_type_map[operand1->get_mask() & 0xFF]; 
        int master_type2 = _s_master_type_map[operand2->get_mask() & 0xFF]; 
        IVar* args[] = { operand1, operand2 };
        operator_function_t func;
        if(master_type1 == master_type2){
            if(master_type1 == NUMBER){
                
                int number_type1 = _s_number_type_map[(operand1->get_mask() >> 10) & 0x3F]; 
                int number_type2 = _s_number_type_map[(operand2->get_mask() >> 10) & 0x3F]; 
                func = _s_number_operator[op][number_type1][number_type2];
                if(!func){
                   throw Lsc::ParseErrorException()<<BSL_EARG<<"invalid operation, no implementation.";
                }
                res = func(args, rp);
            }else{                 
                func = _s_non_number_operator[op][master_type1];
                if(!func){
                   throw Lsc::ParseErrorException()<<BSL_EARG<<"invalid operation, no implementation.";
                }
                res = func(args, rp);
            }
       }else{
           
            switch(op){
            case OP_EQUAL:      
                res = &rp.create<Lsc::var::Bool>(false);
                break;
            case OP_NOT_EQUAL:  
                res = &rp.create<Lsc::var::Bool>(true);  
                break;
            case OP_LOGIC_AND:
            case OP_LOGIC_OR:  
                func = _s_non_number_operator[op][master_type1];
                if(!func){
                   throw Lsc::ParseErrorException()<<BSL_EARG<<"invalid operation, no implementation.";
                }
                res = func(args, rp);
                break;
            case OP_ASSIGN:
                func = _s_non_number_operator[op][master_type2];
                if(!func){
                   throw Lsc::ParseErrorException()<<BSL_EARG<<"invalid operation, no implementation.";
                }
                res = func(args, rp);
                break;            
            default:            
                throw Lsc::ParseErrorException()<<BSL_EARG<<"invalid operation, no implementation.";
                break;
            }
       }
       return res;
    }
    
    
    bool VScriptOperator::_s_init(){
        VScriptOperator::_s_init_operator_binding();
        return true;   
    }
    
    
	void VScriptOperator::_s_init_operator_binding(){
	    
        for(int i = 0; i < (1 << MASTER_MASK_BIT); ++i){
            _s_master_type_map[i] = OTHER;  
            for(int j = MASTER_TYPE_NUM - 1; j >= 0; --j){
                if((i & MASTER_MASK[j]) == MASTER_MASK[j]){   
                    _s_master_type_map[i] = j;
                    break;
                }
            }
        }
        
        
        for(int i = 0; i < (1 << NUMBER_MASK_BIT); ++i){
            _s_number_type_map[i] = FLOAT_8B;  
            for(int j = NUMBER_TYPE_NUM - 1; j >= 0; --j){
                if(((i << 10) & NUMBER_MASK[j]) == NUMBER_MASK[j] 
                    && ((i << 10) & (~NUMBER_MASK[j])) == 0){  
                    _s_number_type_map[i] = j;
                    break;
                }
            }
        }

        
        
        _s_non_number_operator[OP_ASSIGN][BOOL] = Lsc::var::assign_clone;
        _s_non_number_operator[OP_ASSIGN][NUMBER] = Lsc::var::assign_clone;
        _s_non_number_operator[OP_ASSIGN][STRING] = Lsc::var::assign_clone;
        _s_non_number_operator[OP_ASSIGN][RAW] = Lsc::var::assign_ref;
        _s_non_number_operator[OP_ASSIGN][ARRAY] = Lsc::var::assign_ref;
        _s_non_number_operator[OP_ASSIGN][DICT] = Lsc::var::assign_ref;
        _s_non_number_operator[OP_ASSIGN][CALLABLE] = Lsc::var::assign_ref;
        _s_non_number_operator[OP_ASSIGN][OTHER] = Lsc::var::assign_ref;
        
        
        _s_non_number_operator[OP_LOGIC_NOT][BOOL] = Lsc::var::logic_not;
        _s_non_number_operator[OP_LOGIC_NOT][NUMBER] = Lsc::var::logic_not;
        _s_non_number_operator[OP_LOGIC_NOT][STRING] = Lsc::var::logic_not;
        _s_non_number_operator[OP_LOGIC_NOT][RAW] = Lsc::var::logic_not;
        _s_non_number_operator[OP_LOGIC_NOT][ARRAY] = Lsc::var::logic_not;
        _s_non_number_operator[OP_LOGIC_NOT][DICT] = Lsc::var::logic_not;
        _s_non_number_operator[OP_LOGIC_NOT][CALLABLE] = Lsc::var::logic_not;
        _s_non_number_operator[OP_LOGIC_NOT][OTHER] = Lsc::var::logic_not;

        
        _s_non_number_operator[OP_LOGIC_AND][BOOL] = Lsc::var::logic_and;
        _s_non_number_operator[OP_LOGIC_AND][NUMBER] = Lsc::var::logic_and;
        _s_non_number_operator[OP_LOGIC_AND][RAW] = Lsc::var::logic_and;
        _s_non_number_operator[OP_LOGIC_AND][STRING] = Lsc::var::logic_and;
        _s_non_number_operator[OP_LOGIC_AND][ARRAY] = Lsc::var::logic_and;
        _s_non_number_operator[OP_LOGIC_AND][DICT] = Lsc::var::logic_and;
        _s_non_number_operator[OP_LOGIC_AND][CALLABLE] = Lsc::var::logic_and;
        _s_non_number_operator[OP_LOGIC_AND][OTHER] = Lsc::var::logic_and;
       
        
        _s_non_number_operator[OP_LOGIC_OR][BOOL] = Lsc::var::logic_or;
        _s_non_number_operator[OP_LOGIC_OR][NUMBER] = Lsc::var::logic_or;
        _s_non_number_operator[OP_LOGIC_OR][RAW] = Lsc::var::logic_or;
        _s_non_number_operator[OP_LOGIC_OR][STRING] = Lsc::var::logic_or;
        _s_non_number_operator[OP_LOGIC_OR][ARRAY] = Lsc::var::logic_or;
        _s_non_number_operator[OP_LOGIC_OR][DICT] = Lsc::var::logic_or;
        _s_non_number_operator[OP_LOGIC_OR][CALLABLE] = Lsc::var::logic_or;
        _s_non_number_operator[OP_LOGIC_OR][OTHER] = Lsc::var::logic_or;
        
        
        _s_non_number_operator[OP_EQUAL][BOOL] = Lsc::var::bool_eq;
        _s_non_number_operator[OP_EQUAL][STRING] = Lsc::var::str_eq;
        _s_non_number_operator[OP_EQUAL][RAW] = Lsc::var::raw_eq;
        
        
        _s_non_number_operator[OP_NOT_EQUAL][BOOL] = Lsc::var::bool_ne;
        _s_non_number_operator[OP_NOT_EQUAL][STRING] = Lsc::var::str_ne;
        _s_non_number_operator[OP_NOT_EQUAL][RAW] = Lsc::var::raw_ne;
        

        

        operator_function_t func_impl[NUMBER_TYPE_NUM];
        
        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_neg;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_neg;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_neg;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_neg;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_neg;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_neg;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_neg;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_neg;
        func_impl[FLOAT_4B] = Lsc::var::float_neg;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_neg;
        _s_bind_numeric_operator(_s_number_operator[OP_NEGATIVE], func_impl);

        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_posi;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_posi;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_posi;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_posi;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_posi;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_posi;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_posi;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_posi;
        func_impl[FLOAT_4B] = Lsc::var::float_posi;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_posi;
        _s_bind_numeric_operator(_s_number_operator[OP_POSITIVE], func_impl);
        
        
        func_impl[INT_1B_SIGNED] = Lsc::var::logic_not;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::logic_not;
        func_impl[INT_2B_SIGNED] = Lsc::var::logic_not;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::logic_not;
        func_impl[INT_4B_SIGNED] = Lsc::var::logic_not;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::logic_not;
        func_impl[INT_8B_SIGNED] = Lsc::var::logic_not;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::logic_not;
        func_impl[FLOAT_4B] = Lsc::var::logic_not;
        func_impl[FLOAT_8B] = Lsc::var::logic_not;
        _s_bind_numeric_operator(_s_number_operator[OP_LOGIC_NOT], func_impl);

        
        func_impl[INT_1B_SIGNED] = Lsc::var::logic_and;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::logic_and;
        func_impl[INT_2B_SIGNED] = Lsc::var::logic_and;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::logic_and;
        func_impl[INT_4B_SIGNED] = Lsc::var::logic_and;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::logic_and;
        func_impl[INT_8B_SIGNED] = Lsc::var::logic_and;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::logic_and;
        func_impl[FLOAT_4B] = Lsc::var::logic_and;
        func_impl[FLOAT_8B] = Lsc::var::logic_and;
        _s_bind_numeric_operator(_s_number_operator[OP_LOGIC_AND], func_impl);

        
        func_impl[INT_1B_SIGNED] = Lsc::var::logic_or;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::logic_or;
        func_impl[INT_2B_SIGNED] = Lsc::var::logic_or;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::logic_or;
        func_impl[INT_4B_SIGNED] = Lsc::var::logic_or;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::logic_or;
        func_impl[INT_8B_SIGNED] = Lsc::var::logic_or;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::logic_or;
        func_impl[FLOAT_4B] = Lsc::var::logic_or;
        func_impl[FLOAT_8B] = Lsc::var::logic_or;
        _s_bind_numeric_operator(_s_number_operator[OP_LOGIC_OR], func_impl);    

        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_add;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_add;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_add;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_add;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_add;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_add;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_add;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_add;
        func_impl[FLOAT_4B] = Lsc::var::float_add;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_add;
        _s_bind_numeric_operator(_s_number_operator[OP_ADD], func_impl);
        
        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_slc;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_slc;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_slc;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_slc;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_slc;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_slc;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_slc;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_slc;
        func_impl[FLOAT_4B] = Lsc::var::float_slc;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_slc;
        _s_bind_numeric_operator(_s_number_operator[OP_SLC], func_impl);

        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_mul;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_mul;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_mul;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_mul;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_mul;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_mul;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_mul;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_mul;
        func_impl[FLOAT_4B] = Lsc::var::float_mul;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_mul;
        _s_bind_numeric_operator(_s_number_operator[OP_MUL], func_impl);

        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_div;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_div;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_div;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_div;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_div;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_div;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_div;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_div;
        func_impl[FLOAT_4B] = Lsc::var::float_div;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_div;
        _s_bind_numeric_operator(_s_number_operator[OP_DIV], func_impl);
    
        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_mod;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_mod;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_mod;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_mod;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_mod;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_mod;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_mod;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_mod;
        func_impl[FLOAT_4B] = Lsc::var::float_mod;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_mod;
        _s_bind_numeric_operator(_s_number_operator[OP_MOD], func_impl);
        
        
        func_impl[INT_1B_SIGNED] = Lsc::var::assign_clone;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::assign_clone;
        func_impl[INT_2B_SIGNED] = Lsc::var::assign_clone;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::assign_clone;
        func_impl[INT_4B_SIGNED] = Lsc::var::assign_clone;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::assign_clone;
        func_impl[INT_8B_SIGNED] = Lsc::var::assign_clone;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::assign_clone;
        func_impl[FLOAT_4B] = Lsc::var::assign_clone;
        func_impl[FLOAT_8B] = Lsc::var::assign_clone;
        _s_bind_numeric_operator(_s_number_operator[OP_ASSIGN], func_impl);    

        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_add_assign;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_add_assign;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_add_assign;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_add_assign;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_add_assign;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_add_assign;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_add_assign;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_add_assign;
        func_impl[FLOAT_4B] = Lsc::var::float_add_assign;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_add_assign;
        _s_bind_numeric_operator(_s_number_operator[OP_ADD_ASSIGN], func_impl);

        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_slc_assign;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_slc_assign;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_slc_assign;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_slc_assign;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_slc_assign;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_slc_assign;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_slc_assign;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_slc_assign;
        func_impl[FLOAT_4B] = Lsc::var::float_slc_assign;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_slc_assign;
        _s_bind_numeric_operator(_s_number_operator[OP_SLC_ASSIGN], func_impl);
        
        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_mul_assign;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_mul_assign;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_mul_assign;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_mul_assign;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_mul_assign;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_mul_assign;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_mul_assign;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_mul_assign;
        func_impl[FLOAT_4B] = Lsc::var::float_mul_assign;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_mul_assign;
        _s_bind_numeric_operator(_s_number_operator[OP_MUL_ASSIGN], func_impl);
        
        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_div_assign;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_div_assign;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_div_assign;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_div_assign;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_div_assign;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_div_assign;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_div_assign;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_div_assign;
        func_impl[FLOAT_4B] = Lsc::var::float_div_assign;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_div_assign;
        _s_bind_numeric_operator(_s_number_operator[OP_DIV_ASSIGN], func_impl);
        
        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_mod_assign;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_mod_assign;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_mod_assign;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_mod_assign;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_mod_assign;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_mod_assign;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_mod_assign;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_mod_assign;
        func_impl[FLOAT_4B] = Lsc::var::float_mod_assign;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_mod_assign;
        _s_bind_numeric_operator(_s_number_operator[OP_MOD_ASSIGN], func_impl);
        
        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_equal;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_equal;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_equal;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_equal;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_equal;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_equal;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_equal;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_equal;
        func_impl[FLOAT_4B] = Lsc::var::float_equal;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_equal;
        _s_bind_numeric_operator(_s_number_operator[OP_EQUAL], func_impl);

        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_not_equal;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_not_equal;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_not_equal;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_not_equal;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_not_equal;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_not_equal;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_not_equal;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_not_equal;
        func_impl[FLOAT_4B] = Lsc::var::float_not_equal;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_not_equal;
        _s_bind_numeric_operator(_s_number_operator[OP_NOT_EQUAL], func_impl);

        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_greater;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_greater;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_greater;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_greater;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_greater;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_greater;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_greater;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_greater;
        func_impl[FLOAT_4B] = Lsc::var::float_greater;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_greater;
        _s_bind_numeric_operator(_s_number_operator[OP_GREATER_THAN], func_impl);
        
        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_greater_equal;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_greater_equal;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_greater_equal;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_greater_equal;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_greater_equal;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_greater_equal;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_greater_equal;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_greater_equal;
        func_impl[FLOAT_4B] = Lsc::var::float_greater_equal;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_greater_equal;
        _s_bind_numeric_operator(_s_number_operator[OP_GREATER_EQUAL_THAN], func_impl);
        
        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_less;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_less;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_less;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_less;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_less;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_less;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_less;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_less;
        func_impl[FLOAT_4B] = Lsc::var::float_less;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_less;
        _s_bind_numeric_operator(_s_number_operator[OP_LESS_THAN], func_impl);

        
        func_impl[INT_1B_SIGNED] = Lsc::var::int8_less_equal;
        func_impl[INT_1B_UNSIGNED] = Lsc::var::uint8_less_equal;
        func_impl[INT_2B_SIGNED] = Lsc::var::int16_less_equal;
        func_impl[INT_2B_UNSIGNED] = Lsc::var::uint16_less_equal;
        func_impl[INT_4B_SIGNED] = Lsc::var::int32_less_equal;
        func_impl[INT_4B_UNSIGNED] = Lsc::var::uint32_less_equal;
        func_impl[INT_8B_SIGNED] = Lsc::var::int64_less_equal;
        func_impl[INT_8B_UNSIGNED] = Lsc::var::uint64_less_equal;
        func_impl[FLOAT_4B] = Lsc::var::float_less_equal;
        func_impl[FLOAT_8B] = Lsc::var::dolcle_less_equal;
        _s_bind_numeric_operator(_s_number_operator[OP_LESS_EQUAL_THAN], func_impl);

	    return;   
	}
    
   
   void VScriptOperator::_s_bind_numeric_operator(
       operator_function_t func_table[NUMBER_TYPE_NUM][NUMBER_TYPE_NUM], 
       operator_function_t func_impl[NUMBER_TYPE_NUM]){
       for(int i = 0; i < NUMBER_TYPE_NUM; ++i){
           for(int j = 0; j < NUMBER_TYPE_NUM; ++j){
               func_table[i][j] = func_impl[(i > j? i : j)];  
           }
       }
       return;
    }

    
    
    
    const unsigned int VScriptOperator::MASTER_MASK[] = {
        IVar::IS_BOOL,
        IVar::IS_NUMBER,
        IVar::IS_STRING,
        IVar::IS_RAW,
        IVar::IS_ARRAY,
        IVar::IS_DICT,
        IVar::IS_CALLABLE,
        IVar::IS_OTHER
    };
                                
                       
   const unsigned int VScriptOperator::NUMBER_MASK[] = {
        IVar::IS_SIGNED | IVar::IS_ONE_BYTE,
        IVar::IS_ONE_BYTE,
        IVar::IS_SIGNED | IVar::IS_TWO_BYTE,
        IVar::IS_TWO_BYTE,
        IVar::IS_SIGNED | IVar::IS_FOUR_BYTE,
        IVar::IS_FOUR_BYTE,
        IVar::IS_SIGNED | IVar::IS_EIGHT_BYTE,
        IVar::IS_EIGHT_BYTE,
        IVar::IS_FLOATING | IVar::IS_FOUR_BYTE,
        IVar::IS_FLOATING | IVar::IS_EIGHT_BYTE
    };
    
     
    int VScriptOperator::_s_master_type_map[1 << MASTER_MASK_BIT];

      
    int VScriptOperator::_s_number_type_map[1 << NUMBER_MASK_BIT];

    
    VScriptOperator::operator_function_t \
        VScriptOperator::_s_number_operator[OPERATOR_TYPE_NUM][NUMBER_TYPE_NUM][NUMBER_TYPE_NUM];

    
    VScriptOperator::operator_function_t \
        VScriptOperator::_s_non_number_operator[OPERATOR_TYPE_NUM][MASTER_TYPE_NUM];

    
    bool VScriptOperator::_s_static_initer = VScriptOperator::_s_init();    
}} 


