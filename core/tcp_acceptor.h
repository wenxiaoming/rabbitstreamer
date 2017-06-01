/*
  Copyright (c) 2016-2017 Kevin Wen <listream@126.com>
*/

#ifndef RS_TCP_ACCEPTOR_H
#define RS_TCP_ACCEPTOR_H

#include <string>
#include "st.h"
#include "rs_thread.h"
#include "rs_socket_handler.h"

using namespace std;

class RsTcpAcceptor: public RsThread
{
public:
    RsTcpAcceptor(string ip, int port, ITcpHandler* handler);
    ~RsTcpAcceptor();

public:
    int start_listen();

public:
    //implement RsThread's virtual function
    virtual int thread_start();
    virtual int before_loop();
    virtual int loop();
    virtual int end_loop();
    virtual int thread_stop();

private:
    int socket_fd;
    st_netfd_t st_socket_fd;

    string ip_addr;
    int listen_port;

    ITcpHandler* tcp_handler;
};

#endif
