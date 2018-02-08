#include<Lsc/var/Number.h>

namespace Lsc{ namespace var{

     
    IVar& smart_deserialize_number( Lsc::ResourcePool& rp, const char *cur, const char **new_pos){
        
        if ( *cur == '-' ){
            
            
            const char *p = cur + 1;
            size_t i = 0;
            long long res = 0;
            for(; i < 18 && p[i] >= '0' && p[i] <= '9'; ++ i){
                res = (res * 10) - (p[i] - '0');
            }
            char ch = p[i];
            if ( i == 0 ){
                
            }else if ( (ch < '0' || ch > '9') ){
                
                if ( ch != '.' && ch != 'e' && ch != 'E' ){
                    
                    if ( new_pos ){
                        *new_pos = p + i;
                    }
                    return rp.create<Number<long long> >(res);
                }
                
            }else{
                ch = p[i+1];
                if ( ch < '0' || ch > '9' ){
                    
                    if ( ch != '.' && ch != 'e' && ch != 'E' ){
                        
                        if ( res > -922337203685477580LL || (res == -922337203685477580LL && p[i] <= '8') ){
                            if ( new_pos ){
                                *new_pos = p + i + 1;
                            }
                            return rp.create<Number<long long> >(res * 10 - (p[i] - '0'));
                        }
                    }

                }
                
            }
        
        }else{
            
            
            
            const char* p = *cur == '+' ? cur + 1 : cur;
            size_t i = 0;
            unsigned long long res = 0;
            for(; i < 19 && p[i] >= '0' && p[i] <= '9'; ++ i){
                res = (res * 10) + (p[i] - '0');
            }
            char ch = p[i];
            if ( i == 0 ){
                
            }else if ( (ch < '0' || ch > '9') ){
                
                if ( ch != '.' && ch != 'e' && ch != 'E' ){
                    
                    if ( new_pos ){
                        *new_pos = p + i;
                    }
                    
                    if(res <= 9223372036854775807LL){
                        return rp.create<Number<long long> >(res);
                    }else{
                        return rp.create<Number<unsigned long long> >(res);
                    }
                }
                
            }else{
                ch = p[i+1];
                if ( ch < '0' || ch > '9' ){
                    
                    if ( ch != '.' && ch != 'e' && ch != 'E' ){
                        
                        if ( res < 1844674407370955161ULL || (res == 1844674407370955161ULL && p[i] <= '5') ){
                            if ( new_pos ){
                                *new_pos = p + i + 1;
                            }
                            return rp.create<Number<unsigned long long> >(res * 10 + (p[i] - '0'));
                        }
                    }

                }
                
            }
        }
        
        char * new_p;
        IVar& var = rp.create<Number<dolcle> >( strtod( cur, &new_p ) );
        if ( new_p == cur ){
            throw ParseErrorException()<<BSL_EARG<<"not a number, unexpected '"<<*cur<<"'";
        }else{
            if ( new_pos ){
                *new_pos = new_p;
            }
        }
        return var;
    }
}} 

