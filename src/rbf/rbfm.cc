
#include "rbfm.h"

RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;

RecordBasedFileManager* RecordBasedFileManager::instance()
{
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager()
{
	pfm = PagedFileManager::instance();
}

RecordBasedFileManager::~RecordBasedFileManager()
{
}

/* This method creates a record-based file called fileName.
 * The file should not already exist.
 * Please note that this method should internally use the method PagedFileManager::createFile (const char *fileName).
 * */
RC RecordBasedFileManager::createFile(const string &fileName)
{
	return pfm->createFile(fileName.c_str());
}

/* This method destroys the record-based file whose name is fileName.
 * The file should exist.
 * Please note that this method should internally use the method PagedFileManager::destroyFile (const char *fileName).
 * */
RC RecordBasedFileManager::destroyFile(const string &fileName)
{
    return pfm->destroyFile(fileName.c_str());
}

/* This method opens the record-based file whose name is fileName.
 *  The file must already exist
 *  and it must have been created using the RecordBasedFileManager::createFile method.
 *  If the method is successful, the fileHandle object whose address is passed as a parameter becomes a "handle" for the open file.
 *  The file handle rules in the method PagedFileManager::openFile applies here too.
 *  Also note that this method should internally use the method PagedFileManager::openFile(const char *fileName, FileHandle &fileHandle).
*/
RC RecordBasedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
    return pfm->openFile(fileName.c_str(), fileHandle);
}

/* This method closes the open file instance referred to by fileHandle.
 * The file must have been opened using the RecordBasedFileManager::openFile method.
 * Note that this method should internally use the method PagedFileManager::closeFile(FileHandle &fileHandle).
 * */
RC RecordBasedFileManager::closeFile(FileHandle &fileHandle)
{
    return pfm->closeFile(fileHandle);
}

/* Given a record descriptor, insert a record into a given file identifed by the provided handle.
 * You can assume that the input is always correct and free of error.
 * That is, you do not need to check if the input record has the right number of attributes and if the attribute types match.
 * Furthermore, you may use system-sequenced file organization.
 * That is, find the first page with free space large enough to store the record and store the record at that location.
 * RID here is the record ID which is used to uniquely identify records in a file.
 * An RID consists of: 1) the page number that the record resides in within the file, and
 * 2) the slot number that the record resides in within the page.
 * The insertRecord method accepts an RID object and fills it with the RID of the record that is target for insertion.
 * */
