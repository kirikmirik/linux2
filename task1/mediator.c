#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h> 

#define DEVICE_FILE_NAME "/dev/phonebook"
#define COMMAND_SIZE 256
#define RESULT_SIZE 256

int main(int argc, char* argv[]){
    int fd;
    char message[COMMAND_SIZE];
    char result[RESULT_SIZE];

    fd = open(DEVICE_FILE_NAME, O_RDWR);
    if (fd < 0) 
    {
        printf ("Can't open device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }

    while(1)
    {
        printf("> ");
        if(!fgets(message, COMMAND_SIZE, stdin))
        {
          printf("command reading error\n");
          continue;
        } 
        if (message[0]=='\0' || message[0]=='\n') continue;
        if (!strncmp(message, "exit", 4))
        {
            close(fd);
            return 0;
        }
        write(fd, message, COMMAND_SIZE);
        read(fd, result, RESULT_SIZE);
        printf("%s\n",result);
    }
    close(fd);
    return 0;
}
