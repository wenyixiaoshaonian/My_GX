#ifndef _MGX_MYSQL_H_
#define _MGX_MYSQL_H_
 
#include<iostream>
#include<string>
#include"mysql.h"

using namespace std;
extern const int MAX_SQL_SENTENCE_LENGTH;
 
 
class Mgx_mysql
{
public:
	Mgx_mysql();
	~Mgx_mysql();
	//连接数据库 参数为ip 用户名 密码 数据库名 端口
	bool connect(const char* HOST, const char* USER, const char* PASSWORD, const char* DATABASE_NAME, const int PORT);
	// 封装后的操作函数
	template<class ... Args>
	void excute_sql(const char* format, Args ... args);  // 可直接执行sql语句
	template<class ... Args>
	float get_one_request_of_float(const char* format, Args ... args);  // 从数据库获取单个数据(int)
	template<class ... Args>
	char* get_one_request_of_str(const char* format, Args ... args);  // 从数据库获取单个数据(str)
	template<class ... Args>
	MYSQL_ROW get_row_request_of_str(const char* format, Args ... args);  // 从数据库获取整行数据(str)
	bool display__res();  // 打印结果集
	bool query_table(const char* table_name);  	//查询并打印表 参数为表名 这个函数不能使用别名 便捷的打印表的数据(可用来测试) 直接输入表的名称
 
	// 初步封装的函数
	int get__res_field_num();  	//获取结果集的字段数
	int get__res_row_num();  // 返回结果集合中的行的数量 查询集为空返回0
	MYSQL_RES* query();  // 执行查询语句 返回查询集 底层操作函数(不直接调用)
	int get_tag_id(const char*);  // 获取表的最后一行的id+1 (新行的id)
 
 
 
protected:
	bool _state;  //连接状态 true为已连接
	MYSQL* _mysql;  //mysql连接  
	MYSQL_RES* _res;  //这个结构代表返回行的一个查询结果集  
	MYSQL_ROW _column;  //一个行数据的类型安全(type-safe)的表示，表示数据行的列  
	char* _query;  //查询语句 有的查询语句过长会导致内存不够 分配大一点
	int excute_status;  // sql语句执行状态 成功、失败
	int _res_rows_num;  // 结果集合中的行的数量 为0说明结果集为空
	int field_num;  // 结果集中字段数
	string exit_params;  // 退出操作参数
};
 
 
// 封装的执行sql语句 执行成功返回1 失败返回0
/*
	使用方法：
		excute_sql("UPDATE app_produce_data SET is_delete = 1 WHERE id = %d", produce_id)直接输入sql语句
*/
template<class ... Args>
void Mgx_mysql::excute_sql(const char* format, Args ... args) {
	excute_status = 0;  // 初始化状态参数
	_res_rows_num = 0;  // 初始化结果集数据行数
	field_num = 0;
	sprintf(_query, format, args...);
	if (query()) {
		excute_status = 1;  // sql语句执行成功 excute_status = 1
		_res_rows_num = get__res_row_num();  // 结果集数据行数
		field_num = get__res_field_num();
	}
	else {
		excute_status = 0;
		_res_rows_num = 0;
		field_num = 0;
	}
}
 
/*
	使用方法：
		get_one_request_of_int("SELECT produce_price FROM app_produce_data WHERE id='%d'", produce_id)直接输入sql语句
*/
// 从数据库获取单个数据(int)
template<class ... Args>
float Mgx_mysql::get_one_request_of_float(const char* format, Args ... args) {
	excute_sql(format, args...);
	_column = mysql_fetch_row(_res);  // 获取第一行
	//cout << atoi(_column[0]);
	return atof(_column[0]);  // 返回值
}
 
/*
	使用方法：
		get_one_request_of_str("SELECT produce_name FROM app_produce_data WHERE id = %d", produce_id), produce_id)直接输入sql语句
*/
// 从数据库获取单个数据(str)
template<class ... Args>
char* Mgx_mysql::get_one_request_of_str(const char* format, Args ... args) {
	excute_sql(format, args...);
	_column = mysql_fetch_row(_res);  // 获取第一行
	//cout << _column[0];
	return _column[0];  // 返回值
}
 
 
// 从数据库获取整行数据(str)
template<class ... Args>
MYSQL_ROW Mgx_mysql::get_row_request_of_str(const char* format, Args ... args) {
	excute_sql(format, args...);
	_column = mysql_fetch_row(_res);  // 获取第一行
	//cout << _column[0];
	return _column;  // 返回值
}
#endif // _Mgx_mysql_H_