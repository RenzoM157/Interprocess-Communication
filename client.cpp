#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <algorithm>
// Source: Dr. Rincon's code from BlackBoard

using namespace std;
int main(int argc, char *argv[])
{
    int sockfd, portno, n,numProcess, empty = 0, process;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    int pipefd[numProcess+ 1][2];
    int pid;
    int i;
    int token;
    
    char buffer[256];
    if (argc < 3) {
        cout << "usage hostname port" << argv[0] << endl;
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        cout << "ERROR opening socket";
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        cout << "ERROR, no such host\n";
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        cout << "ERROR connecting";
    n = write(sockfd,&empty,sizeof(empty));
    if (n < 0)
        cout << "ERROR writing to socket";
    n = read(sockfd,&numProcess,sizeof(numProcess));
    if (n < 0)
        cout << "ERROR reading from socket";
    close(sockfd);
    numProcess -= 1;
    
    
    for (i = 0; i <= numProcess; i++){
        if (pipe(pipefd[i]) < 0)
        {
            cout << "ERROR: Pipe Creation failed\n";
            exit(1);
        }
    }
    
    for (i = 0; i < numProcess ; i++){
        
        if ((pid=fork())==0)
        {
            while(true){
                
                read(pipefd[i][0], &token, sizeof(token) );
                if (token == -1)
                {
                    token = -1;
                    write(pipefd[i+1][1],&token,sizeof(token));
                    _exit(0);
                }
                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
                    cout << "ERROR connecting";
                n = write(sockfd,&empty,sizeof(int));
                if (n < 0)
                    cout << "ERROR writing to socket";
                n = read(sockfd,&process,sizeof(int));
                close(sockfd);
                if (n < 0)
                    cout << "ERROR reading from socket";
                if (token >= 0 && process == 1){
                    cout << "Process " << i + 1 << " is using the network" << endl;
                }
                if (process == -1)
                {
                    token = -1;
                    write(pipefd[i+1][1],&token,sizeof(token));
                    _exit(0);
                }
                token = token + 1;
                write(pipefd[i+1][1],&token,sizeof(token));
            }
        }
        close (pipefd[i][0]);
        close(pipefd[i + 1][1]);
    }
    
    token = 0;
    
    while(true)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
            cout << "ERROR connecting";
        n = write(sockfd,&empty,sizeof(int));
        if (n < 0)
            cout << "ERROR writing to socket";
        n = read(sockfd,&process,sizeof(int));
        close(sockfd);
        if (n < 0)
            cout << "ERROR reading from socket";
        if (process == 1){
            cout << "Process 0 is using the network" << endl;
        }
        if (process==-1)
        {
            token = -1;
            write(pipefd[0][1],&token,sizeof(token));
            break;
        }
        token += 1;
        
        
        write(pipefd[0][1],&token, sizeof(int));
        
        read(pipefd[numProcess][0], &token, sizeof(int));
        
        if (token == -1)
        {
            token = -1;
            write(pipefd[0][1],&token, sizeof(int));
            break;
        }
    }
    
    for(i=0; i < numProcess; i++)
        wait(NULL);
}
