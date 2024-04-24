#include <stdio.h>
#include <dirent.h>
#include <string.h>

int main(void) 
{
    DIR *dir;
    struct dirent *entity;
    
    dir = opendir(".");
    if (dir == NULL)
        return (1);
    entity = readdir(dir);
    while (entity)
    {
        printf("%s\n", entity->d_name);
        entity = readdir(dir);
    }
    closedir(dir);
    return (0);
}


