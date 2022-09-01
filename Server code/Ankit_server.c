//Created by Ankit Kunwar
//Subject ASP
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#define PORT 12345


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


//Implementation of the noop command
void noop(char buffer[])
{
    strcat(buffer , "\n------------- I am alive -------------\n");
}

//Implementation of the CDUP command
void cdup(char buffer[],char folder[])
{
    if( (folder=getcwd(NULL, 0)) == NULL)
    {
        strcpy(buffer , "\n-------------Failed to get the current directory-------------\n");
        return;
    }
    else
    {
            
        strcat(buffer , "\n-------------Directory changed to the parent-------------\n");
        chdir("..");
    }
}


void write_file(int socketfd)
{
    int n;
    FILE *file_ptr;
    char buffer[1000];
    char filename[] = "file2.txt";
    file_ptr = fopen(filename,"w");
    if(!file_ptr)
    {
        strcpy(buffer , "\n-------------Error in opening the file.-------------\n");
        return;
    }
    while(1)
    {
        n=recv(socketfd,buffer,1000,0);
        if(n<=0)
        {
            return;
        }
        fprintf(file_ptr,"%s", buffer);
        printf("%s ", buffer);
        bzero(buffer,1000);
           
        
    }
    return;
}

//Implementation of the LIST command
void list(char buffer[]) {
    
    struct dirent *dir;
    DIR *object;
    strcat(buffer , "\n------------- List of files and directories in the current directory -------------\n");
    object = opendir(".");
    if(object)
    {
        while( (dir = readdir(object) ) != NULL )
        {
            strcat(buffer , dir->d_name );
            strcat(buffer , "\n");
        }
    }
}

//Implementation of the PWD command
void pwd(char buffer[]) {
    char command_list[PATH_MAX];
    
    if(getcwd(command_list, sizeof(command_list)))
    {
        strcat(buffer , "\nWorking Directory is\t:\t");
        strcat(buffer , command_list);
        strcat(buffer , "\n");
    }
    else
    {
        strcpy(buffer , "\n------------- Issue in the retrival of the current directory-------------\n");
        return;
    }
}

//Implementation of the CWD command
void cwd(char buffer[] , char arr[]) {
    if( arr[3] != ' ')
    {
        strcat(buffer , "\n------------- 504 This form of command is not implemented yet.-------------\n");
        return;
    }
    int length = strlen(arr);
    length = length - 4;
    char address[length];
    strncpy(address , arr + 4 , length);
    address[length] = '\0';
    if (chdir(address) != 0)
        strcat(buffer , "\n-------------  No directory or file exist.  -------------\n");
}

//Implementation of the MKD command
void mkd(char buffer[] , char arr[])
{
    //printf("Inside mkd\n");
    if( arr[3] != ' ') {
        strcat(buffer , "\n-------------504 Implemention is still pending. Team is working on it. -------------\n");
        return;
    }
    int length = strlen(arr);
    length = length - 4;
    char address[length];
    int cnt , cmt;
    for(cnt = 0; cnt < length; cnt++ )
    {
        if(arr[cnt+4] == '/' && cnt != length)
            cmt = cnt;
        if(arr[ cnt + 4 ] == ' ') {
            strcat(buffer ,"\n-------------Path is not correct.-------------\n");
            return;
        }
        
        address[cnt] = arr[cnt+4];
    }
    address[cmt] = '\0';
    char dirName[length - cmt];
    strncpy(dirName , arr + 5 + cmt  , length - cmt);
    //printf("Inside mkd\n");
    if (chdir(address))
    {
        strcat(buffer , "\n-------------  No directory or file exist.  -------------\n");
        return;
    }
   // printf("Inside mkd\n");
    pwd(buffer);
    (mkdir(dirName , 0777) != -1) ? strcat(buffer , "\n-------------Directory has been created-------------\n"), pwd(buffer) : strcat(buffer , "\n-------------Error in Directory Creation-------------\n");
     
}

