
#include<unistd.h>
#include<signal.h>
#include "lleo_admin_work.h"

bool is_fini = false;

void
sig_handler(int)
{
	is_fini = true;
}

int 
error_exit(int grc){
	fprintf(stderr, "ERROR: %s\n", lleo_err2str(grc));
	return grc;
}


int 
admin_watcher(int watch_type, const char *realpath,	Lsc::var::IVar& res_data, void* ctx)
{
	int grc = GALILEO_E_OK;

	GALILEO_POINTER_CHECK(NULL == realpath, "Failed to get a valid realpath");
	GALILEO_POINTER_CHECK(!res_data.is_dict(), "Failed to get a valid res_data dict");
	
	switch(watch_type){
		case GALILEO_ABSTRACT_CHANGED_EVENT:
			fprintf(stdout, "\tTYPE: %s\n", "ABSTRACT NODE CHANGE EVENT");
			break;
		case GALILEO_ENTITY_CHANGED_EVENT:
			fprintf(stdout, "\tTYPE: %s\n", "ENTITY NODE CHANGE EVENT");
			break;
		case GALILEO_RESOURCE_UPDATE_EVENT:
			fprintf(stdout, "\tTYPE: %s\n", "RESOURCE UPDATE EVENT");
			break;
		default:
			fprintf(stdout, "\tTYPE: %s\n", "UNKOWN EVENT");
	}
	
	fprintf(stdout, "\tCTX: %s\n", (char*)ctx);
	fprintf(stdout, "\tPATH: %s\n", realpath);

	Lsc::var::JsonSerializer js;
	Lsc::AutoBuffer buf;
	Lsc::var::IVar * acl_array = lleo_get_acl_from_data(res_data);
	try{
		js.serialize(res_data, buf);
		fprintf(stdout, "\tDATA: %s\n", buf.c_str());
		buf.clear();
		if (!acl_array->is_null()) {
			js.serialize(*acl_array, buf);
			fprintf(stdout, "\tACL:  %s\n", buf.c_str());
		}
	}
	catch (Lsc::Exception& e){
		grc = GALILEO_E_MARSHALLINGERROR;
		fprintf(stderr, "An exception[%s] was thrown from[%s:%d:%s]"\
				" with a message[%s]! stack_trace:%s%s",
				e.name(), e.file(), int(e.line()), e.function(),
				e.what(), e.get_line_delimiter(), e.stack());
	}
	catch (std::exception& e){
		grc = GALILEO_E_MARSHALLINGERROR;
		fprintf(stderr,"a std::exception[%s] was thrown! Definitely a bug!",
				e.what());
	}
	catch(...){
		grc = GALILEO_E_MARSHALLINGERROR;
		fprintf(stderr, "\tERROR: %s\n", lleo_err2str(grc));
	}

	fprintf(stdout, "%s\n", "");
	return grc;
}



