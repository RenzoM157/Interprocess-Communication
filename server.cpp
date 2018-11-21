#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <algorithm>
//Source: Dr. Rincon's code from BlackBoard.

using namespace std;
int main(int argc, char *argv[])
{
    
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n, numOfProcess, contents, empty;
    int counter = 0;
   
    vector<int> mainVec;
    
    cin >> numOfProcess;
    
    while(cin >> contents)
    {
        mainVec.push_back(contents);
    }
    
    if (argc < 2) {
        cout << "ERROR, no port provided\n";
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        cout << "ERROR opening socket";
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if ((::bind(sockfd, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr))) < 0)
        cout << "ERROR on binding";
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
    if (newsockfd < 0)
        cout << "ERROR on accept";
    n = read(newsockfd,&empty,sizeof(int));
    if (n < 0) cout<< "ERROR reading from socket";
    n = write(newsockfd, &numOfProcess, sizeof(numOfProcess));
    if (n < 0) cout << "ERROR writing to socket";
    close(newsockfd);
    
    
    while(true)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if (newsockfd < 0) cout << "ERROR on accept";
        bzero(buffer,256);
        n = read(newsockfd,&empty,sizeof(int));
        if (n < 0) cout << "ERROR reading from socket";
        n = write(newsockfd, &mainVec[counter], sizeof(int));
        close(newsockfd);
        if (mainVec[counter] == -1){
            break;
        }
        counter++;
    }
    
    close(sockfd);
    return 0;
}

