
#ifndef  __BSL_VAR_ASSIGN_H__
#define  __BSL_VAR_ASSIGN_H__
#include "Lsc/var/IVar.h"

namespace Lsc{ namespace var{
    /**
     *
    **/

    inline IVar& assign( IVar& arr
    ){
        arr.clear();
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& arr
        , IVar& arg0
    ){
        arr.clear();
        arr.set(0,arg0);
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& arr
        , IVar& arg0
        , IVar& arg1
    ){
        arr.clear();
        arr.set(0,arg0);
        arr.set(1,arg1);
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& arr
        , IVar& arg0
        , IVar& arg1
        , IVar& arg2
    ){
        arr.clear();
        arr.set(0,arg0);
        arr.set(1,arg1);
        arr.set(2,arg2);
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& arr
        , IVar& arg0
        , IVar& arg1
        , IVar& arg2
        , IVar& arg3
    ){
        arr.clear();
        arr.set(0,arg0);
        arr.set(1,arg1);
        arr.set(2,arg2);
        arr.set(3,arg3);
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& arr
        , IVar& arg0
        , IVar& arg1
        , IVar& arg2
        , IVar& arg3
        , IVar& arg4
    ){
        arr.clear();
        arr.set(0,arg0);
        arr.set(1,arg1);
        arr.set(2,arg2);
        arr.set(3,arg3);
        arr.set(4,arg4);
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& arr
        , IVar& arg0
        , IVar& arg1
        , IVar& arg2
        , IVar& arg3
        , IVar& arg4
        , IVar& arg5
    ){
        arr.clear();
        arr.set(0,arg0);
        arr.set(1,arg1);
        arr.set(2,arg2);
        arr.set(3,arg3);
        arr.set(4,arg4);
        arr.set(5,arg5);
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& arr
        , IVar& arg0
        , IVar& arg1
        , IVar& arg2
        , IVar& arg3
        , IVar& arg4
        , IVar& arg5
        , IVar& arg6
    ){
        arr.clear();
        arr.set(0,arg0);
        arr.set(1,arg1);
        arr.set(2,arg2);
        arr.set(3,arg3);
        arr.set(4,arg4);
        arr.set(5,arg5);
        arr.set(6,arg6);
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& arr
        , IVar& arg0
        , IVar& arg1
        , IVar& arg2
        , IVar& arg3
        , IVar& arg4
        , IVar& arg5
        , IVar& arg6
        , IVar& arg7
    ){
        arr.clear();
        arr.set(0,arg0);
        arr.set(1,arg1);
        arr.set(2,arg2);
        arr.set(3,arg3);
        arr.set(4,arg4);
        arr.set(5,arg5);
        arr.set(6,arg6);
        arr.set(7,arg7);
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& arr
        , IVar& arg0
        , IVar& arg1
        , IVar& arg2
        , IVar& arg3
        , IVar& arg4
        , IVar& arg5
        , IVar& arg6
        , IVar& arg7
        , IVar& arg8
    ){
        arr.clear();
        arr.set(0,arg0);
        arr.set(1,arg1);
        arr.set(2,arg2);
        arr.set(3,arg3);
        arr.set(4,arg4);
        arr.set(5,arg5);
        arr.set(6,arg6);
        arr.set(7,arg7);
        arr.set(8,arg8);
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& arr
        , IVar& arg0
        , IVar& arg1
        , IVar& arg2
        , IVar& arg3
        , IVar& arg4
        , IVar& arg5
        , IVar& arg6
        , IVar& arg7
        , IVar& arg8
        , IVar& arg9
    ){
        arr.clear();
        arr.set(0,arg0);
        arr.set(1,arg1);
        arr.set(2,arg2);
        arr.set(3,arg3);
        arr.set(4,arg4);
        arr.set(5,arg5);
        arr.set(6,arg6);
        arr.set(7,arg7);
        arr.set(8,arg8);
        arr.set(9,arg9);
        return arr;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& dict
        , const Lsc::string& key0, IVar& value0
    ){
        dict.clear();
        dict.set(key0,value0);
        return dict;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& dict
        , const Lsc::string& key0, IVar& value0
        , const Lsc::string& key1, IVar& value1
    ){
        dict.clear();
        dict.set(key0,value0);
        dict.set(key1,value1);
        return dict;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& dict
        , const Lsc::string& key0, IVar& value0
        , const Lsc::string& key1, IVar& value1
        , const Lsc::string& key2, IVar& value2
    ){
        dict.clear();
        dict.set(key0,value0);
        dict.set(key1,value1);
        dict.set(key2,value2);
        return dict;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& dict
        , const Lsc::string& key0, IVar& value0
        , const Lsc::string& key1, IVar& value1
        , const Lsc::string& key2, IVar& value2
        , const Lsc::string& key3, IVar& value3
    ){
        dict.clear();
        dict.set(key0,value0);
        dict.set(key1,value1);
        dict.set(key2,value2);
        dict.set(key3,value3);
        return dict;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& dict
        , const Lsc::string& key0, IVar& value0
        , const Lsc::string& key1, IVar& value1
        , const Lsc::string& key2, IVar& value2
        , const Lsc::string& key3, IVar& value3
        , const Lsc::string& key4, IVar& value4
    ){
        dict.clear();
        dict.set(key0,value0);
        dict.set(key1,value1);
        dict.set(key2,value2);
        dict.set(key3,value3);
        dict.set(key4,value4);
        return dict;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& dict
        , const Lsc::string& key0, IVar& value0
        , const Lsc::string& key1, IVar& value1
        , const Lsc::string& key2, IVar& value2
        , const Lsc::string& key3, IVar& value3
        , const Lsc::string& key4, IVar& value4
        , const Lsc::string& key5, IVar& value5
    ){
        dict.clear();
        dict.set(key0,value0);
        dict.set(key1,value1);
        dict.set(key2,value2);
        dict.set(key3,value3);
        dict.set(key4,value4);
        dict.set(key5,value5);
        return dict;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& dict
        , const Lsc::string& key0, IVar& value0
        , const Lsc::string& key1, IVar& value1
        , const Lsc::string& key2, IVar& value2
        , const Lsc::string& key3, IVar& value3
        , const Lsc::string& key4, IVar& value4
        , const Lsc::string& key5, IVar& value5
        , const Lsc::string& key6, IVar& value6
    ){
        dict.clear();
        dict.set(key0,value0);
        dict.set(key1,value1);
        dict.set(key2,value2);
        dict.set(key3,value3);
        dict.set(key4,value4);
        dict.set(key5,value5);
        dict.set(key6,value6);
        return dict;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& dict
        , const Lsc::string& key0, IVar& value0
        , const Lsc::string& key1, IVar& value1
        , const Lsc::string& key2, IVar& value2
        , const Lsc::string& key3, IVar& value3
        , const Lsc::string& key4, IVar& value4
        , const Lsc::string& key5, IVar& value5
        , const Lsc::string& key6, IVar& value6
        , const Lsc::string& key7, IVar& value7
    ){
        dict.clear();
        dict.set(key0,value0);
        dict.set(key1,value1);
        dict.set(key2,value2);
        dict.set(key3,value3);
        dict.set(key4,value4);
        dict.set(key5,value5);
        dict.set(key6,value6);
        dict.set(key7,value7);
        return dict;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& dict
        , const Lsc::string& key0, IVar& value0
        , const Lsc::string& key1, IVar& value1
        , const Lsc::string& key2, IVar& value2
        , const Lsc::string& key3, IVar& value3
        , const Lsc::string& key4, IVar& value4
        , const Lsc::string& key5, IVar& value5
        , const Lsc::string& key6, IVar& value6
        , const Lsc::string& key7, IVar& value7
        , const Lsc::string& key8, IVar& value8
    ){
        dict.clear();
        dict.set(key0,value0);
        dict.set(key1,value1);
        dict.set(key2,value2);
        dict.set(key3,value3);
        dict.set(key4,value4);
        dict.set(key5,value5);
        dict.set(key6,value6);
        dict.set(key7,value7);
        dict.set(key8,value8);
        return dict;
    }
    /**
     *
    **/

    inline IVar& assign( IVar& dict
        , const Lsc::string& key0, IVar& value0
        , const Lsc::string& key1, IVar& value1
        , const Lsc::string& key2, IVar& value2
        , const Lsc::string& key3, IVar& value3
        , const Lsc::string& key4, IVar& value4
        , const Lsc::string& key5, IVar& value5
        , const Lsc::string& key6, IVar& value6
        , const Lsc::string& key7, IVar& value7
        , const Lsc::string& key8, IVar& value8
        , const Lsc::string& key9, IVar& value9
    ){
        dict.clear();
        dict.set(key0,value0);
        dict.set(key1,value1);
        dict.set(key2,value2);
        dict.set(key3,value3);
        dict.set(key4,value4);
        dict.set(key5,value5);
        dict.set(key6,value6);
        dict.set(key7,value7);
        dict.set(key8,value8);
        dict.set(key9,value9);
        return dict;
    }
}} //namespace Lsc::var
#endif // __BSL_VAR_ASSIGN_H__
/* vim: set ts=4 sw=4 sts=4 tw=100 */
