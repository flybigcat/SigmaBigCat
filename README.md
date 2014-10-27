SigmaBigCat
===========
# Project 2 Roadmap

##RecordBasedFileManager Class

*   `RC deleteRecords(FileHandle &fileHandle);`
*   `RC deleteRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid);`
*   04_RC updateRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, const RID &rid);
*   03_RC readAttribute(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, const string attributeName, void *data);
*   05_RC reorganizePage(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const unsigned pageNumber);
*   06_RC scan(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const string &conditionAttribute,

       const CompOp compOp, // comparision type such as "<" and "="

       const void *value, // used in the comparison

       const vector<string> &attributeNames, RBFM_ScanIterator &rbfm_ScanIterator);
*   RC reorganizeFile(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor);


##RelationManager Class
*   RC createTable(const string &tableName, const vector<Attribute> &attrs);
*   RC deleteTable(const string &tableName);
*   RC getAttributes(const string &tableName, vector<Attribute> &attrs);
*   RC insertTuple(const string &tableName, const void *data, RID &rid);
*   RC deleteTuples(const string &tableName);
*   RC deleteTuple(const string &tableName, const RID &rid);
*   RC updateTuple(const string &tableName, const void *data, const RID &rid);
*   RC readTuple(const string &tableName, const RID &rid, void *data);
*   RC readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data);
*   RC reorganizePage(const string &tableName, const unsigned pageNumber);
*   RC scan(const string &tableName, const string &conditionAttribute,
      
      const CompOp compOp, // comparision type such as "<" and "="

      const void *value, // used in the comparison

      const vector<string> &attributeNames, RM_ScanIterator &rm_ScanIterator);
*   RC addAttribute(const string &tableName, const Attribute &attr);
*   RC dropAttribute(const string &tableName, const string &attributeName);
*   RC reorganizeTable(const string &tableName);

##RM_ScanIterator Class
*   RC getNextTuple(RID &rid, void *data);
*   RC close();
