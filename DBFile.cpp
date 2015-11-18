#include "DBFile.h"

#ifndef L_SET
#define L_SET SEEK_SET
#endif

DB_FILE_ENTRY db_file_entry[DB_MAX_FILENUM];

DB_FILEHEADER DBGetFileHeader(int fileid)
{
	return db_file_entry[fileid].fileheader;
}

void DBUpdateFileHeader(int fileid, DB_FILEHEADER fileheader, short headstat)
{
	db_file_entry[fileid].fileheader = fileheader;
	db_file_entry[fileid].headstat = headstat;
}

void DBInit()
{
	int i;
	DBHashInit();
	for(i=0; i<DB_MAX_FILENUM; i++)
	{
		db_file_entry[i].filename = NULL;
		db_file_entry[i].used = FALSE;
	}
}

int DBCreateFile(char* filename)
{
	int fileid;
	DB_FILEHEADER filehdr;
	int error;
	int i;
	fileid = open(filename, O_CREAT|O_EXCL|O_WRONLY, 0664);
	if(fileid < 0)
	{
		return DB_ERROR_OP;
	}

	filehdr.firstfreepage = DB_PAGE_LIST_END;
	filehdr.pagecnt = 0;
	for(i=0; i<DB_MAX_FILEPAGE; i++)
		filehdr.fixed[i] = FALSE;

	if ((error = write(fileid, (char*)&filehdr, DB_FILEHEADER_SIZE)) != DB_FILEHEADER_SIZE)
	{
		close(fileid);
		unlink(filename);
		if (error < 0)
			return DB_ERROR_OP;
		else 
			return DB_ERROR_WRITE_FAIL;
	}

	if ((error = close(fileid)) == -1)
	{
		return DB_ERROR_OP;
	}

	return DB_OK;
}

int DBDestroyFile(char* filename)
{
	int error;
	if(DBFileIsOpen(filename) != -1)
	{
		return DB_ERROR_FILE_OPEN;
	}

	if((error = unlink(filename)) != 0)
	{
		return DB_ERROR_OP;
	}
	return DB_OK;
}

int DBOpenFile(char* filename)
{
	int fileid;
	int cnt;
	char* newname;
	FILE* file;

	if((fileid = DBFileEntryFull()) == -1)
	{
		return DB_ERROR_TAB_FULL;
	}	
	if((db_file_entry[fileid].fileid = open(filename, O_RDWR | O_BINARY)) < 0)
	{
		return DB_ERROR_OP;
	}


	if((cnt = read(db_file_entry[fileid].fileid, (char*)&db_file_entry[fileid].fileheader, DB_FILEHEADER_SIZE)) != DB_FILEHEADER_SIZE)
		//	file = fopen(filename, "rb");
			//	cnt = fread((char*)&db_file_entry[fileid].fileheader, DB_FILEHEADER_SIZE, 1,file);
				//	if(!cnt)
	{
		close(fileid);		
		if(cnt < 0)
			return DB_ERROR_OP;
		else
			return DB_ERROR_HDR_READ_FAIL;
	}
	db_file_entry[fileid].headstat = FALSE;

	newname = (char*)malloc(strlen(filename) + 1);
	strcpy(newname, filename);
	db_file_entry[fileid].filename = newname;

	//	entry->fileheader = db_file_entry[fileid].fileheader;
	//	entry->fileid = db_file_entry[fileid].fileid;
	//	entry->filename = db_file_entry[fileid].filename;
	//	entry->used = db_file_entry[fileid].used;

	return fileid;
}

int DBCloseFile(int fileid)
{
	int error;
	FILE* file;

	if((error = FileRelease(fileid)) != DB_OK)
		return error;
	if(db_file_entry[fileid].headstat)
	{
		if((error = lseek(db_file_entry[fileid].fileid, (unsigned)0, L_SET)) == -1)
		{
			return DB_ERROR_OP;
		}
		if((error = write(db_file_entry[fileid].fileid, (char*)&db_file_entry[fileid].fileheader, DB_FILEHEADER_SIZE)) != DB_FILEHEADER_SIZE)
			//		file = fopen(db_file_entry[fileid].filename, "wb");
				//		if(!fwrite((char*)&db_file_entry[fileid].fileheader, DB_FILEHEADER_SIZE, 1,file))
		{
			if(error < 0)
				return DB_ERROR_OP;
			else
				return DB_ERROR_WRITE_FAIL;
		}
		db_file_entry[fileid].headstat = FALSE;
	}
	if((error = close(db_file_entry[fileid].fileid)) == -1)
	{
		return DB_ERROR_OP;
	}
	free((char*)db_file_entry[fileid].filename);
	db_file_entry[fileid].filename = NULL;

	return DB_OK;
}

int DBReadFile(int fileid, int pageid, DB_PAGE* bufpage)
{
	int error;
	FILE* file;

	if ((error = lseek(db_file_entry[fileid].fileid, pageid*sizeof(DB_PAGE)+DB_FILEHEADER_SIZE, L_SET)) == -1)
	{
		return DB_ERROR_OP;
	}

	if((error = read(db_file_entry[fileid].fileid, (char*)bufpage, sizeof(DB_PAGE))) != sizeof(DB_PAGE))
		//	file = fopen(db_file_entry[fileid].filename, "rb");
			//	if(!fread((char*)bufpage, sizeof(DB_PAGE), 1,file))
	{
		if (error <0)
			return DB_ERROR_OP;
		else	
			return DB_ERROR_READ_FAIL;
	}

	return DB_OK;
}

int DBWriteFile(int fileid, int pageid, DB_PAGE* bufpage)
{
	int error;

	if((error = lseek(db_file_entry[fileid].fileid, pageid*sizeof(DB_PAGE)+DB_FILEHEADER_SIZE, L_SET)) == -1)
	{
		return DB_ERROR_OP;
	}

	if((error = write(db_file_entry[fileid].fileid, (char*)bufpage, sizeof(DB_PAGE))) != sizeof(DB_PAGE))
	{
		if(error < 0)
			return DB_ERROR_OP;
		else
			return DB_ERROR_WRITE_FAIL;
	}

	return DB_OK;
}



int DBFileIsOpen(char* filename)
{
	int i;
	for(i=0; i<DB_MAX_FILENUM; i++)
		if(db_file_entry[i].filename != NULL && strcmp(db_file_entry[i].filename, filename) == 0)
			return i;
	return -1;
}

int DBFileEntryFull()
{
	int i;
	for(i=0; i<DB_MAX_FILENUM; i++)
		if(db_file_entry[i].filename == NULL)
			return i;
	return -1;
}
