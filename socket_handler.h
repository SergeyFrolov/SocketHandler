// Copyright 2015 Sergey Frolov. All rights reserved.
// Use of this source code is governed by a LGPL license that can be
// found in the LICENSE file.

#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <netinet/in.h>

#include <map>
#include <list>
#include <string>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::list;
using std::string;

class SocketConnection;
struct SetSockOptParams;


class SocketHandler {
protected:
    list<SocketConnection> connections;
    list<SetSockOptParams> default_setsockopts;

    sa_family_t socket_family;
    int socket_type;
    int socket_protocol;

    SocketHandler();
public:
    SocketConnection& operator[](int sock_fd);

    typedef list<SocketConnection>::iterator connections_iterator;
    connections_iterator connections_begin() {return connections.begin();}
    connections_iterator connections_end() {return connections.end  ();}

    void addDefaultSetsockopt(int level, int optname, void *optval, socklen_t optlen, bool retroactive = 0);
    int disconnect(int sock_fd);

};


class SocketConnection{
private:
    struct sockaddr_in addr;
    int sock_fd;
    SocketHandler* sc;
    size_t max_transfer_size;

    int disconnect();
public:
    string name;

    SocketConnection(int sock_fd_, struct sockaddr_in addr_, SocketHandler* sc_, string name_,
                     size_t max_transfer_size_ = 4096);
    ssize_t writeAll(string write_str);
    ssize_t writeAll(char* write_char_ptr, ssize_t len);
    ssize_t readAll(string& read_string, long min_bytes_to_read = 0);
    ssize_t readAll(char* write_char_ptr, ssize_t len, long min_bytes_to_read = 0);
    int get_fd(){return sock_fd;}
};


/* This struct is used in addDefaultSetsockopt()
 * for example, to set such options, as timeout or `do not drop connection` and others
 */
struct SetSockOptParams {
public:
    int level;
    int optname;
    void* optval;
    socklen_t optlen;

    SetSockOptParams(int level_, int optname_, void* optval_, socklen_t optlen_) :
            level(level_), optname(optname_), optlen(optlen_) {
        // Due to setsockopt() signature, we deal with ugly void pointers, so no need for new[] and delete[]
        // Using malloc() and free() for them instead
        optval = malloc(optlen);
        memcpy(optval, optval_, optlen_);
    }

    SetSockOptParams (const SetSockOptParams& s) {
        level = s.level;
        optname = s.optname;
        optlen = s.optlen;
        optval = malloc(optlen);
        memcpy(optval, s.optval, optlen);
    }

    SetSockOptParams& operator=(const SetSockOptParams&) = default;
    SetSockOptParams(SetSockOptParams&&) = default;
    SetSockOptParams& operator=(SetSockOptParams&&) = default;

    ~SetSockOptParams() {
        free(optval);
    }
};
#endif //SOCKETHANDLER_H
