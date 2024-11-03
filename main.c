#include <stdio.h>
#include <stdlib.h>

#define EOL '\0'

// scenario 1:
//
// input: file(s)
//
// output: 
//        1. count new lines
//        2. word count
//        3. byte count
//        4. character count
//
// e.g
// 
// $ wcc test.txt test2.txt
//   20  100 200 test.txt
//   30  150 250 test2.txt
//   50  250 450 total
//
// scenario 2:
//
// input: file(s)
//
// output: 
//        1. count new lines
//        2. word count
//        3. byte count
//        4. character count
//
// e.g
// 
// $ wcc test.txt file_that_does_not_exists.txt
//   20  100 200 test.txt
//   wcc: file_that_does_not_exists.txt: open: No Sock file or directory
//   20  100 200 total

const char * HELP_TEXT = "usage: wcc [file ...]";

// File wrapper
struct File
{
    char* filename;
    FILE* fp;
    size_t size;
};

struct File* open_file(char* filename)
{
    struct File* file = (struct File*) malloc(sizeof(struct File));
    FILE * fp = fopen(filename, "r");
    if(fp) {
        file->filename = filename;
        file->fp = fp;
        fseek(fp, 0 , SEEK_END);
        file->size = ftell(fp);
        fseek(fp, 0 , SEEK_SET);
    } else {
        free(file);
        file = 0;
    }
    return file;
}

int read_file(char* buffer, struct File * file)
{
    size_t len = fread(buffer, sizeof(char), file->size, file->fp);
    if (ferror(file->fp) != 0)
        return -1;
    buffer[len++] = EOF;
    return 0;
}

int close_file(struct File* file)
{
    file->filename = 0;
    fclose(file->fp);
    file->fp = 0;
    free(file);
    return 0;
}

int computeNewLines(const char* buffer)
{
    int newLines = 0;
    while(*buffer != EOF)
    {
        if (*buffer == '\n') newLines++;
        buffer++;
    }
    return newLines;
}

int isWhiteSpace(const char c)
{
    switch(c)
    {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
            return 1;
        default:
            return 0;
    }
}

int computeWords(const char* buffer)
{
    const char *ptr = buffer;
    int wordCount = 0;
    while(*buffer != EOF)
    {
        if (*buffer <= 0x7F)
        {
            // compute the current index where we're at. This is done to
            // ignore the first line if it is only a new line.
            //
            // right now this is what it came to my mind :/ 
            int index = buffer - ptr;
            if (isWhiteSpace(*buffer) && index > 0 && !isWhiteSpace(*(buffer - 1)))
                wordCount ++;
            buffer++;
        }
        if ((*buffer & 0xE0) == 0xC0)
            buffer += 2;
        if ((*buffer & 0xF0) == 0xE0)
            buffer += 3;
        if ((*buffer & 0xF8) == 0xF0)
            buffer += 4;
    }
    return wordCount;
}

int computeBytes(const char* buffer)
{
    int bytesCount = 0;
    while(*buffer != EOF)
    {
        bytesCount++;
        buffer++;
    }
    return bytesCount;
}

int main(int argc, char **argv)
{
    if (argc==1)
    {
        printf("%s\n", HELP_TEXT);
        return 1;
    }

    int totalLinesCount = 0;
    int totalWordCount = 0;
    int totalBytesCount = 0;
    for (int i = 1; i < argc; i++)
    {
        char* filename = argv[i];
        struct File* file = open_file(filename);
        if (file)
        {
            char buffer[file->size + 1];
            if(read_file(buffer, file) != 0)
            {
                printf("wcc: %s: read: Unkown problem reading file", filename);
                continue;
            }

            int linesCount = computeNewLines(buffer);
            int wordCount = computeWords(buffer);
            int bytesCount = computeBytes(buffer);
            printf("     %d     %d    %d %s\n", linesCount, wordCount, bytesCount, file->filename);

            totalLinesCount += linesCount;
            totalWordCount += wordCount;
            totalBytesCount += bytesCount;

            close_file(file);
        }
        else
        {
            printf("wcc: %s: open: No Sock file or directory\n", filename);
        }
    }

    if (argc > 2)
    {
        printf("     %d     %d    %d total\n", totalLinesCount, totalWordCount, totalBytesCount);
    }
    return 0;
}
