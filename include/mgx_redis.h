#ifndef _MGX_REDIS_H_
#define _MGX_REDIS_H_
 
#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include "mgx_log.h"

#include <hiredis/hiredis.h>

class Mgx_Redis
{
public:
 
    Mgx_Redis();
 //释放资源
    ~Mgx_Redis()
	{
        this->_connect = NULL;
		this->_reply = NULL;	    	    
	}
 //创建连接
	bool connect(std::string host, int port)
	{
	    this->_connect = redisConnect(host.c_str(), port);
		if(this->_connect != NULL && this->_connect->err)
		{
            mgx_log(MGX_LOG_STDERR, "connect error: %s\n", this->_connect->errstr);
			return 0;
		}
		return 1;
	}
 //get请求
    std::string get(std::string key)
	{
		this->_reply = (redisReply*)redisCommand(this->_connect, "GET %s", key.c_str());
		std::string str = this->_reply->str;
		freeReplyObject(this->_reply);
		mgx_log(MGX_LOG_NOTICE, "Get the name is %s\n", str.c_str());
		return str;
	}
//set请求
	void set(std::string key, std::string value)
	{
        redisCommand(this->_connect, "SET %s %s", key.c_str(), value.c_str());
	}
 
private:
    redisContext* _connect;
	redisReply* _reply;
				
};
 
#endif  //_REDIS_H_