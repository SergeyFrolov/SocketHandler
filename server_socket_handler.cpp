// Copyright 2015 Sergey Frolov. All rights reserved.
// Use of this source code is governed by a LGPL license that can be
// found in the LICENSE file.

#include "server_socket_handler.h"

ServerSocketHandler::ServerSocketHandler(int port_, int max_active_connections_, int max_pend_connections_) {
    port = port_;
    max_active_connections = max_active_connections_;
    max_pend_connections = max_pend_connections_;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_);
}

void ServerSocketHandler::bindAndListen(){
    listen_fd = socket(socket_family, socket_type, socket_protocol);
    if (!listen_fd)
        throw std::runtime_error("Could not create socket!");
    if (bind(listen_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        throw std::runtime_error("Could not bind!");
    else
        cout << " Bound to port " << port << endl;

    listen(listen_fd, max_pend_connections);
    cout << " Listening." << endl;
};

int ServerSocketHandler::accept_connection() {
    struct sockaddr_in curr_client_address;
    int new_sock_fd;
        socklen_t len = sizeof(curr_client_address);
        new_sock_fd = accept(listen_fd, (struct sockaddr *) &curr_client_address, &len);

        if (new_sock_fd < 0) {
            cerr << "Failed to accept connection, ignoring this one." << endl;
            return 0;
        }
        else {
            if (connections.size() < max_active_connections) {
                SocketConnection connection(new_sock_fd, curr_client_address, this, "socket " +
                        std::to_string(new_sock_fd));
                connections.push_back(connection);

                for (auto sockopt : default_setsockopts)
                    setsockopt(new_sock_fd, sockopt.level, sockopt.optname, sockopt.optval, sockopt.optlen);

                return new_sock_fd;
            }
            else {
                cout << "WARNING! Connection was not accepted, due to exceeding max_active_connections." << endl;
                return 0;
                }
            }
    }
