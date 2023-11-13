#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

char* extensie_fisier(char *filename) 
{
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) 
        return "";
    return dot + 1;
}

void citire_fisier(char *filename, char *output)
{
    if(strcmp("statistica.txt", filename) == 0)
        return;
    struct stat st_input;
    int is_bmp = 0;
    int width;
    int height;

    if(stat(filename, &st_input) == -1)
    {
        printf("error file\n");
        return;
    }

    if(!S_ISREG(st_input.st_mode))
    {
        printf("not regular file\n");
        return;
    }

    if(strcmp(extensie_fisier(filename), "bmp") == 0)
    {
        is_bmp = 1;
        int file_descriptor;
        if((file_descriptor = open(filename, O_RDONLY)) == -1)
        {
            printf("Nu s-a putut deschide fisierul.\n");
            exit(-1);
        }


        lseek(file_descriptor, 16, SEEK_SET);
        read(file_descriptor, &width, sizeof(int));
        read(file_descriptor, &height, sizeof(int));

        if(close(file_descriptor)==-1)
        {
            printf("error close\n");
            exit(1);
        }
    }

    char userAccess[4];
    userAccess[0] = (S_IRUSR == (st_input.st_mode) ? 'r' : '-');
    userAccess[1] = (S_IWUSR == (st_input.st_mode) ? 'w' : '-');
    userAccess[2] = (S_IXUSR == (st_input.st_mode) ? 'x' : '-');
    userAccess[3] = '\0';
    char groupAccess[4];
    groupAccess[0] = (S_IRGRP == (st_input.st_mode) ? 'r' : '-');
    groupAccess[1] = (S_IWGRP == (st_input.st_mode) ? 'w' : '-');
    groupAccess[2] = (S_IXGRP == (st_input.st_mode) ? 'x' : '-');
    groupAccess[3] = '\0';
    char otherAccess[4];
    otherAccess[0] = (S_IROTH == (st_input.st_mode) ? 'r' : '-');
    otherAccess[1] = (S_IWOTH == (st_input.st_mode) ? 'w' : '-');
    otherAccess[2] = (S_IXOTH == (st_input.st_mode) ? 'x' : '-');
    otherAccess[3] = '\0';
    char finalString[500];
    if(is_bmp)
    {
        sprintf(finalString, "nume fisier : %s\ninaltime : %d\nlatime : %d\ndimensiune : %ld\nidentificatorul utilizatorului : %d\n"
                "timpul ultimei modificari : %s\ncontorul de legaturi : %ld\ndrepturi de acces user : %s\ndrepturi de acces grup : %s\n"
                "drepturi de acces altii : %s\n", filename, height, width, st_input.st_size,  st_input.st_uid, ctime(&st_input.st_ctime),
                st_input.st_nlink, userAccess, groupAccess, otherAccess);
    }
    else
    {
        sprintf(finalString, "nume fisier : %s\ndimensiune : %ld\nidentificatorul utilizatorului : %d\n"
                "timpul ultimei modificari : %s\ncontorul de legaturi : %ld\ndrepturi de acces user : %s\ndrepturi de acces grup : %s\n"
                "drepturi de acces altii : %s\n\n\n", filename, st_input.st_size,  st_input.st_uid, ctime(&st_input.st_ctime),
                st_input.st_nlink, userAccess, groupAccess, otherAccess);
    }
    
    int output_descriptor;
    if((output_descriptor = open(output, O_WRONLY | O_APPEND)) == -1)
    {
        printf("Nu s-a putut deschide fisierul.\n");
        exit(-1);
    }

    if(write(output_descriptor, finalString, strlen(finalString)) == -1)
    {
        printf("error write\n");
        exit(1);
    }

    if(close(output_descriptor)==-1)
    {
        printf("error close\n");
        exit(1);
    }
}

void citire_director(char *director){
    DIR *dir;
    dir = opendir(director);
    struct dirent *entry;
    struct stat st_file;
    char str[500];

    while((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
        citire_fisier(entry->d_name, "statistica.txt");
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Argumente incorecte.\n");
        exit(-1);

    }

    citire_director(argv[1]);
    
    return 0;

}


/*
    pid_t pid;
    int status;
    for(int j = 0; j < 5; j++)
    {
        pid = fork();
    }
    if(pid < 0)
    {
        printf("error fork\n");
        exit(-1);
    }
    else if(pid == 0)
    {
        for(int i = 0; i < 10; i++)
        {
            printf("Child with pid =%d printing line%d\n", getpid(), i);
        }
        exit(j+1);
    }
    while((pid = wait(&status)) != -1)
    {
        if(WIFEXITED(status))
        {
            printf("pid = %d, status = %d", pid, WEXITSTATUS(status));
        }
    }
*/