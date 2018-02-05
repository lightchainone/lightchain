#include <ul_log.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "lc_auth.h"
#include "../lc/lcatomic.h"

#define MAX_STEP 4096

typedef struct _ip_node_t
{
	unsigned int _left;
	unsigned int _right;
} ip_node_t;

typedef struct _lc_auth_t
{
	ip_node_t *_nodes;
	unsigned int _nums;
	unsigned int _caps;
	bool _complier;

	lc::AtomicInt  _ref;
} lc_auth_t;

bool operator < (const ip_node_t &_1, const ip_node_t &_2)
{
	return _1._left < _2._left;
}

static int get_addr(unsigned char &_l, unsigned char &_r, const char * &ip)
{
	_l = _r = 0;
	int val = 0;
	if (isdigit(*ip)) {
		while (ip!=0 && isdigit(*ip)) {
			val = val * 10 + *ip - '0';
			++ip;
		}
		if (val<0 || val>255) return -1;
		_r = _l = val;
		return 0;
	} else if (*ip == '*') {
		_l = 0;
		_r = 255;
		++ ip;
		return 0;
	}
	return -1;
}

static int get_ip(ip_node_t &node, const char * &ip)
{
	unsigned char *_l = (unsigned char *)&node._left;
	unsigned char *_r = (unsigned char *)&node._right;
	for (int i=3; i>=0; --i) {
		if (get_addr(_l[i], _r[i], ip) != 0) {
			return -1;
		}
		if (*ip == 0 && i>0) {
			return -1;
		}
		if (i>0 && *ip != '.') {
			return -1;
		}
		++ ip;
	}
	-- ip;
	return 0;
}

static int get_ip_node(ip_node_t &node, const char * &ip)
{
	if (get_ip(node, ip) != 0) return -1;
	if (*ip == 0) return 0;
	if (*ip == '/') {
		++ ip;
		if (ip==0 || !isdigit(*ip)) return -1;
		int val = 0;
		while (isdigit(*ip)) {
			val = val*10 + *ip - '0';
			++ ip;
		}
		if (val>32 && val<=0) return -1;
		val = 32-val;
		node._left = (node._left >> val) << val;
		node._right |= (1<<val) - 1;
		return 0;
	}
	return 0;
}

static void get_skip_sp(const char *&ip)
{
	while (*ip!=0 && (*ip==' ' || *ip=='\t' || *ip=='\n')) ++ ip;
}

static int aly_ip (const char * ip, ip_node_t &node)
{
	get_skip_sp(ip);
	if (get_ip_node(node, ip) != 0) {
		ul_writelog(UL_LOG_WARNING, "aly first ip err");
		return -1;
	}
	get_skip_sp(ip);
	if (*ip != 0) {
		if (*ip == '-') {
			++ ip;
			get_skip_sp(ip);
			ip_node_t n2;
			if (get_ip_node(n2, ip) != 0) {
				ul_writelog(UL_LOG_WARNING, "aly second ip after - err");
				return -1;
			}
			if (n2._left < node._left) {
				ul_writelog(UL_LOG_WARNING, "invalid address, unright region");
				return -1;
			}
			if (n2._right >= node._right) {
				node._right = n2._right;
			} else {
				ul_writelog(UL_LOG_WARNING, "invalid address, unright region");
				return -1;
			}
			return 0;
		} else {
			ul_writelog(UL_LOG_WARNING, "invalid address: unexpect character after first ip");
			return -1;
		}
	}
	return 0;
}

lc_auth_t * lc_auth_create(unsigned int sup_size)
{
	if (sup_size <= 0) {
		return NULL;
	}
	lc_auth_t *auth = (lc_auth_t *) malloc (sizeof(lc_auth_t));
	if (auth == NULL) {
		return NULL;
	}
	auth->_caps = sup_size;
	auth->_nodes = (ip_node_t *) malloc (sizeof(ip_node_t) *auth->_caps);
	auth->_nums = 0;
	auth->_complier = false;
	if (auth->_nodes == NULL) {
		free (auth);
		return NULL;
	}
	auth->_ref.set(0);
	return auth;
}

void lc_auth_destroy(lc_auth_t *auth)
{
	if (auth) {
		if (auth->_nodes) {
			free (auth->_nodes);
		}
		free (auth);
	}
}

int lc_auth_push_ip(lc_auth_t *auth, const char *ip)
{
	auth->_complier = false;

	
	if (auth->_nums >= auth->_caps) {
		int caps = auth->_caps;
		if (caps < MAX_STEP) {
			caps += caps;
		} else {
			caps += MAX_STEP;
		}

		ip_node_t *nodes = (ip_node_t *) malloc (sizeof(ip_node_t) * caps);
		if (nodes == NULL) { 
			return -1;
		}
		memcpy(nodes, auth->_nodes, sizeof(ip_node_t)*auth->_nums);
		free (auth->_nodes);
		auth->_nodes = nodes;
		auth->_caps = caps;
	}

	int ret = aly_ip(ip, auth->_nodes[auth->_nums]);
	if (ret != 0) {
		ul_writelog(UL_LOG_WARNING, "push ip[%s] into auth err, please check the ip[%s]", ip, ip);
		return ret;
	} else {
		ul_writelog(UL_LOG_NOTICE, "push ip[%s] into auth success", ip);
	}
	++ auth->_nums;
	return 0;
}

