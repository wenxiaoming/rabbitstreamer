
#include <gtest/gtest.h>

#include "core/streamer.h"

namespace core {

TEST(StreamerTest, construct) {
    RsStreamer *streamer = new RsStreamer;
    char buf[] = {0x1, 0x2, 0x3, 0x4};
    streamer->initialize(buf, sizeof(buf));

    delete streamer;
}


}