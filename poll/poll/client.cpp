#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>


using namespace std;

int main(){
    string ip = "127.0.0.1";
    int port = 54000;

    int client_soc;

    client_soc = socket(AF_INET, SOCK_STREAM, 0);
    if(client_soc < 0){
        cerr<< "Can't create the Client Socket";
    }

    sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &client.sin_addr);

    int conRes = connect(client_soc, (sockaddr*)&client, sizeof(client));
    if(conRes < 0){
        cerr<< "Can't connect to the server";
    }
    char buffer[4096];
    
    cout<< "You are connect to server \n" << endl;
    
    for(int i = 1; i < 21; i++){ 
        string tmp = to_string(i);
        strcpy(buffer , tmp.c_str());
        send(client_soc, buffer, sizeof(buffer), 0);

        int rec = recv(client_soc, buffer, sizeof(buffer), 0);
        if(rec > 0){
            cout << " server  > " << string(buffer, 0, rec) << endl;
        }
    }
    


    close(client_soc);
    return 0;
}
