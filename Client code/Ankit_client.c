//Created by Ankit Kunwar
//Subject ASP
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdbool.h>
#define PORT_NO 12345

//Structure of the command Password and user name are being stored in the credential_file.txt file
//USER sdfs
//PASS sdfds
//CDUP
//PWD
//ABOR
//QUIT
//LIST
//RMD /Users/alokkunwar/Documents/twete
//CWD /Users/alokkunwar/Downloads
//MKD /Users/alokkunwar/Documents/tw1
//NOOP
//DELE /Users/alokkunwar/Documents/testing.txt
//STOR testing.txt
//RETR testing.txt



void send_file(FILE *file_ptr, int socketfd)
{
    char buffer[1000] ={0};
    while(fgets(buffer,1000,file_ptr)!=NULL)
    {
        if(send(socketfd, buffer,sizeof(buffer),0)==-1)
        {
            printf("\nErroe has been occured\n");
            return;
        }
        bzero(buffer,1000);
    }
}

//Main method for driving the code
int main()
{
    int socket_File_Descriptor;
    struct sockaddr_in socket_Address_str;
    socket_File_Descriptor = socket(AF_INET,SOCK_STREAM,0); //socket creation
    bzero(&socket_Address_str,sizeof(socket_Address_str));
    if(socket_File_Descriptor >= 0)
        printf("\n-------------Socket  has been created -------------\n");
    else
        printf("\n-------------Fail to create the Socket -------------\n"), exit(0);
    
    //setting the value
    socket_Address_str.sin_addr.s_addr=htonl(INADDR_ANY);
    socket_Address_str.sin_family=AF_INET;
    socket_Address_str.sin_port=htons(PORT_NO);
    
    int value = connect(socket_File_Descriptor,(struct sockaddr *)&socket_Address_str,sizeof(socket_Address_str)); //connecting with server
    if(value>=0)
        printf("\n-------------Connection has been made. Please login -------------\n");
    else
        printf("\n-------------Error while creating the connection -------------\n"), exit(0);
    
    char space_buffer[10000] = "";
    char fileName[30];
    for(;1;) {
        
        bzero(space_buffer, sizeof(space_buffer));
        printf("\n$\t:\t");
        scanf("%[^\n]%*c" , space_buffer); //taking the command for input
        write(socket_File_Descriptor , space_buffer, strlen(space_buffer));
        printf("\nData which has been sent\t->\t%s\n" , space_buffer);
        if ( strlen(space_buffer) > 7) {
            int len = strlen(space_buffer) - 5;
            strncpy(fileName , space_buffer + 5 , len);
            fileName[len] = '\0';
        }
        read(socket_File_Descriptor , space_buffer , sizeof(space_buffer));
        int cnd = strcmp(space_buffer ,"\nConnection Termination\n");
        if( cnd == 0 ) {
            printf("\n-------------Connection has been terminated -------------\n");
            break;
        }
        //FOR STOR
        if( strcmp(space_buffer , "\n-------------225 Connection has been opened and transfer is in progress-------------\n") == 0)
        {
            printf("%s" , space_buffer);
            printf("\n-------------File transfer has been started -------------\n");
            FILE *file_ptr = fopen(fileName, "r");
            if(file_ptr)
                send_file(file_ptr, socket_File_Descriptor);
            else
                printf("Issue while opening the file\t:\t%s \n", fileName);
            sleep(2);
            file_ptr = fopen(fileName, "r");
            if(file_ptr)
            {
                char val;
                bzero(space_buffer, sizeof(space_buffer));
                int i = 0;
                for(;1;) {
                    val = fgetc(file_ptr);
                    bool condition1 =  val != EOF;
                    bool condition2 = ( strlen(space_buffer) - 1 != sizeof(space_buffer));
                    while (condition1  && condition2 ) {
                        space_buffer[i++] = val;
                    }
                    space_buffer[i] = '\0';
                    if ( val == EOF) {
                        write(socket_File_Descriptor , space_buffer, strlen(space_buffer));
                        break;
                    }
                    if ( strlen(space_buffer) == sizeof(space_buffer) ) {
                        write(socket_File_Descriptor , space_buffer, strlen(space_buffer));
                        bzero(space_buffer, sizeof(space_buffer));
                        i = 0;
                    }
                }
                printf("\n-------------File has been transfered successfully -------------\n");
                fclose(file_ptr);
            }
            else
                printf("Issue while opening the file\t:\t%s \n", fileName);
            
        }
      //FOR RETR
        else if ( strcmp(space_buffer , "\n------------- 226 Connection has been opened and transfer is in progress. -------------\n") == 0)
        {
            printf("%s" , space_buffer);
            FILE *file_ptr;
            file_ptr = fopen(fileName, "w");
            //printf("I am here\n");
            if (file_ptr)
            {
                bzero(space_buffer, sizeof(space_buffer));
                for(;1;)
                {
                    printf("going to read %s\n", space_buffer);
                    read(socket_File_Descriptor , space_buffer , sizeof(space_buffer));
                    printf("%s" , space_buffer);
                    fputs(space_buffer , file_ptr);
                    
                    if ( strlen(space_buffer) != sizeof(space_buffer) ) {
                        break;
                    }
                    bzero(space_buffer, sizeof(space_buffer));
                }
                printf("\n-------------File has been received successfully -------------\n");
                fclose(file_ptr);
            }
            else
                printf("Issue while opening the file\t:\t%s \n", fileName);
        }
       else
            printf("%s" , space_buffer);
        
    }
    close(socket_File_Descriptor);

    return 0;
}
