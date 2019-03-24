#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct FilePath;
struct File;
struct Tag;
struct Element;
struct Document;
struct DocumentBuilder;
struct Generator;
struct Text;
struct Type;
struct Property;
struct Properties;
struct Method;
struct Methods;
struct Class;


struct FilePath * FilePath_construct(const char * root, const char * relative);
void FilePath_destruct(struct FilePath * this);
char * FilePath_composeFullPath(struct FilePath * this);

struct File * File_construct(char * path);
void File_destruct(struct File * this);
unsigned char File_hasNextCharacter(struct File * this);
unsigned char File_getNextCharacter(struct File * this);

struct Tag * Tag_construct(char * name, unsigned char isClosing);
void Tag_destruct(struct Tag * this);
unsigned char Tag_canCreateElement(struct Tag * this);
struct Element * Tag_createElement(struct Tag * this, struct Element * parent);

struct Element * Element_construct(char * name, struct Element * parent);
void Element_destruct(struct Element * this);
unsigned char Element_isSame(struct Element * this, struct Element * that);
struct Element * Element_getParent(struct Element * this);
void Element_addChild(struct Element * this, struct Element * child);
void Element_generateCode(struct Element * this, struct Generator * generator);
unsigned char Element_hasName(struct Element * this, char * name);

struct Document * Document_construct();
void Document_destruct(struct Document * this);
void Document_addTag(struct Document * this, struct Tag * tag);
void Document_writeText(struct Document * this, unsigned char character);
void Document_generateCode(struct Document * this, struct Generator * generator);

struct DocumentBuilder * DocumentBuilder_construct(struct File * file);
void DocumentBuilder_destruct(struct DocumentBuilder * this);
struct Document * DocumentBuilder_createDocument(struct DocumentBuilder * this);

struct Generator * Generator_construct();
void Generator_destruct(struct Generator * this);
void Generator_useElement(struct Generator * this, struct Element * element);
void Generator_write(struct Generator * this);

struct Type * Type_construct(struct Element * element);
void Type_destruct(struct Type * this);

struct Property * Property_construct();
void Property_destruct(struct Property * this);
void Property_setType(struct Property * this, struct Element * element);
void Property_setName(struct Property * this, struct Element * element);

struct Properties * Properties_construct();
void Properties_destruct(struct Properties * this);
void Properties_add(struct Properties * this);
void Properties_useElement(struct Properties * this, struct Element * element);
void Properties_write(struct Properties * this);

struct Methods * Methods_construct();
void Methods_destruct(struct Methods * this);

struct Class * Class_construct(
    struct Properties * properties,
    struct Methods * methods
);
void Class_destruct(struct Class * this);
void Class_addProperty(struct Class * this);
void Class_useElement(struct Class * this, struct Element * element);
void Class_write(struct Class * this);


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
}

unsigned char Tag_canCreateElement(struct Tag * this)
{
    if ( this->isClosing ) {
        return 0;
    }

    return 1;
}

struct Element * Tag_createElement(struct Tag * this, struct Element * parent)
{
    if ( this->isClosing ) {
        exit(1);
    }

    struct Element * element = Element_construct(this->name, parent);

    Element_addChild(parent, element);

    return element;
}

struct Element
{
    char * name;
    struct Element * parent;
    struct Element ** children;
    int childCount;
    char * text;
};

struct Element * Element_construct(char * name, struct Element * parent)
{
    struct Element * this = malloc(sizeof(struct Element));

    int nameLength = strlen(name);
    this->name = malloc(sizeof(unsigned char) * (nameLength + 1));
    strcpy(this->name, name);

    this->parent = parent;
    this->children = NULL;
    this->childCount = 0;
    this->text = NULL;

    return this;
}

void Element_destruct(struct Element * this)
{
    int index;

    for (index = 0; index < this->childCount; ++index) {
        Element_destruct(this->children[index]);
    }

    free(this->name);

    if ( NULL != this->text ) {
        free(this->text);
    }

    free(this);
}

