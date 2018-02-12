

#ifndef  __DLIST_H_
#define  __DLIST_H_

namespace comspace
{

struct dlist_t
{
	dlist_t *next;
	int size[0];
};

dlist_t *dlist_insert(dlist_t *root, dlist_t *node);
dlist_t *dlist_erase(dlist_t *root, dlist_t *node);

int dlist_walk(dlist_t *root, void (*fun)(void *));
};

#endif  //__DLIST_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
