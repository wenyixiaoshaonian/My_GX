#include "mgx_redis.h"

Mgx_Redis::Mgx_Redis()
{
    if(!connect("127.0.0.1", 6379))
	{
        mgx_log(MGX_LOG_STDERR, "connect error!\n");
	}
}

//todo....