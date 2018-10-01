#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#define RCVBUFSIZE 1000 /* Size of receive buffer */
#define MAXPENDING 5
void DieWithError(char *errorMessage)
{
    perror(errorMessage) ;
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock, chatSock, clntLen, clntSock; /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in echoClntAddr;
    unsigned short echoServPort; /* Echo server port */
    char servlP[100], username[100], username2[100], password[100], sstring[100], recipient[100], msg[1000], msg1[1000], sender[100]; /* Server IP address (dotted quad) */
    char *echoString; /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE], choice[1]; /* Buffer for echo string */
    unsigned int echoStringLen; /* Length of string to echo */
    int connected = 0, bytesRcvd, totalBytesRcvd, recvMsgSize; /* Bytes read in single recv() and total bytes read */
    
    
    if (argc == 4)
        echoServPort = atoi(argv[3]); /* Use given port, if any */
    else
        echoServPort = 7; /* 7 is the well-known port for the echo service */
    
    /* Receive the same string back from the server */
    totalBytesRcvd = 0;
    for(;;)
    {
        
        /* Receive up to the buffer size (minus i to leave space for a null terminator) bytes from the sender */
        printf("0. Connect to the server\n");
        printf("1. Get the user list\n");
        printf("2. Send a message\n");
        printf("3. Get my messages\n");
        printf("4. Initiate a chat with my friend\n");
        printf("5. Chat with my friend\n");
        printf("Your option (enter a number): ");
        scanf("%s", &choice);
        if(connected == 1){
            if(send(sock, choice, 1, 0) != 1){
                DieWithError("send() sent a different number of bytes than expected!");}
        }
        if(strcmp(choice, "0")==0){
            if(connected == 0){
                printf("Please enter the IP address: ");
                scanf("%s", &servlP);
                printf("Please enter the port number: ");
                scanf("%d", &echoServPort);
                printf("Connecting...\n");
                /* Create a reliable, stream socket using TCP */
                if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
                    DieWithError(" socket () failed") ;}
                /* Construct the server address structure */
                memset(&echoServAddr, 0, sizeof(echoServAddr));
                /* Zero out structure */
                echoServAddr.sin_family = AF_INET;
                /* Internet address family */
                echoServAddr.sin_addr.s_addr = inet_addr(servlP);
                /* Server IP address */
                echoServAddr.sin_port = htons(echoServPort); /* Server port */
                /* Establish the connection to the echo server */
                if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0){
                    DieWithError(" connect () failed");}
                connected = 1;
            }
            if(send(sock, choice, 1, 0) != 1){
                DieWithError("send() sent a different number of bytes than expected!");}
            printf("Connected!\nWelcome! Please log in.\nUsername: ");
            scanf("%s", &username);
            echoStringLen = strlen(username);
            if(send(sock, username, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}
            
            printf("Password: ");
            scanf("%s", &password);
            echoStringLen = strlen(password);
            if(send(sock, password, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}
            
            if((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE, 0)) < 0){
                DieWithError("recv() failed");}
            printf("%s\n", echoBuffer);
            if(strcmp(echoBuffer, "login fail\n")==0){
                exit(0);}
        } else if(strcmp(choice, "1")==0){
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0){
                DieWithError("recv() failed or connection closed prematurely");}
            echoBuffer[bytesRcvd] = '\0';
            printf("%s\n", echoBuffer);
        } else if(strcmp(choice, "2")==0){
            printf("Please enter the username: ");
            scanf("%s", recipient);
            echoStringLen = strlen(recipient);
            if(send(sock, recipient, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}
            printf("Please enter the message: ");
            scanf("%s", msg);
            fgets(msg1, 1000, stdin);
            strcat(msg, msg1);
            echoStringLen = strlen(msg);
            if(send(sock, msg, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0){
                DieWithError("recv() failed or connection closed prematurely");}
            echoBuffer[bytesRcvd] = '\0';
            printf("%s\n", echoBuffer);
        } else if(strcmp(choice, "3")==0){
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0){
                DieWithError("recv() failed or connection closed prematurely");}
            echoBuffer[bytesRcvd] = '\0';
            printf("%s\n", echoBuffer);
        } else if(strcmp(choice, "4")==0){
            close(sock);
            printf("-------------------disconnect from the server-------------------\n");
            printf("Please enter the port number you want to listen on: ");
            scanf("%d", &echoServPort);
            if ((chatSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
                DieWithError( "socket () failed");}
            echoServAddr.sin_port = htons(echoServPort);
            if (bind(chatSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0){
                DieWithError ( "bind () failed");}
            /* Mark the socket so it will listen for incoming connections */
            if (listen(chatSock, MAXPENDING) < 0){
                DieWithError("listen() failed") ;}
            printf("I am listening on %s:%d!\n", servlP, echoServPort);
            /* Set the size of the in-out parameter */
            clntLen = sizeof(echoClntAddr);
            /* Wait for a client to connect */
            if ((clntSock = accept(chatSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0){
                DieWithError("accept() failed");}
            printf("Client connected!\n");
            if ((recvMsgSize = recv(clntSock, username2, 100, 0)) < 0){
                DieWithError("recv() failed");}
            username2[recvMsgSize] = '\0';
            printf("%s is listening.\n", username2);
            echoStringLen = strlen(username);
            if(send(clntSock, username, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}
            while(1>0){
                if ((recvMsgSize = recv(clntSock, msg, 1000, 0)) < 0){
                    DieWithError("recv() failed");}
                msg[recvMsgSize] = '\0';
                printf("Type 'Bye' to stop the conversation!\n");
                printf("%s: %s", username2, msg);
                if(strcmp(msg, "Bye\n")==0){ break;}
                memset(msg, 0, 1000);
                printf("%s: ", username);
                scanf("%s", msg);
                fgets(msg1, 1000, stdin);
                strcat(msg, msg1);
                echoStringLen = strlen(msg);
                if(send(clntSock, msg, echoStringLen, 0) != echoStringLen){
                    DieWithError("send() sent a different number of bytes than expected!");}
                if(strcmp(msg, "Bye\n")==0) {break;}}
            close(chatSock);
        } else if(strcmp(choice, "5")==0){
            close(sock);
            printf("-------------------disconnect from the server-------------------\n");
            printf("Please enter your friend's IP address: ");
            scanf("%s", &servlP);
            printf("Please enter your friend's number: ");
            scanf("%d", &echoServPort);
            printf("Connecting to your friend...\n");
            /* Construct the server address structure */
            memset(&echoServAddr, 0, sizeof(echoServAddr));
            if ((chatSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
                DieWithError(" socket () failed") ;}
            /* Zero out structure */
            echoServAddr.sin_family = AF_INET;
            echoServAddr.sin_addr.s_addr = inet_addr(servlP);
            /* Server IP address */
            echoServAddr.sin_port = htons(echoServPort); /* Server port */
            /* Establish the connection to the echo server */
            if (connect(chatSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0){
                DieWithError(" connect () failed");}
            printf("Connected!\n");
            echoStringLen = strlen(username);
            if(send(chatSock, username, echoStringLen, 0) != echoStringLen){
                DieWithError("send() sent a different number of bytes than expected!");}
            if ((recvMsgSize = recv(chatSock, username2, 1000, 0)) < 0){
                DieWithError("recv() failed");}
            while(1>0){
                printf("Type 'Bye' to stop the conversation!\n");
                printf("%s: ", username);
                scanf("%s", msg);
                fgets(msg1, 1000, stdin);
                strcat(msg, msg1);
                echoStringLen = strlen(msg);
                if(send(chatSock, msg, echoStringLen, 0) != echoStringLen){
                    DieWithError("send() sent a different number of bytes than expected!");}
                if(strcmp(msg, "Bye\n")==0){ break;}
                if ((recvMsgSize = recv(chatSock, msg, 1000, 0)) < 0){
                    DieWithError("recv() failed");}
                msg[recvMsgSize] = '\0';
                printf("%s: %s", username2, msg);
                if(strcmp(msg, "Bye\n")==0){ break;}
                memset(msg, 0, 1000);}
            close(chatSock);
        } else {
            exit(0);
        }
        /*echoStringLen = strlen(echoString);
         if(send(sock, echoString, echoStringLen, 0) != echoStringLen)
         DieWithError("send() sent a different number of bytes than expected!");
         
         if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
         DieWithError("recv() failed or connection closed prematurely");
         totalBytesRcvd += bytesRcvd; /* Keep tally of total bytes */
        //echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
        /* Print the echo buffer */
        printf("----------------------------\n");
    }
    
    printf("\n"); /* Print a final linefeed */
    
    close(sock);
    exit(0);
}

