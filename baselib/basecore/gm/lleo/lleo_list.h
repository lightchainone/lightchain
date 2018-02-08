
#ifndef __GALILEO_LIST_H__
#define __GALILEO_LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lleo_def.h"

#ifdef __cplusplus
extern "C" {
#endif


lleo_list_head* lleo_list_create_head(void);



int lleo_list_insert_node(lleo_list_head* head, const char* path,
                             const char* data, int data_len);



int lleo_list_update_node(lleo_list_head* head, const char* path,
                             const char* data, int data_len);



int lleo_list_delete_node(lleo_list_head* head, const char* path);


int lleo_list_destroy(lleo_list_head* head);

#ifdef __cplusplus
}
#endif

#endif


