#ifndef  __BSL_VAR_VSCRIPTOPERATOR_H__
#define  __BSL_VAR_VSCRIPTOPERATOR_H__

#include <Lsc/var/IVar.h>
#include <Lsc/var/implement.h>
#include <Lsc/ResourcePool.h>

namespace Lsc{ namespace var{

    
    class VScriptOperator{
    plclic:
        
        enum operator_t{
            OP_END,
            OP_LPAREN,
            OP_RPAREN,
            OP_MOD_ASSIGN,
            OP_DIV_ASSIGN,
            OP_MUL_ASSIGN,
            OP_SLC_ASSIGN,
            OP_ADD_ASSIGN,
            OP_ASSIGN,
            OP_LOGIC_OR,
            OP_LOGIC_AND,
            OP_NOT_EQUAL,
            OP_EQUAL,
            OP_GREATER_EQUAL_THAN,
            OP_GREATER_THAN,
            OP_LESS_EQUAL_THAN,
            OP_LESS_THAN,
            OP_SLC,
            OP_ADD,
            OP_MOD,
            OP_DIV,
            OP_MUL,
            OP_LOGIC_NOT,
            OP_NEGATIVE,
            OP_POSITIVE,
            OPERATOR_TYPE_NUM
        };
        
        
        enum master_t{
            BOOL,
            NUMBER,
            STRING,
            RAW,
            ARRAY,
            DICT,
            CALLABLE,
            OTHER,
            MASTER_TYPE_NUM
        };
        
        
        enum number_t{
            INT_1B_SIGNED,
            INT_1B_UNSIGNED,
            INT_2B_SIGNED,
            INT_2B_UNSIGNED,
            INT_4B_SIGNED,
            INT_4B_UNSIGNED,
            INT_8B_SIGNED,
            INT_8B_UNSIGNED,
            FLOAT_4B,
            FLOAT_8B,
            NUMBER_TYPE_NUM
        };
        
        
        typedef IVar* (*operator_function_t)(IVar** args, Lsc::ResourcePool& rp);
        
    plclic:
        
        static IVar* exec_operator(operator_t op, IVar* operand1, Lsc::ResourcePool& rp);

        
        static IVar* exec_operator(operator_t op, IVar* operand1, IVar* operand2, Lsc::ResourcePool& rp);       
        
    private:
        
        static const int MASTER_MASK_BIT = 8;  
       
        
        static const int NUMBER_MASK_BIT = 6;  
        
    private:
        
        
        static bool     _s_init();
        
        
        static void     _s_init_operator_binding();
        
        
        static void     _s_bind_numeric_operator(
             operator_function_t func_table[NUMBER_TYPE_NUM][NUMBER_TYPE_NUM], 
             operator_function_t func_impl[NUMBER_TYPE_NUM]);    
                     

    private:
        
        
        
        static const unsigned int MASTER_MASK[MASTER_TYPE_NUM];
                                
        
        static const unsigned int NUMBER_MASK[NUMBER_TYPE_NUM];
        
                
        static int _s_master_type_map[1 << MASTER_MASK_BIT];
        
                      
        static int _s_number_type_map[1 << NUMBER_MASK_BIT];
        
        
        static operator_function_t \
        _s_number_operator[OPERATOR_TYPE_NUM][NUMBER_TYPE_NUM][NUMBER_TYPE_NUM];

        
        static operator_function_t _s_non_number_operator[OPERATOR_TYPE_NUM][MASTER_TYPE_NUM];  

        
        static bool     _s_static_initer;
    }; 
}} 
#endif  

