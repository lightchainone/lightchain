#include <stdint.h>
#include <stdlib.h>
#include "http_parser.h"

namespace lcrpc
{
 
#ifndef HTTP_PARSER_STRICT
# define HTTP_PARSER_STRICT 1
#else
# define HTTP_PARSER_STRICT 0
#endif

#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define MAX_FIELD_SIZE (80*1024)

#define MARK(FOR)                                                    \
	do {                                                                 \
		this->FOR##_mark = p;                                            \
		this->FOR##_size = 0;                                            \
	} while (0)

#define CALLBACK(FOR)                                                \
	do {                                                                 \
		if (0 != FOR##_callback(p)) return (p - data);             \
		this->FOR##_mark = NULL;                                         \
	} while (0)

#define CALLBACK_NOCLEAR(FOR)                                        \
	do {                                                                 \
		if (0 != FOR##_callback(p)) return (p - data);             \
	} while (0)

#define CALLBACK2(FOR)                                               \
	do {                                                                 \
		if (0 != FOR##_callback()) return (p - data);                \
	} while (0)

int http_parser::url_callback (const char *p) 
{ 
	if (!this->url_mark) return 0; 
	const char *mark = this->url_mark; 
	this->url_size += p - mark; 
	if (this->url_size > MAX_FIELD_SIZE) return -1; 
	return 0; 
}

int http_parser::path_callback (const char *p) 
{ 
	if (!this->path_mark) return 0; 
	const char *mark = this->path_mark; 
	this->path_size += p - mark; 
	if (this->path_size > MAX_FIELD_SIZE) return -1; 
	return 0; 
}
int http_parser::query_string_callback (const char *p) 
{ 
	if (!this->query_string_mark) return 0; 
	const char *mark = this->query_string_mark; 
	this->query_string_size += p - mark; 
	if (this->query_string_size > MAX_FIELD_SIZE) return -1; 
	return 0; 
}
int http_parser::fragment_callback (const char *p) 
{ 
	if (!this->fragment_mark) return 0; 
	const char *mark = this->fragment_mark; 
	this->fragment_size += p - mark; 
	if (this->fragment_size > MAX_FIELD_SIZE) return -1; 
	return 0; 
}
int http_parser::header_field_callback (const char *p) 
{ 
	if (!this->header_field_mark) return 0; 
	const char *mark = this->header_field_mark; 
	this->header_field_size += p - mark; 
	if (this->header_field_size > MAX_FIELD_SIZE) return -1; 
	return 0; 
}
int http_parser::header_value_callback (const char *p) 
{ 
	if (!this->header_value_mark) return 0; 
	const char *mark = this->header_value_mark; 
	this->header_value_size += p - mark; 
	if (this->header_value_size > MAX_FIELD_SIZE) return -1; 
	return 0; 
}

int http_parser::body_callback (const char *, size_t ) 
{ 
	return 0; 
}

int http_parser::headers_complete_callback ()
{
	return 0;
}

int http_parser::message_begin_callback ()
{
	return 0;
}

int http_parser::message_complete_callback ()
{
	return 0;
}

const char CONNECTION [] = "connection";
const char CONTENT_LENGTH [] = "content-length";
const char TRANSFER_ENCODING [] = "transfer-encoding";
const char CHUNKED [] = "chunked";
const char KEEP_ALIVE [] = "keep-alive";
const char CLOSE [] = "close";


static const unsigned char lowcase[] =
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0-\0\0" "0123456789\0\0\0\0\0\0"
	"\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
	"\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

static const int unhex[] =
{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
	,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
		,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
		, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1
		,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1
		,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
		,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1
		,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};


static const uint32_t  usual[] = {
	0xffffdbfe, 

	
	0x7ffffff6, 

	
	0xffffffff, 

	
	0xffffffff, 

	0xffffffff, 
	0xffffffff, 
	0xffffffff, 
	0xffffffff  
};

#define USUAL(c) (usual[c >> 5] & (1 << (c & 0x1f)))



#define CR '\r'
#define LF '\n'
#define LOWER(c) (unsigned char)(c | 0x20)



#define STRICT_CHECK(cond) if (cond) goto error
#define NEW_MESSAGE() s_dead

size_t http_parser::parse (const char *data, size_t len, states start_state)
{
	char c, ch; 
	const char *p, *pe;
	ssize_t to_read;

	enum states state = this->state;
	enum header_states header_state = this->header_state;
	size_t header_index = this->header_index;

	if (len == 0) {
		if (state == s_body_identity_eof) {
			CALLBACK2(message_complete);
		}
		return 0;
	}

	if (this->header_field_mark)   this->header_field_mark   = data;
	if (this->header_value_mark)   this->header_value_mark   = data;
	if (this->fragment_mark)       this->fragment_mark       = data;
	if (this->query_string_mark)   this->query_string_mark   = data;
	if (this->path_mark)           this->path_mark           = data;
	if (this->url_mark)            this->url_mark            = data;

	for (p=data, pe=data+len; p != pe; p++) {
		ch = *p;
		switch (state) {

			case s_dead:
				
				goto error;

			case s_start_res:
				{
					this->flags = 0;
					this->content_length = -1;

					CALLBACK2(message_begin);

					switch (ch) {
						case 'H':
							state = s_res_H; 
							break;

						case CR:
						case LF:
							break;

						default:
							goto error;
					}
					break;
				}

			case s_res_H:
				STRICT_CHECK(ch != 'T');
				state = s_res_HT;
				break;

			case s_res_HT:
				STRICT_CHECK(ch != 'T');
				state = s_res_HTT;
				break;

			case s_res_HTT: 
				STRICT_CHECK(ch != 'P');
				state = s_res_HTTP;
				break;

			case s_res_HTTP:
				STRICT_CHECK(ch != '/');
				state = s_res_first_http_major;
				break;

			case s_res_first_http_major:
				if (ch < '1' || ch > '9') goto error;
				this->http_major = ch - '0';
				state = s_res_http_major;
				break;

				
			case s_res_http_major:
				{
					if (ch == '.') {
						state = s_res_first_http_minor;
						break;
					}

					if (ch < '0' || ch > '9') goto error;

					this->http_major *= 10;
					this->http_major += ch - '0';

					if (this->http_major > 999) goto error;
					break;
				}

				
			case s_res_first_http_minor:
				if (ch < '0' || ch > '9') goto error;
				this->http_minor = ch - '0';
				state = s_res_http_minor;
				break;

				
			case s_res_http_minor:
				{
					if (ch == ' ') {
						state = s_res_first_status_code;
						break;
					}

					if (ch < '0' || ch > '9') goto error;

					this->http_minor *= 10;
					this->http_minor += ch - '0';

					if (this->http_minor > 999) goto error;
					break;
				}

			case s_res_first_status_code:
				{
					if (ch < '0' || ch > '9') {
						if (ch == ' ') {
							break;
						}
						goto error;
					}
					this->status_code = ch - '0';
					state = s_res_status_code;
					break;
				}

			case s_res_status_code:
				{
					if (ch < '0' || ch > '9') {
						switch (ch) {
							case ' ':
								state = s_res_status;
								break;
							case CR:
								state = s_res_line_almost_done;
								break;
							case LF:
								state = s_header_field_start;
								break;
							default:
								goto error;
						}
						break;
					}

					this->status_code *= 10;
					this->status_code += ch - '0';

					if (this->status_code > 999) goto error;
					break;
				}

			case s_res_status:
				
				if (ch == CR) {
					state = s_res_line_almost_done;
					break;
				}

				if (ch == LF) {
					state = s_header_field_start;
					break;
				}
				break;

			case s_res_line_almost_done:
				STRICT_CHECK(ch != LF);
				state = s_header_field_start;
				break;

			case s_start_req:
				{
					this->flags = 0;
					this->content_length = -1;

					CALLBACK2(message_begin);

					switch (ch) {
						
						case 'G':
							state = s_req_method_G;
							break;

							
						case 'P':
							state = s_req_method_P;
							break;

							
						case 'H':
							state = s_req_method_H;
							break;

							
						case 'D':
							state = s_req_method_D;
							break;

						case CR:
						case LF:
							break;

						default:
							goto error;
					}
					break;
				}

				

			case s_req_method_G:
				STRICT_CHECK(ch != 'E');
				state = s_req_method_GE;
				break;

			case s_req_method_GE:
				STRICT_CHECK(ch != 'T');
				this->method = HTTP_GET;
				state = s_req_spaces_before_url;
				break;

				

			case s_req_method_H:
				STRICT_CHECK(ch != 'E');
				state = s_req_method_HE;
				break;

			case s_req_method_HE:
				STRICT_CHECK(ch != 'A');
				state = s_req_method_HEA;
				break;

			case s_req_method_HEA:
				STRICT_CHECK(ch != 'D');
				this->method = HTTP_HEAD;
				state = s_req_spaces_before_url;
				break;

				

			case s_req_method_P:
				switch (ch) {
					case 'O':
						state = s_req_method_PO;
						break;

					case 'U':
						state = s_req_method_PU;
						break;

					default:
						goto error;
				}
				break;

				

			case s_req_method_PU:
				STRICT_CHECK(ch != 'T');
				this->method = HTTP_PUT;
				state = s_req_spaces_before_url;
				break;

				

			case s_req_method_PO:
				STRICT_CHECK(ch != 'S');
				state = s_req_method_POS;
				break;

			case s_req_method_POS:
				STRICT_CHECK(ch != 'T');
				this->method = HTTP_POST;
				state = s_req_spaces_before_url;
				break;

				

			case s_req_method_D:
				STRICT_CHECK(ch != 'E');
				state = s_req_method_DE;
				break;

			case s_req_method_DE:
				STRICT_CHECK(ch != 'L');
				state = s_req_method_DEL;
				break;

			case s_req_method_DEL:
				STRICT_CHECK(ch != 'E');
				state = s_req_method_DELE;
				break;

			case s_req_method_DELE:
				STRICT_CHECK(ch != 'T');
				state = s_req_method_DELET;
				break;

			case s_req_method_DELET:
				STRICT_CHECK(ch != 'E');
				this->method = HTTP_DELETE;
				state = s_req_spaces_before_url;
				break;


			case s_req_spaces_before_url:
				{
					if (ch == ' ') break;

					if (ch == '/') {
						MARK(url);
						MARK(path);
						state = s_req_path;
						break;
					}

					c = LOWER(ch);

					if (c >= 'a' && c <= 'z') {
						MARK(url);
						state = s_req_schema;
						break;
					}

					goto error;
				}

			case s_req_schema:
				{
					c = LOWER(ch);

					if (c >= 'a' && c <= 'z') break;

					if (ch == ':') {
						state = s_req_schema_slash;
						break;
					}

					goto error;
				}

			case s_req_schema_slash:
				STRICT_CHECK(ch != '/');
				state = s_req_schema_slash_slash;
				break;

			case s_req_schema_slash_slash:
				STRICT_CHECK(ch != '/');
				state = s_req_host;
				break;

			case s_req_host:
				{
					c = LOWER(ch);
					if (c >= 'a' && c <= 'z') break;
					if ((ch >= '0' && ch <= '9') || ch == '.' || ch == '-') break;
					switch (ch) {
						case ':':
							state = s_req_port;
							break;
						case '/':
							MARK(path);
							state = s_req_path;
							break;
						case ' ':
							
							CALLBACK(url);
							state = s_req_http_start;
							break;
						default:
							goto error;
					}
					break;
				}

			case s_req_port:
				{
					if (ch >= '0' && ch <= '9') break;
					switch (ch) {
						case '/':
							MARK(path);
							state = s_req_path;
							break;
						case ' ':
							
							CALLBACK(url);
							state = s_req_http_start;
							break;
						default:
							goto error;
					}
					break;
				}

			case s_req_path:
				{
					if (USUAL(ch)) break;

					switch (ch) {
						case ' ':
							CALLBACK(url);
							CALLBACK(path);
							state = s_req_http_start;
							break;
						case CR:
							CALLBACK(url);
							CALLBACK(path);
							this->http_minor = 9;
							state = s_req_line_almost_done;
							break;
						case LF:
							CALLBACK(url);
							CALLBACK(path);
							this->http_minor = 9;
							state = s_header_field_start;
							break;
						case '?':
							CALLBACK(path);
							state = s_req_query_string_start;
							break;
						case '#':
							CALLBACK(path);
							state = s_req_fragment_start;
							break;
						default:
							goto error;
					}
					break;
				}

			case s_req_query_string_start:
				{
					if (USUAL(ch)) {
						MARK(query_string);
						state = s_req_query_string;
						break;
					}

					switch (ch) {
						case '?':
							break; 
						case ' ':
							CALLBACK(url);
							state = s_req_http_start;
							break;
						case CR:
							CALLBACK(url);
							this->http_minor = 9;
							state = s_req_line_almost_done;
							break;
						case LF:
							CALLBACK(url);
							this->http_minor = 9;
							state = s_header_field_start;
							break;
						case '#':
							state = s_req_fragment_start;
							break;
						default:
							goto error;
					}
					break;
				}

			case s_req_query_string:
				{
					if (USUAL(ch)) break;

					switch (ch) {
						case ' ':
							CALLBACK(url);
							CALLBACK(query_string);
							state = s_req_http_start;
							break;
						case CR:
							CALLBACK(url);
							CALLBACK(query_string);
							this->http_minor = 9;
							state = s_req_line_almost_done;
							break;
						case LF:
							CALLBACK(url);
							CALLBACK(query_string);
							this->http_minor = 9;
							state = s_header_field_start;
							break;
						case '#':
							CALLBACK(query_string);
							state = s_req_fragment_start;
							break;
						default:
							goto error;
					}
					break;
				}

			case s_req_fragment_start:
				{
					if (USUAL(ch)) {
						MARK(fragment);
						state = s_req_fragment;
						break;
					}

					switch (ch) {
						case ' ':
							CALLBACK(url);
							state = s_req_http_start;
							break;
						case CR:
							CALLBACK(url);
							this->http_minor = 9;
							state = s_req_line_almost_done;
							break;
						case LF:
							CALLBACK(url);
							this->http_minor = 9;
							state = s_header_field_start;
							break;
						case '?':
							MARK(fragment);
							state = s_req_fragment;
							break;
						case '#':
							break;
						default:
							goto error;
					}
					break;
				}

			case s_req_fragment:
				{
					if (USUAL(ch)) break;

					switch (ch) {
						case ' ':
							CALLBACK(url);
							CALLBACK(fragment);
							state = s_req_http_start;
							break;
						case CR:
							CALLBACK(url);
							CALLBACK(fragment);
							this->http_minor = 9;
							state = s_req_line_almost_done;
							break;
						case LF:
							CALLBACK(url);
							CALLBACK(fragment);
							this->http_minor = 9;
							state = s_header_field_start;
							break;
						case '?':
						case '#':
							break;
						default:
							goto error;
					}
					break;
				}

			case s_req_http_start:
				switch (ch) {
					case 'H':
						state = s_req_http_H;
						break;
					case ' ':
						break;
					default:
						goto error;
				}
				break;

			case s_req_http_H:
				STRICT_CHECK(ch != 'T');
				state = s_req_http_HT;
				break;

			case s_req_http_HT:
				STRICT_CHECK(ch != 'T');
				state = s_req_http_HTT;
				break;

			case s_req_http_HTT:
				STRICT_CHECK(ch != 'P');
				state = s_req_http_HTTP;
				break;

			case s_req_http_HTTP:
				STRICT_CHECK(ch != '/');
				state = s_req_first_http_major;
				break;

				
			case s_req_first_http_major:
				if (ch < '1' || ch > '9') goto error;
				this->http_major = ch - '0';
				state = s_req_http_major;
				break;

				
			case s_req_http_major:
				{
					if (ch == '.') {
						state = s_req_first_http_minor;
						break;
					}

					if (ch < '0' || ch > '9') goto error;

					this->http_major *= 10;
					this->http_major += ch - '0';

					if (this->http_major > 999) goto error;
					break;
				}

				
			case s_req_first_http_minor:
				if (ch < '0' || ch > '9') goto error;
				this->http_minor = ch - '0';
				state = s_req_http_minor;
				break;

				
			case s_req_http_minor:
				{
					if (ch == CR) {
						state = s_req_line_almost_done;
						break;
					}

					if (ch == LF) {
						state = s_header_field_start;
						break;
					}

					

					if (ch < '0' || ch > '9') goto error;

					this->http_minor *= 10;
					this->http_minor += ch - '0';

					if (this->http_minor > 999) goto error;
					break;
				}

				
			case s_req_line_almost_done:
				{
					if (ch != LF) goto error;
					state = s_header_field_start;
					break;
				}

			case s_header_field_start:
				{
					if (ch == CR) {
						state = s_headers_almost_done;
						break;
					}

					if (ch == LF) {
						state = s_headers_done;
						break;
					}

					c = LOWER(ch);

					if (c < 'a' || 'z' < c) goto error;

					MARK(header_field);

					header_index = 0;
					state = s_header_field;

					switch (c) {
						case 'c':
							header_state = h_C;
							break;

						case 't':
							header_state = h_matching_transfer_encoding;
							break;

						default:
							header_state = h_general;
							break;
					}
					break;
				}

			case s_header_field:
				{
					c = lowcase[(int)ch];

					if (c) {
						switch (header_state) {
							case h_general:
								break;

							case h_C:
								header_index++;
								header_state = (c == 'o' ? h_CO : h_general);
								break;

							case h_CO:
								header_index++;
								header_state = (c == 'n' ? h_CON : h_general);
								break;

							case h_CON:
								header_index++;
								switch (c) {
									case 'n':
										header_state = h_matching_connection;
										break;
									case 't':
										header_state = h_matching_content_length;
										break;
									default:
										header_state = h_general;
										break;
								}
								break;

								

							case h_matching_connection:
								header_index++;
								if (header_index > sizeof(CONNECTION)-1
										|| c != CONNECTION[header_index]) {
									header_state = h_general;
								} else if (header_index == sizeof(CONNECTION)-2) {
									header_state = h_connection;
								}
								break;

								

							case h_matching_content_length:
								header_index++;
								if (header_index > sizeof(CONTENT_LENGTH)-1
										|| c != CONTENT_LENGTH[header_index]) {
									header_state = h_general;
								} else if (header_index == sizeof(CONTENT_LENGTH)-2) {
									header_state = h_content_length;
								}
								break;

								

							case h_matching_transfer_encoding:
								header_index++;
								if (header_index > sizeof(TRANSFER_ENCODING)-1
										|| c != TRANSFER_ENCODING[header_index]) {
									header_state = h_general;
								} else if (header_index == sizeof(TRANSFER_ENCODING)-2) {
									header_state = h_transfer_encoding;
								}
								break;

							case h_connection:
							case h_content_length:
							case h_transfer_encoding:
								if (ch != ' ') header_state = h_general;
								break;

							default:
								
								break;
						}
						break;
					}

					if (ch == ':') {
						CALLBACK(header_field);
						state = s_header_value_start;
						break;
					}

					if (ch == CR) {
						state = s_header_almost_done;
						CALLBACK(header_field);
						break;
					}

					if (ch == LF) {
						CALLBACK(header_field);
						state = s_header_field_start;
						break;
					}

					goto error;
				}

			case s_header_value_start:
				{
					if (ch == ' ') break;

					MARK(header_value);

					state = s_header_value;
					header_index = 0;

					c = lowcase[(int)ch];

					if (!c) {
						if (ch == CR) {
							header_state = h_general;
							state = s_header_almost_done;
							break;
						}

						if (ch == LF) {
							state = s_header_field_start;
							break;
						}

						header_state = h_general;
						break;
					} 

					switch (header_state) {
						case h_transfer_encoding:
							
							if ('c' == c) {
								header_state = h_matching_transfer_encoding_chunked;
							} else {
								header_state = h_general;
							}
							break;

						case h_content_length:
							if (ch < '0' || ch > '9') goto error;
							this->content_length = ch - '0';
							break;

						case h_connection:
							
							if (c == 'k') {
								header_state = h_matching_connection_keep_alive;
								
							} else if (c == 'c') {
								header_state = h_matching_connection_close;
							} else {
								header_state = h_general;
							}
							break;

						default:
							header_state = h_general;
							break;
					}
					break;
				}

			case s_header_value:
				{
					c = lowcase[(int)ch];

					if (!c) {
						if (ch == CR) {
							CALLBACK(header_value);
							state = s_header_almost_done;
							break;
						}

						if (ch == LF) {
							CALLBACK(header_value);
							state = s_header_field_start;
							break;
						}
						break;
					}

					switch (header_state) {
						case h_general:
							break;

						case h_connection:
						case h_transfer_encoding:
							
							break;

						case h_content_length:
							if (ch < '0' || ch > '9') goto error;
							this->content_length *= 10;
							this->content_length += ch - '0';
							break;

							
						case h_matching_transfer_encoding_chunked:
							header_index++;
							if (header_index > sizeof(CHUNKED)-1 
									|| c != CHUNKED[header_index]) {
								header_state = h_general;
							} else if (header_index == sizeof(CHUNKED)-2) {
								header_state = h_transfer_encoding_chunked;
							}
							break;

							
						case h_matching_connection_keep_alive:
							header_index++;
							if (header_index > sizeof(KEEP_ALIVE)-1
									|| c != KEEP_ALIVE[header_index]) {
								header_state = h_general;
							} else if (header_index == sizeof(KEEP_ALIVE)-2) {
								header_state = h_connection_keep_alive;
							}
							break;

							
						case h_matching_connection_close:
							header_index++;
							if (header_index > sizeof(CLOSE)-1 || c != CLOSE[header_index]) {
								header_state = h_general;
							} else if (header_index == sizeof(CLOSE)-2) {
								header_state = h_connection_close;
							}
							break;

						case h_transfer_encoding_chunked:
						case h_connection_keep_alive:
						case h_connection_close:
							if (ch != ' ') header_state = h_general;
							break;

						default:
							state = s_header_value;
							header_state = h_general;
							break;
					}
					break;
				}

			case s_header_almost_done:
				{
					STRICT_CHECK(ch != LF);

					state = s_header_field_start;

					switch (header_state) {
						case h_connection_keep_alive:
							this->flags |= F_CONNECTION_KEEP_ALIVE;
							break;
						case h_connection_close:
							this->flags |= F_CONNECTION_CLOSE;
							break;
						case h_transfer_encoding_chunked:
							this->flags |= F_CHUNKED;
							break;
						default:
							break;
					}
					break;
				}

			case s_headers_almost_done:
				{
					STRICT_CHECK(ch != LF);

					if (this->flags & F_TRAILING) {
						
						CALLBACK2(message_complete);
						state = NEW_MESSAGE();
						break;
					}

					this->body_read = 0;

					CALLBACK2(headers_complete);

					if (this->flags & F_CHUNKED) {
						
						state = s_chunk_size_start;
					} else {
						if (this->content_length == 0) {
							
							CALLBACK2(message_complete);
							state = NEW_MESSAGE();
						} else if (this->content_length > 0) {
							
							state = s_body_identity;
						} else {
							if (start_state == s_start_req || should_keep_alive()) {
								
								CALLBACK2(message_complete);
								state = NEW_MESSAGE();
							} else {
								
								state = s_body_identity_eof;
							}
						}
					}

					break;
				}

			case s_body_identity:
				to_read = MIN(pe - p, (ssize_t)(this->content_length - this->body_read));
				if (to_read > 0) {
					this->body_callback(p, to_read);
					p += to_read - 1;
					this->body_read += to_read;
					if (this->body_read == this->content_length) {
						CALLBACK2(message_complete);
						state = NEW_MESSAGE();
					}
				}
				break;

				
			case s_body_identity_eof:
				to_read = pe - p;
				if (to_read > 0) {
					this->body_callback(p, to_read);
					p += to_read - 1;
					this->body_read += to_read;
				}
				break;

			case s_chunk_size_start:
				{
					

					c = unhex[(int)ch];
					if (c == -1) goto error;
					this->content_length = c;
					state = s_chunk_size;
					break;
				}

			case s_chunk_size:
				{
					

					if (ch == CR) {
						state = s_chunk_size_almost_done;
						break;
					}

					c = unhex[(int)ch];

					if (c == -1) {
						if (ch == ';' || ch == ' ') {
							state = s_chunk_parameters;
							break;
						}
						goto error;
					}

					this->content_length *= 16;
					this->content_length += c;
					break;
				}

			case s_chunk_parameters:
				{
					
					
					if (ch == CR) {
						state = s_chunk_size_almost_done;
						break;
					}
					break;
				}

			case s_chunk_size_almost_done:
				{
					
					STRICT_CHECK(ch != LF);

					if (this->content_length == 0) {
						this->flags |= F_TRAILING;
						state = s_header_field_start;
					} else {
						state = s_chunk_data;
					}
					break;
				}

			case s_chunk_data:
				{
					

					to_read = MIN(pe - p, (ssize_t)(this->content_length));

					if (to_read > 0) {
						this->body_callback(p, to_read);
						p += to_read - 1;
					}

					if (to_read == this->content_length) {
						state = s_chunk_data_almost_done;
					}

					this->content_length -= to_read;
					break;
				}

			case s_chunk_data_almost_done:
				
				STRICT_CHECK(ch != CR);
				state = s_chunk_data_done;
				break;

			case s_chunk_data_done:
				
				STRICT_CHECK(ch != LF);
				state = s_chunk_size_start;
				break;

			default:
				
				goto error;
		}
	}

	CALLBACK_NOCLEAR(header_field);
	CALLBACK_NOCLEAR(header_value);
	CALLBACK_NOCLEAR(fragment);
	CALLBACK_NOCLEAR(query_string);
	CALLBACK_NOCLEAR(path);
	CALLBACK_NOCLEAR(url);

	this->state = state;
	this->header_state = header_state;
	this->header_index = header_index;

	return len;

error:
	return (p - data);
}


	size_t
http_parser::parse_request (const char *data, size_t len)
{
	if (!this->state) this->state = s_start_req;
	return parse(data, len, s_start_req);
}


	size_t
http_parser::parse_response (const char *data, size_t len)
{
	if (!this->state) this->state = s_start_res;
	return parse(data, len, s_start_res);
}

	int
http_parser::should_keep_alive ()
{
	if (this->http_major > 0 && this->http_minor > 0) {
		
		if (this->flags & F_CONNECTION_CLOSE) {
			return s_init;
		} else {
			return s_dead;
		}
	} else {
		
		if (this->flags & F_CONNECTION_KEEP_ALIVE) {
			return s_dead;
		} else {
			return s_init;
		}
	}
}


http_parser::http_parser ()
{
	this->state = s_init;
	this->header_field_mark = NULL;
	this->header_value_mark = NULL;
	this->query_string_mark = NULL;
	this->path_mark = NULL;
	this->url_mark = NULL;
	this->fragment_mark = NULL;
	this->header_field_size = 0;
	this->header_value_size = 0;
	this->query_string_size = 0;
	this->url_size = 0;
	this->fragment_size= 0;
}

}
