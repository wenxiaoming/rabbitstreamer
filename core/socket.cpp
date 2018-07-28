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

#include "socket.h"
#include "error_code.h"
#include "logger.h"

RsSocket::RsSocket(st_netfd_t client_stfd)
{
    stfd = client_stfd;
    send_timeout = recv_timeout = ST_UTIME_NO_TIMEOUT;
    recv_bytes = send_bytes = 0;
}

RsSocket::~RsSocket()
{
}

bool RsSocket::is_never_timeout(int64_t timeout_us)
{
    return timeout_us == (int64_t)ST_UTIME_NO_TIMEOUT;
}

void RsSocket::set_recv_timeout(int64_t timeout_us)
{
    recv_timeout = timeout_us;
}

int64_t RsSocket::get_recv_timeout()
{
    return recv_timeout;
}

void RsSocket::set_send_timeout(int64_t timeout_us)
{
    send_timeout = timeout_us;
}

int64_t RsSocket::get_send_timeout()
{
    return send_timeout;
}

int64_t RsSocket::get_recv_bytes()
{
    return recv_bytes;
}

int64_t RsSocket::get_send_bytes()
{
    return send_bytes;
}

int RsSocket::read(void* buf, size_t size, ssize_t* nread)
{
    int ret = ERROR_SUCCESS;

    ssize_t nb_read = st_read(stfd, buf, size, recv_timeout);
    if (nread) {
        *nread = nb_read;
    }

    if (nb_read <= 0) {
        if (nb_read < 0 && errno == ETIME) {
            return ERROR_SOCKET_TIMEOUT;
        }

        if (nb_read == 0) {
            errno = ECONNRESET;
        }

        return ERROR_SOCKET_READ;
    }

    recv_bytes += nb_read;

    return ret;
}

int RsSocket::read_fully(void* buf, size_t size, ssize_t* nread)
{
    int ret = ERROR_SUCCESS;

    ssize_t nb_read = st_read_fully(stfd, buf, size, recv_timeout);
    if (nread) {
        *nread = nb_read;
    }

    if (nb_read != (ssize_t)size) {
        if (nb_read < 0 && errno == ETIME) {
            return ERROR_SOCKET_TIMEOUT;
        }

        if (nb_read >= 0) {
            errno = ECONNRESET;
        }

        return ERROR_SOCKET_READ_FULLY;
    }

    recv_bytes += nb_read;

    return ret;
}

int RsSocket::write(void* buf, size_t size, ssize_t* nwrite)
{
    int ret = ERROR_SUCCESS;

    ssize_t nb_write = st_write(stfd, buf, size, send_timeout);
    if (nwrite) {
        *nwrite = nb_write;
    }

    if (nb_write <= 0) {
        if (nb_write < 0 && errno == ETIME) {
            return ERROR_SOCKET_TIMEOUT;
        }

        return ERROR_SOCKET_WRITE;
    }

    send_bytes += nb_write;

    return ret;
}

int RsSocket::writev(const iovec *iov, int iov_size, ssize_t* nwrite)
{
    int ret = ERROR_SUCCESS;

    ssize_t nb_write = st_writev(stfd, iov, iov_size, send_timeout);
    if (nwrite) {
        *nwrite = nb_write;
    }


    if (nb_write <= 0) {
        if (nb_write < 0 && errno == ETIME) {
            return ERROR_SOCKET_TIMEOUT;
        }

        return ERROR_SOCKET_WRITE;
    }

    send_bytes += nb_write;

    return ret;
}

#ifdef __linux__
#include <sys/epoll.h>

bool rs_st_epoll_is_supported(void)
{
    struct epoll_event ev;

    ev.events = EPOLLIN;
    ev.data.ptr = NULL;
    /* Guaranteed to fail */
    epoll_ctl(-1, EPOLL_CTL_ADD, -1, &ev);

    return (errno != ENOSYS);
}
#endif

int rs_st_init()
{
    int ret = ERROR_SUCCESS;

#ifdef __linux__
    if (!rs_st_epoll_is_supported()) {
        ret = ERROR_ST_SET_EPOLL;
        RSLOGE("epoll required on Linux. ret=%d", ret);
        return ret;
    }
#endif

    if (st_set_eventsys(ST_EVENTSYS_ALT) == -1) {
        ret = ERROR_ST_SET_EPOLL;
        RSLOGE("st_set_eventsys use %s failed. ret=%d", st_get_eventsys_name(), ret);
        return ret;
    }
    RSLOGE("st_set_eventsys to %s", st_get_eventsys_name());

    if(st_init() != 0){
        ret = ERROR_ST_INITIALIZE;
        RSLOGE("st_init failed. ret=%d", ret);
        return ret;
    }
    RSLOGE("st_init success, use %s", st_get_eventsys_name());

    return ret;
}

void rs_close_stfd(st_netfd_t& stfd)
{
    if (stfd) {
        int fd = st_netfd_fileno(stfd);
        st_netfd_close(stfd);
        stfd = NULL;

        // st does not close it sometimes,
        // close it manually.
        close(fd);
    }
}
