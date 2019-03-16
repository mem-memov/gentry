#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct FilePath;
struct FilePath * FilePath_construct(const char * root, const char * relative);
void FilePath_destruct(struct FilePath * this);
struct File * FilePath_createFile(struct FilePath * this);

struct File;
struct File * File_construct(char * path);
void File_destruct(struct File * this);
unsigned char File_hasNextCharacter(struct File * this);
unsigned char File_getNextCharacter(struct File * this);

struct FilePath
{
    const char * root;
    const char * relative;
};

struct FilePath * FilePath_construct(const char * root, const char * relative)
{
    struct FilePath * this = malloc(sizeof(struct FilePath));
    
    this->root = root;
    this->relative = relative;

    return this;
}

void FilePath_destruct(struct FilePath * this)
{
    free(this);
    this = NULL;
}

struct File * FilePath_createFile(struct FilePath * this)
{
    int pathLength = strlen(this->root) + strlen(this->relative);
    char * path = malloc(sizeof(char) * (pathLength  + 1));
    strcat(path, this->root);
    strcat(path, this->relative);

    return File_construct(path);
}

struct File {
    char * path;
    FILE * descriptor;
    int character;
};

struct File * File_construct(char * path)
{
    struct File * this = malloc(sizeof(struct File));

    this->path = path;
    this->descriptor = NULL;
    this->character = 0;

    return this;
}

void File_destruct(struct File * this)
{
    free(this->path);    

    free(this);
    this = NULL;
}

unsigned char File_hasNextCharacter(struct File * this)
{
    if ( NULL == this->descriptor ) {
        this->descriptor = fopen(this->path, "r");
    }

    this->character = fgetc(this->descriptor);

    if ( feof(this->descriptor) ) {
        fclose(this->descriptor);
        this->descriptor = NULL;
        return 0;
    }

    return 1;
}

unsigned char File_getNextCharacter(struct File * this)
{
    return this->character;
}

int main(int argc, char *argv[])
{
    char * rootPath = argv[1];
    char * filePath = argv[2];

    unsigned char character;

    struct FilePath * path = FilePath_construct(rootPath, filePath);
    struct File * file = FilePath_createFile(path);

    while (File_hasNextCharacter(file)) {
        character = File_getNextCharacter(file);
        printf("%c \n", character);
    }

    File_destruct(file);

    return 0;
}
