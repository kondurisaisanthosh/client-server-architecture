#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <unistd.h>

void insertData(int,int ,int,int[]);  //inserting data in dataframe
int hammingTransmitter(int[],int);     //inserting hamming bits to dataframe


int main(int argc,char *args[]){
    FILE *iptr,*optr;
    iptr=fopen(args[1],"r");
    optr=fopen(args[2],"a");
    if(iptr==NULL){
        printf("FILE NOT EXIST");
    }
    int numberOfChars,dataframe[522],current,counter,start,hamminglength;
    char c;
    fseek(iptr,0,SEEK_END);
    numberOfChars=ftell(iptr);
    fseek(iptr,0,SEEK_SET);
    
    current=numberOfChars>=64?64:numberOfChars;
//    printf("before while loop of hamming");
    while(current>0)
    {
//        printf("1 - ");
        counter=0;
        start=0;
        while(counter<current)
        {
            if((c=fgetc(iptr))!=EOF){
                insertData(start,start+7,c,dataframe);  //appending each character to dataframe
                start+=8;
                counter++;
            }
        }
        int length=current*8;
        hamminglength=hammingTransmitter(dataframe,length);       //inserting hamming bits to dataframe
        for(int i=0;i<hamminglength;i++){
//            printf("%d",dataframe[i]);
            fprintf(optr,"%d",dataframe[i]);

        }
        numberOfChars-=64;
        current=numberOfChars>=64?64:numberOfChars; //remaining characters
    }

    fclose(iptr);
    fclose(optr);
//    printf("Exit of hamming");
}
void insertData(int start,int end,int number,int dataframe[])  //inserting data in dataframe
{
    int counter=0;
    while(number!=0){
        dataframe[end]=(number%2==0)?0:1;
        if(number%2==1){
            counter++;
        }
        number/=2;
        end--;
    }
    while(end>start){
        dataframe[end]=0;
        end--;
    }
    dataframe[start]=(counter%2)==0?1:0;
}

int hammingTransmitter(int dataframe[],int length){       //inserting hamming bits to dataframe
    int codeword[1000],i,j,k,checkcounter=0,paritycounter=0,skipcounter=0,codelength,current=length;
    j=0,k=0;
    for(i=0;k<length;i++){
        if(i==(int)pow(2,j)-1){
            codeword[i]=9;                 //inserting 9 instead of p in place of parity bits in dataframe
            j++;
            continue;
        }
        codeword[i]=dataframe[k];
        k++;
    }
    codelength=i;
    for(i=0;i<codelength;i++){               //performing hamming code operation
        if(codeword[i]==9){
            paritycounter=0;
            skipcounter=0;
            checkcounter=0;
            for(j=i;j<codelength;j++){
                if(checkcounter==skipcounter){
                    skipcounter=0;
                    checkcounter=0;
                }
                if(checkcounter!=i+1){
                    if(codeword[j]==1){
                        paritycounter++;
                    }
                    checkcounter++;
                    continue;
                }
                if(skipcounter!=i+1){
                    skipcounter++;
                    continue;
                }
            }
            codeword[i]=paritycounter%2;         //Even parity
        }
    }
    for(i=0;i<codelength;i++){
        dataframe[i]=codeword[i];             //appending codeword to dataframe
    }

    return codelength;       //returning frame length after inserting hamming bits

}
