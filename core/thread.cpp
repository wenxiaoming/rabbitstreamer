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

#include "rs_thread.h"
#include "rs_error_code.h"
#include "rs_logger.h"

RsThread::RsThread()
{
    context_id = -1;
    tid = 0;
    thread_name = "";
    joinable = false;
    can_run = false;
    loop_flag = false;
    loop_interval_us = 0;
}

RsThread::~RsThread()
{

}

int RsThread::run(string name)
{
    int ret = ERROR_SUCCESS;

    thread_name = name;

    if(tid) {
        RSLOGD("thread %s already running.", thread_name.c_str());
        return ret;
    }

    if((tid = st_thread_create(thread_loop, this, (joinable? 1:0), 0)) == NULL){
        ret = ERROR_CREATE_ST_THREAD;
        RSLOGD("st_thread_create failed. ret=%d", ret);
        return ret;
    }

    loop_flag = true;

    //wait for thread_loop is ready.
    while (context_id < 0) {
        st_usleep(10 * 1000);
    }

    //thread loop can run.
    can_run = true;

    return ret;
}

void* RsThread::thread_loop(void* user)
{
    int ret = ERROR_SUCCESS;

    RsThread* self = (RsThread*)user;

    rs_context->generate_id();
    RSLOGD("thread %s loop start", self->thread_name);

    self->thread_start();

    self->context_id = rs_context->get_id();

    while (!self->can_run && self->loop_flag) {
        st_usleep(10 * 1000);
    }

    while (self->loop_flag) {
        if ((ret = self->before_loop()) != ERROR_SUCCESS) {
            RSLOGD("thread %s before loop failed, ret=%d", self->thread_name.c_str(), ret);
            goto failed;
        }

        if ((ret = self->loop()) != ERROR_SUCCESS) {
            goto failed;
        }

        if ((ret = self->end_loop()) != ERROR_SUCCESS) {
            RSLOGD("thread %s on end loop failed, ret=%d", self->thread_name.c_str(), ret);
            goto failed;
        }

    failed:
        if (!self->loop_flag) {
            break;
        }

        if (self->loop_interval_us != 0) {
            st_usleep(self->loop_interval_us);
        }
    }

    self->thread_stop();
    RSLOGD("thread %s loop finished", self->thread_name.c_str());
}

//set the flag to exit the thread
void RsThread::stop()
{

}

//set the exit flag of thread then wait for the exit of the thread
void RsThread::stop_wait()
{

}

