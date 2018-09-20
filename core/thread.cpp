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
#include "thread.h"
#include "st.h"
#include "error_code.h"
#include "logger.h"

RsThread::RsThread(const char* name)
{
    _name = name;
    _cid = -1;
    tid = 0;
    cycle_interval_us = 0;
    loop_flag = false;
}

RsThread::~RsThread()
{
    tid = 0;
}

int RsThread::start_thread()
{
    int ret = ERROR_SUCCESS;

    if(tid) {
        RSLOGE("thread %s already running.", _name);
        return ret;
    }

    if((tid = st_thread_create(thread_intermediary, this, (_joinable? 1:0), 0)) == NULL){
        ret = ERROR_ST_CREATE_CYCLE_THREAD;
        RSLOGE("st_thread_create failed. ret=%d", ret);
        return ret;
    }

    disposed = false;
    // we set to loop to true for thread to run.
    loop_flag = true;

    // wait for cid to ready, for parent thread to get the cid.
    while (_cid < 0) {
        st_usleep(10 * 1000);
    }

    // now, cycle thread can run.
    can_run = true;

    return ret;
}

void RsThread::thread_loop()
{
    int ret = ERROR_SUCCESS;


    _cid = 0;//hard code;


    on_thread_start();

    // thread is running now.
    really_terminated = false;

    // wait for cid to ready, for parent thread to get the cid.
    while (!can_run && loop_flag) {
        st_usleep(10 * 1000);
    }

    while (loop_flag) {
        if ((ret = on_before_loop()) != ERROR_SUCCESS) {
            RSLOGE("thread %s on before cycle failed, ignored and retry, ret=%d", _name, ret);
            goto failed;
        }
        RSLOGE("thread %s on before cycle success", _name);

        if ((ret = loop()) != ERROR_SUCCESS) {
            goto failed;
        }

        if ((ret = on_end_loop()) != ERROR_SUCCESS) {
            RSLOGE("thread %s on end cycle failed, ignored and retry, ret=%d", _name, ret);
            goto failed;
        }
        RSLOGE("thread %s on end cycle success", _name);

    failed:
        if (!loop_flag) {
            break;
        }

        if (cycle_interval_us != 0) {
            st_usleep(cycle_interval_us);
        }
    }

    // readly terminated now.
    really_terminated = true;

    on_thread_stop();
}

void RsThread::stop_thread()
{

}

//the thread loop
void* RsThread::thread_intermediary(void* arg)
{
    RsThread* obj = (RsThread*)arg;

    obj->thread_loop();
}

void RsThread::dispose()
{

}