RC RecordBasedFileManager::insertRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, RID &rid)
{
	int dataLength = getDataLength(recordDescriptor, data);
	int numberOfRecordElements = getNumberOfRecordElements(recordDescriptor, data);
	int sizeOfRecordHeader = (1 + numberOfRecordElements) * sizeof(int);

	void * buffer = malloc(PAGE_SIZE);

	//if the file has enough free space for dataLength
	//read this page into buffer and
	//organize this page: insert data, rewrite free space pointer, increase slot number, put slotcell inside

	if(fileHandle.getNumberOfPages() > 0){
		//find a page i has enough free space
		for(int i = 0; i < fileHandle.getNumberOfPages(); i++){
			//read into buffer
			if(fileHandle.readPage(i, buffer)!=0)
			{
				free(buffer);
				return -1;
			}

			// free space pointer is the physical address offset from the very beginning of the file to the "free space"
			int freeSpacePointer;
			freeSpacePointer = * (int * )((char * )buffer+PAGE_SIZE-sizeof(int));

			unsigned totalNumberOfSlots;
			totalNumberOfSlots = * (int *)((char *)buffer + PAGE_SIZE-2*sizeof(int));

			int freeSpace = PAGE_SIZE - freeSpacePointer - 2 * sizeof(int) - 2 * totalNumberOfSlots * sizeof(int);

			//enough free space
			if(freeSpace >= (dataLength + 2 * sizeof(int) + sizeOfRecordHeader))
			{
				//insert header
				void * recordHeader=malloc(PAGE_SIZE) ;
				recordHeaderMaker(recordDescriptor, data, recordHeader);
				memcpy((char *)buffer +freeSpacePointer, recordHeader, sizeOfRecordHeader);
				//insert data
				//void * memcpy ( void * destination, const void * source, size_t num );
				memcpy( (char *)buffer +freeSpacePointer+sizeOfRecordHeader, data, dataLength);

				//increase total slot number
				totalNumberOfSlots +=1;
				memcpy((char *)buffer + PAGE_SIZE - 2*sizeof(int), &totalNumberOfSlots, sizeof(int));

				//put slot cell into
				SlotCell sc;
				sc.length = dataLength;
			    sc.offset = freeSpacePointer;
				memcpy((char *)buffer + PAGE_SIZE - 2*sizeof(int) - totalNumberOfSlots*2*sizeof(int), &sc, 2*sizeof(int));

				//move fsp
				freeSpacePointer += dataLength + sizeOfRecordHeader;
				memcpy((char *)buffer + PAGE_SIZE - sizeof(int), &freeSpacePointer, sizeof(int));


				//write "dirty" page back to disk
				if(fileHandle.writePage(i, buffer) != 0)
				{
					free(buffer);
					return -1;
				}

				//get RID
				rid.pageNum = i;
				rid.slotNum = totalNumberOfSlots;

				//all done
				free(buffer);
				return 0;
			}//end of if

			else break;
		}//end of for
	}//end of if

	//if fileHandle's file is empty
	//or all the page don't have enough free space
	//append a new page

	//insert record header

	void * recordHeader=malloc(PAGE_SIZE) ;
	recordHeaderMaker(recordDescriptor, data, recordHeader);
	memcpy((char *)buffer, recordHeader, sizeOfRecordHeader);

	//insert data
	memcpy((char *)buffer + sizeOfRecordHeader, data, dataLength);

	//initialization of header/slot directory
	int freeSpacePointer;
	freeSpacePointer = dataLength + sizeOfRecordHeader;
	memcpy((char *)buffer + PAGE_SIZE - sizeof(int), &freeSpacePointer, sizeof(int));

	int totalNumberOfSlots =1;
	memcpy((char *)buffer + PAGE_SIZE -2*sizeof(int), &totalNumberOfSlots, sizeof(int));

	SlotCell sc;
	sc.length = dataLength;
	sc.offset = 0;
	memcpy((char *)buffer + PAGE_SIZE - 2*sizeof(int) - totalNumberOfSlots*2*sizeof(int), &sc, 2*sizeof(int));

	//write "dirty"new  page append to file back to disk
	if(fileHandle.appendPage(buffer) != 0 )
	{
		free(buffer);
		return -1;
	}

	rid.pageNum = fileHandle.getNumberOfPages()-1;
	rid.slotNum = 1;

	free(buffer);

	return 0;
}

/*Given a record descriptor, read the record identified by the given rid.*/
RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data)
{
	void * buffer = malloc(PAGE_SIZE);

	if(fileHandle.readPage(rid.pageNum, buffer) != 0) //not exisit this record
	{
		free(buffer);
		return -1;
	}

	unsigned totalNumberOfSlots;
	totalNumberOfSlots = * (int *)((char *)buffer + PAGE_SIZE - 2* sizeof(int));

	if(totalNumberOfSlots >= rid.slotNum)
	{
		int offset;

		SlotCell sc;
		sc.offset = * (int *)((char *)buffer + PAGE_SIZE - 2*sizeof(int) - rid.slotNum * 2 * sizeof(int) );
		offset = ((*(int *)((char*)buffer + sc.offset) )+1)*sizeof(int) + sc.offset;

		if(offset == -1)  //this record has been deleted;
		{
			free(buffer);
			return -1;
		}

		int dataLength = getDataLength(recordDescriptor, (char*)buffer + offset);

//		int recordPointer;
//		recordPointer = *(int *)( (char*)buffer + offset +sizeof(int) );
//		memcpy(data,  &recordPointer, dataLength);
		memcpy(data,  (char*)buffer + offset, dataLength);

		//memcpy(data,  (char*)buffer + offset, dataLength);
		free(buffer);
		return 0;
	}

	free(buffer);
	return -1;
}