int 
check_group_config(admin_req_t *req, comcfg::Configure *conf)
{
	int grc = 0;
	int err_no = 0;
	int name_item_num = 0;
	int node_num = 0;
	const char *parent_path = NULL;

	GALILEO_POINTER_CHECK(NULL == req, "Failed to get a valid request");
	GALILEO_POINTER_CHECK(NULL == conf, "Failed to get a valid config");

	parent_path = (*conf)["PARENT_PATH"].to_cstr(&err_no);
	GALILEO_INT_CHECK(0 != err_no, "Failed to get parent path", err_no);

	name_item_num = (int)(*conf)["NODE_NAME_ITEM_ARR"].size();
	GALILEO_INT_CHECK(0 >= name_item_num, "Failed to get a valid item num", name_item_num);

	node_num = (int)(*conf)["NODE_ARR"].size();
	for (int i = 0; i < node_num; i++){
		
		const char *data = (*conf)["NODE_ARR"][i]["DATA"].to_cstr(&err_no);
		if (0 != err_no){
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get data for idx:%d, err:%d",
				__FILE__, __LINE__, i, err_no);
			return GALILEO_E_BADPARAM;
		}

		Lsc::ResourcePool rp;
		Lsc::var::JsonDeserializer jd(rp);
		Lsc::var::Dict node_data;
		try{
			node_data = jd.deserialize(data);
		}
		catch (Lsc::Exception& e){
			ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
					" with a message[%s]! stack_trace:%s%s",
					e.name(), e.file(), int(e.line()), e.function(),
					e.what(), e.get_line_delimiter(), e.stack());
			return GALILEO_E_BADPARAM;
		}
		catch (std::exception& e){
			ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
				"Definitely a bug!", e.what());
			return GALILEO_E_BADPARAM;
		}
		catch(...){
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize josn text, idx:%d",
					__FILE__, __LINE__, i);
			return GALILEO_E_BADPARAM;
		}

		for (int j = 0; j < name_item_num; j++){
			const char *key = (*conf)["NODE_NAME_ITEM_ARR"][j]["ITEM_KEY"].to_cstr(&err_no);
			if (0 != err_no){
				ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get item for idx:%d, err:%d",
						__FILE__, __LINE__, j, err_no);
				return GALILEO_E_BADPARAM;
			}

			if (node_data.get(key).is_null()){
				ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid name item(idx:%d),"\
						" from pack(idx:%d)", __FILE__, __LINE__, j, i);
				return GALILEO_E_BADPARAM;
			}
		}

		
		Lsc::var::Dict node_acl;
		const char *acl = (*conf)["NODE_ARR"][i]["ACL"].to_cstr(&err_no);
		if (0 == err_no){
			
			try{
				node_acl = jd.deserialize(acl);
			}
			catch (Lsc::Exception& e){
				ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
						" with a message[%s]! stack_trace:%s%s",
						e.name(), e.file(), int(e.line()), e.function(),
						e.what(), e.get_line_delimiter(), e.stack());
				return GALILEO_E_BADPARAM;
			}
			catch (std::exception& e){
				ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
						"Definitely a bug!", e.what());
				return GALILEO_E_BADPARAM;
			}
			catch(...){
				ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize json text, idx:%d",
						__FILE__, __LINE__, i);
				return GALILEO_E_BADPARAM;
			}
		}
	}

	return grc;
}



