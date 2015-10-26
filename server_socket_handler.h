// Copyright 2015 Sergey Frolov. All rights reserved.
// Use of this source code is governed by a LGPL license that can be
// found in the LICENSE file.

#ifndef SERVERSOCKETHANDLER_H
#define SERVERSOCKETHANDLER_H

#include "socket_handler.h"

class ServerSocketHandler : public SocketHandler {
private:
    struct sockaddr_in server_address;
    int max_pend_connections;
    int max_active_connections;
    int port;
    int listen_fd;

public:
    ServerSocketHandler(int port_, int max_active_connections_ = 30, int max_pend_connections_ = 5);
    void bindAndListen();
    int accept_connection(); // blocking call
};

#endif //SERVERSOCKETHANDLER_H