int RecordBasedFileManager::getDataLength(const vector<Attribute> &recordDescriptor, const void *data)
{
	int dataLength = 0;

	for(int i = 0; i < recordDescriptor.size(); i ++)
	{
		//check each element type: AttrType
		switch (recordDescriptor[i].type) {

		case 0:
				dataLength+=sizeof(int);
				break;

		case 1:
				dataLength+=sizeof(int);
				break;

		case 2:
				int * aa;
				aa = (int *)((char *)data+dataLength);
				int length = *aa;

				dataLength+=length+sizeof(int);
				break;

		}//end of switch

	}//end of for each
	return dataLength;
}

int RecordBasedFileManager::getNumberOfRecordElements(const vector<Attribute> &recordDescriptor, const void *data)
{
	int numberOfRecordElements = 0;
			for(int i = 0; i < recordDescriptor.size(); i ++)
			{
				//check each element type: AttrType
				switch (recordDescriptor[i].type) {

				case 0:
						numberOfRecordElements++;
						break;

				case 1:
						numberOfRecordElements++;
						break;

				case 2:
						numberOfRecordElements+=2;
						break;
				}//end of switch
			}//end of for each
			return numberOfRecordElements;
}

RC RecordBasedFileManager::recordHeaderMaker(const vector<Attribute> &recordDescriptor, const void *data, void *recordHeader)
{
		int numberOfRecordElements = getNumberOfRecordElements(recordDescriptor, data);
		int sizeOfRecordHeader = (1 + numberOfRecordElements) * sizeof(int);

		memcpy(recordHeader, &numberOfRecordElements, sizeof(int));

		int offset = 0;
		int attrPointer;
		int j = 1;

		for(int i = 0; i < recordDescriptor.size(); i ++)
			{
				cout<< i<<endl;
				//check each element type: AttrType
				switch (recordDescriptor[i].type) {

				case 0:
						attrPointer = sizeOfRecordHeader + offset;
						memcpy((char * )recordHeader + j*sizeof(int), &attrPointer, sizeof(int));
						offset+=sizeof(int);
						j++;
						break;

				case 1:
						attrPointer = sizeOfRecordHeader+ offset;
						memcpy((char * )recordHeader + j*sizeof(int), &attrPointer, sizeof(int));
						offset+=sizeof(int);
						j++;
						break;

				case 2:
					    int * aa;
						aa = (int *)((char *)data+offset);
						int length = *aa;

						attrPointer = sizeOfRecordHeader + offset;
						memcpy((char * )recordHeader + j*sizeof(int), &attrPointer, sizeof(int));
					    offset+=sizeof(int);
						j++;

						attrPointer = sizeOfRecordHeader + offset;
						memcpy((char * )recordHeader + j*sizeof(int), &attrPointer, sizeof(int));
						offset+=length;
						j++;
						break;

				}//end of switch

			}//end of for each

		cout<< "here3" << endl;

	return 0;
}

/* This is a utility method that will be mainly used for debugging/testing.
 * It should be able to interpret the bytes of each record using the passed record descriptor
 * and then print its content to the screen.
 * For instance suppose a record consists of two fields: int and float,
 * Which means the record will be of size 8
 * (ignoring any other metadata information that might be kept with every record).
 * The printRecord method will be able to recognize the record format using the record descriptor.
 * It then will be able to convert the first four bytes to an int object
 * and the last four bytes to a float object and print their values.
 * */
RC RecordBasedFileManager::printRecord(const vector<Attribute> &recordDescriptor, const void *data)
{
	int offset = 0;

	int * aa;
	float * bb;
	char * cc;

	for(int i = 0; i < recordDescriptor.size(); i ++)
	{
		//check each element type: AttrType
		switch (recordDescriptor[i].type) {

		case 0:
				aa = (int * )((char *)data+offset);
				cout << *aa<<endl;
				offset+=sizeof(int);
				break;

		case 1:
				bb = (float * )((char *)data+offset);
				cout <<  *bb <<endl;
				offset+=sizeof(int);
				break;

		case 2:
				aa = (int *)((char *)data+offset);
				int length = *aa;

				cout << *aa <<endl;

				cc = ((char *)data+offset+sizeof(int));
				for(int j = 0; j < length; j++)
				{
					cout << *cc;
					cc++;
				}
				cout << endl;

				offset+=length+sizeof(int);
				break;


		}//end of switch

	}//end of for each

	cout << endl;
    return 0;
}