unsigned char Element_isSame(struct Element * this, struct Element * that)
{
    if ( this == that ) {
        return 1;
    }

    return 0;
}

struct Element * Element_getParent(struct Element * this)
{
    return this->parent;
}

void Element_addChild(struct Element * this, struct Element * child)
{
    int index;

    struct Element ** children = malloc(sizeof(struct Element *) * (this->childCount + 1));

    for (index = 0; index < this->childCount; ++index) {
        children[index] = this->children[index];
    }

    children[this->childCount] = child;

    if ( 0 != this->childCount ) {
        free(this->children);
    }

    this->children = children;

    this->childCount = this->childCount + 1;
}

unsigned char Element_canContainText(struct Element * this)
{
    if ( 0 == strcmp(this->name, "text") ) {
        return 1;
    }

    return 0;
}

void Element_appendCharacter(struct Element * this, char character)
{
    char * text;
    int textLength;

    if ( 0 != strcmp(this->name, "text") ) {
        exit(1);
    }

    if ( NULL == this->text ) {
        this->text = malloc(sizeof(char) * 2);
        this->text[0] = character;
        this->text[1] = '\0';
        return;
    }

    textLength = strlen(this->text);
    text = malloc(sizeof(char) * (textLength + 2));
    strcpy(text, this->text);
    text[textLength] = character;
    text[textLength + 1] = '\0';

    free(this->text);
    this->text = text;

    printf("%s\n", this->text);
}

void Element_generateCode(struct Element * this, struct Generator * generator)
{
    Generator_useElement(generator, this);

    int index;

    for (index = 0; index < this->childCount; ++index)
    {
        Element_generateCode(this->children[index], generator);
    }
}

unsigned char Element_hasName(struct Element * this, char * name)
{
    if ( 0 == strcmp(this->name, name) ) {
        return 1;
    }

    return 0;
}

struct Document
{
    struct Element * root;
    struct Element * lastElement;
};

struct Document * Document_construct()
{
    struct Document * this = malloc(sizeof(struct Document));

    this->root = Element_construct("document", NULL);
    this->lastElement = this->root;

    return this;
}

void Document_destruct(struct Document * this)
{
    if ( Element_isSame(this->root, this->lastElement) ) {
        Element_destruct(this->root);
    } else {
        Element_destruct(this->root);
        Element_destruct(this->lastElement);
    }

    free(this);
}

void Document_addTag(struct Document * this, struct Tag * tag)
{
    if ( Tag_canCreateElement(tag) ) {
        this->lastElement = Tag_createElement(tag, this->lastElement);
    } else {
        this->lastElement = Element_getParent(this->lastElement);
    }
}

void Document_writeText(struct Document * this, unsigned char character)
{
    if ( Element_canContainText(this->lastElement) ) {
        Element_appendCharacter(this->lastElement, character);
    }
}

void Document_generateCode(struct Document * this, struct Generator * generator)
{
    Element_generateCode(this->root, generator);
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
            continue;
        }
        if ( ! tagDetected ) {
            Document_writeText(document, character);
        }
    }

    return document;
}

struct Generator
{
    struct Class * class;
};

struct Generator * Generator_construct(struct Class * class)
{
    struct Generator * this = malloc(sizeof(struct Generator));

    this->class = class;

    return this;
}

void Generator_destruct(struct Generator * this)
{
    Class_destruct(this->class);

    free(this);
}

void Generator_useElement(struct Generator * this, struct Element * element)
{
    if (Element_hasName(element, "property")) {
        Class_addProperty(this->class);
    }

    Class_useElement(this->class, element);
}

void Generator_write(struct Generator * this)
{
    Class_write(this->class);
}

struct Text
{
    char ** lines;
};

struct Type
{
    struct Element * name;
};

struct Type * Type_construct(struct Element * element)
{
    struct Type * this = malloc(sizeof(struct Type));

    this->name = element;

    return this;
}

void Type_destruct(struct Type * this)
{
    free(this);
}

struct Property
{
    struct Type * type;
    struct Element * name;
};

