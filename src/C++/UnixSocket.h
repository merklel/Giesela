//
// Created by lukas on 8/21/17.
//

#ifndef GISELA_UNIXSOCKET_H
#define GISELA_UNIXSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <thread>
#include <stdio.h>
#include <errno.h>

#include <stdlib.h>



class UnixSocket{
private:
    int sock, msgsock, rval;
    struct sockaddr_un server;
    char buf[1024];
    char buf_return[1024];
    void SocketServer_thread();
public:
    //Konstruktor und Destruktor
    UnixSocket(std::string path);
    ~UnixSocket();

    void startSocketServer();

    char* getSocketBuffer();
};

#endif //GISELA_UNIXSOCKET_H