// Calling to remove the directory
void rmvDir(const char address[] , char buffer[]) {
    size_t path_len;
    char *full_address;
    DIR *dir;
    struct stat stat_path, stat_entry;
    struct dirent *entry;
    stat(address, &stat_path);
    if (!S_ISDIR(stat_path.st_mode))
    {
        strcat(buffer , "\n-------------  No directory or file exist.  -------------\n");
        return;
    }
    if (!(dir = opendir(address))) {
        strcat(buffer , "\n-------------  Issue while opening the provided directory -------------\n");
        return;
    }
    path_len = strlen(address);
    while ((entry = readdir(dir)) != NULL)
    {
        
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        
        full_address = calloc(path_len + strlen(entry->d_name) + 1, sizeof(char));
        strcpy(full_address, address);
        strcat(full_address, "/");
        strcat(full_address, entry->d_name);
        stat(full_address, &stat_entry);
        if (S_ISDIR(stat_entry.st_mode))
        {
            rmvDir(full_address , buffer);
            continue;
        }
        if(unlink(full_address))
            strcat(buffer , "\nCould not able to remove the file \t:\t"), strcat(buffer , full_address);
        else
            strcat(buffer , "\nFile has been removed\t\t:\t"), strcat(buffer , full_address);
    }
    if(rmdir(address))
        strcat(buffer , "\nCould not able to remove the Directory \t:\t"), strcat(buffer , full_address);
    else
        strcat(buffer , "\nDirectory has been removed\t:\t"), strcat(buffer , address);
    
    closedir(dir);
}

//Implementation of the RMD command
void rmd(char buffer[] , char arr[]) {
    if( arr[3] != ' ')
    {
        strcat(buffer , "\n-------------504 Implemention is still pending. Team is working on it. -------------\n");
        return;
    }
    int length = strlen(arr) - 4;
    //length = length - 4;
    char address[length];
    strncpy(address , arr + 4 , length);
    address[length] = '\0';
    rmvDir(address , buffer);
    
}

//Implementation of the DELE command
void dele(char buffer[] , char arr[]) {
    if( arr[4] != ' ')
    {
        strcat(buffer , "\n-------------504 Implemention is still pending. Team is working on it. -------------\n");
        return;
    }
    int length = strlen(arr) - 5;
    //length = length - 4;
    char address[length];
    strncpy(address , arr + 5 , length);
    address[length] = '\0';
    printf("%s the file to remove", address);
    
    if (remove(address) == 0) {
        
        strcpy(buffer , "\n-------------File removed sucessfully-------------\n");
        }
    else
    {
        strcat(buffer , "\n-------------File doesnot exist or issue during deletion -------------\n");
        return;
    }
    
}
//Users/alokkunwar/Documents


//Implementation of the STOR command
char fileName[50];
void stor(char buffer[] , char arr[])
{
    int length = strlen(arr);
    length-=5;
    strncpy(fileName , arr + 5 , length);
    fileName[length] = '\0';
    FILE *file_ptr = fopen(fileName , "w");
    if (file_ptr == NULL)
    {
        strcat(buffer , fileName), strcat(buffer , "\t:\t does not exist.\n");
        return;
    }
    strcpy(buffer , "\n-------------225 Connection has been opened and transfer is in progress-------------\n");
    fclose(file_ptr);
}

//Implementation of the RETR command
void retr(char buffer[] , char arr[] ) {
    int length = strlen(arr) - 5;
    strncpy(fileName , arr + 5 , length);
    fileName[length] = '\0';
    FILE *file_ptr = fopen(fileName , "r");
    if (file_ptr == NULL)
    {
        strcat(buffer , fileName), strcat(buffer , "\t:\t does not exist.\n");
        return;
    }
    strcpy(buffer , "\n------------- 226 Connection has been opened and transfer is in progress. -------------\n");
    fclose(file_ptr);
}

//Checking the login status for the user
void fun_for_error(int flag, char buffer[])
{
    if(flag == 0)
        strcat(buffer , "\n-------------  530 Please insert user id and password for login  -------------\n");
     if(flag == 1)
        strcat(buffer , "\n-------------  331 Please insert the password  -------------\n");
}

//Making the decision which method have to call

