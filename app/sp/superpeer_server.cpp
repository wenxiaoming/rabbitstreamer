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
#include <memory>
#include "app/common/server_base.h"
#include "app/sp/sp_cs_manager.h"
#include "app/sp/sp_np_manager.h"
#include "app/sp/sp_tracker_manager.h"
#include "core/async_logger.h"
#include "protocol/sp/sp_source_manager.h"
#include "core/resource.h"

using namespace rs::app::common;
using namespace rs::protocol::sp;
using namespace rs::app::sp;

int main() {
    init_log_system();

    rs_st_init();

    // start the listener of super peer server
    std::unique_ptr<SpNpManager> np_manager = make_unique_ptr<SpNpManager>("68.168.137.118", 2222);
    np_manager->start_listener();

    // listen from capture server
    std::unique_ptr<SpCsManager> cs_manager = make_unique_ptr<SpCsManager>("68.168.137.118", 12345);
    cs_manager->start_listener();

    // listen from normal peer or supoer peer
    RsSourceManager::instance()->initialize("", "", "68.168.137.118:3333");

    // update and check status with tracker server
    std::unique_ptr<SpTrackerManager> tracker_manager =
        make_unique_ptr<SpTrackerManager>("68.168.137.118", 4444);
    tracker_manager->start_connect();

    std::unique_ptr<RsBaseServer> server = make_unique_ptr<RsBaseServer>(SUPER_PEER);
    server->loop();

    deinit_log_system();

    printf("hello sp!\n");
    return 0;
}
