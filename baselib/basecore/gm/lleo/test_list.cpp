#include "lleo_list.h"

int main()
{
    int ret = 0;
    lleo_list_head *head = lleo_list_create_head();

    if (NULL == head){
        printf("failed to get a valid list head");
    }

    for (int i = 0; i < 5; i++){
        char buff[5];
        snprintf(buff, 5, "%d", i);
        ret = lleo_list_insert_node(head, buff, buff, 5);
        if (0 != ret){
            printf("fail to insert %dth node, count:%d\n", i, head->totl_node);
        }
        printf("count[%d]\n", head->totl_node);
    }
    printf("total_node:%d ", head->totl_node);
    printf("insert done\n");

    lleo_list_node *node = head->next;
    while(node != NULL){
        printf("path[%s] data[%s] count[%d]\n", node->path, node->data, head->totl_node);
        node = node->next;
    }

    for (int i = 2; i < 7; i++){
        char buff[5];
        snprintf(buff, 5, "%d", i);
        ret = lleo_list_delete_node(head, buff);
        if (0 != ret){
            printf("fail to delete %dth node, ret:%d\n", i, ret);
        }
    }
    printf("total_node:%d ", head->totl_node);
    printf("delete done\n");

    lleo_list_destroy(head);
    return 0;
}
