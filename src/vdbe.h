/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Header file for the Virtual DataBase Engine (VDBE)
**
** This header defines the interface to the virtual database engine
** or VDBE.  The VDBE implements an abstract machine that runs a
** simple program to access and modify the underlying database.
**
** $Id: vdbe.h,v 1.55 2002/06/14 22:38:43 drh Exp $
*/

/**
 * @file vdbe.h
 * @brief vbde引擎头文件
 */
#ifndef _SQLITE_VDBE_H_
#define _SQLITE_VDBE_H_
#include <stdio.h>

/*
** A single VDBE is an opaque structure named "Vdbe".  Only routines
** in the source file sqliteVdbe.c are allowed to see the insides
** of this structure.
*/
/**
 * @brief VDBE结构体的前向声明
 * 这是VDBE结构体的前向声明，用于在其他地方引用Vdbe类型而不需要知道其具体实现
 * @note sqliteVdbe.c中的代码被允许看到内部真实实现
 */
typedef struct Vdbe Vdbe;

/*
** A single instruction of the virtual machine has an opcode
** and as many as three operands.  The instruction is recorded
** as an instance of the following structure:
*/

/*
** @brief VDBE指令结构体
** 每条虚拟机指令包含一个操作码和最多三个操作数
** @note p3type字段用于指示p3的类型，可以是静态字符串、动态字符串或指针
**       opcode字段表示指令的操作码
**       p1和p2是操作数，p3是第三个操作数
**       p1和p2通常用于表示跳转目标或其他操作数
*/
struct VdbeOp {
  int opcode;         /* What operation to perform */ // 操作码
  int p1;             /* First operand */ // 第一个操作数
  int p2;             /* Second parameter (often the jump destination) */ // 第二个操作数，通常用于表示跳转目标
  char *p3;           /* Third parameter */ // 第三个操作数
  int p3type;         /* P3_STATIC, P3_DYNAMIC or P3_POINTER */ // p3的类型，静态，动态或指针
};
typedef struct VdbeOp VdbeOp;

/*
** Allowed values of VdbeOp.p3type
** @note P3_NOTUSED表示p3未使用
**       P3_DYNAMIC表示p3是从sqliteMalloc()获取的字符串指针
**       P3_STATIC表示p3是静态字符串指针
**       P3_POINTER表示p3是指向某个结构体或对象的指针
*/
#define P3_NOTUSED    0   /* The P3 parameter is not used */
#define P3_DYNAMIC    1   /* Pointer to a string obtained from sqliteMalloc() */
#define P3_STATIC   (-1)  /* Pointer to a static string */
#define P3_POINTER  (-2)  /* P3 is a pointer to some structure or object */

/*
** The following macro converts a relative address in the p2 field
** of a VdbeOp structure into a negative number so that 
** sqliteVdbeAddOpList() knows that the address is relative.  Calling
** the macro again restores the address.
** @note 该宏用于将p2字段中的相对地址转换为负数，以便sqliteVdbeAddOpList()知道地址是相对的
**       再次调用该宏可以恢复地址
*/
#define ADDR(X)  (-1-(X))

/*
** These are the available opcodes.
**
** If any of the values changes or if opcodes are added or removed,
** be sure to also update the zOpName[] array in sqliteVdbe.c to
** mirror the change.
**
** The source tree contains an AWK script named renumberOps.awk that
** can be used to renumber these opcodes when new opcodes are inserted.
** 
** @note 如果这些值发生变化，或者添加或删除了操作码，请确保同时更新sqliteVdbe.c中的zOpName[]数组以反映更改
**       否则可能导致虚拟机指令与操作码名称不匹配
** 源代码中包含一个名为renumberOps.awk的AWK脚本，可用于在插入新操作码时重新编号这些操作码
*/
#define OP_Transaction         1 // 事务操作
#define OP_Checkpoint          2 // 检查点操作
#define OP_Commit              3 // 提交事务
#define OP_Rollback            4 // 回滚事务

#define OP_ReadCookie          5 // 读取cookie
#define OP_SetCookie           6 // 设置cookie
#define OP_VerifyCookie        7 // 验证cookie

#define OP_Open                8 // 打开表
#define OP_OpenTemp            9 // 打开临时表
#define OP_OpenWrite          10 // 以写模式打开表
#define OP_OpenAux            11 // 打开辅助表
#define OP_OpenWrAux          12 // 以写模式打开辅助表
#define OP_Close              13 // 关闭表
#define OP_MoveTo             14 // 移动到指定位置
#define OP_NewRecno           15 // 新建记录号
#define OP_PutIntKey          16 // 插入整数键
#define OP_PutStrKey          17 // 插入字符串键
#define OP_Distinct           18 // 去重
#define OP_Found              19 // 找到记录
#define OP_NotFound           20 // 未找到记录
#define OP_IsUnique           21 // 是否唯一
#define OP_NotExists          22 // 不存在
#define OP_Delete             23 // 删除记录
#define OP_Column             24 // 读取列
#define OP_KeyAsData          25 // 将键作为数据
#define OP_Recno              26 // 获取记录号
#define OP_FullKey            27 // 获取完整键
#define OP_NullRow            28 // 空行
#define OP_Last               29 // 最后一条记录
#define OP_Rewind             30 // 回到第一条记录
#define OP_Next               31 // 移动到下一条记录

#define OP_Destroy            32 // 销毁表
#define OP_Clear              33 // 清空表
#define OP_CreateIndex        34 // 创建索引
#define OP_CreateTable        35 // 创建表
#define OP_IntegrityCk        36 // 完整性检查

