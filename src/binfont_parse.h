
#ifndef _BINFONT_PARSE_H_
#define _BINFONT_PARSE_H_

#include "node_list.h"

#ifndef NULL
#define NULL (void*(0))
#endif

#ifndef FAIL
#define FAIL -1
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#define HEADSTR_LEN  (16)

#define UNICODENAME_MAXLEN  128

#define CHARSET_INFO_MAXLEN 128


#if defined (__cplusplus)
 extern "C" 
 {
#endif

typedef struct _charset_info
{
    LIST_NODE_S node;                  //链表节点
    sint16 identify_index;               //用来标示字符集索引
    sint16 width;                           //字符宽度
    sint16 height;                          //字符高度
    sint16 char_num;                    //字符个数
    uint32 start_addr;                   //文件地址起始
    uint32 end_addr;                     //文件地址结束
    uint16 ucode_start;                 //unicode 起始
    uint16 ucode_end;                   //unicode 结束
    uint8 *pdraw_width;                  //字符绘制宽度
    uint8 *pshape_info;                   //字符形状数据

}CHARSET_INFO_S,*PCHARSET_INFO_S;

typedef struct _fontfile_info
{
    
    sint8 bad;                                //是否读取成功
    sint8 head_str[HEADSTR_LEN];    //字库头
    sint32 file_size;                       //文件数据大小
    sint32 chrset_num;                  //字符集数目
    PCHARSET_INFO_S pcharset_info ;
}FONTFILE_INFO_S,*PFONTFILE_INFO_S;

typedef struct
{
    uint32 start_index;
    uint32 end_index;
    sint8 charset_name[UNICODENAME_MAXLEN];
}UNICODE_TABLE_S;

extern const UNICODE_TABLE_S unicode_table[];

/***************************************************************
*Description:
   读取并保存unicode 字库文档信息
*Input:
    file_name --- 字库文件路径及名称
*Output:
    fontfile_info---字库信息
*Return:
    <-1 ---失败     其他---成功
****************************************************************/
sint32  fontfile_parse(char *file_name, FONTFILE_INFO_S *fontfile_info);

/***************************************************************
*Description:
    解析结束，释放掉申请的内存
*Input:
    fontfile_info---字库信息
*Output:
    无
*Return:
    <-1 ---失败     其他---成功
****************************************************************/
sint32 fontfile_parse_release(FONTFILE_INFO_S *fontfile_info);

/***************************************************************
*Description:
    打印unicode点阵字库相关信息
*Input:
    fontfile_info ---字库文件信息
*Output:
    无
*Return:
    无
****************************************************************/
void fontfile_show_info(FONTFILE_INFO_S *fontfile_info);

/***************************************************************
*Description:
    打印unicode点阵字库相关信息
*Input:
    fontfile_info ---字库文件信息
*Output:
    无
*Return:
    无
****************************************************************/
void fontfile_show_charshape(FONTFILE_INFO_S *fontfile_info, uint32 ucode_index);

/***************************************************************
*Description:
   读取并保存unicode 字库文档信息, 只适用与16*16  24*24 32*32
*Input:
    dir_path --- 保存路径
*Output:
    无
*Return:
    <0 ---失败     其他---成功
****************************************************************/
sint32  fontfile_save(char* dir_path, PFONTFILE_INFO_S font_file_info);

/***************************************************************
*Description:
   读取并保存unicode 字库文档信息, 只适用与16*16  24*24 32*32
*Input:
    file_name --- 带路径的文件名
    font_file_info --- 字库信息
*Output:
    无
*Return:
    <0 ---失败     其他---成功
****************************************************************/
sint32  fontfile_save_by_name(char* file_name, PFONTFILE_INFO_S font_file_info);

/***************************************************************
*Description:
    一个字符集节点的信息
    [1 ]: 24x24,   0x0000-0x0080，128 char,  9216B, C0控制符及基本拉丁文
*Input:
    fontfile_info ---一个字符集节点
    info --- 打印标示，显示在打印信息开头
*Output:
    info ---信息字符串，128字节就够了
*Return:
    无
****************************************************************/
void fontfile_get_one_charset_info(PCHARSET_INFO_S charset_info, sint8* info);

/***************************************************************
*Description:
   打印一个字符集节点信息
*Input:
    fontfile_info ---字符集节点
    index --- 打印标示，显示在打印信息开头
*Output:
    无
*Return:
    无
****************************************************************/
void fontfile_show_one_charset_info(PCHARSET_INFO_S charset_info);

 /***************************************************************
*Description:
    打印一个字符集链表
*Input:
    plist_head ---字符集链表头
*Output:
    无
*Return:
    无
****************************************************************/
void fontfile_list_show_list_info(PCHARSET_INFO_S plist_head);

/***************************************************************
*Description:
    根据链表索引，获取一个新的节点，新申请内存
*Input:
    plist_head --- 字符集链表
    index --- 索引
    identify_code---字库标识码
*Output:
    无
*Return:
    获取出的节点
****************************************************************/
PCHARSET_INFO_S fontfile_list_get_node_by_index(PCHARSET_INFO_S plist_head, sint32 index);

/***************************************************************
*Description:
    在字符集链表后面添加节点,该函数会判断要添加的链表，是否已经添加过
*Input:
    plist_head --- 字符集链表
    node --- 新节点
*Output:
    无
*Return:
    添加后的链表头
****************************************************************/
PCHARSET_INFO_S fontfile_list_add_node_to_list_rear(PCHARSET_INFO_S plist_head, PCHARSET_INFO_S node);

/***************************************************************
*Description:
    根据链表索引，获取一个新的节点，新申请内存
*Input:
    plist_head --- 字符集链表
    index --- 索引
    identify_code---字库标识码
*Output:
    无
*Return:
    <0 ---失败     其他---成功
****************************************************************/
PCHARSET_INFO_S fontfile_list_delet_node_by_index(PCHARSET_INFO_S plist_head, sint32 index);

/***************************************************************
*Description:
    获取链表中节点的数目
*Input:
    plist_head --- 字符集链表
*Output:
    无
*Return:
    list_count --- 节点数目
****************************************************************/
uint32 fontfile_list_get_node_count(PCHARSET_INFO_S plist_head);

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
PCHARSET_INFO_S fontfile_list_combo_two_lists(PCHARSET_INFO_S plist_head1, PCHARSET_INFO_S plist_head2);

/***************************************************************
*Description:
    复制字符集链表，会申请新内存
*Input:
    plist_head---源链表头
*Output:
    无
*Return:
    新创建的链表头
****************************************************************/
PCHARSET_INFO_S  fontfile_list_copy(PCHARSET_INFO_S plist_head);

/***************************************************************
*Description:
    根据nodeid,将链表进行排序
*Input:
    plist_head---源链表头
*Output:
    无
*Return:
    排序后的链表头
****************************************************************/
PCHARSET_INFO_S  fontfile_list_sort_by_nodeid(PCHARSET_INFO_S plist_head);

/***************************************************************
*Description:
   将字符集链表进行排序，合并相同区间的字符集
*Input:
    plist_info ---字符集链表
*Output:
    无
*Return:
    处理后的字符集链表头
****************************************************************/
PCHARSET_INFO_S  fontfile_list_filter_charset_info(PCHARSET_INFO_S plist_head);

/***************************************************************
*Description:
    释放字符集链表
*Input:
    plist_head --- 字符文件指针
*Output:
    无
*Return:
    <0 ---失败     其他---成功
****************************************************************/
sint32 fontfile_list_release(PCHARSET_INFO_S plist_head);

/***************************************************************
*Description:
    通过字符集链表创建，字符集文件描述结构体。
    用于字符文件的裁剪和合并。
*Input:
    plist_info --- 排序后的字符集结构体链表
*Output:
    pfont_file_info --- 字符文件指针
*Return:
    <0 ---失败     其他---成功
****************************************************************/
sint32  fontfile_list_create_font_info(PFONTFILE_INFO_S pfont_file_info, PCHARSET_INFO_S plist_info);

#if defined (__cplusplus)
}
#endif

#endif

