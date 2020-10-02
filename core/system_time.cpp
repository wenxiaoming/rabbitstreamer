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

#include "system_time.h"
#include <stdio.h>
#include <sys/time.h>

#define rs_min(a, b) (((a) < (b)) ? (a) : (b))
#define rs_max(a, b) (((a) < (b)) ? (b) : (a))

namespace rs {
namespace core {

SystemTime *SystemTime::p = new SystemTime;
SystemTime *SystemTime::instance() { return p; }

constexpr uint64_t kSystemTime_Resolution_us = 300 * 1000;

SystemTime::SystemTime() {

}

SystemTime::~SystemTime() {}

int64_t SystemTime::update_system_time_ms() {
    timeval now;

    if (gettimeofday(&now, NULL) < 0) {
        return -1;
    }

    int64_t now_us = ((int64_t)now.tv_sec) * 1000 * 1000 + (int64_t)now.tv_usec;

    if (system_time_us_cache <= 0) {
        system_time_us_cache = now_us;
        system_time_startup_time = now_us;
        return system_time_us_cache / 1000;
    }

    // use relative time.
    int64_t diff = now_us - system_time_us_cache;
    diff = rs_max(0, diff);
    if (diff < 0 || diff > 1000 * kSystemTime_Resolution_us) {
        system_time_startup_time += diff;
    }

    system_time_us_cache = now_us;

    return system_time_us_cache / 1000;
}

int64_t SystemTime::get_system_time_ms() {
    if (system_time_us_cache <= 0) {
        update_system_time_ms();
    }

    return system_time_us_cache / 1000;
}

int64_t SystemTime::get_system_startup_time_ms() {
    if (system_time_startup_time <= 0) {
        update_system_time_ms();
    }

    return system_time_startup_time / 1000;
}

} // namespace core
} // namespace rs