#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct FilePath;
struct FilePath * FilePath_construct(const char * root, const char * relative);
void FilePath_destruct(struct FilePath * this);
char * FilePath_composeFullPath(struct FilePath * this);

struct File;
struct File * File_construct(char * path);
void File_destruct(struct File * this);
unsigned char File_hasNextCharacter(struct File * this);
unsigned char File_getNextCharacter(struct File * this);

struct Tag;

struct Element;

struct Document;

struct DocumentBuilder;
struct DocumentBuilder * DocumentBuilder_construct(struct File * file);
void DocumentBuilder_destruct(struct DocumentBuilder * this);
struct Document * DocumentBuilder_createDocument(struct DocumentBuilder * this);

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

char * FilePath_composeFullPath(struct FilePath * this)
{
    int pathLength = strlen(this->root) + strlen(this->relative);
    char * path = malloc(sizeof(char) * (pathLength  + 1));
    strcat(path, this->root);
    strcat(path, this->relative);
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

struct Tag
{
    char * name;
    unsigned char isClosing;
};

struct Tag * Tag_construct(char * name, unsigned char isClosing)
{
    struct Tag * this = malloc(sizeof(struct Tag));

    int nameLength = strlen(name);
    this->name = malloc(sizeof(unsigned char) * (nameLength + 1));
    strcpy(this->name, name);

    printf("%s\n", this->name);

    this->isClosing = isClosing;

    return this;
}

void Tag_destruct(struct Tag * this)
{
    free(this->name);

    free(this);
    this = NULL;
}

struct Element
{
    char * name;
    struct Element * parent;
};

struct Element * Element_construct(char * name, struct Element * parent)
{
    struct Element * this = malloc(sizeof(struct Element));

    int nameLength = strlen(name);
    this->name = malloc(sizeof(unsigned char) * (nameLength + 1));
    strcpy(this->name, name);

    this->parent = parent;

    return this;
}

void Element_destruct(struct Element * this)
{
    free(this->name);

    free(this);
    this = NULL;
}

struct Document
{
    struct Element * root;
};

struct Document * Document_construct()
{
    struct Document * this = malloc(sizeof(struct Document));

    this->root = Element_construct("document", NULL);

    return this;
}

void Document_destruct(struct Document * this)
{
    free(this);
    this = NULL;
}

void Document_addTag(struct Document * this, struct Tag * tag)
{
    
}

struct DocumentBuilder
{
    struct File * file;
};

struct DocumentBuilder * DocumentBuilder_construct(struct File * file)
{
    struct DocumentBuilder * this = malloc(sizeof(struct DocumentBuilder));

    this->file = file;

    return this;
}

void DocumentBuilder_destruct(struct DocumentBuilder * this)
{
    free(this);
    this = NULL;
}

struct Document * DocumentBuilder_createDocument(struct DocumentBuilder * this)
{
    unsigned char character;
    unsigned char characters[256];
    unsigned char tagDetected;
    unsigned char isTagClosing;
    int tagLength;
    int tagCount;
    struct Tag * tag;

    struct Document * document = Document_construct();

    tagCount = 0;
    tagDetected = 0;
    while (File_hasNextCharacter(this->file)) {

        character = File_getNextCharacter(this->file);

        if ( '<' == character ) {
            tagDetected = 1;
            tagLength = 0;
            isTagClosing = 0;
            continue;
        }
        if ( '>' == character ) {
            tagDetected = 0;
            characters[tagLength] = '\0';
            tag = Tag_construct(characters, isTagClosing); 
            Document_addTag(document, tag);
            continue;
        }
        if ( tagDetected && 0 == tagLength && '/' == character ) {
            isTagClosing = 1;
            continue;
        }
        if ( tagDetected ) {
            characters[tagLength] = character;
            tagLength = tagLength + 1;
        }
    }

    return document;
}

int main(int argc, char *argv[])
{
    char * rootPath = argv[1];
    char * filePath = argv[2];

    struct FilePath * path = FilePath_construct(rootPath, filePath);
    struct File * file = File_construct( FilePath_composeFullPath(path) );
    struct DocumentBuilder * builder = DocumentBuilder_construct(file);
    struct Document * document = DocumentBuilder_createDocument(builder);

    Document_destruct(document);
    DocumentBuilder_destruct(builder);
    File_destruct(file);
    FilePath_destruct(path);

    return 0;
}
