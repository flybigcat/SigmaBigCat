#include "pfm.h"


PagedFileManager* PagedFileManager::_pf_manager = 0;

PagedFileManager* PagedFileManager::instance()
{
    if(!_pf_manager)
        _pf_manager = new PagedFileManager();

    return _pf_manager;
}


PagedFileManager::PagedFileManager()
{
}


PagedFileManager::~PagedFileManager()
{
	_pf_manager=NULL;
}

//This method creates a paged file called fileName. The file should not already exist.
RC PagedFileManager::createFile(const char *fileName)
{
	FILE * pFile;

	//check if the file named "filName" exists(ready for input) as a binary file
	pFile = fopen(fileName, "rb");

	if(pFile != NULL) //if already exist: close it and return error msg
		{
			fclose(pFile);
			return -1;
		}

	//the file named "filName" ready for output as a binary file
	pFile = fopen(fileName, "wb");
	fclose(pFile);

	//"fileName" pointer is created and "
    return 0;
}

//This method destroys the paged file whose name is fileName. The file should exist.
RC PagedFileManager::destroyFile(const char *fileName)
{
	//check if a file named "fileName" exits
//	if(fopen(fileName, "rb") == NULL) //not exits
//		return -1;
    ifstream f(fileName);
    if (!f.good()) {
          f.close();
          return -1;
      }else
          f.close();

	//delete it and check
	if( remove(fileName) != 0)
		return -1; //delete failed

	return 0;
}

/*This method opens the paged file whose name is fileName.
 * The file must already exist and it must have been created using the createFile method.
 * If the method is successful,
 * the fileHandle object whose address is passed as a parameter becomes a "handle" for the open file.
 * The file handle is used to manipulate the pages of the file (see the FileHandle class description below).
 * It is an error if fileHandle is already a handle for an open file when it is passed to the openFile method.
 * It is not an error to open the same file more than once if desired, using a different fileHandle object each time.
 * Each call to the openFile method creates a new "instance" of the open file.
 * Warning: Opening a file more than once for data modification is not prevented by the PF component,
 * but doing so is likely to corrupt the file structure and may crash the PF component.
 * Opening a file more than once for reading is no problem.
 * */

RC PagedFileManager::openFile(const char *fileName, FileHandle &fileHandle)
{
	FILE * pFile;

	pFile = fopen(fileName, "rb");

	//check if a file named "fileName" exits
	if(pFile == NULL) //not exits
		return -1;

	fclose(pFile);

	//check if the fileHandle is already a handle for an open file
	if(fileHandle.hasFile())
	{
		return -1;
	}

	pFile = fopen(fileName, "r+b");

	return fileHandle.loadFile(fileName, pFile);
//	return 0;

}

/*This method closes the open file instance referred to by fileHandle.
 * The file must have been opened using the openFile method.
 * All of the file's pages are flushed to disk when the file is closed.
 * */
RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
	//check if this fileHandle is a handle for an open file
	if(!fileHandle.hasFile())
		return -1;

	fileHandle.closeFile();

    return 0;
}


FileHandle::FileHandle()
{
	//hFile = NULL;
	isFile = false;
}


FileHandle::~FileHandle()
{

}

//This method reads the page into the memory block pointed by data.
//The page should exist. Note the page number starts from 0.
RC FileHandle::readPage(PageNum pageNum, void *data)
{
	//cout<<getNumberOfPages();

	//check it this file has this page
	if(pageNum > getNumberOfPages())
		return -1;

	if(fseek(hFile, pageNum*PAGE_SIZE, SEEK_SET)!=0)
		return -1;

	//Read block of data from stream
	//size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
	if(fread(data, PAGE_SIZE, 1, hFile) != 1)
		return -1;

	return 0;
}

//This method writes the data into a page specified by the pageNum.
//The page should exist. Note the page number starts from 0.
RC FileHandle::writePage(PageNum pageNum, const void *data)
{
	//if the pageNum th is 1 more than the last page, add to the end
	if(pageNum == getNumberOfPages())
		appendPage(data);

	//check if pageNum not exits OR can no be attached to the file
	if(pageNum > getNumberOfPages())
		return -1;

	if(fseek(hFile, pageNum*PAGE_SIZE, SEEK_SET)!=0)
		return -1;

	//Write block of data to stream
	//size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
	if (fwrite(data, PAGE_SIZE, 1, hFile) != 1)
		return -1;

    return 0;
}

//This method appends a new page to the file,
//and writes the data into the new allocated page.
RC FileHandle::appendPage(const void *data)
{
	if(fseek(hFile, 0, SEEK_END)!=0)
		return -1;

	if(fwrite(data, PAGE_SIZE, 1, hFile) != 1)
		return -1;

	return 0;

}

//This method returns the total number of pages in the file.
unsigned FileHandle::getNumberOfPages()
{
	//Reposition stream position indicator
	//int fseek ( FILE * stream, long int offset, int origin );
	fseek(hFile, 0, SEEK_END);

	//Get current position in stream
	//long int ftell ( FILE * stream );
	numberOfPages =  (ftell(hFile))/PAGE_SIZE;

	return numberOfPages;

	return -1;
}

RC FileHandle::loadFile(const char * fileName, FILE * loadedFile)
{
	hFileName.assign(fileName);
	hFile = loadedFile;
	isFile = true;
	return 0;
}

RC FileHandle::unloadFile()
{
	hFile = NULL;
	isFile = false;
	return 0;
}

RC FileHandle::closeFile()
{
	if(!isFile)
		return -1;

	// All of the file's pages are flushed to disk
	fflush(hFile);

	fclose(hFile);
	return 0;
}

bool FileHandle::hasFile()
{
	return isFile;
}

FILE FileHandle::getFile()
{
	return  *hFile;
}

string FileHandle::getFileName()
{
	return hFileName;
}
