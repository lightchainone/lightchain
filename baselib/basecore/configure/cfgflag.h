

#ifndef  __XAUTOFRAME_H_
#define  __XAUTOFRAME_H_

#include <Lsc/containers/string/Lsc_string.h>
#include "Configure.h"
#include "CmdOption.h"

namespace comcfg
{

extern const char *DEFHELP;

class Flag
{

	typedef void (*fun_t)();

	Lsc::string _modname;
	Lsc::string _cfpath;
	Lsc::string _cffile;
	Lsc::string _cfrange;

	comcfg::CmdOption _opt;
	comcfg::Configure _cfg;

	fun_t _version;
	fun_t _help;
plclic:
	
	Flag(fun_t ver=Flag::default_version, fun_t help=Flag::default_help);

	virtual ~Flag();

	
	virtual int init(int argc, char **argv, const char *optstr=NULL);

	
	inline const comcfg::CmdOption & opt() const { return _opt; }
	
	inline comcfg::CmdOption & opt() { return _opt; }
	
	inline const comcfg::ConfigUnit & opt(const char *str) const { return _opt[str]; }

	
	inline const comcfg::Configure & cfg() const { return _cfg; }
	
	inline comcfg::Configure & cfg() { return _cfg; }

	
	inline const comcfg::ConfigUnit & cfg(const char *str) const { return _cfg[str]; }

	
	inline void set_cfpath(const Lsc::string &__cfpath) { _cfpath = __cfpath; }
	
	inline const char * cfpath() const { return _cfpath.c_str(); }

	
	inline void set_cffile(const Lsc::string &__cffile) { _cffile = __cffile; }
	
	inline const char * cffile() const { return _cffile.c_str(); }

	
	inline void set_cfrange(const Lsc::string &__cfrange) { _cfrange = __cfrange; }
	
	inline const char *cfrange() const { return _cfrange.c_str(); }

	
	void set_confpath(const char *path, const char *file, const char *range=NULL);

	
	inline void set_version(fun_t fun) { _version = fun; }
	
	inline void set_help(fun_t fun) { _help = fun; }

	
	virtual int loadconfig();
	
	virtual int loadconfig(const char *path, const char *conf);

protected:
	static void default_version();
	static void default_help();

	
	Lsc::string get_defrange(const Lsc::string &file);
	Lsc::string get_defconf(const Lsc::string &mod);
	void auto_def();
	bool file_exist(const char *fn);
	Lsc::string fullpath(const Lsc::string &path, const Lsc::string &file);
};
}

#endif  


