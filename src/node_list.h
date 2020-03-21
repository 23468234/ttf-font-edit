
#ifndef NODE_LIST_H
#define NODE_LIST_H

#if defined (__cplusplus)
 extern "C" 
 {
#endif

 typedef char sint8;
 typedef signed short sint16;
 typedef signed int sint32;
 typedef signed long long sint64;
 typedef unsigned char uint8;
 typedef unsigned short uint16;
 typedef unsigned int uint32;
 typedef unsigned long long uint64;

typedef struct _list_node
{
    uint32 id;
    struct _list_node *pnext;
}LIST_NODE_S, *PLIST_NODE_S; //bmp转换后的字符集信息


/***************************************************************
*Description:
    将一个新的节点添加到链表尾部
*Input:
    plist_head --- 链表头结点
    pnew --- 要添加的节点
*Output:
    无
*Return:
    plist_head --- 排序后的链表头
****************************************************************/
PLIST_NODE_S list_added_to_rear(PLIST_NODE_S plist_head, PLIST_NODE_S pnew);

/***************************************************************
*Description:
    根据节点的id，删除节点
*Input:
    plist_head --- 链表头结点
    node_id---节点的id
*Output:
    无
*Return:
    plist_head---排序后的链表头
****************************************************************/
PLIST_NODE_S list_delete_by_id(PLIST_NODE_S plist_head, uint32 node_id);

/***************************************************************
*Description:
  将两条链表合并成一条.plist_head2链接到plist_head1后面
*Input:
    plist_head1 --- 链表头1
    plist_head2 --- 链表头2
*Output:
    无
*Return:
    plist_head --- 合并后的链表头
****************************************************************/
PLIST_NODE_S list_combo_two_lists(PLIST_NODE_S plist_head1, PLIST_NODE_S plist_head2);

/***************************************************************
*Description:
    根据链的node_id，进行冒泡排序
*Input:
    plist_head --- 链表头
*Output:
    无
*Return:
    plist_head---排序后的链表头
****************************************************************/
PLIST_NODE_S list_sort_by_id(PLIST_NODE_S plist_head);

/***************************************************************
*Description:
    获取该链表节点的数目
*Input:
    plist_head --- 链表头
*Output:
    无
*Return:
    list_count--- 链表节点数
****************************************************************/
uint32 list_get_node_count(PLIST_NODE_S plist_head);

/***************************************************************
*Description:
    释放链表
*Input:
    plist_head --- 链表头
*Output:
    无
*Return:
    无
****************************************************************/
void list_free(PLIST_NODE_S plist_head);

/***************************************************************
*Description:
    打印链表中的所有节点
*Input:
    plist_head --- 链表头结点
*Output:
    无
*Return:
    plist_head---排序后的链表头
****************************************************************/
void  print_list(PLIST_NODE_S plist_head);

#if defined (__cplusplus)
}
#endif

#endif