int 
process_admin_req_g(admin_req_t *req)
{
	int rc = 0;
	int grc = GALILEO_E_OK;
	int name_item_num = 0;
	int node_num = 0;

	lleo_t *hdlr = NULL;
	comcfg::Configure group_conf;
	char *pch = NULL;
	char conf_dir[GALILEO_DEFAULT_PATH_LEN];
	char conf_file[GALILEO_DEFAULT_PATH_LEN];

	GALILEO_POINTER_CHECK(NULL == req, "Failed to get a valid request");
	
	
	GALILEO_POINTER_CHECK('\0' == req->host[0], "Failed to get a valid host");
	GALILEO_POINTER_CHECK('\0' == req->conf[0], "Failed to get a conf file");

	
	pch = strrchr(req->conf, '/');
	if (NULL == pch){
		sprintf(conf_dir, "%s", "./");
		sprintf(conf_file, "%s", req->conf);
	}
	else{
		int len = pch - req->conf;
		strncpy(conf_dir, req->conf, len);
		conf_dir[len] = 0;
		strcpy(conf_file, ++pch);
	}
	
	rc = group_conf.load(conf_dir, conf_file);
	GALILEO_INT_CHECK(0 != rc, "Failed to load configure for group nodes create", rc);

	rc = check_group_config(req, &group_conf);
	GALILEO_INT_CHECK(0 != rc, "Failed to check configure", rc);

	hdlr = lleo_init(req->host);
	GALILEO_POINTER_CHECK(NULL == hdlr, "Failed to get a valid handler");

	Lsc::ResourcePool rp;
	Lsc::var::JsonDeserializer jd(rp);
	Lsc::var::Dict node_data;
	Lsc::var::Dict node_acl;
	
	name_item_num = (int)group_conf["NODE_NAME_ITEM_ARR"].size();
	node_num = (int)group_conf["NODE_ARR"].size();
	for (int i = 0; i < node_num; i++){
		int err_no = 0;
		char node_name[GALILEO_DEFAULT_PATH_LEN];
		node_name[0] = '\0';
		int name_len = strlen(node_name);

		
		const char *data = group_conf["NODE_ARR"][i]["DATA"].to_cstr(&err_no);
		try{
			node_data = jd.deserialize(data);
		}
		catch (Lsc::Exception& e){
			ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
					" with a message[%s]! stack_trace:%s%s",
					e.name(), e.file(), int(e.line()), e.function(),
					e.what(), e.get_line_delimiter(), e.stack());

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
		catch (std::exception& e){
			ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
					"Definitely a bug!", e.what());

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
		catch(...){
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize josn text, idx:%d",
					__FILE__, __LINE__, i);

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
		
		
		const char *parepath = group_conf["PARENT_PATH"].to_cstr(&err_no);
		GALILEO_INT_CHECK(0 != err_no, "Failed to get a valid parent path", err_no);
		sprintf(node_name, "%s", parepath);
		name_len = strlen(node_name);
		if ('/' != node_name[name_len - 1]){
			node_name[name_len] = '/';
			node_name[name_len + 1] = '\0';
		}

		for (int j = 0; j < name_item_num; j++){
			name_len = strlen(node_name);
			const char *key = group_conf["NODE_NAME_ITEM_ARR"][j]["ITEM_KEY"].to_cstr(&err_no);
			if (node_data.get(key).is_null()){
				ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid name item(idx:%d),"\
						" from pack(idx:%d)", __FILE__, __LINE__, j, i);
				if (NULL != hdlr)
					lleo_destroy(hdlr);
				return error_exit(GALILEO_E_BADPARAM);
			}
			const char* value = node_data.get(key).c_str();

			if (0 == j){
				sprintf(node_name+name_len, "%s", value);
				*(node_name+name_len+strlen(value)) = '\0';
			}
			else{
				sprintf(node_name+name_len, "_%s", value);
				*(node_name+name_len+strlen(value)+1) = '\0';
			}
		}
		
		
		const char *acl = group_conf["NODE_ARR"][i]["ACL"].to_cstr(&err_no);
		if (0 == err_no){
			try{
				node_acl = jd.deserialize(acl);
			}
			catch (Lsc::Exception& e){
				ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
						" with a message[%s]! stack_trace:%s%s",
						e.name(), e.file(), int(e.line()), e.function(),
						e.what(), e.get_line_delimiter(), e.stack());
				if (NULL != hdlr)
					lleo_destroy(hdlr);
				return error_exit(GALILEO_E_BADPARAM);
			}
			catch (std::exception& e){
				ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
						"Definitely a bug!", e.what());
				if (NULL != hdlr)
					lleo_destroy(hdlr);
				return error_exit(GALILEO_E_BADPARAM);
			}
			catch(...){
				ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize josn text, idx:%d",
						__FILE__, __LINE__, i);
				if (NULL != hdlr)
					lleo_destroy(hdlr);
				return error_exit(GALILEO_E_BADPARAM);
			}

			fprintf(stdout, "[CREATE %dth NODE]\nDATA: %s\nACL: %s\n", i, data, acl);
			grc = lleo_admin_create_abstract(hdlr, node_name, node_data, node_acl);
			if (0 != grc){
				ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create node, path:%s",
					 __FILE__, __LINE__, node_name);
				if (NULL != hdlr)
					lleo_destroy(hdlr);
				return error_exit(GALILEO_E_BADPARAM);
			}
		}
		else{
			fprintf(stdout, "[CREATE %dth NODE]\nDATA: %s\nACL: Use parent ACL\n", i, data);
			grc = lleo_admin_create_abstract(hdlr, node_name, node_data, node_acl);
			if (0 != grc){
				ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create node, path:%s",
					 __FILE__, __LINE__, node_name);
				if (NULL != hdlr)
					lleo_destroy(hdlr);
				return error_exit(GALILEO_E_BADPARAM);
			}
		}
	}
	
	if (GALILEO_E_OK != grc){
		fprintf(stderr, "ERROR: %s\n", lleo_err2str(grc));
	}
	
	if (NULL != hdlr)
		lleo_destroy(hdlr);
	return grc;
}


