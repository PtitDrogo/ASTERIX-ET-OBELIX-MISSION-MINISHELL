#include <stdio.h>
#include <dirent.h>
#include <string.h>

int ls() 
{
    DIR *dir;
    struct dirent *entity;
    int counter;

    counter = 0;
    dir = opendir(".");
    if (dir == NULL)
        return (1);
    entity = readdir(dir);
    while (entity)
    {
        printf("%s ", entity->d_name);
        counter++;
        if (counter == 4)
        {    
            printf("\n");
            counter = 0;
        }
        entity = readdir(dir);
    }
    closedir(dir);
    return (0);
}