int query_Search(char* query_string)
{

    if(query_string[0] == 'U' && query_string[1] == 'S' && query_string[2] == 'E' && query_string[3] == 'R')
        return 2;
    else if(query_string[0] =='C' && query_string[1] =='D' && query_string[2] =='U' && query_string[3] =='P')
        return 3;
    else if(query_string[0] =='N' && query_string[1] =='O' && query_string[2] =='O' && query_string[3] =='P')
        return 4;
    else if(query_string[0] == 'P' && query_string[1] == 'A' && query_string[2] == 'S' && query_string[3] == 'S')
         return 5;
    else if(query_string[0] == 'P' && query_string[1] == 'W' && query_string[2] == 'D')
        return 6;
    else if(query_string[0] == 'M' && query_string[1] == 'K' && query_string[2] == 'D')
        return 7;
    else if(query_string[0] == 'C' && query_string[1] == 'W' && query_string[2] == 'D')
        return 8;
    else if(query_string[0] == 'R' && query_string[1] == 'E' && query_string[2] == 'T' && query_string[3] == 'R')
        return 9;
    else if(query_string[0] == 'R' && query_string[1] == 'M' && query_string[2] == 'D')
        return 10;
    else if(query_string[0] == 'S' && query_string[1] == 'T' && query_string[2] == 'O' && query_string[3] == 'R')
        return 11;
    else if(query_string[0] == 'L' && query_string[1] == 'I' && query_string[2] == 'S' && query_string[3] == 'T')
        return 12;
    else if(query_string[0] == 'A' && query_string[1] == 'B' && query_string[2] == 'O' && query_string[3] == 'R')
        return 13;
    else if(query_string[0] == 'Q' && query_string[1] == 'U' && query_string[2] == 'I' && query_string[3] == 'T')
        return 14;
    else if(query_string[0] == 'D' && query_string[1] == 'E' && query_string[2] == 'L' && query_string[3] == 'E')
        return 15;
    else
        return 0;
        
       
}

int index_v = 0,flag = 0;

