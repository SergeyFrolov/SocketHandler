# SocketHandler
Yet another c++ class wrapper for sockets.

## Features
Includes meta SocketHandler, ServerSocketHandler and ClientSocketHandler.
They store and handle socket connections. Sockets are added via
acceptConnection(port) and connectSock(serv_addr, port) for server and client.
The, you may do writeAll() or readAll() for specific socket:
serverSocketHandler[socket_descriptor].writeAll("Hello, world");
You also may addDefaultSetsockopt. (see below)

## Plans
Add TLS/SSL encryption.

# Usage example:

## On server:
ServerSocketHandler socketHandler(port); // e.g. port = 10001
socketHandler.bindAndListen();
// add default sockopts, if wanted
int conn_fd = socketHandler->accept_connection(); // connections are usually connected in cycle, sock descriptors are stored
if (conn_fd > 0) {
    string request_string = "Hello, client!",
            response_string;
    ssize_t bytes_read, bytes_written;
    bytes_written = socketHandler[conn_fd].writeAll(response_string);
    bytes_read = socketHandler[conn_fd].readAll(request_string);
    cout << response_string << endl;
    socketHandler[conn_fd].disconnect();
}

## On client:
ClientSocketHandler socketHandler();
// add default sockopts, if wanted
int conn_fd = socketHandler.connectSock(serv_host, serv_port); // e.g. serv_host = 127.0.0.1, port = 10001
if (conn_fd > 0) {
    string request_string = "Hello, server!",
            response_string;
    ssize_t bytes_read, bytes_written;
    bytes_written = socketHandler[conn_fd].writeAll(response_string);
    bytes_read = socketHandler[conn_fd].readAll(request_string);
    cout << response_string << endl;
    socketHandler[conn_fd].disconnect();
}

## Examples for setsockopt:
// reuse socket(do not close connection), set 5 sec timeout for send and 10 sec timeout for recv
int b_reuse_sock = 1;
socketHandler.addDefaultSetsockopt(SOL_SOCKET, SO_REUSEADDR, (char *) &b_reuse_sock, sizeof(b_reuse_sock));

timeval timeout;
timeout.tv_sec = 5; // 5 sec time timeout for send
timeout.tv_usec = 0 ;
socketHandler.addDefaultSetsockopt(SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout));

timeout.tv_sec = 10; // 10 secs timeout for recv, as it could process some stuff
socketHandler.addDefaultSetsockopt(SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
// you still can use generic setsockopt() to set socket-specific options