struct Property * Property_construct()
{
    struct Property * this = malloc(sizeof(struct Property));

    this->type = NULL;
    this->name = NULL;

    return this;
}

void Property_destruct(struct Property * this)
{
    Type_destruct(this->type);

    free(this);
}

void Property_setType(struct Property * this, struct Element * element)
{
    this->type = Type_construct(element);
}

void Property_setName(struct Property * this, struct Element * element)
{
    this->name = element;
}

struct Properties
{
    struct Property ** items;
    int length;
};

struct Properties * Properties_construct()
{
    struct Properties * this = malloc(sizeof(struct Properties));

    this->items = NULL;
    this->length = 0;

    return this;
}

void Properties_destruct(struct Properties * this)
{
    int index;

    for (index = 0; index < this->length; ++index)
    {
        Property_destruct(this->items[index]);
    }

    if ( NULL != this->items ) {
        free(this->items);
    }

    free(this);
}

void Properties_add(struct Properties * this)
{
    struct Property ** items = malloc(sizeof(struct Property *) * (this->length + 1));

    int index;

    for (index = 0; index < this->length; ++index)
    {
        items[index] = this->items[index];
    }

    items[this->length] = Property_construct();

    free(this->items);

    this->items = items;
    this->length = this->length + 1;
}

void Properties_useElement(struct Properties * this, struct Element * element)
{
    struct Property * property = this->items[this->length - 1];

    if ( Element_hasName(element, "name") ) {
        Property_setName(property, element);
    }

    if ( Element_hasName(element, "type") ) {
        Property_setType(property, element);
    }
}

void Properties_write(struct Properties * this)
{

}

struct Argument
{
    struct Type * type;
    char * name;
};

struct Arguments
{
    struct Agrument ** items;
    int length;
};

struct Method
{
    struct Type * type;
    char * name;
    struct Arguments * arguments;
};

struct Methods
{
    struct Method ** items;
    int length;
};

struct Methods * Methods_construct()
{
    struct Methods * this = malloc(sizeof(struct Methods));

    this->items = NULL;
    this->length = 0;

    return this;
}

void Methods_destruct(struct Methods * this)
{
    if ( NULL != this->items ) {
        free(this->items);
    }

    free(this);
}

struct Class
{
    struct Properties * properties;
    struct Methods * methods;
    unsigned char isAddingProperty;
    unsigned char isAddingMethod;
};

struct Class * Class_construct(
    struct Properties * properties,
    struct Methods * methods
) {
    struct Class * this = malloc(sizeof(struct Class));

    this->properties = properties;
    this->methods = methods;
    this->isAddingProperty = 0;
    this->isAddingMethod = 0;

    return this;
}

void Class_destruct(struct Class * this)
{
    Properties_destruct(this->properties);
    Methods_destruct(this->methods);

    free(this);
}

void Class_addProperty(struct Class * this)
{
    this->isAddingProperty = 1;
    this->isAddingMethod = 1;

    Properties_add(this->properties);
}

void Class_useElement(struct Class * this, struct Element * element)
{
    if (this->isAddingProperty) {
        Properties_useElement(this->properties, element);
    }
}

void Class_write(struct Class * this)
{
    Properties_write(this->properties);
}

int main(int argc, char *argv[])
{
    char * rootPath = argv[1];
    char * filePath = argv[2];

    struct FilePath * path = FilePath_construct(rootPath, filePath);
    struct File * file = File_construct( FilePath_composeFullPath(path) );
    struct DocumentBuilder * builder = DocumentBuilder_construct(file);
    struct Document * document = DocumentBuilder_createDocument(builder);

    struct Generator * generator = Generator_construct(
        Class_construct(
            Properties_construct(),
            Methods_construct()
        )
    );

    Document_generateCode(document, generator);

    Generator_write(generator);

    Generator_destruct(generator);
    Document_destruct(document);
    DocumentBuilder_destruct(builder);
    File_destruct(file);
    FilePath_destruct(path);

    return 0;
}
