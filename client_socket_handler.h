// Copyright 2015 Sergey Frolov. All rights reserved.
// Use of this source code is governed by a LGPL license that can be
// found in the LICENSE file.

#ifndef CLIENTSOCKETHANDLER_H
#define CLIENTSOCKETHANDLER_H

#include "socket_handler.h"

class ClientSocketHandler : public SocketHandler {
public:
    ClientSocketHandler() {};
    int connectSock(string host, int portnum);
};

#endif //CLIENTSOCKETHANDLER_H
