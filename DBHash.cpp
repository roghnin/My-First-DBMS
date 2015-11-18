#include "DBHash.h"
#include <stdlib.h>
void DBHashInit()
{
	int i;
	for(i=0; i<DB_HASH_TAB_SIZE; i++)
	{
		db_hash_table[i] = NULL;
	}
}

BUF_PAGE *DBHashFind(int fileid, int pageid)
{
	int bucket;
	DB_HASH_ENTRY* entry;
	bucket = DBHASH(fileid, pageid);
	for(entry=db_hash_table[bucket]; entry!=NULL; entry = entry->nextentry)
	{
		if(entry->fileid == fileid && entry->pageid == pageid)
			return entry->bufpage;
	}
	return NULL;	
}

int DBHashInsert(int fileid, int pageid, BUF_PAGE* bufpage)
{
	int bucket;
	DB_HASH_ENTRY* entry;
	if(DBHashFind(fileid, pageid) != NULL)
		return -1;
	bucket = DBHASH(fileid, pageid);
	if((entry = (DB_HASH_ENTRY*)malloc(sizeof(DB_HASH_ENTRY))) == NULL)
		return -1;
	entry->fileid = fileid;
	entry->pageid = pageid;
	entry->bufpage = bufpage;
	entry->nextentry = db_hash_table[bucket];
	entry->preventry = NULL;
	if(db_hash_table[bucket] != NULL)
		db_hash_table[bucket]->preventry = entry;
	db_hash_table[bucket] = entry;
	return 0;
}

int DBHashDelete(int fileid, int pageid)
{
	int bucket;
	DB_HASH_ENTRY* entry;
	bucket = DBHASH(fileid, pageid);
	for(entry=db_hash_table[bucket]; entry!=NULL; entry=entry->nextentry)
		if(entry->fileid == fileid && entry->pageid == pageid)
			break;
	if(entry==NULL)
		return -1;
	if(entry == db_hash_table[bucket])
		db_hash_table[bucket] = entry->nextentry;
	if(entry->preventry != NULL)
		entry->preventry->nextentry = entry->nextentry;
	if(entry->nextentry != NULL)
		entry->nextentry->preventry = entry->preventry;
	free(entry);
	return 0;
}

void DBHashPrint()
{
	int i;
	DB_HASH_ENTRY* entry;
	for(i=0; i<DB_HASH_TAB_SIZE; i++)
	{
		printf("bucket %d\n", i);
		if(db_hash_table[i] == NULL)
			printf("Empty\n");
		else
		{
			for(entry=db_hash_table[i]; entry!=NULL; entry=entry->nextentry)
				printf("\tfileid: %d, pageid: %d %d\n",entry->fileid, entry->pageid,entry->bufpage);
		}
	}
}