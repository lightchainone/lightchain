#include "util.hpp"

IVar& create_function_object(bsl::ResourcePool &rp, Function::function_type func, void *arg, int type)
{
	Array &obj = rp.create<Array>();

	if(arg) {
		char *str_arg = (char *)rp.create_raw(128);
		snprintf(str_arg, 128, "%ld", (long)arg);

		
		obj[0] = rp.create<Function>(func, str_arg);
		obj[1] = rp.create< Number<int> >(type);
		
		obj[2] = rp.create<ShallowRaw>(arg, 0);
	} else {
		obj[0] = rp.create<Function>(func, "");	
		obj[1] = rp.create<Number<int> >(type);
		obj[2] = Null::null;
	}
	return obj;
}

IVar& call_function_object(bsl::ResourcePool &rp, IVar &obj, const String &key)
{
	IVar &func = obj[0];

	IVar &call_value = rp.create<Dict>();
	
	call_value["Key"] = rp.create<String>(key);
	
	
	call_value["Arg"] = rp.create<String>(func.to_string());

	
	call_value["NewArg"] = obj[2];

	return func(call_value, rp);
}



