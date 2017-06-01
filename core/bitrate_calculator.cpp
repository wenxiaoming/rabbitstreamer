/*
  Copyright (c) 2016-2017 Kevin Wen <listream@126.com>
*/

#include "rs_bitrate_calculator.h"
#include "rs_logger.h"
#include <sys/time.h>

RsBitrateCalculator::RsBitrateCalculator(string name)
{
    calculator_name = name;
    recv_buffer_size = 0;
    last_recv_msec = 0;
}

RsBitrateCalculator::~RsBitrateCalculator()
{

}

void RsBitrateCalculator::update_buffersize(uint32_t size)
{
    timeval tv;
    recv_buffer_size += size;
    if (gettimeofday(&tv, NULL) == -1) {
        return;
    }

    uint32_t curr_ms = tv.tv_sec*1000 + tv.tv_usec/1000;
    uint32_t delta_ms = curr_ms - last_recv_msec;
    if(delta_ms > 1000)
    {
        RSLOGI("%s bitrate:%d kbps", calculator_name.c_str(), 1000*recv_buffer_size/(1024*delta_ms));
        recv_buffer_size = 0;
        last_recv_msec = curr_ms;
    }
}
