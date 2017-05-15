/*
  Copyright (c) 2016-2017 Kevin Wen <listream@126.com>
*/

#ifndef CORE_RS_UDP_LISTENER_H_
#define CORE_RS_UDP_LISTENER_H_

#include <string>
#include <st.h>
#include "rs_thread.h"
#include "rs_socket_handler.h"

using namespace std;

class RsUdpListener: public RsThread
{

public:
    RsUdpListener(string ip, int port, IUdpHandler* handler);
    ~RsUdpListener();

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

    char* recv_buffer;
    int buffer_size;
    IUdpHandler* udp_handler;
};



#endif /* CORE_RS_UDP_LISTENER_H_ */
