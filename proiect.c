#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char *extensie_fisier(char *filename)
{
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot + 1;
}

void citire_fisier(char *filename, char *output)
{
    if (strcmp("statistica.txt", filename) == 0)
        return;
    struct stat st_input;
    int is_bmp = 0;
    int width;
    int height;

    if (stat(filename, &st_input) == -1)
    {
        printf("error file\n");
        return;
    }

    if (!S_ISREG(st_input.st_mode))
    {
        printf("not regular file\n");
        return;
    }

    if (strcmp(extensie_fisier(filename), "bmp") == 0)
    {
        is_bmp = 1;
        int file_descriptor;
        if ((file_descriptor = open(filename, O_RDONLY)) == -1)
        {
            printf("Nu s-a putut deschide fisierul.\n");
            exit(-1);
        }

        lseek(file_descriptor, 16, SEEK_SET);
        read(file_descriptor, &width, sizeof(int));
        read(file_descriptor, &height, sizeof(int));

        int pixel_size = 3;

        lseek(file_descriptor, 54, SEEK_SET);

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                unsigned char pixel[3];
                read(file_descriptor, &pixel, sizeof(unsigned char) * pixel_size);

                unsigned char grey = 0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2];
                pixel[0] = pixel[1] = pixel[2] = grey;

                lseek(file_descriptor, -pixel_size, SEEK_CUR);
                write(file_descriptor, &pixel, sizeof(unsigned char) * pixel_size);
            }
        }

        if (close(file_descriptor) == -1)
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
    if (is_bmp)
    {
        sprintf(finalString, "nume fisier : %s\ninaltime : %d\nlatime : %d\ndimensiune : %ld\nidentificatorul utilizatorului : %d\n"
                             "timpul ultimei modificari : %s\ncontorul de legaturi : %ld\ndrepturi de acces user : %s\ndrepturi de acces grup : %s\n"
                             "drepturi de acces altii : %s\n",
                filename, height, width, st_input.st_size, st_input.st_uid, ctime(&st_input.st_ctime),
                st_input.st_nlink, userAccess, groupAccess, otherAccess);
    }
    else
    {
        sprintf(finalString, "nume fisier : %s\ndimensiune : %ld\nidentificatorul utilizatorului : %d\n"
                             "timpul ultimei modificari : %s\ncontorul de legaturi : %ld\ndrepturi de acces user : %s\ndrepturi de acces grup : %s\n"
                             "drepturi de acces altii : %s\n\n\n",
                filename, st_input.st_size, st_input.st_uid, ctime(&st_input.st_ctime),
                st_input.st_nlink, userAccess, groupAccess, otherAccess);
    }

    int output_descriptor;
    char stat_file_name[100];
    sprintf(stat_file_name, "%s_statistica.txt", filename);
    if ((output_descriptor = creat(stat_file_name, S_IWUSR)) == -1)
    {
        printf("Nu s-a putut creea fisierul.\n");
        exit(-1);
    }

    if ((output_descriptor = open(output, O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, S_IWUSR)) == -1)
    {
        printf("Nu s-a putut deschide fisierul.\n");
        exit(-1);
    }

    if (write(output_descriptor, finalString, strlen(finalString)) == -1)
    {
        printf("error write\n");
        exit(1);
    }

    if (close(output_descriptor) == -1)
    {
        printf("error close\n");
        exit(1);
    }
}

void citire_director(char *director_input, char *director_output)
{
    DIR *dir;
    dir = opendir(director_input);
    struct dirent *entry;
    int status;
    pid_t pid;

    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
        char output_file_name[500];
        sprintf(output_file_name, "%s\\%s_statistica.txt", director_output, entry->d_name);
        pid = fork();
        if (pid == -1)
        {
            printf("Error forking process\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            citire_fisier(entry->d_name, output_file_name);
            exit(EXIT_SUCCESS);
        }
    }
    while (wait(&status) >= 0)
    {
        if (WIFEXITED(status))
        {
            printf("S-a incheiat procesul cu pid-ul %d si codul %d\n", pid, WEXITSTATUS(status));
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Argumente incorecte.\n");
        exit(-1);
    }

    citire_director(argv[1], argv[2]);

    return 0;
}