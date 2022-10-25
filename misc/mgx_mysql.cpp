#include "mgx_mysql.h"
#include <sstream>
#include <string.h>
#include "mgx_log.h"

const int MAX_SQL_SENTENCE_LENGTH = 1000;
 
 
// 数据库操作封装
/*
    1.void excute_sql(const char* format, Args ... args)
        1.获取sql语句执行状态 接收字符串执行sql语句 sql语句执行成功excute_status = 1 失败excute_status = 0
        2.获取结果集数据行数 _res_rows_num
    2.bool display__res(int field_num)  1.打印查询集
*/
 
// 构造函数
Mgx_mysql::Mgx_mysql()
{
    _state = false;  // 连接状态
    _mysql = new MYSQL;
    _query = new char[MAX_SQL_SENTENCE_LENGTH];  // 分配内存
    _res = nullptr;  // 初始化结果集
    _column = nullptr;  // 初始化结果行
    memset(_query, NULL, sizeof(_query));  // 初始化查询语句
    connect("localhost", "root", "root", "db1", 3306);  	//连接数据库 参数为ip、用户名、密码、数据库名、端口
    _res_rows_num = 0;  // 结果集合中的行的数量 为0说明结果集为空
    field_num = 0;  // 结果集中字段数
    exit_params = "0";  // 退出操作参数
}
// 析构函数
Mgx_mysql::~Mgx_mysql()
{
}
 
// 连接数据库
bool Mgx_mysql::connect(const char* HOST, const char* USER, const char* PASSWORD, const char* DATABASE_NAME, const int PORT)
{
    if (_state == true)
    {
        mgx_log(MGX_LOG_NOTICE, "Connected mysql succeed !");
        return true;
    }
    //初始化mysql  
    mysql_init(_mysql);
    //返回false则连接失败，返回true则连接成功  
    if (!(mysql_real_connect(_mysql, HOST, USER, PASSWORD, DATABASE_NAME, PORT, NULL, 0)))  // 主机用户名、密码、数据库名、端口号
    {
        printf("Error connecting to database:%s\n", mysql_error(_mysql));
        mgx_log(MGX_LOG_STDERR, "Error connecting to database:%s\n", mysql_error(_mysql));
        return false;
    }
    else
    {
        _state = true;
        return true;
    }
}
 
// 封装查询语句 返回结果集  执行失败返回nullptr
MYSQL_RES* Mgx_mysql::query() {
    MYSQL_RES* error = nullptr;
    if (_state == false)  // 判断数据库是否连接
    {
        mgx_log(MGX_LOG_STDERR, "Database not connected !");
        return error;
    }

    //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码 
    mysql_query(_mysql, "set names gbk");
    
 
    // 获取查询结果 并进行查询结果判断
    if (mysql_query(_mysql, _query))    //执行SQL语句
    {
        printf("Query failed (%s)\n", mysql_error(_mysql));
        return error;
    }
    else
    {
        //printf("query success\n");
    }
    //获取结果集  
    if (!(_res = mysql_store_result(_mysql)))   //获得sql语句结束后返回的结果集  
    {
        //printf("Couldn't get result from %s\n", mysql_error(_mysql));
        //return error;
    }
    if (_res == nullptr) {
        _res = new MYSQL_RES;  // 区分 插入、更新数据时无返回结果集导致无法sql语句判断是否执行成功 不让_res == error
    }
    return _res;
}
 
// 打印结果集
bool Mgx_mysql::display__res() {
 
    char* str_field[MAX_SQL_SENTENCE_LENGTH];  // 字段名
    for (int i = 0; i < field_num; i++)  //在获得字段数量的情况下获取字段名  
    {
        str_field[i] = mysql_fetch_field(_res)->name;  // 返回下一个表字段名
    }
    //打印字段名  打印表的第一行字段名
    for (int i = 0; i < field_num; i++)
    {
        mgx_log(MGX_LOG_NOTICE, "%10s\t", str_field[i]);
    }
    //打印获取的数据  
    while (_column = mysql_fetch_row(_res))   //在已知字段数量情况下，获取并打印下一行  无下一行后mysql_fetch_row返回false结束循环
    {
        for (int i = 0; i < field_num; i++)
        {
            mgx_log(MGX_LOG_NOTICE, "%10s\t", _column[i]);      //column是列数组  一行包括多列
        }
        // printf("\n");
    }
    return true;
}
 
// 返回结果集合中的行的数量 查询集为空返回0
int Mgx_mysql::get__res_row_num() {
    return mysql_num_rows(_res);
}
 
// 返回结果集中的字段数
int Mgx_mysql::get__res_field_num() {
    return mysql_field_count(_mysql);
}
 
// 查询表 直接传入表名
bool Mgx_mysql::query_table(const char* table_name)
{
    //查询内容
    if (!strcmp(table_name, "app_produce_data")) {  // 如果为商品信息表 增加是否删除判断
        excute_sql("select * from %s WHERE is_delete = 0", table_name); //执行查询语句 
    }
    else {
        excute_sql("select * from %s", table_name); //执行查询语句 
    }
 
    //打印数据行数  
    mgx_log(MGX_LOG_NOTICE, "number of dataline returned: %lld\n", mysql_affected_rows(_mysql));
    // 打印表数据  
    display__res();
    return true;
}
 
// 获取表的最后一行的id 在此基础上加1 返回新的一行的id  
int Mgx_mysql::get_tag_id(const char* c) {
    //查询内容
    excute_sql("SELECT id FROM %s ORDER BY id DESC LIMIT 1", c);
    _column = mysql_fetch_row(_res);
 
    //printf("%10s\t", _column[0]);  //column是列数组  
    if (_column == nullptr) {
        return 1;
    }
    int row_num = atoi(_column[0]);
    return row_num + 1;
}
