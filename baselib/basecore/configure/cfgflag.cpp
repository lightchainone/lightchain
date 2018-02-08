
#include "cfgflag.h"
#include "cfgext.h"

namespace comcfg
{

const char *DEFHELP = 
	" -d:     conf_dir\n"
	" -f:     conf_file\n"
	" -r:	  range_file\n"
	" -h:     show this help page\n"
	" -t:     check config file syntax\n"
	" -g:     generate config sample file\n"
	" -v:     show version infomation\n ";

void Flag::default_version()
{
	std::cout<<"call default_version function, you must reload it"<<std::endl;
}

void Flag::default_help()
{
	printf ("%s", DEFHELP);
}

static int rfind(const char *s, char flag) {
	if (s == NULL) return -1;
	int z = strlen(s);
	do {
		--z;
	} while (z>=0 && s[z] != flag);
	return z;
}

Flag::Flag(fun_t ver, fun_t help)
{
	_modname = "";
	_cfpath = "";
	_cffile = "";
	_cfrange = "";
	_version = ver;
	_help = help;
}

Flag::~Flag() 
{
}

Lsc::string Flag::get_defrange(const Lsc::string &file)
{
	Lsc::string str=file;
	if (file.size() > 5 && file.slcstr(file.size()-5) == ".conf") {
		str = file.slcstr(0,file.size()-5);
	}
	str.append(".range");
	return str;
}

Lsc::string Flag::get_defconf(const Lsc::string &mod)
{
	int pos = comcfg::rfind(mod.c_str(), '/');
	Lsc::string cf;
	if (pos < 0) {
		cf = Lsc::string(mod.c_str()).append(".conf");
	} else {
		cf = Lsc::string(mod.c_str()+pos+1).append(".conf");
	}
	return Lsc::string("conf/").append(cf);
}

int Flag::init(int argc, char **argv, const char *optstr)
{
	int ret = 0;
	try {
		if (optstr == NULL) {
			ret = _opt.init(argc, argv, Lsc::string("f:d:r:gtvh").c_str());
		} else {
			ret = _opt.init(argc, argv, Lsc::string("f:d:r:gtvh").append(optstr).c_str());
		}
		if (ret != 0) return ret;

		_modname = argv[0];

		if (_opt.hasOption("d")) {
			_cfpath = _opt["d"].to_cstr();
		} else if (_opt.hasOption("dir")) {
			_cfpath = _opt["dir"].to_cstr();
		} else {
			_cfpath = "";
		}

		if (_opt.hasOption("f")) {
			_cffile = _opt["f"].to_cstr();
		} else if (_opt.hasOption("conf")) {
			_cffile = _opt["conf"].to_cstr();
		} else {
			_cffile = "";
		}

		if (_opt.hasOption("r")) {
			_cfrange = _opt["r"].to_cstr();
		} else {
			_cfrange = "";
		}

		if (_opt.hasOption("v") || _opt.hasOption("version")) {
			_version();
			exit (0);
		}
		if (_opt.hasOption("h") || _opt.hasOption("help")) {
			_help();
			exit (0);
		}
		if (_opt.hasOption("t")) {
			ret = 0;
			try {
				ret = this->loadconfig();
			} catch (Lsc::Exception &e) {
				ul_writelog(UL_LOG_WARNING, "check configure error : exception %s , at%s\n", e.what(), e.stack());
				ret = -1;
			}
			if (ret != 0) {
				ul_writelog(UL_LOG_WARNING, "check configure[%s/%s] error",
						this->_cfpath.c_str(), this->_cffile.c_str());
			} else {
				ul_writelog(UL_LOG_NOTICE, "check configure[%s/%s] success",
						this->_cfpath.c_str(), this->_cffile.c_str());
			}
			exit (0);
		}
		if (_opt.hasOption("g")) {
			Lsc::string file = "";
			try {
				auto_def();
				file = comcfg::autoConfigGen(fullpath(_cfpath, _cfrange).c_str());
			} catch (Lsc::Exception &e) {
				ul_writelog(UL_LOG_WARNING, "auto gen configure error for %s/%s:\nExp %s, %s, %s", _cfpath.c_str(), _cffile.c_str()
						, e.what(), e.name(), e.stack());
				exit(0);
			}
			Lsc::string path = _cfpath;
			path.append("/").append(_cffile);
			FILE *fp = fopen(path.c_str(), "w");
			if (fp == NULL) {
				ul_writelog(UL_LOG_WARNING, "auto gen configure error for can't open file [%s]", path.c_str());
				exit(0);
			}
			fwrite(file.c_str(), 1, file.size(), fp);
			fclose(fp);
			ul_writelog(UL_LOG_NOTICE, "auto configure %s success", path.c_str());
			exit (0);
		}

		return 0;
	} catch (Lsc::Exception &e) {
		return -1;
	}
}

Lsc::string Flag::fullpath(const Lsc::string &path, const Lsc::string &file)
{
	Lsc::string full = path;
	return full.append("/").append(file);
}

bool Flag::file_exist(const char *fn)
{
	FILE *fp = fopen(fn, "r");
	if (fp) {
		fclose(fp);
		return true;
	}
	return false;
}

int Flag::loadconfig()
{
	try {
		auto_def();
		const char *r = NULL;
		if (file_exist(fullpath(_cfpath, _cfrange).c_str())) {
			r = _cfrange.c_str();
		}
		if (r != NULL) {
			ul_writelog(UL_LOG_DEBUG, "configure.load(%s, %s, %s); check range: %s exist",
					_cfpath.c_str(), _cffile.c_str(), r, _cfrange.c_str());
		} else {
			ul_writelog(UL_LOG_DEBUG, "configure.load(%s, %s, NULL); check range: %s noexist",
					_cfpath.c_str(), _cffile.c_str(), _cfrange.c_str());
		}
		int ret = _cfg.load(_cfpath.c_str(), _cffile.c_str(), r);
		if (ret != 0) {
			return ret;
		}
	} catch (Lsc::Exception &e) {
		return -1;
	}
	return 0;
}

int Flag::loadconfig(const char *path, const char *conf) 
{
	if (path == NULL) {
		_cfpath = "";
	} else {
		_cfpath = path;
	}
	if (conf == NULL) {
		_cffile = "";
	} else {
		_cffile = conf;
	}
	return loadconfig();
}

void Flag::set_confpath(const char *path, const char *file, const char *range)
{
	if (path) { 
		set_cfpath(path); 
	} else { 
		set_cfpath(""); 
	}
	if (file) { 
		set_cffile(file); 
	} else { 
		set_cffile(""); 
	}
	if (range) { 
		set_cfrange(range); 
	} else { 
		set_cfrange(""); 
	}
}

void Flag::auto_def()
{
	if (_cffile.size() == 0) {
		_cffile = get_defconf(_modname);
	}
	if (_cfpath.size() == 0) {
		int pos = rfind(_cffile.c_str(), '/');
		if (pos >= 0) {
			_cfpath = _cffile.slcstr(0, pos+1);
			_cffile = _cffile.slcstr(pos+1);
		} else {
			_cfpath = "./";
		}
	}
	if (_cfrange.size() == 0) {
		_cfrange = get_defrange(_cffile);
	}
}

}


