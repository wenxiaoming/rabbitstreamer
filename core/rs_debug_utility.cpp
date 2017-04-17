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

#include "rs_debug_utility.h"
#include <execinfo.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int get_process_name(char* buf, int size)
{
    int ret = readlink("/proc/self/exe", buf, size);
    if((ret < 0)||(ret >= size))
        return 1;

    buf[ret] = '\0';
    return 0;
}

#define CALLSTACK_DEPTH 64
#define MAX_BUF_SIZE 1024

void print_backtrace(void)
{
    void *trace[CALLSTACK_DEPTH];
    char process_name[MAX_BUF_SIZE];
    FILE* file = NULL;

    char command[MAX_BUF_SIZE] = "addr2line -f -e ";

    if(get_process_name(process_name, MAX_BUF_SIZE))
        return;

    memcpy(command+strlen(command), process_name, strlen(process_name));

    file = popen(command, "w");
    if(!file)
        return;

    int nptrs = backtrace(trace, CALLSTACK_DEPTH);
    for (int i = 0; i < nptrs; i++)
    {
        fprintf(file, "%p\n\r", trace[i]);
    }

    fclose(file);
}

//#define UNIT_TEST
#ifdef UNIT_TEST //build with -g
int main()
{
    print_backtrace();
    return 0;
}
#endif
