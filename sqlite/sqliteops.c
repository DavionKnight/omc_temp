/********************  COPYRIGHT(C)***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: sqliteops.c
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: sqlite数据库操作文件
**--------------历史版本信息----------------------------------------------------
** 创建人: 于宏图
** 版  本:
** 日　期:
** 描　述: 原始版本
**--------------当前版本修订----------------------------------------------------
** 修改人:
** 版  本:
** 日　期:
** 描　述:
**------------------------------------------------------------------------------
**
*******************************************************************************/

#include "sqliteops.h"

pthread_mutex_t mutex_save_database;//存储数据库锁
sqlite3 *db = NULL; //声明sqlite关键结构指针

/*******************************************************************************
*函数名称 : void SqliteInit(void)
*功    能 : 数据库控制操作初始化函数
*输入参数 : None
*输出参数 : None
*******************************************************************************/
void SqliteInit(void)
{
  pthread_mutex_init(&mutex_save_database, NULL);//互斥锁初始化
}

/*******************************************************************************
*函数名称 : void SqliteExit(void)
*功    能 : 数据库控制操作退出操作函数
*输入参数 : None
*输出参数 : None
*******************************************************************************/
void SqliteExit(void)
{
  pthread_mutex_destroy(&mutex_save_database);
}

/*******************************************************************************
*函数名称 : int SqliteOpen(const char *dbName)
*功    能 : 打开数据库
*输入参数 : 打开的数据库名称
*输出参数 : SQLITE_OK:打开成功;-1：打开失败
*******************************************************************************/
int SqliteOpen(const char *dbName)
{
int result;

  result = sqlite3_open(dbName, &db);
  if (result != SQLITE_OK)
  {
    printf("数据库打开失败.\n");//数据库打开失败
  }
  return result;
}

/*******************************************************************************
*函数名称 : void SqliteClose(void)
*功    能 : 关闭数据库
*输入参数 : none
*输出参数 : none
*******************************************************************************/
void SqliteClose(void)
{
  sqlite3_close(db);
  printf("close Sqlite.\n");//数据库关闭
}

/*******************************************************************************
*函数名称 : int SqliteSelect(const char *sql, SqlResult_t *prs)
*功    能 : 查询数据库
*输入参数 : sql：查询数据库SQL语句；*prs:返回的结果集指针
*输出参数 : SQLITE_OK：执行成功;否则，执行失败
*******************************************************************************/
int SqliteSelect(const char *sql, SqlResult_t *prs)
{
int result;

  result = sqlite3_get_table(db, sql, &prs->dbResult, &prs->nRow, &prs->nColumn, &prs->errmsg);
  if (result != SQLITE_OK)
  {
    printf("查询记录失败,错误码:%d,错误原因:%s\n", result, prs->errmsg);
  }
  return result;
}

/*******************************************************************************
*函数名称 : int SqliteUpdate(const char *sql)
*功    能 : 更新数据库
*输入参数 : sql：更新数据库SQL语句
*输出参数 : SQLITE_OK：执行成功;否则，执行失败
*******************************************************************************/
int SqliteUpdate(const char *sql)
{
int result;
char *errmsg = NULL;

  pthread_mutex_lock(&mutex_save_database);
  result = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (result != SQLITE_OK)
  {
    printf("更新记录失败,错误码:%d,错误原因:%s\n", result, errmsg);
  }
  pthread_mutex_unlock(&mutex_save_database);
  return result;
}

/*******************************************************************************
*函数名称 : int SqliteInsert(const char *sql)
*功    能 : 插入数据库
*输入参数 : sql：插入数据库SQL语句
*输出参数 : SQLITE_OK：执行成功;否则，执行失败
*******************************************************************************/
int SqliteInsert(const char *sql)
{
int result;
char *errmsg = NULL;

  pthread_mutex_lock(&mutex_save_database);
  result = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (result != SQLITE_OK)
  {
    printf("插入记录失败,错误码:%d,错误原因:%s\n", result, errmsg);
  }
  pthread_mutex_unlock(&mutex_save_database);
  return result;
}

/*******************************************************************************
*函数名称 : int SqliteDelete(const char *sql)
*功    能 : 删除数据库
*输入参数 : sql：删除数据库SQL语句
*输出参数 : SQLITE_OK：执行成功;否则，执行失败
*******************************************************************************/
int SqliteDelete(const char *sql)
{
int result;
char *errmsg = NULL;

  pthread_mutex_lock(&mutex_save_database);
  result = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (result != SQLITE_OK)
  {
    printf("删除记录失败,错误码:%d,错误原因:%s\n", result, errmsg);
  }
  pthread_mutex_unlock(&mutex_save_database);
  return result;
}

/*******************************************************************************
*函数名称 : int SqliteCreate(const char *sql)
*功    能 : 创建数据库表
*输入参数 : sql：创建数据库表SQL语句
*输出参数 : SQLITE_OK：执行成功;否则，执行失败
*******************************************************************************/
int SqliteCreate(const char *sql)
{
int result;
char *errmsg = NULL;

  pthread_mutex_lock(&mutex_save_database);
  result = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (result != SQLITE_OK)
  {
    printf("创建数据表失败,错误码:%d,错误原因:%s\n", result, errmsg);
  }
  pthread_mutex_unlock(&mutex_save_database);
  return result;
}

/*******************************************************************************
*函数名称 : int SqliteTransaction(const char *sql)
*功    能 : 数据库的事务处理
*输入参数 : sql：数据库的事务处理SQL语句
*输出参数 : SQLITE_OK：执行成功;否则，执行失败
*******************************************************************************/
int SqliteTransaction(const char *sql)
{
int result;
char *errmsg = NULL;

  return SQLITE_OK;

  pthread_mutex_lock(&mutex_save_database);
  result = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
  if (result != SQLITE_OK)
  {
    printf("事务处理失败,错误码:%d,错误原因:%s\n", result, errmsg);
  }
  pthread_mutex_unlock(&mutex_save_database);
  return result;
}
/*******************************************************************************/
