#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>


void error(const char *msg)                             //throws error message
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;                   //variable declaration
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");        //error if no port provided
        exit(1);
    }
    fprintf(stdout, "Run client by providing host and port\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding\n");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    while(1)
    {
//        printf("<<<< i >>>>>\n");
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
        if (newsockfd < 0)
           error("ERROR on accept\n");  //throws error if there is any issue in socket
        int pid1 = fork();
        if(pid1 < 0) {
            printf("ERROR While creating child\n");  //error on creating fork
        }
        else if (pid1 == 0) //child process
        {
            int presentSocketFd = newsockfd;
            bzero(buffer,256);
            n = read(presentSocketFd,buffer,255);
            if (n < 0) {
                error("ERROR reading from socket\n");
            }
            printf("Here is the message: %s\n",buffer);
            char request[100];
            strcpy(request,buffer);
            char element;
            int fn=0;
            for(int k=0;request[k]!='\0';k++){
                if(request[k]==' ' && request[k+2]==','){
                    element=request[k+1];       //getting requested character from client
                    fn=k+3;
                }
            }
            char filename[100];
            int j=0;
            for(int k=fn;request[k]!='\0';k++){
                filename[j]=request[k];
                j++;    //getting filename from request
            }
            filename[j-1]='\0';
            char c;
            int count=0;
            FILE *fptr,*tptr;
            fptr=fopen(filename,"r");
            tptr=fopen("temp.txt","a");
            if(fptr==NULL){
                printf("FILE NOT EXIST");//throws error if file doesnt exist
                exit(0);
            }
            if(tptr==NULL){
                printf("FILE NOT EXIST");
                exit(0);
            }
            
            while((c=fgetc(fptr))!=EOF){
                fprintf(tptr,"%c",toupper(c));
                if(c==element){
                    count++;
                }
            }
            fprintf(tptr,"\nThe character %c is %d times",element,count);
            fclose(tptr);
            fclose(fptr);           //closing files
            int pid2 = fork();
            if(pid2 < 0) {
                printf("ERROR While creating sub-child\n");
            } else if (pid2 == 0) { //child process
                char *args[]={"./hammingTransmitter","temp.txt","output.txt",NULL};
                execv(args[0],args);
            } else if (pid2 > 0) { //parent process
                wait(NULL);
//                printf("In main process of sub-thread");
            }
            printf("back to child server");
            FILE *optr;
            optr=fopen("output.txt","r");
            if(optr==NULL){
                printf("File not exist");
            }
            printf("Back to child");
            char ch;
            while((ch=fgetc(optr))!=EOF){
                printf("%c",ch);
                n = write(presentSocketFd,&ch,1); //Writes to client socket
            }
            ch='r';
            n = write(presentSocketFd,&ch,1); //Writes to client socket

//            printf("sai");
            close(presentSocketFd); //closing socket
            fclose(optr);
        }
        else if(pid1 > 0) {
//            wait(NULL);
            continue;
        }
    }
    
    close(sockfd);  //closing socket
    return 0;
}
