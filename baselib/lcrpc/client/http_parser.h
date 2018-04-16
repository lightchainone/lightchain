#ifndef http_parser_h
#define http_parser_h

#ifdef _MSC_VER
#  include <stddef.h>
#endif
#include <sys/types.h>

namespace lcrpc
{


enum http_method
{
	HTTP_DELETE    = 0x0002,
	HTTP_GET       = 0x0004,
	HTTP_HEAD      = 0x0008,
	HTTP_POST      = 0x0100,
	HTTP_PUT       = 0x0800
};

enum states
{
	s_init = 0,
	s_dead = 1 ,
	s_start_res,
	s_res_H,
	s_res_HT,
	s_res_HTT,
	s_res_HTTP,
	s_res_first_http_major,
	s_res_http_major,
	s_res_first_http_minor,
	s_res_http_minor,
	s_res_first_status_code,
	s_res_status_code,
	s_res_status,
	s_res_line_almost_done,
	s_start_req,
	s_req_method_G,
	s_req_method_GE,
	s_req_method_P,
	s_req_method_PU,
	s_req_method_PO,
	s_req_method_POS,
	s_req_method_H,
	s_req_method_HE,
	s_req_method_HEA,
	s_req_method_D,
	s_req_method_DE,
	s_req_method_DEL,
	s_req_method_DELE,
	s_req_method_DELET,
	s_req_spaces_before_url,
	s_req_schema,
	s_req_schema_slash,
	s_req_schema_slash_slash,
	s_req_host,
	s_req_port,
	s_req_path,
	s_req_query_string_start,
	s_req_query_string,
	s_req_fragment_start,
	s_req_fragment,
	s_req_http_start,
	s_req_http_H,
	s_req_http_HT,
	s_req_http_HTT,
	s_req_http_HTTP,
	s_req_first_http_major,
	s_req_http_major,
	s_req_first_http_minor,
	s_req_http_minor,
	s_req_line_almost_done,
	s_header_field_start,
	s_header_field,
	s_header_value_start,
	s_header_value,
	s_header_almost_done,
	s_headers_almost_done,
	s_headers_done,
	s_chunk_size_start,
	s_chunk_size,
	s_chunk_size_almost_done,
	s_chunk_parameters,
	s_chunk_data,
	s_chunk_data_almost_done,
	s_chunk_data_done,
	s_body_identity,
	s_body_identity_eof,
};

enum header_states
{
	h_general = 0,
	h_C,
	h_CO,
	h_CON,
	h_matching_connection,
	h_matching_content_length,
	h_matching_transfer_encoding,
	h_connection,
	h_content_length,
	h_transfer_encoding,
	h_matching_transfer_encoding_chunked,
	h_matching_connection_keep_alive,
	h_matching_connection_close,
	h_transfer_encoding_chunked,
	h_connection_keep_alive,
	h_connection_close,
};

enum flags
{ F_CHUNKED               = 0x0001,
	F_CONNECTION_KEEP_ALIVE = 0x0002,
	F_CONNECTION_CLOSE      = 0x0004,
	F_TRAILING              = 0x0010,
};

class http_parser 
{
plclic:
	http_parser();
	size_t parse_request(const char *data, size_t len);
	size_t parse_response(const char *data, size_t len);
	
	int should_keep_alive();
	unsigned short get_status_code()
	{
		return status_code;
	}
	http_method get_method()
	{
		return method;
	}
	unsigned short get_http_major()
	{
		return http_major;
	}
	unsigned short get_http_minor()
	{
		return http_minor;
	}
	const char * get_url()
	{
		return url_mark;
	}
	size_t get_url_size()
	{
		return url_size; 
	}
	const char * get_path()
	{
		return path_mark;
	}
	size_t get_path_size()
	{
		return path_size; 
	}
	const char * get_query()
	{
		return query_string_mark;
	}
	size_t get_query_size()
	{
		return query_string_size;
	}
	const char * get_fragment()
	{
		return fragment_mark;
	}
	size_t get_fragment_size()
	{
		return fragment_size;
	}

	
protected:
	int url_callback (const char *p);
	int path_callback (const char *p);
	int query_string_callback (const char *p);
	int fragment_callback (const char *p);
	int header_field_callback (const char *p);
	int header_value_callback (const char *p);
	int headers_complete_callback ();
	int message_begin_callback ();
	int message_complete_callback ();
	int body_callback (const char *p, size_t n);

	size_t parse (const char *data, size_t len, states start_state);

	unsigned short status_code; 
	unsigned short http_major;
	unsigned short http_minor;
	char flags;
	enum http_method method;    
	
	states state;
	header_states header_state;
	size_t header_index;

	ssize_t body_read;
	ssize_t content_length;
	size_t      header_field_size;
	size_t      header_value_size;
	size_t      query_string_size;
	size_t      url_size;
	size_t      fragment_size;
	size_t      path_size;
	const char *header_field_mark;
	const char *header_value_mark;
	const char *query_string_mark;
	const char *url_mark;
	const char *fragment_mark;
	const char *path_mark;
};

}

#endif
