//
// Created by lukas on 8/21/17.
//

#include "UnixSocket.h"


UnixSocket::UnixSocket(std::string path) {

    //Populate server struct
    server.sun_family = AF_UNIX;
    std::strcpy(server.sun_path, path.c_str());
};

UnixSocket::~UnixSocket(){
    
}

void UnixSocket::SocketServer_thread() {

    close(sock);
    unlink(server.sun_path);

    //Init Socket Server
    printf("Init Socket...\n");
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cout << "couldn't create socket" << std::endl;
    }


    int ib = bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un));
    if (ib != 0) {
        std::cout << "couldn't bind socket. code: " << ib << " errno: " << strerror(errno) << " ..." << std::endl;
    }else{
         printf("Socket bound...\n");
    }

    //Setzte Berechtigungen
    system("chmod 777 /home/pi/Gisela/gSockets/gSocketCpp.sock");

    //Start listening...
    listen(sock, 5);

    //Loop for connections
    printf("start listening on socket...\n");
    while (1) {
        msgsock = accept(sock, 0, 0);
        if (msgsock == -1)
            std::cout << "socket problem with accepting..." << std::endl;
        else
            do {
                //bzero(buf, sizeof(buf));

                if ((rval = read(msgsock, buf, 1024)) < 0)
                    std::cout << "reading stream message error" << std::endl;
                else if (rval == 0)
                    printf("Ending connection\n");
                else
                    printf("-->%s\n", buf);
            } while (rval > 0);
            bzero(buf, sizeof(buf));
            printf("socket runs!\n");
        close(msgsock);
    }
    close(sock);
    unlink(server.sun_path);
}

void UnixSocket::startSocketServer() {
    std::thread socketThread(&UnixSocket::SocketServer_thread, this);
    socketThread.detach();
}


char * UnixSocket::getSocketBuffer(){
	std::strcpy(buf_return, buf);
	bzero(buf, sizeof(buf));
    return buf_return;
}