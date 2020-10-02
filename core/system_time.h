/*
The MIT License (MIT)

Copyright (c) 2016-2020 RabbitStreamer

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef CORE_SYSTEM_TIME_H
#define CORE_SYSTEM_TIME_H

#include <stdint.h>
#include <vector>

namespace rs {
namespace core {

class SystemTime {
public:
    SystemTime();
    virtual ~SystemTime();

public:
    // for single instance
    static SystemTime *instance();

public:
    int64_t get_system_time_ms();
    int64_t get_system_startup_time_ms();
    int64_t update_system_time_ms();

private:
    static SystemTime *p;

private:
    int64_t system_time_us_cache = 0;
    int64_t system_time_startup_time = 0;
};

} // namespace core
} // namespace rs

#endif