int 
process_admin_req_q(admin_req_t *req)
{
	int grc = GALILEO_E_OK;
	lleo_t *hdlr = NULL;
	Lsc::ResourcePool rp;
	Lsc::AutoBuffer buf;
	Lsc::var::JsonSerializer js;

	Lsc::var::Dict& node_data = rp.create<Lsc::var::Dict>();
	Lsc::var::Dict& node_acl = rp.create<Lsc::var::Dict>();
	GALILEO_POINTER_CHECK(NULL == req, "Failed to get a valid request");

	
	GALILEO_POINTER_CHECK('\0' == req->host[0], "Failed to get a valid host");
	GALILEO_POINTER_CHECK('\0' == req->path[0], "Failed to get a valid path");
	
	
	fprintf(stdout, "PATH: %s\n", req->path);

	hdlr = lleo_init(req->host);
	GALILEO_POINTER_CHECK(NULL == hdlr, "Failed to get a valid handler");

	
	if ('\0' != req->id[0]){

		grc = zoo_add_auth(hdlr->zh, "digest", req->id, strlen(req->id), NULL, NULL);
		if (grc != ZOK) {
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add auth", __FILE__, __LINE__);
			lleo_destroy(hdlr);
			return error_exit(GALILEO_E_AUTHFAILED);
		}
	}

	grc = lleo_admin_get_abstract(hdlr, req->path, node_data, node_acl, rp);
	if (GALILEO_E_OK != grc){
		ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get abstract data, rc:%d", 
			 __FILE__, __LINE__, grc);
		lleo_destroy(hdlr);
		return error_exit(grc);
	}

	try{
		js.serialize(node_data, buf);
		fprintf(stdout, "DATA: %s\n", buf.c_str());
		
		buf.clear();
		js.serialize(node_acl, buf);
		fprintf(stdout, "ACL: %s\n", buf.c_str());
	}
	catch (Lsc::Exception& e){
		ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
				" with a message[%s]! stack_trace:%s%s",
				e.name(), e.file(), int(e.line()), e.function(),
				e.what(), e.get_line_delimiter(), e.stack());

		lleo_destroy(hdlr);
		return error_exit(GALILEO_E_MARSHALLINGERROR);
	}
	catch (std::exception& e){
		ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
				"Definitely a bug!", e.what());

		lleo_destroy(hdlr);
		return error_exit(GALILEO_E_MARSHALLINGERROR);
	}
	catch(...){
		ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to serialize resource data, path:%s",
				__FILE__, __LINE__, req->path);

		lleo_destroy(hdlr);
		return error_exit(GALILEO_E_MARSHALLINGERROR);
	}

	lleo_destroy(hdlr);
	return grc;
}



int 
process_admin_req_s(admin_req_t *req)
{
	int  zrc = ZOK;
	int  grc = GALILEO_E_OK;
	char buff[GALILEO_DEFAULT_BUF_SIZE];
	int  buff_len = 0;
	struct Stat stat;
	lleo_t *hdlr = NULL;

	GALILEO_POINTER_CHECK(NULL == req, "Failed to get a valid request");

	
	GALILEO_POINTER_CHECK('\0' == req->host[0], "Failed to get a valid host");
	GALILEO_POINTER_CHECK('\0' == req->path[0], "Failed to get a valid path");
	
	
	fprintf(stdout, "PATH: %s\n", req->path);

	hdlr = lleo_init(req->host);
	GALILEO_POINTER_CHECK(NULL == hdlr, "Failed to get a valid handler");

	
	if ('\0' != req->id[0]){
		zrc = zoo_add_auth(hdlr->zh, "digest", req->id, strlen(req->id), NULL, NULL);
		if (zrc != ZOK) {
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add auth", __FILE__, __LINE__);
			lleo_destroy(hdlr);
			return error_exit(GALILEO_E_AUTHFAILED);
		}
	}

	zrc = zoo_get(hdlr->zh, req->path, 0, buff, &buff_len, &stat);
	grc = zrc;

	if (GALILEO_E_OK == grc){
		fprintf(stdout, "%s\n", "STAT:");
		fprintf(stdout, "\tczxid: %lld\n", stat.czxid);
		fprintf(stdout, "\tmzxid: %lld\n", stat.mzxid);
		fprintf(stdout, "\tctime: %lld\n", stat.ctime);
		fprintf(stdout, "\tmtime: %lld\n", stat.mtime);
		fprintf(stdout, "\tversion: %d\n", stat.version);
		fprintf(stdout, "\tcversion: %d\n", stat.cversion);
		fprintf(stdout, "\taversion: %d\n", stat.aversion);
		fprintf(stdout, "\tephemeralOwner: %lld\n", stat.ephemeralOwner);
		fprintf(stdout, "\tdataLength: %d\n", stat.dataLength);
		fprintf(stdout, "\tnumChildren: %d\n", stat.numChildren);
		fprintf(stdout, "\tpzxid: %lld\n", stat.pzxid);
	}
	
	if (GALILEO_E_OK != grc){
		fprintf(stderr, "ERROR: %s\n", lleo_err2str(grc));
	}

	if (NULL != hdlr){
		lleo_destroy(hdlr);
	}

	return grc;
}



