#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char * rootPath = argv[1];
    char * filePath = argv[2];

    //if ( 0 != strcmp() ) {
        
    //}

    int pathLength = strlen(rootPath) + strlen(filePath);
    char * path = malloc(sizeof(char) * (pathLength  + 1));
    strcat(path, rootPath);
    strcat(path, filePath);

    printf("Hello! %s \n", path);

    free(path);

    return 0;
}
