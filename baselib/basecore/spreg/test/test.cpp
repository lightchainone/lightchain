#include <stdio.h>
#include <stdlib.h>

#include "ul_log.h"
#include "spreg.h"

#define MAX_SLC_MATCH 10


int search_all_callback(spreg_callback_param_t *callback_match)
{
	spreg_match_t *rmatch = callback_match->rmatch;
	const char *str = callback_match->src;
	fprintf(stderr, "search all : [%d %d] [%.*s]\n", rmatch[0].match_begin, rmatch[0].match_end, 
			rmatch[0].match_end - rmatch[0].match_begin, 
			str + rmatch[0].match_begin);
	return 0;
}

int split_callback(spreg_callback_param_t *callback_match)
{
	spreg_match_t *rmatch = callback_match->rmatch;
	const char *str = callback_match->src;
	fprintf(stderr, "split : [%d %d] [%.*s]\n", rmatch[0].match_begin, rmatch[0].match_end, 
			rmatch[0].match_end - rmatch[0].match_begin, 
			str + rmatch[0].match_begin);
	return 0;
}



int main()
{
	spreg_match_t *rmatch = spreg_match_init(MAX_SLC_MATCH);
	if (NULL == rmatch) {
		ul_writelog(UL_LOG_FATAL, "no memory for spreg_match_t");
		return -1;
	}

	char regex_string[] = "[0-9]+([A-Z]+)";
	char match_string[] = "M88734AADFBBahjdjd18734XADFBB";

	int match_string_size = strlen(match_string);
	char replace_string[] = "OK\\1";
	char buffer[1024];
	int buffer_size = sizeof(buffer);
		
	char const *err = NULL;
	int ret; 
	spreg_t *re = NULL;
	
	re = spreg_init(regex_string, &err);
	if (err != NULL) {
		
		ul_writelog(UL_LOG_WARNING, "spreg_init error [%s]", err);

	} else {
		
		ret = spreg_match(re, match_string, match_string_size);
		if (SPREG_NOMATCH == ret) {
			
			ul_writelog(UL_LOG_TRACE, "spreg_match NO MATCH!");	
		} else if (ret < 0) {
			ul_writelog(UL_LOG_WARNING, "spreg_match error [%d]", ret);
		} else {
			ul_writelog(UL_LOG_TRACE, "spreg_match MATCH");
		}


		
		ret = spreg_search(re, match_string, match_string_size, rmatch, MAX_SLC_MATCH);	
		if (ret < 0) {
			ul_writelog(UL_LOG_WARNING, "spreg_search error [%d]", ret);
		} else {
			fprintf(stderr, "\nspreg_search result\n");
			for (int i = 0; i < ret; ++i) {
				fprintf(stderr, "%d : [%d %d] [%.*s]\n", i, rmatch[i].match_begin, rmatch[i].match_end,
						rmatch[i].match_end - rmatch[i].match_begin, 
						match_string + rmatch[i].match_begin);
			}
			fprintf(stderr, "\n");
		}

		
		ret = spreg_search_all(re, match_string, match_string_size, search_all_callback,
				match_string);
		if (ret < 0) {
			ul_writelog(UL_LOG_WARNING, "spreg_search_all error [%d]", ret);
		} else {
			fprintf(stderr, "search %d match\n\n", ret);
		}


		ret = spreg_split(re, match_string, match_string_size, split_callback, match_string);
		if (ret < 0) {
			ul_writelog(UL_LOG_WARNING, "spreg_split error [%d]", ret);
		} else {
			fprintf(stderr, "split %d \n\n", ret);
		}

		ret = spreg_replace(re, match_string, match_string_size, 
				replace_string, buffer, buffer_size, 1);
		
		if (ret < 0) {
			ul_writelog(UL_LOG_WARNING, "spreg_replace error [%d]", ret);
		} else {
			
			buffer[ret] = '\0';
			fprintf(stderr, "result string : %s\n", buffer);
		}

		
		spreg_destroy(re);
		spreg_match_destroy(rmatch);

	}

	return 0;
}



