int 
process_admin_req_c(admin_req_t *req)
{
	int i = 0;
	int zrc = ZOK;
	int grc = GALILEO_E_OK;
	struct String_vector str_vec = {0, NULL};
	lleo_t *hdlr = NULL;

	GALILEO_POINTER_CHECK(NULL == req, "Failed to get a valid request");

	
	GALILEO_POINTER_CHECK('\0' == req->host[0], "Failed to get a valid host");
	GALILEO_POINTER_CHECK('\0' == req->path[0], "Failed to get a valid path");

	
	fprintf(stdout, "PATH: %s\n", req->path);

	hdlr = lleo_init(req->host);
	GALILEO_POINTER_CHECK(NULL == hdlr, "Failed to get a valid handler");

	
	if ('\0' != req->id[0]){
		grc = zoo_add_auth(hdlr->zh, "digest", req->id, strlen(req->id), NULL, NULL);
		if (grc != ZOK) {
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add auth", __FILE__, __LINE__);
			lleo_destroy(hdlr);
			return error_exit(GALILEO_E_AUTHFAILED);
		}
	}

	zrc = zoo_get_children(hdlr->zh, req->path, 0, &str_vec);
	grc = zrc;

	if (GALILEO_E_OK == zrc){
		fprintf(stdout, "%s\n", "CHILDREN:");
		for (i = 0; i < str_vec.count; i++){
			fprintf(stdout, "\t%s\n", str_vec.data[i]);
		}
	}
	deallocate_String_vector(&str_vec);
	if (GALILEO_E_OK != grc){
		fprintf(stderr, "ERROR: %s\n", lleo_err2str(grc));
	}
	
	if (NULL != hdlr){
		lleo_destroy(hdlr);
	}

	return grc;
}



