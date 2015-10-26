// Copyright 2015 Sergey Frolov. All rights reserved.
// Use of this source code is governed by a LGPL license that can be
// found in the LICENSE file.

#include "socket_handler.h"

#ifndef MAX_TRANSFER_SIZE
#define MAX_TRANSFER_SIZE 4096
#endif

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdexcept>

SocketConnection::SocketConnection(int sock_fd_, struct sockaddr_in addr_, SocketHandler* sc_,
                                   string name_, size_t max_transfer_size_) :
        sock_fd(sock_fd_), addr(addr_), sc(sc_), name(name_), max_transfer_size(max_transfer_size_){}

ssize_t SocketConnection::writeAll(string write_str) {
        const char *write_str_p = write_str.data();
        ssize_t bytes_written_total = 0, bytes_written;
        while (bytes_written_total < write_str.length()) {
            cout << name << ": written=" << bytes_written_total << ". Trying to write " <<
            write_str.length() - bytes_written_total << endl;
            size_t write_str_size = write_str.length() - bytes_written_total > max_transfer_size ?
                                    max_transfer_size : write_str.length() - bytes_written_total;
            bytes_written = write(sock_fd, write_str_p + bytes_written_total, write_str_size);
            cout << name << ": written=" << bytes_written_total << ". Just wrote " << bytes_written << endl;
            if (bytes_written < 0) {
                cout << name << " Error writing to socket";
                return bytes_written;
            }
            else
                bytes_written_total += bytes_written;
        }
        return bytes_written_total;
};

ssize_t SocketConnection::readAll(string& read_string, long min_bytes_to_read) {
    ssize_t bytes_read, bytes_read_total = 0;
    read_string = "";
    do {
        char buffer[max_transfer_size];
        memset(buffer, 0, max_transfer_size);
        bytes_read = read(sock_fd, buffer, max_transfer_size);
        if (bytes_read < 0) {
            cerr << "Connection with " << name << " dropped." << endl;
            return disconnect();
        }
        if (bytes_read == 0) {
            cerr << "Connection with " << name << " closed by remote host." << endl;
            return disconnect();
        }
        else {
            read_string += string(&(buffer[0]), bytes_read);
        }
        bytes_read_total += bytes_read;
    } while (bytes_read_total < min_bytes_to_read);
    return bytes_read_total;
}

int SocketConnection::disconnect() {
    return sc->disconnect(sock_fd);
};

/*
 * SocketHandler functions
 */

SocketHandler::SocketHandler(){
    socket_family = PF_INET;
    socket_type = SOCK_STREAM;
    socket_protocol = IPPROTO_TCP; // hardcoded, because other variants were not tested
};

SocketConnection& SocketHandler::operator[](int sock_fd) {
    for (auto &conn : connections) {
        if (conn.get_fd() == sock_fd)
            return conn;
    }
    throw std::runtime_error("SocketHandler: accessing nonexisting connection with sock_fd=" + std::to_string(sock_fd));
}

/*
 * Would call setsockopt() with those parameters for every new socket
 * With retroactive=1, apply for already existing sockets
 * To setsockopt for specific sock_fd, just use generic setsockopt()
 */
void SocketHandler::addDefaultSetsockopt(int level, int optname, void *optval, socklen_t optlen,
                                         bool retroactive) {
    default_setsockopts.push_back(SetSockOptParams(level, optname, optval, optlen));

    if (retroactive) {
        for (auto it = connections_begin(); it != connections_end(); it++)
            setsockopt(it->get_fd(), level, optname, optval, optlen);
    }
}

int SocketHandler::disconnect(int sock_fd) {
    for (std::list<SocketConnection>::iterator sc_iter = connections.begin(); sc_iter != connections.end(); sc_iter++)
    {
        if (sc_iter->get_fd() == sock_fd) {
            close(sock_fd);
            connections.erase(sc_iter);
            return 0;
        }
    }
    return -1;
};

