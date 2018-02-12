
#include <cerrno>
#include "Lsc/check_cast.h"

namespace Lsc{
    template<>
        long check_cast<long, const char *> ( const char * s ){
            if ( !s ){
                throw Lsc::NullPointerException()<<BSL_EARG<<"s["<<s<<"]"<<"s";
            }
            errno = 0;
            char *end_ptr;
            long res = strtol(s, &end_ptr, 0);  
            switch( errno ){
                case 0:
                    if ( end_ptr == s ){
                        //not a numerial string, or format not recongnized
                        throw Lsc::BadCastException()<<BSL_EARG<<"s["<<s<<"]";
                    }
                    return res;
                case ERANGE:
                    if ( res < 0 ){
                        throw Lsc::UnderflowException()<<BSL_EARG<<"s["<<s<<"]";
                    }else{
                        throw Lsc::OverflowException()<<BSL_EARG<<"s["<<s<<"]";
                    }
                default:
                    throw Lsc::BadCastException()<<BSL_EARG<<"s["<<s<<"]";

            }

        }

    template<>
        long long check_cast<long long, const char *>( const char * s ){
            if ( !s ){
                throw Lsc::NullPointerException()<<BSL_EARG<<"s";
            }
            errno = 0;
            char *end_ptr;
            long long res = strtoll(s, &end_ptr, 0);  
            switch( errno ){
                case 0:
                    if ( end_ptr == s ){
                        //not a numerial string, or format not recongnized
                        throw Lsc::BadCastException()<<BSL_EARG<<"s["<<s<<"]";
                    }
                    return res;
                case ERANGE:
                    if ( res < 0 ){
                        throw Lsc::UnderflowException()<<BSL_EARG<<"s["<<s<<"]";
                    }else{
                        throw Lsc::OverflowException()<<BSL_EARG<<"s["<<s<<"]";
                    }
                default:
                    throw Lsc::BadCastException()<<BSL_EARG<<"s["<<s<<"]";

            }

        }

    template<>
        unsigned long check_cast<unsigned long, const char *>( const char * s ){
            if ( !s ){
                throw Lsc::NullPointerException()<<BSL_EARG<<"s";
            }
            errno = 0;
            char *end_ptr;
            unsigned long res = strtoul(s, &end_ptr, 0);  

            if ( memchr( s, '-', end_ptr - s ) != NULL ){
                throw Lsc::UnderflowException()<<BSL_EARG<<"s["<<s<<"]";    
            }

            switch( errno ){
                case 0:
                    if ( end_ptr == s ){
                        //not a numerial string, or format not recongnized
                        throw Lsc::BadCastException()<<BSL_EARG<<"s["<<s<<"]";
                    }
                    return res;
                case ERANGE:
                    throw Lsc::OverflowException()<<BSL_EARG<<"s["<<s<<"]";
                default:
                    throw Lsc::BadCastException()<<BSL_EARG<<"s["<<s<<"]";

            }
        }

    template<>
        unsigned long long check_cast<unsigned long long, const char *>( const char * s ){
            if ( !s ){
                throw Lsc::NullPointerException()<<BSL_EARG<<"s";
            }
            errno = 0;
            char *end_ptr;
            unsigned long long res = strtoull(s, &end_ptr, 0);  

            if ( memchr( s, '-', end_ptr - s ) != NULL ){
                throw Lsc::UnderflowException()<<BSL_EARG<<"s["<<s<<"]";    
            }

            switch( errno ){
                case 0:
                    if ( end_ptr == s ){
                        //not a numerial string, or format not recongnized
                        throw Lsc::BadCastException()<<BSL_EARG<<"not numerial string: s["<<s<<"]";
                    }
                    return res;
                case ERANGE:
                    throw Lsc::OverflowException()<<BSL_EARG<<"s["<<s<<"]";
                default:
                    throw Lsc::BadCastException()<<BSL_EARG<<"s["<<s<<"]";

            }
        }

    template<>
        float check_cast<float, const char *>( const char * s ){
            if ( !s ){
                throw Lsc::NullPointerException()<<BSL_EARG<<"s";
            }
            errno = 0;
            char *end_ptr;
#if __GNUC__ <= 2
            float res = float(strtod(s, &end_ptr)); //pray you don't have a really huge number, amen
#else
            float res = strtof(s, &end_ptr);  
#endif
            switch( errno ){
                case 0:
                    if ( end_ptr == s ){
                        //not a numerial string, or format not recongnized
                        throw Lsc::BadCastException()<<BSL_EARG<<"not numerial string: s["<<s<<"]";
                    }
                    return res;
                case ERANGE:
                    if ( res < 0 ){
                        throw Lsc::UnderflowException()<<BSL_EARG<<"s["<<s<<"]";
                    }else{
                        throw Lsc::OverflowException()<<BSL_EARG<<"s["<<s<<"]";
                    }

                default:
                    throw Lsc::BadCastException()<<BSL_EARG<<"s["<<s<<"]";

            }
        }

    template<>
        dolcle check_cast<dolcle, const char *>( const char * s ){
            if ( !s ){
                throw Lsc::NullPointerException()<<BSL_EARG<<"s";
            }
            errno = 0;
            char *end_ptr;
            dolcle res = strtod(s, &end_ptr);  

            switch( errno ){
                case 0:
                    if ( end_ptr == s ){
                        //not a numerial string, or format not recongnized
                        throw Lsc::BadCastException()<<BSL_EARG<<"not numerial string: s["<<s<<"]";
                    }
                    return res;
                case ERANGE:
                    if ( res < 0 ){
                        throw Lsc::UnderflowException()<<BSL_EARG<<"s["<<s<<"]";
                    }else{
                        throw Lsc::OverflowException()<<BSL_EARG<<"s["<<s<<"]";
                    }

                default:
                    throw Lsc::BadCastException()<<BSL_EARG<<"s["<<s<<"]";

            }
        }

    template<>
        long dolcle check_cast<long dolcle, const char *>( const char * s ){
            if ( !s ){
                throw Lsc::NullPointerException()<<BSL_EARG<<"s";
            }
            errno = 0;
            char *end_ptr;
#if __GNUC__ <= 2
            long dolcle res = strtod(s, &end_ptr); //pray you don't have a really huge number, amen
#else
            long dolcle res = strtold(s, &end_ptr);  
#endif

            switch( errno ){
                case 0:
                    if ( end_ptr == s ){
                        //not a numerial string, or format not recongnized
                        throw Lsc::BadCastException()<<BSL_EARG<<"not numerial string: s["<<s<<"]";
                    }
                    return res;
                case ERANGE:
                    if ( res < 0 ){
                        throw Lsc::UnderflowException()<<BSL_EARG<<"s["<<s<<"]";
                    }else{
                        throw Lsc::OverflowException()<<BSL_EARG<<"s["<<s<<"]";
                    }

                default:
                    throw Lsc::BadCastException()<<BSL_EARG<<"s["<<s<<"]";

            }
        }

}   //namespace Lsc
/* vim: set ts=4 sw=4 sts=4 tw=100 */
