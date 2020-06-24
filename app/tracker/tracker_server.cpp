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
#include "app/common/server_base.h"
#include "app/tracker/tracker_np_manager.h"
#include "app/tracker/tracker_sp_manager.h"
#include "core/async_logger.h"
#include "core/resource.h"

using namespace rs::app::common;
using namespace rs::app::tracker;

int main() {
    init_log_system();

    rs_st_init();

    std::unique_ptr<TrackerSpManager> sp_tracker = make_unique_ptr<TrackerSpManager>("68.168.137.118", 4444);
    sp_tracker->start_listener();

    std::unique_ptr<TrackerNpManager> np_tracker = make_unique_ptr<TrackerNpManager>("68.168.137.118", 3333);
    np_tracker->start_listener();

    std::unique_ptr<RsBaseServer> server = make_unique_ptr<RsBaseServer>(TRACKER);
    server->loop();

    printf("hello tracker!\n");

    deinit_log_system();

    return 0;
}
