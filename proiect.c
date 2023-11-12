#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

// void citire_director(char *director){
//     DIR *dir;
//     dir = opendir(director);
//     struct dirent *entry;
//     struct stat st_file;
//     char str[500];

//     while(entry = readdir(dir) != NULL)
//     {
//         printf("%s", entry->d_name);
//         sprintf(str, "%s/%s", director, entry->d_name);
//         stat(str, &st_file);
//     }
// }

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Argumente incorecte.\n");
        exit(-1);

    }

    char *filename = argv[1];
    struct stat st_input;

    if(stat(filename, &st_input) == -1)
    {
        printf("error file");
        exit(-1);
    }

    if(!S_ISREG(st_input.st_mode))
    {
        printf("not regular file");
        exit(-1);
    }

    int file_descriptor;
    if((file_descriptor = open(filename, O_RDONLY)) == -1)
    {
        printf("%s\n", strerror(errno));
        printf("Nu s-a putut deschide fisierul.\n");
        exit(-1);
    }


    lseek(file_descriptor, 16, SEEK_SET);
    int width;
    read(file_descriptor, &width, sizeof(int));
    int height;
    read(file_descriptor, &height, sizeof(int));

    if(close(file_descriptor)==-1)
    {
        printf("error close");
        exit(1);
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
    sprintf(finalString, "nume fisier : %s\ninaltime : %d\nlatime : %d\ndimensiune : %d\nidentificatorul utilizatorului : %d\n"
    "timpul ultimei modificari : %s\ncontorul de legaturi : %d\ndrepturi de acces user : %s\ndrepturi de acces grup : %s\n"
    "drepturi de acces altii : %s\n", filename, height, width, st_input.st_size,  st_input.st_uid, ctime(&st_input.st_ctime),
    st_input.st_nlink, userAccess, groupAccess, otherAccess);

    int output_descriptor;
    if((output_descriptor = open("statistica.txt", O_WRONLY, O_APPEND)))
    {
        printf("Nu s-a putut deschide fisierul.\n");
        exit(-1);
    }

    if(write(output_descriptor, finalString, strlen(finalString)) == -1)
    {
        printf("error write");
        exit(1);
    }

    if(close(output_descriptor)==-1)
    {
        printf("error close");
        exit(1);
    }

    printf("Succes\n");

    return 0;

    

    //citire_director(argv[1]);
}