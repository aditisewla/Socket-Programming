#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <sys/ioctl.h>
#include <fcntl.h>

using namespace std;
unsigned long fact(int n);

int main(){ 
    int server_soc, client_soc;

    server_soc = socket(AF_INET, SOCK_STREAM, 0);
    if(server_soc < 0){
        cerr<<"Cant create the socket"<<endl;
        return 0;
    }

    sockaddr_in server, client;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(54000);

    memset(server.sin_zero, 8, 0);

    unsigned len = sizeof(sockaddr_in);

    fcntl(server_soc, F_SETFL, O_NONBLOCK);

    int b = bind(server_soc, (sockaddr*)&server, len);
    if(b < 0){
        cerr<<"Cant bind"<<endl;
        return 0;
    }

    int lis = listen(server_soc, 5);

    if(lis < 0){
        cerr << "cant bind"<<endl;
        return 0;
    }

    char buffer[4096];

    fd_set current_soc, ready_soc;
    FD_ZERO(&current_soc);
    FD_SET(server_soc, &current_soc);

    while(true){
        FILE *file = fopen("test.txt", "a");
        ready_soc = current_soc;

        int s = select(FD_SETSIZE, &ready_soc, NULL, NULL, NULL);
        if(s < 0){
            cerr << "select err";
            return 0;
        }

        for(int i = 0; i < FD_SETSIZE; i++){
            if(FD_ISSET(i, &ready_soc)){
                if(i == server_soc){
                    client_soc = accept(server_soc, (sockaddr*)&client, &len);
                    cout<<"connected";
                    fcntl(client_soc, F_SETFL, O_NONBLOCK);
                    FD_SET(client_soc, &current_soc);
                }
            }
            else{
                for(int i = 1; i < 21; i++){
                    int bytRec = recv(client_soc, buffer, 4096, 0);

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
                    }
                }
                FD_CLR(i, &current_soc);
            }
        }
        fclose(file);
        close(client_soc);
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



