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
#ifndef CORE_THREAD_H_
#define CORE_THREAD_H_

#include "st.h"
//every thread must inherit from this class to support actual threading operation
class RsThread
{
public:
	RsThread(const char* name);
	virtual ~RsThread();
public:
	int start_thread();
	void stop_thread();       
	//inherited class must implement those functions
	virtual int on_thread_start() = 0;
	virtual int on_before_loop() = 0;
	virtual int loop() = 0;
	virtual int on_end_loop() = 0;
	virtual int on_thread_stop() = 0;
private:
    virtual void dispose();
    static void* thread_intermediary(void* arg);
    void thread_loop();
public:
    bool loop_flag;
private:
    st_thread_t tid;
    int _cid;

    bool can_run;
    bool really_terminated;
    bool _joinable;
    const char* _name;
    bool disposed;
public:
    int64_t cycle_interval_us;
};

#endif
