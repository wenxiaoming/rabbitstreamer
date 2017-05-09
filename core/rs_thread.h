/*
The MIT License (MIT)

Copyright (c) 2016-2017 RabbitStreamer

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

#ifndef RS_THREAD_H
#define RS_THREAD_H

#include <st.h>
#include <string>

using namespace std;

//every thread must inherit from this class to support actual threading operation
class RsThread
{
public:
    RsThread();
    virtual ~RsThread();
public:
    int run(string name);
    void stop();
    void stop_wait();
public:
    //inherited class must implement those functions
    virtual int thread_start() = 0;
    virtual int before_loop() = 0;
    virtual int loop() = 0;
    virtual int end_loop() = 0;
    virtual int thread_stop() = 0;
private:
    static void* thread_loop(void* user);

    st_thread_t tid;
    int context_id;

    bool can_run;
    bool joinable;
    string thread_name;
public:
    bool loop_flag;
    int64_t loop_interval_us;
};

#endif
