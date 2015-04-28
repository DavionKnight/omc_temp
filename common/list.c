#include <stdio.h>
#include "list.h"

// 链表头初始化
void INIT_LIST_HEAD(struct list_head * list)
{
	list->next = list;
	list->prev = list;
}
// 真正实现链表插入操作
void _list_add(struct list_head * nnew, struct list_head * prev, struct list_head * next)
{
	next->prev = nnew;
	nnew->next = next;
	nnew->prev = prev;
	prev->next = nnew;
}
// 向链表插入一个节点
void list_add(struct list_head * nnew, struct list_head * head)
{
	_list_add(nnew, head, head->next);
}

// 真正实现链表删除操作
void _list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}
// 删除链表中的一个节点
void list_del(struct list_head * entry)
{
	_list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}
