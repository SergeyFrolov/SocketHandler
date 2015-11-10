// Copyright 2015 Sergey Frolov. All rights reserved.
// Use of this source code is governed by a LGPL license that can be
// found in the LICENSE file.

#include "client_socket_handler.h"
#include <arpa/inet.h>
#include <netdb.h>

#include <regex>

int ClientSocketHandler::connectSockByHostname(string hostname, int portnum) {
    /* returns socketnum > 0, if success
     * returns negative number, if fail
     */
    list<string> ip_addresses;
    if (hostname_to_ip(hostname.c_str(), ip_addresses)){
        return -1;
    }
    int sock_fd;
    for (auto ip_addr : ip_addresses) {
        sock_fd = connectSockByIP(ip_addr, portnum);
        if (sock_fd > 0) {
            (*this)[sock_fd].name = hostname + " (" + (*this)[sock_fd].name + ")";
            return sock_fd;
        }
    }
    return -2;
}

int ClientSocketHandler::connectSockByIP(string ip, int portnum) {
    /* returns socketnum > 0, if success
     * returns negative number, if fail
     */
    sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = socket_family; // Candidate to be a field
    serv.sin_port = htons(portnum);
    inet_aton(ip.data(), &(serv.sin_addr));


    int sock_fd;
    sock_fd = socket(socket_family, socket_type, socket_protocol);
    if (sock_fd < 0)
        throw std::runtime_error("can't create socket:" + string(strerror(errno)));


    if (connect(sock_fd, (sockaddr *)(&serv), sizeof(serv)) < 0) {
        cerr << "WARNING! Can't connect to " + ip + ":" + std::to_string(portnum) << endl;
        close(sock_fd);
        return 0;
    }
    else {
        SocketConnection connection(sock_fd, serv, this, ip + ":" + std::to_string(portnum));
        connections.push_back(connection);
        for (auto sockopt : default_setsockopts) {
            setsockopt(sock_fd, sockopt.level, sockopt.optname, sockopt.optval, sockopt.optlen);
        }
    }

    return sock_fd;
}

int ClientSocketHandler::hostname_to_ip(const string& host, list<string>& ip_address)
{
    string host_name;

    string http_starting = "http://";
    string https_starting = "https://";
    if (host.compare(0, http_starting.size(), http_starting) == 0)
        host_name = host.substr(http_starting.size(), host.size() - http_starting.size());
    else
    if (host.compare(0, https_starting.size(), https_starting) == 0)
        host_name = host.substr(https_starting.size(), host.size() - https_starting.size());
    else
        host_name = host;

    string www_starting = "www.";
    if (host_name.compare(0, www_starting.size(), www_starting) != 0)
        host_name = www_starting + host_name;

    std::size_t found = host_name.find_first_of("/");
    if (found!=std::string::npos) {
        host_name = host_name.substr(0, found);
    }

    size_t ip_buff_size = 100;
    char ip_char[ip_buff_size];
    memset(ip_char, 0, ip_buff_size);

    struct addrinfo *servinfo;
    int ret_value;
    if ((ret_value = getaddrinfo(host_name.c_str(), NULL, NULL, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret_value));
        cerr << "getaddrinfo(" << host_name << ") does not translate into anything." << endl;
        return 1;
    }

    struct sockaddr_in *tmp_sockaddr_in;
    struct addrinfo *addrinfo_iter;
    for(addrinfo_iter = servinfo; addrinfo_iter != NULL; addrinfo_iter = addrinfo_iter->ai_next)
    {
        tmp_sockaddr_in = (struct sockaddr_in *) addrinfo_iter->ai_addr;
        strcpy(ip_char, inet_ntoa( tmp_sockaddr_in->sin_addr ) );
        ip_address.push_back(std::string(ip_char));
    }

    freeaddrinfo(servinfo);

    if (ip_address.size() != 0)
        return 0;
    else {
        cerr << "gethostbyname(" << host_name << "): no working ip addresses." << endl;
        return 1;
    }
}