int 
process_admin_req_b(admin_req_t *req)
{
	int grc = GALILEO_E_OK;
	lleo_t *hdlr = NULL;
	Lsc::ResourcePool rp;
	Lsc::var::JsonDeserializer jd(rp);
	Lsc::var::Dict node_data;
	Lsc::var::Dict node_acl;

	GALILEO_POINTER_CHECK(NULL == req, "Failed to get a valid request");

	
	GALILEO_POINTER_CHECK('\0' == req->host[0], "Failed to get a valid host");
	GALILEO_POINTER_CHECK('\0' == req->path[0], "Failed to get a valid path");
	GALILEO_POINTER_CHECK('\0' == req->data_pack[0], "Failed to get a valid data_pack");
	
	
	fprintf(stdout, "PATH: %s\n", req->path);

	hdlr = lleo_init(req->host);
	GALILEO_POINTER_CHECK(NULL == hdlr, "Failed to get a valid handler");

	
	if ('\0' != req->id[0]){
		grc = zoo_add_auth(hdlr->zh, "digest", req->id, strlen(req->id), NULL, NULL);
		if (grc != ZOK) {
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add auth", __FILE__, __LINE__);
			lleo_destroy(hdlr);
			return error_exit(GALILEO_E_AUTHFAILED);
		}
	}

	
	try{
		node_data = jd.deserialize(req->data_pack);
	}
	catch (Lsc::Exception& e){
		ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
				" with a message[%s]! stack_trace:%s%s",
				e.name(), e.file(), int(e.line()), e.function(),
				e.what(), e.get_line_delimiter(), e.stack());

		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(GALILEO_E_BADPARAM);
	}
	catch (std::exception& e){
		ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
				"Definitely a bug!", e.what());

		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(GALILEO_E_BADPARAM);
	}
	catch(...){
		ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize DATA josn text, path:%s",
				__FILE__, __LINE__, req->path);

		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(GALILEO_E_BADPARAM);
	}
	
	if ('\0' != req->acl_pack[0]){
		
		try{
			node_acl = jd.deserialize(req->acl_pack);
		}
		catch (Lsc::Exception& e){
			ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
					" with a message[%s]! stack_trace:%s%s",
					e.name(), e.file(), int(e.line()), e.function(),
					e.what(), e.get_line_delimiter(), e.stack());

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
		catch (std::exception& e){
			ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
					"Definitely a bug!", e.what());

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
		catch(...){
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize ACL josn text, path:%s",
					__FILE__, __LINE__, req->path);

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}

		grc = lleo_admin_create_abstract(hdlr, req->path, node_data, node_acl);
	}
	else{
		
		grc = lleo_admin_create_abstract(hdlr, req->path, node_data, node_acl);
	}
	
	if (GALILEO_E_OK == grc){
		fprintf(stdout, "DATA: %s\n", req->data_pack);
		if ('\0' != req->acl_pack[0]){
			fprintf(stdout, "ACL: %s\n", req->acl_pack);}
		else{
			fprintf(stdout, "ACL: Use parent ACL\n");}
	}

	if (GALILEO_E_OK != grc){
		fprintf(stderr, "ERROR: %s\n", lleo_err2str(grc));
	}

	if (NULL != hdlr){
		lleo_destroy(hdlr);
	}
	return grc;
}



