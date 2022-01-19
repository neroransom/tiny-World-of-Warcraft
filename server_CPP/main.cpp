#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include "handlRequest.h"

using namespace std;


socklen_t client_len = sizeof(struct sockaddr);
unsigned char recv_buffer[EE122_SOCKET_BUFFER_SIZE] = {0};


void send_to_all(unsigned char *wbuf, int len) {

    for (auto it = client_fd.begin(); it != client_fd.end(); ++it) {
        send(*it, wbuf, len, 0);
    }
    delete[](wbuf);
}

void send_all_to_one(char selffd) {
    unsigned char* buff;int on= 0;
    for (auto it = client_fd.begin(); it != client_fd.end(); ++it) {
        if(selffd!=*it) {
            buff = userInform(*it);
            send(selffd, reinterpret_cast<char *>(buff), 24, 0);
            on = 1;
        }
    }
    if(on) delete[] buff;
}

int main(int argc, char *argv[]) {
    int sock_tcp_fd, tmpfd;
    struct sockaddr_in saddr;
    int size = sizeof(struct sockaddr_in);

    struct epoll_event ev;                //用于往epoll句柄加入新的待检测套接字
    struct epoll_event wait_event[10];   //用于装epoll_wait（）返回的响应的套接字的消息，判断哪个套接字发生响应

    int on = 1;
    int num;
    int port;

    system("mkdir -p ./user/");

#pragma region argument operation
    char opt;
    while ((opt = getopt(argc, argv, "s:p:")) != EOF) {
        switch (opt) {
            case 'p':
                port = stoi(optarg);
                break;
            default:
                fprintf(stdout, "! Invalid argument: '%c'\n", (char) opt);
                //          usage(argv[0]);
                exit(1);
                return 0;
        }
    }
#pragma endregion

    //设置TCP套接字的一些属性

#pragma region TCP setting
    bzero(&saddr, size);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sock_tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sock_tcp_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bind(sock_tcp_fd, (struct sockaddr *) &saddr, sizeof(struct sockaddr));
    listen(sock_tcp_fd, 30);
#pragma endregion

#pragma region epoll setting
    int epfd = epoll_create(30);   //创建epoll句柄epfd 10 size maybe 没意义，只要是正数即可。

    ev.data.fd = sock_tcp_fd;         //加入监听套接字
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock_tcp_fd, &ev);

    //加入标准输入，使服务端可以发信息
    ev.data.fd = 0;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &ev);
#pragma endregion
    while (true) {
        num = epoll_wait(epfd, wait_event, 10, -1);
        for (int i = 0; i < num; ++i) {
            if (wait_event[i].data.fd == sock_tcp_fd) //有新的连接
            {
                struct sockaddr_in caddr;
                tmpfd = accept(sock_tcp_fd, (sockaddr *) &caddr, &client_len); //accept这个连接
                if (tmpfd < 0) { //error
                    perror("accept_failure");
                    exit(1);
                }
                client_fd.insert(client_fd.begin(), tmpfd);
                ev.events = EPOLLIN;
                ev.data.fd = tmpfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, tmpfd, &ev);  //将新的fd添加到epoll的监听队列中(regist)

                char *str = inet_ntoa(caddr.sin_addr);
                cout << "accapt a connection from " << str << endl;
            } else if (wait_event[i].events & EPOLLIN) //接收到数据，读socket
            {
                int n;
                cout << "EPOLLIN" << endl;
                if ((tmpfd = wait_event[i].data.fd) < 0) continue;
                if ((n = recv(tmpfd, recv_buffer, EE122_SOCKET_BUFFER_SIZE, 0)) > 0) {    //读
                    handleRequest hr(tmpfd);
                    hr.initial(recv_buffer);
                    int ret = hr.handle();
                    int length = hr.process();
                    unsigned char *buff = hr.makepacket();
                    if (ret == -1) {
                        cout << "fd=" << tmpfd << " logout" << endl;
                        close(tmpfd);
                        wait_event[i].data.fd = -1;
                        client_fd.remove(tmpfd);
                        send_to_all(buff,16);
                    } else {
                        pack a(tmpfd, buff, length, hr.brcst);
                        if(hr.brcst == -1)
                        {
                            a.vicebuf = userInform(tmpfd);
                        }
                        ev.data.ptr = &a;
                        ev.events = EPOLLOUT;
                        epoll_ctl(epfd, EPOLL_CTL_MOD, tmpfd, &ev);//修改标识符，等待下一个循环时发送数据，异步处理的精髓
                    }
                } else if (n == 0) {      //close
                    cout << "fd=" << tmpfd << " logout" << endl;
                    close(tmpfd);
                    wait_event[i].data.fd = -1;
                    client_fd.remove(tmpfd);
                    user_map_l.erase(user_map_r.find(tmpfd)->second);
                    user_map_r.erase(tmpfd);
                } else if (errno == ECONNRESET) {
                    close(tmpfd);
                    wait_event[i].data.fd = -1;
                    client_fd.remove(tmpfd);
                } else {
                    std::cout << "readline error" << std::endl;
                }
            } else if (wait_event[i].events & EPOLLOUT) //有数据待发送，写socket
            {
                pack *md = (pack *) wait_event[i].data.ptr;    //取数据
                if (md->brcst == 1) {
                    send_to_all(md->buff, md->len);
                } else if (md->brcst == 0) {
                    send(md->fd, md->buff, md->len, 0);        //发送数据
                    delete[] (md->buff);
                }else if(md->brcst == -1)
                {
                    send(md->fd, md->buff, md->len, 0);
                    send_to_all(userInform(tmpfd),24);
                    send_all_to_one(tmpfd);
                    delete[] (md->buff);
                }
                ev.data.fd = tmpfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_MOD, tmpfd, &ev); //修改标识符，等待下一个循环时接收数据
            } else {
                //其他的处理
            }
        }
    }
}
