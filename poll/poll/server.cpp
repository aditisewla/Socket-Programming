#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sys/poll.h>
#include <sys/ioctl.h>

using namespace std;
unsigned long fact(int n);

int main(){ 
    int server_soc, client_soc;
    server_soc = socket(AF_INET, SOCK_STREAM, 0);
    int max_connections= 10;

    sockaddr_in server , client;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(54000);
    unsigned len = sizeof(sockaddr_in);

    memset(server.sin_zero, len, 0);

    int b = bind(server_soc, (sockaddr*)&server, len);
    int lis = listen(server_soc, 10);

    char buffer[4096];
    int num_fds = 0;
    int max_fd = 0;

    pollfd pollfds[max_connections];
    pollfds[0].fd = server_soc;
    pollfds[0].events = POLLIN;
    num_fds++;

    // update_maxfd(server_soc, &max_fd); 
    max_fd = server_soc;  
    

    while (true){
        int fd_ind;
        int read;
        FILE *file = fopen("test.txt", "a");

        poll(pollfds, num_fds, 10000);
        
        for(fd_ind =0; fd_ind < num_fds; fd_ind++){
            if(pollfds[fd_ind].revents && POLLIN){
                if(pollfds[fd_ind].fd == server_soc){
                    client_soc = accept(server_soc, (sockaddr*)&client, &len);
                    pollfds[num_fds].fd = client_soc;
                    pollfds[num_fds].events = POLLIN;
                    num_fds++;
                    max_fd = client_soc;
                }
                else{
                    if(read == 0){
                        close(pollfds[fd_ind].fd);
                        num_fds--;
                        pollfds[fd_ind].events = 0;
                        pollfds[fd_ind].fd = -1;
                        close(client_soc);
                    }
                    else{
                        for(int i = 1; i < 21; i++){
                            int bytRec = recv(client_soc, buffer, 4096, 0);
                            // cout<< "byte : "<< bytRec << endl;

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

                                send(client_soc, buffer, bytRec, 0);
                                memset(buffer, bytRec, 0);
                            }
                        }
                    }
                }
            }
        }
        fclose(file);
    }
    close(client_soc);
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