#define OP_IdxPut             37 // 插入索引
#define OP_IdxDelete          38 // 删除索引
#define OP_IdxRecno           39 // 获取索引记录号
#define OP_IdxGT              40 // 索引大于
#define OP_IdxGE              41 // 索引大于等于

#define OP_MemLoad            42 // 从内存加载
#define OP_MemStore           43 // 存储到内存

#define OP_ListWrite          44 // 写入列表
#define OP_ListRewind         45 // 列表回绕
#define OP_ListRead           46 // 读取列表
#define OP_ListReset          47 // 重置列表
#define OP_ListPush           48 // 压入列表
#define OP_ListPop            49 // 弹出列表

#define OP_SortPut            50 // 插入排序
#define OP_SortMakeRec        51 // 创建排序记录
#define OP_SortMakeKey        52 // 创建排序键
#define OP_Sort               53 // 排序
#define OP_SortNext           54 // 排序下一个
#define OP_SortCallback       55 // 排序回调
#define OP_SortReset          56 // 重置排序

#define OP_FileOpen           57 // 打开文件
#define OP_FileRead           58 // 读取文件
#define OP_FileColumn         59 // 文件列

#define OP_AggReset           60 // 聚合重置
#define OP_AggFocus           61 // 聚合焦点
#define OP_AggNext            62 // 聚合下一个
#define OP_AggSet             63 // 设置聚合
#define OP_AggGet             64 // 获取聚合
#define OP_AggFunc            65 // 聚合函数
#define OP_AggInit            66 // 初始化聚合
#define OP_AggPush            67 // 压入聚合
#define OP_AggPop             68 // 弹出聚合

#define OP_SetInsert          69 // 插入集合
#define OP_SetFound           70 // 集合找到
#define OP_SetNotFound        71 // 集合未找到
#define OP_SetFirst           72 // 集合第一个
#define OP_SetNext            73 // 集合下一个

#define OP_MakeRecord         74 // 创建记录
#define OP_MakeKey            75 // 创建键
#define OP_MakeIdxKey         76 // 创建索引键
#define OP_IncrKey            77 // 增加键

#define OP_Goto               78 // 跳转
#define OP_If                 79 // 如果
#define OP_IfNot              80 // 如果不
#define OP_Halt               81 // 停止

#define OP_ColumnCount        82 // 列计数
#define OP_ColumnName         83 // 列名
#define OP_Callback           84 // 回调
#define OP_NullCallback       85 // 空回调

#define OP_Integer            86 // 整数
#define OP_String             87 // 字符串
#define OP_Pop                88 // 弹出
#define OP_Dup                89 // 复制
#define OP_Pull               90 // 拉取
#define OP_Push               91 // 压入
#define OP_MustBeInt          92 // 必须是整数

#define OP_Add                93 // 加
#define OP_AddImm             94 // 增加常数
#define OP_Subtract           95 // 减
#define OP_Multiply           96 // 乘
#define OP_Divide             97 // 除
#define OP_Remainder          98 // 取余
#define OP_BitAnd             99 // 按位与
#define OP_BitOr             100 // 按位或
#define OP_BitNot            101 // 按位非
#define OP_ShiftLeft         102 // 左移
#define OP_ShiftRight        103 // 右移
#define OP_AbsValue          104 // 绝对值
#define OP_Eq                105 // 等于
#define OP_Ne                106 // 不等于
#define OP_Lt                107 // 小于
#define OP_Le                108 // 小于等于
#define OP_Gt                109 // 大于
#define OP_Ge                110 // 大于等于
#define OP_IsNull            111 // 是空
#define OP_NotNull           112 // 非空
#define OP_Negative          113 // 取负
#define OP_And               114 // 与
#define OP_Or                115 // 或
#define OP_Not               116 // 非
#define OP_Concat            117 // 连接
#define OP_Noop              118 // 无操作
#define OP_Function          119 // 函数

#define OP_Limit             120 // 限制
#define OP_LimitCk           121 // 检查限制


#define OP_MAX               121 // 最大数

/*
** Prototypes for the VDBE interface.  See comments on the implementation
** for a description of what each of these routines does.
*/
// VDBE 接口函数原型
/**
 * @brief 创建一个新的 VDBE 实例
 * 
 * @param db 数据库连接对象
 * @return Vdbe* 返回创建的 VDBE 实例
 * 
 */
Vdbe *sqliteVdbeCreate(sqlite*);

/**
 * @brief 设置 VDBE 的回调函数
 * 
 * @param vdbe VDBE 实例
 * @param callback 回调函数指针
 * 
 * 
 */
void sqliteVdbeCreateCallback(Vdbe*, int*);
int sqliteVdbeAddOp(Vdbe*,int,int,int);
int sqliteVdbeAddOpList(Vdbe*, int nOp, VdbeOp const *aOp);
void sqliteVdbeChangeP1(Vdbe*, int addr, int P1);
void sqliteVdbeChangeP2(Vdbe*, int addr, int P2);
void sqliteVdbeChangeP3(Vdbe*, int addr, const char *zP1, int N);
void sqliteVdbeDequoteP3(Vdbe*, int addr);
int sqliteVdbeMakeLabel(Vdbe*);
void sqliteVdbeDelete(Vdbe*);
int sqliteVdbeOpcode(const char *zName);
int sqliteVdbeExec(Vdbe*,sqlite_callback,void*,char**,void*,
                   int(*)(void*,const char*,int));
int sqliteVdbeList(Vdbe*,sqlite_callback,void*,char**);
void sqliteVdbeResolveLabel(Vdbe*, int);
int sqliteVdbeCurrentAddr(Vdbe*);
void sqliteVdbeTrace(Vdbe*,FILE*);
void sqliteVdbeCompressSpace(Vdbe*,int);

#endif