//based on the specific query specific method is being called.
void intermediate_fun(int accept_Output_val, char buffer[])
{
    char address[500] = "";
    strcpy(address , buffer);
    strcpy(buffer , "\n-------------200 Correct command  -------------\n");
   // int v=0;
    if(accept_Output_val == 2)
    {
        if( flag == 1 || flag == 2)
        {
            strcpy(buffer , "\n-------------503 Command is not valid. Please provide the corrcect command -------------\n");
            return;
        }
        if( address[4] != ' ')
        {
            strcpy(buffer , "\n-------------503 Command is not valid. Please provide the corrcect command -------------\n");
            return;
        }
        char userID[10];
        strncpy(userID , address + 5 , 9);
        static const char filename[] = "credential_file.txt";
        FILE *file_ptr = fopen ( filename, "r" );
        char each_line [ 100 ], temp_id[10];
        while ( fgets ( each_line, sizeof each_line, file_ptr ) != NULL )
        {
            strncpy(temp_id , each_line , 9);
            temp_id[9] = '\0';
            index_v++;
            if ( strcmp( temp_id , userID ) == 0 )
            {
                flag = 1;
                break;
            }
        }
        fclose ( file_ptr );
        if ( flag == 1)
            strcat(buffer , "\n-------------  331 Please insert the password  -------------\n");
        else
        {
            strcpy(buffer , "\n------------- Do not have access to the system -------------\n");
            index_v = 0;
            return;
        }
    }
    else if(accept_Output_val == 3)
    {
        if(flag == 0 || flag==1)
        {
            fun_for_error(flag,buffer);
            return;
        }
        cdup(buffer,address), pwd(buffer);
    }
        
    
    else if(accept_Output_val == 4)
    {
        if(flag == 0 || flag==1)
        {
            fun_for_error(flag,buffer);
            return;
        }
        noop(buffer);
    }
    else if ( accept_Output_val == 5)
    {
        if( flag == 0 || flag == 2)
        {
            strcpy(buffer , "\n-------------503 Command is not valid. Please provide the corrcect command -------------\n");
            return;
        }
        if( address[4] != ' ')
        {
            strcpy(buffer , "\n-------------503 Command is not valid. Please provide the corrcect command -------------\n");
            return;
                        
        }
        for(int ct = 5; ct < 11; ct++ )
        {
            if( (address[ct] - '0') != ct - 4 )
            {
                strcat(buffer , "\n------------- Wrong Password  -------------\n");
                return;
            }
        }
        strcat(buffer , "\n-------------  Login successful. Run the command now-------------\n");
        char passrd[9];
        static const char filename[] = "credential_file.txt";
        flag = 2;
        strncpy(passrd , address + 5 , 8);
        FILE *file = fopen ( filename, "r" );
        int count = 1;
        char each_line [ 100 ], pass[9];
        while ( fgets ( each_line, sizeof each_line, file ) != NULL )
        {
            if( count == index_v )
            {
              strncpy(pass , each_line + 10 , 8), pass[8] = '\0';
              if ( strcmp( pass , passrd ) == 0 )
                flag = 2;
              break;
            }
            count += 1;
        }
        fclose ( file );
        if(flag!=2)
        {
            strcpy(buffer , "\n------------- Wrong Password  -------------\n");
             return;
        }
                         
    }
    else if ( accept_Output_val == 7)
    {
        if(flag == 0 || flag==1)
        {
            fun_for_error(flag,buffer);
            return;
        }
      //  printf("I am here\n");
        mkd(buffer , address);
    }
    else if ( accept_Output_val == 6)
    {
        if(flag == 0 || flag==1)
        {
            fun_for_error(flag,buffer);
            return;
        }
        if( strlen(address) > 3)
        {
            strcpy(buffer , "\n-------------503 Command is not valid. Please provide the corrcect command -------------\n");
            return;
        }
        if( address[3] == ' ')
        {
            strcat(buffer , "\n-------------504 Implemention is still pending. Team is working on it. -------------\n");
            return;
        }
            pwd(buffer);
     }
    
    else if ( accept_Output_val == 8)
    {
        if(flag == 0 || flag==1)
        {
            fun_for_error(flag,buffer);
            return;
        }
            pwd(buffer);
            cwd(buffer, address);
            pwd(buffer);
     }
    else if ( accept_Output_val == 9)
    {
        if(flag == 0 || flag==1)
        {
            fun_for_error(flag,buffer);
            return;
        }
        if( address[4] != ' ')
        {
            strcpy(buffer , "\n-------------503 Command is not valid. Please provide the corrcect command -------------\n");
            return;
        }
            retr(buffer , address);
     }
     else if ( accept_Output_val == 10)
     {
         if(flag == 0 || flag==1)
         {
            fun_for_error(flag,buffer);
            return;
         }
            rmd(buffer , address);
     }
     else if ( accept_Output_val == 11)
     {
         if(flag == 0 || flag==1)
         {
             fun_for_error(flag,buffer);
             return;
         }
         if( address[4] != ' ')
         {
             strcpy(buffer , "\n-------------503 Command is not valid. Please provide the corrcect command -------------\n");
             return;
         }
            stor(buffer , address);
     }
     else if ( accept_Output_val == 12)
     {
         if(flag == 0 || flag==1)
         {
             fun_for_error(flag,buffer);
             return;
         }
         if( strlen(address) > 4)
         {
             strcpy(buffer , "\n-------------503 Command is not valid. Please provide the corrcect command -------------\n");
             return;
         }
         if( address[4] == ' ')
         {
             strcat(buffer , "\n-------------504 Implemention is still pending. Team is working on it. -------------\n");
             return;
         }
            list(buffer);
     }
     else if ( accept_Output_val == 13)
     {
         if(flag == 0 || flag==1)
         {
             fun_for_error(flag,buffer);
             return;
         }
         if( strlen(address) > 4)
         {
             strcat(buffer , "\n-------------503 Command is not valid. Please provide the corrcect command -------------\n");
                   return;
         }
         if( address[4] == ' ')
         {
             strcat(buffer , "\n-------------504 Implemention is still pending. Team is working on it. -------------\n");
             return;
         }
        flag = 0;
        strcat(buffer , "\n-------------551 Program aborted. Please Login. -------------\n");
        index_v = 0;
     }
     else if ( accept_Output_val == 14)
     {
         strcpy(buffer ,  "\n-------------  Connection has been terminated  -------------\n");
         return;
     }
     else if(accept_Output_val == 15)
     {
         if(flag == 0 || flag==1)
         {
             fun_for_error(flag,buffer);
             return;
         }
         if( address[4] != ' ')
         {
             strcpy(buffer , "\n-------------503 Command is not valid. Please provide the corrcect command -------------\n");
             return;
         }
         dele(buffer,address);
     }
     
}

