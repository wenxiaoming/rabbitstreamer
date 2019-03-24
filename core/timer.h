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
#ifndef CORE_TIMER_H_
#define CORE_TIMER_H_
#include <vector>
#include <stdint.h>
#include "thread.h"

using namespace std;

namespace core {

class ITimerHandler
{
public:
    ITimerHandler()
    {
    }
    virtual ~ITimerHandler()
    {
    }

public:
    virtual int handle_timeout(int64_t timerid) = 0;
};

typedef struct timer_item
{
    int64_t timeout;
    int64_t timerid;
    int64_t last_signal_time;
    ITimerHandler* callback;
    bool operator==(const timer_item& rhs)
    {
        if((callback==rhs.callback)&&(timeout==rhs.timeout)
            &&(timerid==rhs.timerid))
            return true;
        return false;
    }
}timer_item;

class RsTimer : public RsThread
{
private:
    static RsTimer* p;
public:
    RsTimer();
    virtual ~RsTimer();
public:
    //for single instance
    static RsTimer* instance();
public:
    //implement rs_thread's virtual function
    virtual int on_thread_start();
    virtual int on_before_loop();
    virtual int loop();
    virtual int on_end_loop();
    virtual int on_thread_stop();
public:
    //for others to get timer service
    void add_timer(int64_t timeout, int64_t timerid, ITimerHandler* callback);
    void delete_timer(int64_t timeout, int64_t timerid, ITimerHandler* callback);
private:
    void check_timeout();
private:
    bool thread_start_flag;
    int64_t last_thread_time;
    vector<timer_item> timer_vector;
};

} /* namespace core */

#endif /* CORE_TIMER_H_ */
