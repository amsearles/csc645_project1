#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define RCVBUFSIZE 1000
void DieWithError(char *errorMessage)
{
    perror(errorMessage) ;
    exit(1);
}
//void HandleTCPClient(int clntSocket, char *echoBuffer); /* TCP client handling function */

int main(int argc, char *argv[])
{
    int servSock, clntSock, msgNum = 0, clntNum = 0,login = 0, i, echoStrLen; 
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort; /* Server port */
    unsigned int clntLen; /* Length of client address data structure */
    long recvMsgSize = 1; 
    char users[2][20], passwords[2][20], choice[2], username[100], password[100], recipient[100], sender[100], msg[1000], msgStat[100], echoBuffer[RCVBUFSIZE], sizeBuffer[10];
    strcpy(users[0], "Alice");
    strcpy(users[1], "Bob");
    strcpy(passwords[0], "12345");
    strcpy(passwords[1], "12345");
    if (argc != 2) /* Test for correct number of arguments */
    {
     fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]) ;
     exit(1);
     }
    
   
    
    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError( "socket () failed") ;
    
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(8000); /* Local port */
    
    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError ( "bind () failed");
    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed") ;
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);
        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");
        /* clntSock is connected to a client! */
        clntNum++;
        printf("Client %d connected!\n", clntNum);
        
        if ((recvMsgSize = recv(clntSock, choice, 2, 0)) < 0)
            DieWithError("recv() failed");
        choice[recvMsgSize] = '\0';
        
        /* Send received string and receive again until end of transmission */
        while (recvMsgSize > 0) /* zero indicates end of transmission */
        {
            /* Echo message back to client */
            memset(echoBuffer, 0, RCVBUFSIZE);
            if(login ==1){
                if ((recvMsgSize = recv(clntSock, choice, 2, 0)) < 0){
                    DieWithError("recv() failed");}}
            if(strcmp(choice, "0")==0){
                if ((recvMsgSize = recv(clntSock, username, RCVBUFSIZE, 0)) < 0)
                    DieWithError("recv() failed: unverifiable un") ;
                printf("Login username is %s\n", username);
                if ((recvMsgSize = recv(clntSock, password, RCVBUFSIZE, 0)) < 0)
                    DieWithError("recv() failed: unverifiable pw");
                printf("Login password is %s\n", password);
                strcpy(echoBuffer, "login fail\n");
                for(i = 0; i < 2; i++){
                    if(strcmp(username, users[i])==0){
                        if(strcmp(password, passwords[i]) == 0){
                            strcpy(echoBuffer, "login success\n");
                            login=1;}}}
                printf("%s", echoBuffer);
                echoStrLen = strlen(echoBuffer);
                if (send(clntSock, echoBuffer, echoStrLen, 0) != echoStrLen){
                    DieWithError("send() failed: Login Status");}
            }
            else if(strcmp(choice, "1")==0){
                snprintf(sizeBuffer, 10, "%d", i);
                strcpy(echoBuffer, "There are ");
                strcat(echoBuffer, sizeBuffer);
                strcat(echoBuffer, " users:\n");
                for(i=0; i< 2; i++){
                    strcat(echoBuffer, users[i]);
                    strcat(echoBuffer, "\n");}
                echoStrLen = strlen(echoBuffer);
                if(send(clntSock, echoBuffer, echoStrLen, 0) != echoStrLen)
                    DieWithError("send() failed");
                printf("Return User List!\n");
            } else if(strcmp(choice, "2")==0){
                if ((recvMsgSize = recv(clntSock, recipient, RCVBUFSIZE, 0)) < 0){
                    DieWithError("recv() failed: unverifiable rec");}
                strcpy(sender, username);
                if ((recvMsgSize = recv(clntSock, msg, RCVBUFSIZE, 0)) < 0){
                    DieWithError("recv() failed: unverifiable msg");}
                msg[recvMsgSize] = '\0';
                printf("A message to %s\n", recipient);
                printf("Message is %s\n", msg);
                strcpy(msgStat, "Message sent successfully!");
                echoStrLen = strlen(msgStat);
                msgStat[strlen(msgStat)] = '\0';
                if(send(clntSock, msgStat, strlen(msgStat), 0) != strlen(msgStat))
                    DieWithError("send() failed");
                memset(msgStat, 0, 100);
            } else if(strcmp(choice, "3")==0){
                strcpy(echoBuffer, "You have ");
                if(strcmp(recipient, username)==0){
                    msgNum+=1;
                    char noS[2];
                    snprintf(noS, 2, "%d", msgNum);
                    strcat(echoBuffer, noS);
                    strcat(echoBuffer, " message(s)!\n");
                    strcat(echoBuffer, msg);
                    memset(msg, 0, 1000);
                } else {
                    strcat(echoBuffer, "0 message(s)!\n");
                }
                echoStrLen = strlen(echoBuffer);
                echoBuffer[echoStrLen] = '\0';
                if(send(clntSock, echoBuffer, echoStrLen, 0) != echoStrLen){
                    DieWithError("send() failed");}
                msgNum = 0;
                printf("Sent back %s's message!\n", recipient);
            }
        }
        printf("Client disconnected!\n");
        close(clntSock);
        memset(username, 0, 100);
        memset(password, 0, 100);
        
        login = 0;
    }
    /* NOT REACHED */
}
