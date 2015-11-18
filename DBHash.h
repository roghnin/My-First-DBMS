#ifndef _DBHASH_
#define _DBHASH_

#include <stdio.h>
#include "DBPageType.h"

typedef struct DB_HASH_ENTRY
{
	struct DB_HASH_ENTRY* nextentry;
	struct DB_HASH_ENTRY* preventry;
	int fileid;
	int pageid;
	BUF_PAGE* bufpage;

}DB_HASH_ENTRY;

#define DBHASH(fileid, pageid) (((fileid)+(pageid))%DB_HASH_TAB_SIZE)

static DB_HASH_ENTRY* db_hash_table[DB_HASH_TAB_SIZE];

void DBHashInit();
BUF_PAGE *DBHashFind(int fileid, int pageid);
int DBHashInsert(int fileid, int pageid, BUF_PAGE* bufpage);
int DBHashDelete(int fileid, int pageid);
void DBHashPrint();

#endif