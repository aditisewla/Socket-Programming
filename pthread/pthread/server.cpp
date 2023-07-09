#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>


using namespace std;
void *handel_connection(void *newSoc);
unsigned long fact(int n);
FILE *file;

int main(){
    int server_soc, client_soc;

    server_soc = socket(AF_INET, SOCK_STREAM, 0);
    if(server_soc < 0){
        cerr<< "Cant create the socket" <<endl;
    }

    sockaddr_in server, client;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(54000);

    memset(server.sin_zero, 8, 0);

    unsigned len = sizeof(sockaddr_in);
    int b = bind(server_soc, (sockaddr*)&server, len);
    if(b < 0){
        cerr<< "Cant Bind" << endl;
    }

    int lis = listen(server_soc, 10);
    if(lis < 0){
        cerr<<"cant listen";
    }

    int newSoc;
    
    for(int i = 0; i < 11; i++){
        newSoc = accept(server_soc, (sockaddr*)&client, &len);
        if(newSoc < 0){
            cerr<< "Cant create the socket" <<endl;
        }
        file = fopen("text.txt", "a");

        pthread_t t;
        int *pClinet = (int*) malloc(sizeof(int));
        *pClinet = newSoc;
        pthread_create(&t, NULL, handel_connection, pClinet);

    }

    close(client_soc);

    return 0;
} 


void *handel_connection(void *newSoc){
    char buffer[4096];
    int soc = (*(int*)newSoc);

    for(int i = 1; i < 21; i++){
        int bytRec = recv(soc, buffer, 4096, 0);
        cout<< "byte : "<< bytRec << endl;

        if(bytRec > 0){
            string msg = string(buffer,0,bytRec);
            cout << "msg : " << msg << endl;

            int x;
            sscanf(msg.data(), "%d", &x);
            unsigned long f = fact(x);

            string s_val = to_string(f);

            // string final = "IP : " + to_string(client.sin_addr.s_addr) + " Port Add: " + to_string(client.sin_port) + " Factorial : " + s_val;

            // fprintf(file, "%s\n", final.c_str());
            string final =  "Factorial : " + s_val;

            fprintf(file, "%s\n", final.c_str());

            cout<<  "string : " << s_val<< endl;
            strcpy(buffer , s_val.c_str());
            cout << "msg : " << string(buffer,0,bytRec) << endl;

            send(soc, buffer, bytRec, 0);
        }
    }
    close(soc);

    pthread_exit(NULL);
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
