#ifndef _DBFILETYPE_
#define _DBFILETYPE_

#ifdef __cplusplus
extern "C" {
#endif 

#include "DBFUtility.h"

typedef struct
{
	int firstfreepage;
	int pagecnt;
	short fixed[DB_MAX_FILEPAGE];

}DB_FILEHEADER;

typedef struct
{
	char* filename;
	short used;
	short headstat;
	int fileid;
	DB_FILEHEADER fileheader;

}DB_FILE_ENTRY;

#define DB_FILEHEADER_SIZE sizeof(DB_FILEHEADER)

#ifdef __cplusplus
}
#endif 

#endif