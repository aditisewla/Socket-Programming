#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>


using namespace std;

void* create_client(void *newSoc){
    string ip = "127.0.0.1";
    int port = 54000;

    int client_soc;

    client_soc = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &client.sin_addr);

    int conRes = connect(client_soc, (sockaddr*)&client, sizeof(client));

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
    return NULL;
}

int main(){

    for(int i = 0; i < 10; i++){
        pthread_t id;

        pthread_create(&id, NULL, create_client, NULL);
        sleep(1);
    }
    pthread_exit(NULL);

    return 0;
}