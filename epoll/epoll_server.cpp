#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <cstring>

using namespace std;
unsigned long fact(int n);

int main(){ 
    int server_soc, client_soc;
    
    server_soc = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server , client;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(54000);

    memset(server.sin_zero, 8, 0);

    unsigned len = sizeof(sockaddr_in);
    int MAX_EVENT_NUMBER = 10;
    int b = bind(server_soc, (sockaddr*)&server, len);
    if(b < 0){
        cout << "cant bind"<<endl;
    }

    int l = listen(server_soc, 10);
    if(l < 0){
        cout << "cant listern"<<endl;
    }

    epoll_event ev[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(10);
    if(epollfd < 0){
        cerr << "Cant create the epoll fd" << endl;
    }

    char buffer[4096];
    // Add FD
    epoll_event event;
    event.data.fd = server_soc;
    event.events = EPOLLIN;
    int e = epoll_ctl( epollfd, EPOLL_CTL_ADD, server_soc, &event );
    if(e < 0){
        cerr<< "Err"<<endl;
    }

    while(true){
        int number = epoll_wait( epollfd, ev, MAX_EVENT_NUMBER, 500 );
        for ( int i = 0; i < number; i++ ){
            int currfd = ev[i].data.fd;

            if ( currfd == server_soc ){
                socklen_t client_addrlength = sizeof( client );
                client_soc = accept( server_soc, ( struct sockaddr* )&client, &client_addrlength);
                if(client_soc < 0){
                    cout<<"cant accept"<<endl;
                }
                
                // addfd 
                epoll_event event;
                event.data.fd = client_soc;
                event.events = EPOLLIN;
                int e = epoll_ctl( epollfd, EPOLL_CTL_ADD, client_soc, &event );
                if(e < 0){
                    cerr<< "Err"<<endl;
                }
            }
            else if (ev[i].events & EPOLLIN){
                FILE *file = fopen("test.txt", "a");
                for(int i = 1; i < 21; i++){
                    int bytRec = recv(currfd, buffer, 4096, 0);
                    
                    if(bytRec > 0){
                        string msg = string(buffer,0,bytRec);
                        cout << "msg : " << msg << endl;

                        int x;
                        sscanf(msg.data(), "%d", &x);
                        unsigned long f = fact(x);

                        string s_val = to_string(f);

                        string final = "IP : " + to_string(client.sin_addr.s_addr) + " Port Add: " + to_string(client.sin_port) + " Factorial : " + s_val;

                        fprintf(file, "%s\n", final.c_str());

                        cout<<  "string : " << s_val<< endl;
                        strcpy(buffer , s_val.c_str());
                        cout << "msg : " << string(buffer,0,bytRec) << endl;

                        send(currfd, buffer, bytRec, 0);
                    }
                }
                fclose(file);
            }
        }
    }

    return 0;
}


unsigned long fact(int n){
    unsigned long factorial = 1.0;

    if (n < 0)
        cout << "Error! Factorial of a negative number doesn't exist.";
    else {
        for(int i = 1; i <= n; ++i) {
            factorial *= i;
        }
        return factorial;   
    }

    return 0;
}