int lc_auth_load_ip(lc_auth_t *auth, const char *file)
{
	FILE *fp = fopen(file, "r");
	if (fp == NULL) {
		ul_writelog(UL_LOG_WARNING, "can't open file[%s]", file);
		return -1;
	}
	char *ptr = 0;
	char line[1024];
	while ((ptr=fgets(line, sizeof(line)-1, fp)) != NULL) {
		line[sizeof(line)-1] = 0;
		if (ptr[0] != '#') {
			int len = strlen(ptr)-1;
			for (; len>=0; --len) {
				if (ptr[len] == '\n') {
					ptr[len] = ' ';
					break;
				}
			}
			lc_auth_push_ip(auth, ptr);
		}
	}
	fclose(fp);
	return 0;
}

int lc_auth_load_ip_ex(lc_auth_t *auth, const char *file, void (*cb)(const char*, void *), void *data)
{
	FILE *fp = fopen(file, "r");
	if (fp == NULL) {
		ul_writelog(UL_LOG_WARNING, "can't open file[%s]", file);
		return -1;
	}
	char *ptr = 0;
	char line[1024];
	while ((ptr=fgets(line, sizeof(line)-1, fp)) != NULL) {
		line[sizeof(line)-1] = 0;
		if (ptr[0] != '#') {
			int len = strlen(ptr)-1;
			for (; len>=0; --len) {
				if (ptr[len] == '\n') {
					ptr[len] = ' ';
					break;
				}
			}

			if(lc_auth_push_ip(auth, ptr) == 0) {
				cb(ptr, data);
			}
		}
	}
	fclose(fp);
	return 0;
}


int lc_auth_compile(lc_auth_t *auth)
{
	if (auth->_nums <= 0) return 0;
	
	std::sort(auth->_nodes, auth->_nodes+auth->_nums);
	int ptr = 0;
	for (unsigned int i=1; i<auth->_nums; ++i) {
		if (auth->_nodes[ptr]._right == (unsigned int)-1) continue;

		if (auth->_nodes[ptr]._right+1 >= auth->_nodes[i]._left) {
			if (auth->_nodes[ptr]._right < auth->_nodes[i]._right) {
				auth->_nodes[ptr]._right = auth->_nodes[i]._right;
			}
		} else {
			++ ptr;
			auth->_nodes[ptr]._left = auth->_nodes[i]._left;
			auth->_nodes[ptr]._right = auth->_nodes[i]._right;
		}
	}
	auth->_nums = ptr+1;

	auth->_complier = true;
	return 0;
}

int _ip_auth(lc_auth_t *auth, const unsigned int ip)
{
	if (auth->_complier) {
		int left=0, right=auth->_nums, mid;
		while (left <= right) {
			mid = (left+right)/2;
			if (ip < auth->_nodes[mid]._left) {
				right = mid-1;
			} else if (ip > auth->_nodes[mid]._right) {
				left = mid+1;
			} else {
				return 1;
			}
		}
	} else {
		for (int i=0; i<(int)auth->_nums; ++i) {
			if (ip >= auth->_nodes[i]._left && ip <= auth->_nodes[i]._right) return 1;
		}
	}
	return 0;
}

int lc_auth_ip_str(lc_auth_t *auth, const char *ip)
{
	ip_node_t node;
	if (get_ip(node, ip) != 0) return 0;
	return _ip_auth (auth, node._left);
}

int lc_auth_ip_int(lc_auth_t *auth, const unsigned int ip)
{
	unsigned char *ips = (unsigned char *)&ip;
	unsigned char tmp = ips[0]; ips[0] = ips[3]; ips[3] = tmp; 
	tmp = ips[1]; ips[1] = ips[2]; ips[2] = tmp;
	return _ip_auth(auth, ip);
}

int lc_auth_ip_fd(lc_auth_t *auth, const int fd)
{
	sockaddr_in addr;
	socklen_t slen = sizeof(addr);
	getpeername(fd, (sockaddr *)&addr, &slen);
	return lc_auth_ip_int(auth, addr.sin_addr.s_addr);
}

int lc_auth_addref(lc_auth_t *auth)
{
	return auth->_ref.incrementAndGet();
}

void lc_auth_delref(lc_auth_t *auth)
{
	auth->_ref.decrementAndGet();
}

int lc_auth_refcnt(lc_auth_t *auth)
{
	return auth->_ref.get();
}

