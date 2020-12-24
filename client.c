#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<math.h>

int hammingReceiver(int[],int ,int[]);
int hammingError(int [],int );         //converts error binary array to decimal which gives the position where error is passed
void hammingData(int [],int,int []); //removes hamming bits from dataframe
int hammingBits(int );        //gives number of hamming bits inserted in the dataframe
int binary_to_value(int []);      //converting 8 bit binary to character


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;            //variable declaration
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);//host and port where server is running
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");         //error message
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)     &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));      //writing message into socket
    if (n < 0)
         error("ERROR writing to socket");
    
    bzero(buffer,256);
    char input[100],select[100];
    FILE *fptr,*final_out;
    fptr=fopen("output_client.txt","a");    //fileChar file
    if(fptr==NULL){
        printf("FILE NOT EXIST");
    }
    final_out=fopen("./fileChar.txt","a");  //opening output file
    if(final_out==NULL){
        printf("File Not Found");                //file not found message
        exit(0);
    }
    printf("data is before accepting\n");
    while((n=read(sockfd,buffer,1))>0)
    {
        if(strcmp(buffer,"r")==0 || buffer[0]=='r'){
            break;
        }
        printf("%s",buffer);
        fprintf(fptr,"%s",buffer);
       
    }
    if (n < 0)
         error("ERROR reading from socket");
    close(sockfd);
    fclose(fptr);
    fptr=fopen("output_client.txt","r");
    int currentframe=0,numberbits,bin_length=0;
    char ch;
    int data[522],dataframe[522],binary[8];
    while(1){
        int length=0;
        while(length<522){
            ch=fgetc(fptr);
            if(ch==EOF){
                break;
            }
            dataframe[length]=ch-48;
            length++;
        }
        for(int i=0;i<length;i++){           //copying only data into data array leavning 22,22,size of data
            data[i]=dataframe[i];
        }
        int error=hammingReceiver(data,length,dataframe);
        if(error>=0){
            printf("******************************************************\n");
            printf("Error detected and corrected at position %d in dataframe %d\n",error,currentframe); //printing where the error occured and also corrected
            printf("******************************************************\n");
        }
        numberbits=hammingBits(length);
        printf("ActualData\n");
        for(int i=0;i<length-numberbits;i++){
            printf("%d",dataframe[i]);           //printing actual data
        }
        printf("\n");
        
        for(int i=0;i<length-numberbits;i++){
            if(bin_length<8){
                binary[bin_length]=dataframe[i];
                bin_length++;
            }
            if(bin_length==8){
                printf("%c",binary_to_value(binary));
                fprintf(final_out,"%c",binary_to_value(binary)); //adding decoded value to file
                bin_length=0;
            }
        }
        printf("\n\n\n");
        if(ch==EOF){
            break;
        }
        currentframe++;       //getting current frame
    }
    
    fclose(fptr);
    fclose(final_out);
    return 0;
}

int hammingReceiver(int codeword[],int length,int dataframe[]){ //adds hamming parity bits
int i,j=0,k,error[100],l=0,paritycounter=0,skipcounter=0,checkcounter=0,errorLen;
for(i=0;i<length;i++){
    if(i==(int)pow(2,j)-1){        //checks for parity bit based on pow(2,j)location
        paritycounter=0;
        skipcounter=0;
        checkcounter=0;
        for(k=i;k<length;k++){
            if(checkcounter==skipcounter){
                skipcounter=0;
                checkcounter=0;
            }
            if(checkcounter!=i+1){
                if(codeword[k]==1){
                    paritycounter++;         //checks for 1
                }
                checkcounter++;
                continue;
            }
            if(skipcounter!=i+1){
                skipcounter++;
                continue;
            }
        }
        error[l]=paritycounter%2;   //maintains even parity and stores the result to error array
        l++;
        j++;
    }
}
errorLen=l;
int errorbit=hammingError(error,errorLen);        //gives location where error is present
codeword[errorbit-1]=(errorbit-1>0)?(codeword[errorbit-1]^1):codeword[errorbit-1];//correcting the error
hammingData(codeword,length,dataframe); //removing hamming bits from dataframe
return errorbit-1;
}
int hammingError(int error[],int errorLen){         //converts error binary array to decimal which gives the position where error is passed
int i,j=0,sum=0;
for(i=0;i<errorLen;i++){
    sum+=(pow(2,j)*error[i]);
    j++;
}
return sum;
}
void hammingData(int codeword[],int length,int dataframe[]){  //removes hamming bits from dataframe
int data[1000],i,j=0,k=0,datalength;
for(i=0;i<length;i++){
    if(i==pow(2,j)-1){         //removes based on pow(2,j)
        j++;
        continue;
    }
    data[k]=codeword[i];
    k++;
}
datalength=k;
for(i=0;i<datalength;i++){
    dataframe[i]=data[i];
//        printf("%d",data[i]);
}
}
int hammingBits(int n){               //gives number of hamming bits inserted in the dataframe
int p=0;
while(1){
    if(n<=(int)pow(2,p)-1){
        break;
    }
    p++;
}
return p;
}
int binary_to_value(int binary[]){      //converting 8 bit binary to character
    int sum=0;
    for(int i=1;i<8;i++){
        sum+=(int)(binary[i]*pow(2,(7-i)));
    }
    return sum;
}
