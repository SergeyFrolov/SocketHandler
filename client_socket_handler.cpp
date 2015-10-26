// Copyright 2015 Sergey Frolov. All rights reserved.
// Use of this source code is governed by a LGPL license that can be
// found in the LICENSE file.

#include "client_socket_handler.h"
#include <arpa/inet.h>

int ClientSocketHandler::connectSock(string host, int portnum) {
    int sock_fd;
    sock_fd = socket(socket_family, socket_type, socket_protocol);
    if (sock_fd < 0)
        throw std::runtime_error("can't create socket:" + string(strerror(errno)));

    sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET; // Candidate to be a field
    serv.sin_port = htons(portnum);
    inet_aton(host.data(), &(serv.sin_addr));

    if (connect(sock_fd, (sockaddr *)(&serv), sizeof(serv)) < 0) {
        cerr << "WARNING! Can't connect to " + host + ":" + std::to_string(portnum) << endl;
        close(sock_fd);
        return 0;
    }
    else {
        SocketConnection connection(sock_fd, serv, this, host + ":" + std::to_string(portnum));
        connections.push_back(connection);
    }

    for (auto sockopt : default_setsockopts) {
        setsockopt(sock_fd, sockopt.level, sockopt.optname, sockopt.optval, sockopt.optlen);
    }

    return sock_fd;
}