int 
process_admin_req_u(admin_req_t *req)
{
	int grc = 0;
	lleo_t *hdlr = NULL;
	Lsc::ResourcePool rp;
	Lsc::AutoBuffer buf;
	Lsc::var::JsonSerializer js;
	Lsc::var::JsonDeserializer jd(rp);
	Lsc::var::Dict& node_data = rp.create<Lsc::var::Dict>();
	Lsc::var::Dict& node_acl = rp.create<Lsc::var::Dict>();

	GALILEO_POINTER_CHECK(NULL == req, "Failed to get a valid request");
	
	GALILEO_POINTER_CHECK('\0' == req->host[0], "Failed to get a valid host");
	GALILEO_POINTER_CHECK('\0' == req->path[0], "Failed to get a valid path");
	GALILEO_POINTER_CHECK('\0' == req->data_pack[0] && '\0' == req->acl_pack[0],
						  "Failed to get a valid data_pack or a valid acl_pack");

	
	fprintf(stdout, "PATH: %s\n", req->path);

	hdlr = lleo_init(req->host);
	GALILEO_POINTER_CHECK(NULL == hdlr, "Failed to get a valid handler");

	
	if ('\0' != req->id[0]){
		grc = zoo_add_auth(hdlr->zh, "digest", req->id, strlen(req->id), NULL, NULL);
		if (grc != ZOK) {
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add auth", __FILE__, __LINE__);
			lleo_destroy(hdlr);
			return error_exit(GALILEO_E_AUTHFAILED);
		}
	}

	
	grc = lleo_admin_get_abstract(hdlr, req->path, node_data, node_acl, rp);
	if (GALILEO_E_OK != grc){
		ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get abstract data, rc:%d",
			 __FILE__, __LINE__, grc);
		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(GALILEO_E_BADPARAM);
	}

	try{
		js.serialize(node_data, buf);
		fprintf(stdout, "OLD DATA: %s\n", buf.c_str());

		buf.clear();
		js.serialize(node_acl, buf);
		fprintf(stdout, "OLD ACL: %s\n", buf.c_str());
	}
	catch (Lsc::Exception& e){
		ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
				" with a message[%s]! stack_trace:%s%s",
				e.name(), e.file(), int(e.line()), e.function(),
				e.what(), e.get_line_delimiter(), e.stack());

		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(GALILEO_E_MARSHALLINGERROR);
	}
	catch (std::exception& e){
		ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
				"Definitely a bug!", e.what());

		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(GALILEO_E_MARSHALLINGERROR);
	}
	catch(...){
		ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to serialize josn text, path:%s",
				__FILE__, __LINE__, req->path);

		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(GALILEO_E_MARSHALLINGERROR);
	}

	
	Lsc::var::Dict& new_node_data = rp.create<Lsc::var::Dict>();
	Lsc::var::Dict& new_node_acl = rp.create<Lsc::var::Dict>();
	if ('\0' != req->data_pack[0]){
		try{
			new_node_data = jd.deserialize(req->data_pack);
		}
		catch (Lsc::Exception& e){
			ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
					" with a message[%s]! stack_trace:%s%s",
					e.name(), e.file(), int(e.line()), e.function(),
					e.what(), e.get_line_delimiter(), e.stack());

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
		catch (std::exception& e){
			ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
					"Definitely a bug!", e.what());

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
		catch(...){
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize josn text, path:%s",
					__FILE__, __LINE__, req->path);

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
	}

	if ('\0' != req->acl_pack[0]){
		try{
			new_node_acl = jd.deserialize(req->acl_pack);
		}
		catch (Lsc::Exception& e){
			ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
					" with a message[%s]! stack_trace:%s%s",
					e.name(), e.file(), int(e.line()), e.function(),
					e.what(), e.get_line_delimiter(), e.stack());

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
		catch (std::exception& e){
			ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
					"Definitely a bug!", e.what());

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
		catch(...){
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize josn text, path:%s",
					__FILE__, __LINE__, req->path);

			if (NULL != hdlr)
				lleo_destroy(hdlr);
			return error_exit(GALILEO_E_BADPARAM);
		}
	}
	
	grc = lleo_admin_update_abstract(hdlr, req->path, new_node_data, new_node_acl);
	if (GALILEO_E_OK != grc){
		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(grc);
	}

	if ('\0' != req->data_pack[0]){
		fprintf(stdout, "NEW DATA: %s\n", req->data_pack);
	}
	else{
		buf.clear();
		js.serialize(node_data, buf);
		fprintf(stdout, "NEW DATA: %s\n", buf.c_str());
	}

	if ('\0' != req->acl_pack[0]){
		fprintf(stdout, "NEW ACL: %s\n", req->acl_pack);
	}
	else{
		buf.clear();
		js.serialize(node_acl, buf);
		fprintf(stdout, "NEW ACL: %s\n", buf.c_str());
	}

	if (NULL != hdlr)
		lleo_destroy(hdlr);

	if (GALILEO_E_OK != grc){
		fprintf(stderr, "ERROR: %s\n", lleo_err2str(grc));
	}
	return grc;
}




int process_admin_req_r(admin_req_t *req)
{
	int grc = 0;
	lleo_t *hdlr = NULL;

	GALILEO_POINTER_CHECK(NULL == req, "Failed to get a valid request");

	
	GALILEO_POINTER_CHECK('\0' == req->host[0], "Failed to get a valid host");
	GALILEO_POINTER_CHECK('\0' == req->path[0], "Failed to get a valid path");
	
	
	fprintf(stdout, "PATH: %s\n", req->path);

	hdlr = lleo_init(req->host);
	GALILEO_POINTER_CHECK(NULL == hdlr, "Failed to get a valid handler");

	grc = lleo_admin_delete_abstract(hdlr, req->path);

	if (GALILEO_E_OK == grc){
		fprintf(stdout, "%s\n", "DELETE SUCCESSFUL");
	}
	else{
		fprintf(stderr, "ERROR: %s\n", lleo_err2str(grc));
	}

	if (NULL != hdlr)
		lleo_destroy(hdlr);
	
	return grc;
}



