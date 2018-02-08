

#include "comlogplugin.h"

#define NOTHROW(x) try{x}catch(...){}		  
#define GETDEFAULT(x, y, z) try{x=y;}catch(...){x=z;}		  

int load_device(const comcfg::ConfigUnit & devcfg, com_device_t * dev){
	int ret = 0;
	if(dev == NULL){
		ul_writelog(UL_LOG_WARNING, "load_device : dev = NULL");
		return -1;
	}
	try{
		
		const char * dev_type = devcfg[CP_KEY_TYPE].to_cstr();
		if(! comspace::AppenderFactory::isAppender(dev_type)){
			throw comcfg::ConfigException() << BSL_EARG << "Unknown type : " << dev_type;
		}
		snprintf(dev->type, sizeof(dev->type), "%s", dev_type);
		
		
		if(devcfg[CP_KEY_TYPE].to_Lsc_string() != "TTY"){
			snprintf(dev->host, sizeof(dev->host), "%s", devcfg[CP_KEY_PATH].to_cstr());
			snprintf(dev->file, sizeof(dev->file), "%s", devcfg[CP_KEY_FILE].to_cstr());
		}

		
		
		
		NOTHROW(snprintf(dev->auth, sizeof(dev->auth), "%s", devcfg[CP_KEY_AUTH].to_cstr());)
		NOTHROW(snprintf(dev->reserved1, sizeof(dev->reserved1), "%s", 
					devcfg[CP_KEY_RESERVED1].to_cstr());)
		NOTHROW(snprintf(dev->reserved2, sizeof(dev->reserved2), "%s", 
					devcfg[CP_KEY_RESERVED2].to_cstr());)
		NOTHROW(snprintf(dev->reserved3, sizeof(dev->reserved3), "%s", 
					devcfg[CP_KEY_RESERVED3].to_cstr());)
		NOTHROW(snprintf(dev->reserved4, sizeof(dev->reserved4), "%s", 
					devcfg[CP_KEY_RESERVED4].to_cstr());)

		
		GETDEFAULT(dev->open, devcfg[CP_KEY_OPEN].to_int32(), 1)

		
		Lsc::string dev_split;
		GETDEFAULT(dev_split, devcfg[CP_KEY_SPLIT_TYPE].to_Lsc_string(), COMLOG_DEVICE_TRUNCT)
		if(dev_split == COMLOG_DEVICE_TRUNCT){
			dev->splite_type = COM_TRUNCT;
		}else if(dev_split == COMLOG_DEVICE_DATECUT){
			dev->splite_type = COM_DATECUT;
		}else if(dev_split == COMLOG_DEVICE_SIZECUT){
			dev->splite_type = COM_SIZECUT;
		}else{
			dev->splite_type = devcfg[CP_KEY_SPLIT_TYPE].to_int32();
		}
		
		
		if(dev->splite_type < COM_TRUNCT && dev->splite_type > COM_DATECUT){
			throw comcfg::ConfigException() << BSL_EARG << "split_type invalid.";
		}

		
		GETDEFAULT(dev->log_size, devcfg[CP_KEY_LOG_SIZE].to_int32(), 2048)
		GETDEFAULT(dev->compress, devcfg[CP_KEY_COMPRESS].to_int32(), 0)
		GETDEFAULT(dev->cuttime, devcfg[CP_KEY_CUTTIME].to_int32(), 60)

		
		int dev_sysl;
		GETDEFAULT(dev_sysl, devcfg[CP_KEY_SYSLEVEL].to_int32(), 16)
		dev->log_mask = 0;
		for(int i = 1; i <= dev_sysl; ++i){
			COMLOG_ADDMASK(*dev, i);
		}

		
		Lsc::string dev_self;
		GETDEFAULT(dev_self, devcfg[CP_KEY_SELFLEVEL].to_Lsc_string(), "")
		if(dev_self.size() > 0){
			set_comdevice_selflog(dev, dev_self.c_str());
		}

		
		Lsc::string layout, layout_ndc;
		GETDEFAULT(layout, devcfg[CP_KEY_LAYOUT].to_Lsc_string(), "")
		GETDEFAULT(layout_ndc, devcfg[CP_KEY_LAYOUT_NDC].to_Lsc_string(), "")
		if(layout.size() > 0 || layout_ndc.size() > 0){
			comspace :: Layout* layout = new comspace :: Layout();
			const int max_pattern_size = 1024;
			char laybuf[max_pattern_size]; 
			if(devcfg[CP_KEY_LAYOUT].get_cstr(laybuf, max_pattern_size) == 0 && laybuf[0] != 0){
				layout->setPattern(laybuf);
			}
			if(devcfg[CP_KEY_LAYOUT_NDC].get_cstr(laybuf, max_pattern_size) == 0 && laybuf[0] != 0){
				layout->setNdcPattern(laybuf);
			}
			dev->layout = layout;
		}
	}catch(comcfg::ConfigException e){
		
		ul_writelog(UL_LOG_WARNING, "load device errer : %s [path/file/type must be given]", e.what());
		ret = -1;
	}
	return ret;
}

int comlog_init(const comcfg::ConfigUnit & logcfg){
	int ret = 0;
	int err;
	const int TIME_FORMAT_SIZE = 128;
	char timefmt[TIME_FORMAT_SIZE];
	com_device_t * dev = NULL;
	com_logstat_t logstat;
	try{
		
		
		GETDEFAULT(logstat.sysevents, logcfg[CP_KEY_LEVEL].to_int32(), 16)
		NOTHROW(snprintf(logstat.userevents, sizeof(logstat.userevents), "%s", logcfg[CP_KEY_SELFDEFINE].to_cstr());)

		int log_length = logcfg[CP_KEY_LOG_LENGTH].to_int32(&err);
		if(err == 0){
			comspace::Event::setMaxLogLength(log_length);
		}

		err = logcfg[CP_KEY_TIMEFORMAT].get_cstr(timefmt, sizeof(timefmt));
		if(err == 0 && timefmt[0] != 0){
			comspace::Layout::setTimeFormat(timefmt);
		}
		
		int dev_num = int(logcfg[CP_KEY_DEVICE].size());
		if(dev_num > 32){
			throw comcfg::ConfigException() << BSL_EARG << "Too many devices...";
		}

		
		dev = new com_device_t[dev_num];
		for(int i = 0; i < dev_num; ++i){
			if(load_device(logcfg[CP_KEY_DEVICE][i], &dev[i]) != 0){
				throw comcfg::ConfigException() << BSL_EARG << "device[" << i << "] Error..";
			}
		}
		
		Lsc::string procname;
		GETDEFAULT(procname, logcfg[CP_KEY_PROCNAME].to_cstr(), "NoModuleName")
		ret = com_openlog(procname.c_str(), dev, dev_num, &logstat);
	}
	catch(comcfg::ConfigException e){
		ul_writelog(UL_LOG_WARNING, "comlog init Error : %s", e.what());
		ret = -1;
	}
	if(dev){
		delete [] dev;
	}
	return ret;
}


















