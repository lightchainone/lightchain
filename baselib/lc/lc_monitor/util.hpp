#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <bsl/var/IVar.h>
#include <bsl/var/Null.h>
#include <bsl/var/Function.h>
#include <bsl/var/Number.h>
#include <bsl/var/String.h>
#include <bsl/var/ShallowRaw.h>
#include <bsl/var/Array.h>
#include <bsl/var/Dict.h>
#include <bsl/ResourcePool.h>


using namespace bsl::var;


IVar& create_function_object(bsl::ResourcePool &rp, Function::function_type func, void *arg, int type);
IVar& call_function_object(bsl::ResourcePool &rp, IVar &obj, const String &key);

#endif

