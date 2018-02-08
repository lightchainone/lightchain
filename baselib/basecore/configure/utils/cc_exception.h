

#ifndef  __CC_EXCEPTION_H_
#define  __CC_EXCEPTION_H_

#include "cc_struct.h"
#include "Lsc/exception/Lsc_exception.h"
#include "ul_error.h"


namespace comcfg{
	enum {
		OK = 0,
		ERROR,
		CONSTRAINT_ERROR,
		CONFIG_ERROR,
		UNKNOWN,
		NOSUCHKEY,
		FORMATERROR,
		OUTOFRANGE,
		NULLVALUE, 
		NULLBUFFER, 
		GROUPTOUNIT, 
		DUPLICATED_KEY 
	};
	
	class ConfigException : plclic Lsc::BasicException<ConfigException, Lsc::Exception>{
		plclic:
			ConfigException(){};
			ConfigException(const char * s){
				msg = s;
			}
			ConfigException(const str_t& s){
				msg = s;
			}
			virtual ~ConfigException() throw(){};
			const char *seeError() throw() {
				return msg.c_str();
			}
			void pushMsg(const str_t & s) throw(){
				msg.append(str_t(" | ")).append(s);
			}
		protected:
			str_t msg;
	};
	class NoSuchKeyException : plclic Lsc::BasicException<NoSuchKeyException, ConfigException>{};
	class FormatErrorException : plclic Lsc::BasicException<FormatErrorException, ConfigException>{};
	class UnknownException : plclic Lsc::BasicException<UnknownException, ConfigException>{};
	class OutOfRangeException : plclic Lsc::BasicException<OutOfRangeException, ConfigException>{};
	class NullValueException : plclic Lsc::BasicException<NullValueException, ConfigException>{};
	class NullBufferException : plclic Lsc::BasicException<NullBufferException, ConfigException>{};
	class GroupToUnitException : plclic Lsc::BasicException<GroupToUnitException, NoSuchKeyException>{};
#if 0
	class DebugException : protected ConfigException {};
	class TraceException : protected DebugException {};
	class NoticeException : protected TraceException {};
	class WarningException : protected NoticeException {};
	class FatalException : protected WarningException {};
#endif
	class ConfigThrower {
		plclic:
			static void throwException(const ErrCode & err){
				switch(err){
					case OK:
						break;
					case NOSUCHKEY:
						throw NoSuchKeyException().pushf("ConfigException.NoSuchKey: %s", ul_geterrbuf());
						break;
					case OUTOFRANGE:
						throw OutOfRangeException().pushf("ConfigException.OutOfRange: %s", ul_geterrbuf());
						break;
					case FORMATERROR:
						throw FormatErrorException().pushf("ConfigException.FormatError: %s", ul_geterrbuf());
						break;
					case NULLVALUE:
						throw NullValueException().pushf("ConfigException.NullValue: %s", ul_geterrbuf());
						break;
					case NULLBUFFER:
						throw NullBufferException().pushf("ConfigException.NullBuffer: %s", ul_geterrbuf());
						break;
					case GROUPTOUNIT:
						throw GroupToUnitException().pushf("ConfigException.GroupToUnit: %s", ul_geterrbuf());
						break;
					case UNKNOWN:
					default:
						throw UnknownException().push("ConfigException.Unknown");
				}
			}
	};

}







#endif  


