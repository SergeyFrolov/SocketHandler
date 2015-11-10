// Copyright 2015 Sergey Frolov. All rights reserved.
// Use of this source code is governed by a LGPL license that can be
// found in the LICENSE file.

#ifndef CLIENTSOCKETHANDLER_H
#define CLIENTSOCKETHANDLER_H

#include "socket_handler.h"

class ClientSocketHandler : public SocketHandler {
public:
    ClientSocketHandler() {};
    int connectSockByIP(string ip, int portnum);
    int connectSockByHostname(string hostname, int portnum);
private:
    int hostname_to_ip(const string& host, list<string>& ip_address);
};

#endif //CLIENTSOCKETHANDLER_H
