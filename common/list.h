#ifndef _LIST_H_
#define _LIST_H_

// 链表头结构
struct list_head{
	struct list_head * next, * prev;
};
#define list_for_each(pos, head) \
	for(pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) \
	for(pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

// 根据节点中的一个成员在节点中的偏移量找到节点的起始地址
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

extern void INIT_LIST_HEAD(struct list_head * list);
extern void list_add(struct list_head * nnew, struct list_head * head);
extern void list_del(struct list_head * entry);
#endif