int process_admin_req_t(admin_req_t *req)
{
	int grc = 0;
	int tag_no = 0;
	lleo_t *hdlr = NULL;
	Lsc::ResourcePool rp;
	Lsc::var::JsonDeserializer jd(rp);
	
	GALILEO_POINTER_CHECK(NULL == req, "Failed to get a valid request");

	
	GALILEO_POINTER_CHECK('\0' == req->host[0], "Failed to get a valid host");
	GALILEO_POINTER_CHECK('\0' == req->path[0], "Failed to get a valid path");
	GALILEO_POINTER_CHECK('\0' == req->data_pack[0], "Failed to get a valid data_pack");
	
	
	fprintf(stdout, "PATH: %s\n", req->path);

	hdlr = lleo_init(req->host);
	GALILEO_POINTER_CHECK(NULL == hdlr, "Failed to get a valid handler");

	
	if ('\0' != req->id[0]){
		grc = zoo_add_auth(hdlr->zh, "digest", req->id, strlen(req->id), NULL, NULL);
		if (grc != ZOK) {
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add auth", __FILE__, __LINE__);
			lleo_destroy(hdlr);
			return error_exit(GALILEO_E_AUTHFAILED);
		}
	}

	try{
		Lsc::var::IVar& node_data = jd.deserialize(req->data_pack);
		grc = lleo_register_resource(hdlr, req->path, node_data, &tag_no);
	}
	catch (Lsc::Exception& e){
		ul_writelog(UL_LOG_WARNING, "An exception[%s] was thrown from[%s:%d:%s]"\
				" with a message[%s]! stack_trace:%s%s",
				e.name(), e.file(), int(e.line()), e.function(),
				e.what(), e.get_line_delimiter(), e.stack());

		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(GALILEO_E_BADPARAM);
	}
	catch (std::exception& e){
		ul_writelog(UL_LOG_WARNING,"a std::exception[%s] was thrown!"\
				"Definitely a bug!", e.what());

		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(GALILEO_E_BADPARAM);
	}
	catch(...){
		ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize josn text, path:%s",
				__FILE__, __LINE__, req->path);

		if (NULL != hdlr)
			lleo_destroy(hdlr);
		return error_exit(GALILEO_E_BADPARAM);
	}
	
	if (GALILEO_E_OK == grc){
		fprintf(stdout, "TAG_NO: %d\n", tag_no);
		fprintf(stdout, "DATA: %s\n", req->data_pack);
		fprintf(stdout, "\n%s\n", "Press Ctrl+C to quit...");
		signal(SIGINT, sig_handler);
		while(!is_fini){
			sleep(10);
		}
	}

	if (GALILEO_E_OK != grc){
		fprintf(stderr, "ERROR: %s\n", lleo_err2str(grc));
	}

	if (NULL != hdlr){
		lleo_destroy(hdlr);
	}
	return grc;
}



int process_admin_req_w(admin_req_t *req)
{
	int grc = GALILEO_E_OK;
	lleo_t *hdlr = NULL;

	GALILEO_POINTER_CHECK(NULL == req, "Failed to get a valid request");
	
	
	GALILEO_POINTER_CHECK('\0' == req->host[0], "Failed to get a valid host");
	GALILEO_POINTER_CHECK('\0' == req->path[0], "Failed to get a valid path");

	hdlr = lleo_init(req->host);
	GALILEO_POINTER_CHECK(NULL == hdlr, "Failed to get a valid handler");
	
	
	if ('\0' != req->id[0]){
		grc = zoo_add_auth(hdlr->zh, "digest", req->id, strlen(req->id), NULL, NULL);
		if (grc != ZOK) {
			ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add auth", __FILE__, __LINE__);
			lleo_destroy(hdlr);
			return error_exit(GALILEO_E_AUTHFAILED);
		}
	}

	
	fprintf(stdout, "PATH: %s\n", req->path);

	grc = lleo_watch_resource(hdlr, req->path, admin_watcher, (void*)"admin_watcher");

	if (GALILEO_E_OK == grc){
		fprintf(stdout, "%s\n", "WAITING FOR CHANGE...");
		signal(SIGINT, sig_handler);
		while(!is_fini){
			sleep(10);
		}
	}
	else{
		fprintf(stderr, "ERROR: %s\n", lleo_err2str(grc));
	}

	if (NULL != hdlr){
		lleo_destroy(hdlr);
	}
	return grc;
}


