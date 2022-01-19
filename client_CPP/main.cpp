#include <iostream>
#include "constants.h"
#include "messages.h"
#include "makeRequest.h"
#include "handleResponse.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>

using namespace std;

unsigned char recv_buffer[1024] = {0};

std::string &trim(std::string &s) {
    if (s.empty()) {
        return s;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

void listenning(int sock_fd_tcp)
{
    int max_fd;
    fd_set readable, writable;
    while(1) {
        max_fd = sock_fd_tcp;

        FD_ZERO(&readable);

        FD_SET(sock_fd_tcp,&readable);

        select(max_fd+1,&readable,nullptr,nullptr,nullptr);
        if(FD_ISSET(sock_fd_tcp,&readable)) {
            int p = 0;
            while((length = recv(sock_fd_tcp, recv_buffer+p, EE122_SOCKET_BUFFER_SIZE, 0))>0) {
                while(recv_buffer[0]!='\0') {
                    handleResponse hr;
                    if(p = hr.initial(recv_buffer))
                    {
                        break;
                    }
                    if(hr.handle())continue;
                    hr.processResponse();
                }
            }
            if(length == 0){
                on_disconnection_from_server();
                exit(1);
            }
        }
 }
}


int main(int argc, char *argv[]) {
    unsigned char *buff;
    int address;
    int port;

    char opt;
    while ((opt = getopt(argc, argv, "s:p:")) != EOF) {
        switch (opt) {
            case 's':
                address = inet_addr(optarg);
                break;
            case 'p':
                port = stoi(optarg);
                break;
            default: fprintf(stdout, "! Invalid argument: '%c'\n", (char) opt);
                usage(argv[0]);
                exit(1);
               return 0;
        }
    }

    int sock_fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd_tcp < 0) {
        message_on_server_port_bind_failure();
        exit(-1);
    }

//  long o = 1; socklen_t ol = sizeof(long);
//  setsockopt(sock_fd_tcp, IPPROTO_TCP, TCP_NODELAY, (char*)&o, ol);

//     int flags  = fcntl(sock_fd_tcp,F_GETFL,0);
//     fcntl(sock_fd_tcp,F_SETFL,flags&~O_NONBLOCK);

    sockaddr_in si{};
    si.sin_addr.s_addr = address;
    si.sin_port = htons(port);
    si.sin_family = AF_INET;
    if (connect(sock_fd_tcp, (sockaddr *) &si, sizeof(si)) < 0) {
        perror("Oops");
        on_client_connect_failure();
        exit(-1);
    }
    show_prompt();

    pthread_t t;
    pthread_create(&t, nullptr, reinterpret_cast<void *(*)(void *)>(&listenning), (void*)sock_fd_tcp);

    while (1) {
        string command;
        getline(cin, command);
        show_prompt();
        trim(command);
        makeRequest mr(command);
        int len = mr.handle();
        if(len == -1)
        {
            continue;
        }
        buff = mr.createPacket();
        if(send(sock_fd_tcp, buff, len, 0)==-1)
        {
            on_disconnection_from_server();
            exit(0);
        }
    }
    return 0;
}



