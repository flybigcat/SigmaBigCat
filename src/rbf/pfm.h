#ifndef _pfm_h_
#define _pfm_h_

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

typedef int RC;
typedef unsigned PageNum;

#define PAGE_SIZE 4096

class FileHandle;


class PagedFileManager
{
public:
    static PagedFileManager* instance();                     // Access to the _pf_manager instance

    RC createFile    (const char *fileName);                         // Create a new file
    RC destroyFile   (const char *fileName);                         // Destroy a file
    RC openFile      (const char *fileName, FileHandle &fileHandle); // Open a file
    RC closeFile     (FileHandle &fileHandle);                       // Close a file

    void testOpenFile(const char *fileName, FileHandle &fileHandle);

protected:
    PagedFileManager();                                   // Constructor
    ~PagedFileManager();                                  // Destructor

private:
    static PagedFileManager *_pf_manager;
};


class FileHandle
{
public:
    FileHandle();                                                    // Default constructor
    ~FileHandle();                                                   // Destructor

    RC readPage(PageNum pageNum, void *data);                           // Get a specific page
    RC writePage(PageNum pageNum, const void *data);                    // Write a specific page
    RC appendPage(const void *data);                                    // Append a specific page
    unsigned getNumberOfPages();                                        // Get the number of pages in the file

    RC loadFile(const char * fileName, FILE * loadedFile);				// link a file(and only one) to a fileHandle
    RC unloadFile();													// make hFile to NULL, unlink

    RC closeFile();														// fclose an open file

    bool hasFile();														// if this handle has been linked to a file
    FILE getFile();														// get linked file
    string getFileName();												// get linked file's name

private:
    FILE * hFile;														// linked file
    string hFileName;													// linked file's name
    unsigned numberOfPages;												// the total number of pages of linked file
    bool isFile;														// if this handle has a linked to a file
};

 #endif