//Driver code which is calling the method
void driver_code(int Output_val) {
    char buffer[10000] = "";
    char temp[10000] = " ";
    while(1)
    {
      
        if( strcmp(temp , "\n-------------225 Connection has been opened and transfer is in progress-------------\n") == 0)
        {
            FILE *file_ptr = fopen(fileName, "w");
            printf("File is %s",fileName );
            printf("%s\n", fileName);
            if(file_ptr != NULL)
            {
                bzero(buffer, sizeof(buffer));
                while (1)
                {
                    read(Output_val , buffer , sizeof(buffer)), fputs(buffer , file_ptr);
                    if ( strlen(buffer) - 1 != sizeof(buffer) )
                        break;
                    bzero(buffer, sizeof(buffer));
                }
                printf("\n-------------File has been received successfully-------------\n");
                fclose(file_ptr), bzero(temp , sizeof(temp));
            }
            else
                printf("Cannot open file %s \n", fileName);
        }
        bzero(buffer, sizeof(buffer)), read(Output_val , buffer , sizeof(buffer));
        printf("\nMessage Received\t:\t%s\n" , buffer);
        
        //Here the query has been seearched
        int accept_Output_val = query_Search(buffer);
        printf("value is %d \n", accept_Output_val);
         (accept_Output_val == 0 ) ? strcpy(buffer , "502     Command not implemented\n") : intermediate_fun(accept_Output_val , buffer); //Here we are calling the specif method.
        
        write(Output_val , buffer , strlen(buffer));
        if( strcmp( "\n-------------  Connection has been terminated  -------------\n" , buffer) == 0)
        {
            sleep(3);
            printf("%s" , buffer);
            exit(0);
        }
        printf("%s" , buffer),strcpy(temp , buffer);
        
       if ( strcmp(temp , "\n------------- 226 Connection has been opened and transfer is in progress. -------------\n") == 0 )
       {
            printf("\n-------------File is being send -------------\n");
            sleep(2);
            FILE *file_ptr = fopen(fileName, "r");
           if(file_ptr !=NULL)
           {
               char char_val;
               bzero(buffer, sizeof(buffer));
               int i = 0;
               for(;1;) {
                   while ( ( (char_val = fgetc(file_ptr)) != EOF ) && ( strlen(buffer) - 1 != sizeof(buffer)) )
                       buffer[i++] = char_val;
                   buffer[i] = '\0';
                   if ( char_val == EOF)
                   {
                       write(Output_val , buffer, strlen(buffer));
                       break;
                   }
                   if ( strlen(buffer) == sizeof(buffer) )
                   {
                       write(Output_val , buffer, strlen(buffer)), bzero(buffer, sizeof(buffer));
                       i = 0;
                   }
                   
               }
               printf("\n-------------File has been send sucessfully -------------\n");
               fclose(file_ptr);
           }
           else
               printf("Issue with sending the file\t:\t%s \n", fileName);
        }
    }
}

//Main method for server
int main()
{
   
    int  bind_val , c_len, socket_discriptor;
    int accept_val ;
    struct sockaddr_in client_details;
    struct sockaddr_in server_details;
    socket_discriptor = socket(AF_INET,SOCK_STREAM,0); //socket
    if(socket_discriptor>=0)
        printf("\n-------------Socket has been created.. now going to bind-------------\n");
    else
        printf("\n-------------Issue while creating the socket.-------------\n"), exit(0);
    
    //setting the value
    server_details.sin_family=AF_INET;
    server_details.sin_addr.s_addr=htonl(INADDR_ANY);
    server_details.sin_port=htons(PORT);
    
    bind_val = bind(socket_discriptor,(struct sockaddr *)&server_details, sizeof(server_details));
    if(bind_val>=0)
        printf("\n-------------Binding has been done..waiting for the client-------------\n");
    else
        printf("\n-------------Issue with the binding-------------\n"), exit(0);
    listen(socket_discriptor , 5 ); //listening
    for(;1;)
    {
        c_len = sizeof(client_details);
        accept_val = accept(socket_discriptor,(struct sockaddr *)&client_details , &c_len); //Accepting the connection
        if(accept_val>=0)
            printf("\n-------------Connection made with client..Please login-------------\n");
        else
            printf("\n-------------Issue while making the connection with client.-------------\n"), exit(0);
        pid_t process_id;
        if((process_id = fork()) == 0)
        {
            close(socket_discriptor);
            driver_code(accept_val);
            close(accept_val);
            exit(0);
        }
            close(accept_val);
        }
    return 0;
}




