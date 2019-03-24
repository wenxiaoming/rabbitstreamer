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
#include "debug_utility.h"
#include <execinfo.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

namespace core {

void print_backtrace(void)
{
    int i;
    int MAX_CALLSTACK_DEPTH = 32;
    void *traceback[MAX_CALLSTACK_DEPTH];

    char cmd[512] = "addr2line -f -e ";
    char *prog = cmd + strlen(cmd);

    int r = readlink("/proc/self/exe",prog,sizeof(cmd)-(prog-cmd)-1);

    /*printf("%s\n",cmd);*/
    FILE *fp = popen(cmd, "w");

    int depth = backtrace(traceback, MAX_CALLSTACK_DEPTH);
    for (i = 0; i < depth; i++)
    {
        /*printf("%p\n",traceback[i]);*/
        fprintf(fp, "%p\n\r", traceback[i]);
    }
    fclose(fp);
}

} /* namespace core */