
#include "rm.h"

RelationManager* RelationManager::_rm = 0;

RelationManager* RelationManager::instance()
{
    if(!_rm)
        _rm = new RelationManager();

    return _rm;
}

RelationManager::RelationManager()
{
	//rbfm = RecordBasedFileManager::instance();
}

RelationManager::~RelationManager()
{
}

/* This method creates a table called tableName with a vector of attributes (attrs).*/
RC RelationManager::createTable(const string &tableName, const vector<Attribute> &attrs)
{
	//catalog-tableInfor
    //check "system" vs. "user" flag to see if it is legal to create table

	//not legal
	//return -1

	//legal
	//create fileHandle
	//load/create the file "tableName" to this fileHandle

	//rbfm.createFile(tableInfor);
	//append a page with one record
		//vector<Attribute> &tableInforEntry([0]name=tableName, type=2, length=tableName.length)
		//*tableInfor = (tableName, table_id)
		//rbfm.insertRecord()

	//create another fileHandle
	//load/create the file "attrsInfor" to this fileHandle
	//rbfm.createFile(attrsInfor);
	//append a page with one record
		//vector<Attribute> &attrs
		//*attrInfor = (atts[0].name...)
		//rbfm.insertRecord()
	return -1;
}

/* This method deletes a table called tableName.*/
RC RelationManager::deleteTable(const string &tableName)
{

    //check "system" vs. "user" flag to see if it is legal to create table

	//not legal
	//return -1

	//legal

	//rbfm.destroyFile(tableName);

	//delete attrsInfor table all the information about this table
	//scan, tableName=&tableName
	//deleteRecord with RID

	//delete tableInfor table all the infor about this table
	//...similar to attrsInfor
    return -1;
}

/* This method gets the attributes (attrs) of a table called tableName. */
RC RelationManager::getAttributes(const string &tableName, vector<Attribute> &attrs)
{
	//scan attrsInfor when tableName=&tableName
	//get each record, put back into attrs
	return -1;
}

/* This method inserts a tuple into a table called tableName.
 * You can assume that the input is always correct and free of error.
 * That is, you do not need to check if the input tuple has the right number of attributes
 * and if the attribute types match.*/
RC RelationManager::insertTuple(const string &tableName, const void *data, RID &rid)
{
	//get getAttributes(recordDescriptor) from tableName

	//then insert record using rbfm.insertRecord()s
	return -1;
}

/* This method deletes all tuples in a table called tableName. This command should result in an empty table.*/
RC RelationManager::deleteTuples(const string &tableName)
{
	//create a fileHanle with

    //delete deleteRecords(FileHandle &fileHandle)

	return -1;
}

/* This method deletes a tuple with a given rid.*/
RC RelationManager::deleteTuple(const string &tableName, const RID &rid)
{
	//create a fileHanle with

	//get getAttributes(recordDescriptor) from tableName

	//delete deleteRecord(FileHandle &fileHandle)

    return -1;
}

/* This method updates a tuple identified by a given rid.
 * Note: if the tuple grows (i.e., the size of tuple increases)
 * and there is no space in the page to store the tuple (after the update),
 * then, the tuple is migrated to a new page with enough free space.
 * Since you will implement an index structure (e.g., B-tree) in project 3,
 * you can assume that tuples are identified by their rids and when they migrate,
 * they leave a tombstone behind pointing to the new location of the tuple.*/
RC RelationManager::updateTuple(const string &tableName, const void *data, const RID &rid)
{
	//get getAttributes(recordDescriptor) from tableName

	//updateRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, const RID &rid)

    return -1;
}

/* This method reads a tuple identified by a given rid.*/
RC RelationManager::readTuple(const string &tableName, const RID &rid, void *data)
{
	//get getAttributes(recordDescriptor) from tableName

	//readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data)

	return -1;
}

/* This method reads a specific attribute of a tuple identified by a given rid.*/
RC RelationManager::readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data)
{
	//get getAttributes(recordDescriptor) from tableName

	//readTuple

	//read spefic field

	return -1;
}

/* This method reorganizes the tuples in a page.
 * That is, it pushes the free space towards the end of the page.
 * Note: In this method you are NOT allowed to change the rids,
 * since they might be used by other external index structures,
 * and it's too expensive to modify those structures for each such a function call.
 * It's OK to keep those deleted tuples and their slots*/
RC RelationManager::reorganizePage(const string &tableName, const unsigned pageNumber)
{
    //fileHandle

	//rbfm.reorganizePage

	return -1;
}

/* This method scans a table called tableName.
 * That is, it sequentially reads all the entries in the table.
 * This method returns an iterator called rm_ScanIterator
 * to allow the caller to go through the records in the table one by one.
 * A scan has a filter condition associated with it,
 * e.g., it consists of a list of attributes to project out as well as a predicate on an attribute (ÒSal > 40000Ó).
 * Note: the RBFM_ScanIterator should not cache the entire scan result in memory.
 * In fact, you need to be looking at one (or a few) page(s) of data at a time, ever.
 * In this project, let the OS do the memory-management work for you.*/
RC RelationManager::scan(const string &tableName,
      const string &conditionAttribute,
      const CompOp compOp,                  
      const void *value,                    
      const vector<string> &attributeNames,
      RM_ScanIterator &rm_ScanIterator)
{
    //rbfm.scan
	return -1;
}

// Extra credit
/* This method drops an attribute called attributeName from a table called tableName.
 * Note: This operation will update the catalogs but should not involve touching the data itself.
HINT: The following two methods will affect the way the operations that
access the fields of the records work when accessing a tuple that was created before such a schema change.*/
RC RelationManager::dropAttribute(const string &tableName, const string &attributeName)
{
    //delete one record from attri
	return -1;
}

// Extra credit
/* This method adds a new attribute (attr) to a table called tableName.
 * Note: This operation will update the catalogs but should not involve touching the data itself.*/
RC RelationManager::addAttribute(const string &tableName, const Attribute &attr)
{
    return -1;
}

// Extra credit
/* This method reorganizes a table that causes reorganization of the tuples
 * such that the tuples are collected towards the beginning of the file.
 * Also, tuple redirection is eliminated.
 * Note: In this case, and only this case,
 * you are allowed to change the records rids since they might need to move to other pages.*/
RC RelationManager::reorganizeTable(const string &tableName)
{
    return -1;
}
