/*
The MIT License (MIT)

Copyright (c) 2016-2018 RabbitStreamer

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
#include "timer.h"
#include "error_code.h"
#include "core_utility.h"
#include <st.h>
#include <assert.h>
#include <algorithm>

namespace rs {
namespace core {

#define SYS_TIMER_LOOP_INTERVAL 500 //sleep for 500ms

RsTimer* RsTimer::p = new RsTimer;
RsTimer* RsTimer::instance()
{
    return p;
}

RsTimer::RsTimer()
        : RsThread("rstimer")
{
    last_thread_time = 0;
    thread_start_flag = false;
}

RsTimer::~RsTimer()
{

}

int RsTimer::on_end_loop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsTimer::on_thread_stop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsTimer::on_thread_start()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsTimer::on_before_loop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsTimer::loop()
{
    int ret = ERROR_SUCCESS;
    check_timeout();
    st_usleep(SYS_TIMER_LOOP_INTERVAL * 1000);
    return ret;
}
void RsTimer::add_timer(int64_t timeout, int64_t timerid, ITimerHandler* callback)
{
    assert(callback!=NULL);
    if(!thread_start_flag)
    {
        start_thread();
        thread_start_flag = true;
    }
    timer_item item;
    item.timeout = timeout;
    item.timerid = timerid;
    item.callback = callback;
    item.last_signal_time = 0;
    timer_vector.push_back(item);
}

void RsTimer::delete_timer(int64_t timeout, int64_t timerid, ITimerHandler* callback)
{
    timer_item item;
    item.timeout = timeout;
    item.timerid = timerid;
    item.callback = callback;
    timer_vector.erase(remove(timer_vector.begin(), timer_vector.end(), item));
}

void RsTimer::check_timeout()
{
    int64_t curr = get_system_time_ms();

    vector<timer_item>::iterator iter = timer_vector.begin();
    //init the first signal time
    if(last_thread_time == 0)
    {
        last_thread_time = curr;
        for(; iter < timer_vector.end(); iter++)
        {
            iter->last_signal_time = curr;
        }
        return;
    }
    int64_t delta = 0;
    //iterate the vector to trigger the timer's callback
    iter = timer_vector.begin();
    for(; iter < timer_vector.end(); iter++)
    {
       int64_t timeout = iter->timeout;
       delta = curr-iter->last_signal_time;
       if(delta >= timeout)
       {
           (iter->callback)->handle_timeout(iter->timerid);
           iter->last_signal_time = curr;
       }
    }
    return;
}

} } // namespace rs::core