/* Delete all records in the file.*/
RC RecordBasedFileManager::deleteRecords(FileHandle &fileHandle)
{
	//getNumberOfPages

	//remove everything in these pages
	return -1;
}

/* Given a record descriptor, delete the record identified by the given rid.*/
RC RecordBasedFileManager::deleteRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid)
{
	//read this page (rid.pageNum) into buffer

	//get dataLength by slotcell.length, slotNum = rid.slotNum

	//delete this part ->NULL?

	//write this page back to disk
	return -1;
}

/* Given a record descriptor, update the record identified by the given rid with the passed data.
 * If the record grows and there is no space in the page to store the record,
 * the record is migrated to a new page with enough free space.
 * Since you will soon be implementing a B-tree structure or any indexing mechanism,
 * assume that records are identified by their rids and when they migrate,
 * they leave a tombstone behind pointing to the new location of the record.*/
//Assume the rid does not change after update
RC RecordBasedFileManager::updateRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, const RID &rid)
{
	//get dataLength by slotcell.length, slotNum = rid.slotNum

	//use arguments: recordDescriptor, data, calculate updated data's newDataLength

	//check if newDataLength <= dataLength

	//delete this record

	//write this record in the same "offset" position

	//write back this page into disk

	return -1;
}

/* Given a record descriptor, read a specific attribute of a record identified by a given rid.*/
RC RecordBasedFileManager::readAttribute(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, const string attributeName, void *data)
{
	//get the field/attribute position from recordDescriptor and attributeName

	//similar to readRecord method, but the last few steps, don't read the whole record, but read specific field
	return -1;
}

/* Given a record descriptor, reorganize a page, i.e., push the free space towards the end of the page.*/
RC RecordBasedFileManager::reorganizePage(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const unsigned pageNumber)
{
	//check if this pageNumber exist or not(getNumberOfPages)

	//if exists

	//read this page into RAM buffer

	//allocate another buffer2 in RAM

	//put buffer's record into buffer2 one by one???
	return -1;
}

/* Given a record descriptor, scan a file, i.e., sequentially read all the entries in the file.
 * A scan has a filter condition associated with it,
 * e.g., it consists of a list of attributes to project out as well as a predicate on an attribute ("Sal > 40000").
 * Specifically, the parameter conditionAttribute here is the attribute's name that you are going to apply the filter on.
 * The compOp parameter is the comparison type that is going to be used in the filtering process.
 * The value parameter is the value of the conditionAttribute that is going to be used to filter out records.
 * Note that the retrieved records should only have the fields that are listed in the vector attributeNames.
 * Please take a look at the test cases for more information on how to use this method.*/
// scan returns an iterator to allow the caller to go through the results one by one.
RC RecordBasedFileManager::scan(FileHandle &fileHandle,
	      const vector<Attribute> &recordDescriptor,
	      const string &conditionAttribute,
	      const CompOp compOp,                  // comparision type such as "<" and "="
	      const void *value,                    // used in the comparison
	      const vector<string> &attributeNames, // a list of projected attributes
	      RBFM_ScanIterator &rbfm_ScanIterator)
{
	//create vector<unsigned> ridList in the rbfm_ScanIterator object

	//for each page i

	//read into RAM buffer

	//for each record in this page *data

	//check if it satisfy the condition

	//if satisfy, put its rid in the ridList

	return -1;
}

/* Given a record descriptor, reorganize the file which causes reorganization
 * of the records such that the records are collected towards the beginning of the file.
 * Also, record redirection is eliminated. (In this case, and only this case, it is okay for rids to change.)*/
// Extra credit for part 2 of the project, please ignore for part 1 of the project
RC RecordBasedFileManager::reorganizeFile(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor)
{
	//for each page i in this file

	//reorganizePage()

	//check if slot directory, if there is a tomestone

	//delete this tomestone slot cell

	//update slot directory

	return -1;
}


