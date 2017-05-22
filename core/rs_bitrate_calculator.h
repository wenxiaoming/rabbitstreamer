/*
  Copyright (c) 2016-2017 Kevin Wen <listream@126.com>
*/

#ifndef CORE_RS_BITRATE_CALCULATOR_H_
#define CORE_RS_BITRATE_CALCULATOR_H_

#include <string>
#include <stdint.h>

using namespace std;

class RsBitrateCalculator {
public:
    RsBitrateCalculator(string name);
    virtual ~RsBitrateCalculator();
public:
    void update_buffersize(uint32_t size);
private:
    string calculator_name;
    uint32_t recv_buffer_size;
    uint32_t last_recv_msec;
};

#endif /* CORE_RS_BITRATE_CALCULATOR_H_ */
