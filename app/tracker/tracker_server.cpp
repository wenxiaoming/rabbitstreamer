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
#include <stdio.h>
#include "app/tracker/tracker_sp_manager.h"
#include "app/tracker/tracker_np_manager.h"
#include "app/common/server_base.h"
#include "core/async_logger.h"

RsLogBase* rs_log = new RsAsyncLogger();
RsContextBase* rs_context = new RsThreadContext();

int main()
{
    rs_st_init();

    TrackerSpManager* sp_tracker = new TrackerSpManager("68.168.137.118", 4444);
    sp_tracker->start_listener();

    TrackerNpManager* np_tracker = new TrackerNpManager("68.168.137.118", 5555);
    np_tracker->start_listener();

    RsBaseServer* server = new RsBaseServer;
    server->loop();

    printf("hello tracker!\n");

    delete sp_tracker;

    delete np_tracker;

    delete server;

    return 0;
}


