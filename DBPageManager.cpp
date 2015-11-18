#include "DBPageManager.h"

                                                                                                                                                                                                                  
short fixed_pages[DB_MAX_FILENUM][DB_MAX_FILEPAGE];

short DBInvalidFileID(int fileid)
{
	if(fileid >= 0 && fileid < DB_MAX_FILENUM)
		return TRUE;
	return FALSE;
}

short DBInvalidPageID(int fileid, int pageid)
{
	if(pageid >= 0 && pageid < DBGetFileHeader(fileid).pagecnt)
		return TRUE;
	return FALSE;
}

int DBFilePageChanged(int fileid, int pageid, BOOL dirty)
{
	BUF_PAGE* bufpage;

	if((bufpage = DBHashFind(fileid, pageid)) == NULL)
		return DB_ERROR_HASH_NOTFOUND;
	bufpage->dirty = dirty;
	return DB_OK;
}

int DBGetPage(int fileid, int pageid, char** buf)
{
	int error;
	DB_PAGE* page;

	if(!DBInvalidFileID(fileid))
	{
		return DB_ERROR_INVALID_FILE;
	}
	if(!DBInvalidPageID(fileid, pageid))
	{
		return DB_ERROR_INVALID_PAGE;
	}
	if((error = BufRead(fileid, pageid, &page)) != DB_OK)
	{
		if(error = DB_ERROR_PAGE_USED)
			*buf = page->pgdata;
		return error;
	}
	*buf = page->pgdata;
	return DB_OK;
}

int DBGetFirstPage(int fileid, char** buf)
{
	return DBGetNextPage(fileid, -1, buf);
}

int DBGetNextPage(int fileid, int pageid, char** buf)
{
	int error;
	int nextpage;
	DB_PAGE* page;

	if(!DBInvalidFileID(fileid))
	{
		return DB_ERROR_INVALID_FILE;
	}
	if(!DBInvalidPageID(fileid, pageid))
	{
		if(pageid != -1)
			return DB_ERROR_INVALID_PAGE;
	}
	for(nextpage = pageid+1; nextpage < DBGetFileHeader(fileid).pagecnt; nextpage++)
	{
		if((error = BufRead(fileid, nextpage, &page)) != DB_OK)
			return error;
		else
		{
			*buf = page->pgdata;
			return DB_OK;
		}
	}
	return DB_ERROR_EOF;
}

int DBAllocPage(int fileid, int* pageid, DB_PAGE** page)
{
	int error;
	int i;
	DB_FILEHEADER fileheader;

	if(!DBInvalidFileID(fileid))
	{
		return DB_ERROR_INVALID_FILE;
	}
	fileheader = DBGetFileHeader(fileid);
	if (fileheader.firstfreepage != DB_PAGE_LIST_END)
	{
		*pageid = fileheader.firstfreepage;
		if ((error = BufRead(fileid, *pageid, page)) != DB_OK)
			return error;
		for(i=fileheader.firstfreepage; i<fileheader.pagecnt; i++)
		{
			if(fileheader.fixed[i] == FALSE)
			{
				fileheader.firstfreepage = i;
				break;
			}
		}
		if(i == fileheader.pagecnt)
			fileheader.firstfreepage = DB_PAGE_LIST_END;
		DBUpdateFileHeader(fileid, fileheader, TRUE);
	}
	else 
	{
		*pageid = fileheader.pagecnt;
		if ((error = BufAlloc(fileid, *pageid, page)) != DB_OK)
			return error;
		fileheader.pagecnt++;
		fileheader.firstfreepage = *pageid;
		DBUpdateFileHeader(fileid, fileheader, TRUE);

		if ((error = DBFilePageChanged(fileid, *pageid, TRUE)) != DB_OK)
		{
			printf("Interal error: DBAllocPage()\n");
			exit(1);
		}
	}	
	return DB_OK;
}

int DBDisposePage(int fileid, int pageid)
{
	int error;
	int i;
	DB_PAGE* page;
	BUF_PAGE* bufpage;
	DB_FILEHEADER fileheader = DBGetFileHeader(fileid);

	if(!DBInvalidFileID(fileid))
	{
		return DB_ERROR_INVALID_FILE;
	}
	if(!DBInvalidPageID(fileid, pageid))
	{
		return DB_ERROR_INVALID_PAGE;
	}
	if((bufpage = DBHashFind(fileid, pageid)) == NULL)
		return DB_ERROR_PAGE_NOTINBUF;
	if((error = BufRead(fileid, pageid, &page)) != DB_OK)
		return error;
	if((error = BufWrite(fileid, pageid, bufpage)) != DB_OK)
		return error;

	return DB_OK;
}

int DBPageFix(int fileid, int pageid)
{
	int i;
	DB_FILEHEADER fileheader = DBGetFileHeader(fileid);
	if(!DBInvalidFileID(fileid))
	{
		return DB_ERROR_INVALID_FILE;
	}
	if(!DBInvalidPageID(fileid, pageid))
	{
		return DB_ERROR_INVALID_PAGE;
	}	
	fileheader.fixed[pageid] = TRUE;
	for(i=0; i<fileheader.pagecnt; i++)
	{
		if(fileheader.fixed[i] == FALSE)
		{
			fileheader.firstfreepage = i;
			break;
		}
	}
	if(i == fileheader.pagecnt)
		fileheader.firstfreepage = DB_PAGE_LIST_END;
	DBUpdateFileHeader(fileid, fileheader, TRUE);
	return DB_OK;
}

int DBPageUnfix(int fileid, int pageid)
{
	DB_FILEHEADER fileheader = DBGetFileHeader(fileid);
	if(!DBInvalidFileID(fileid))
	{
		return DB_ERROR_INVALID_FILE;
	}
	if(!DBInvalidPageID(fileid, pageid))
	{
		return DB_ERROR_INVALID_PAGE;
	}	
	fileheader.fixed[pageid] = FALSE;
	if(fileheader.firstfreepage > pageid || fileheader.firstfreepage == DB_PAGE_LIST_END)
		fileheader.firstfreepage = pageid;
	DBUpdateFileHeader(fileid, fileheader, TRUE);
	return DB_OK;
}
int DBPageUsed(int fileid, int pageid)
{
	return BufUse(fileid, pageid);
}
int DBPageRelease(int fileid, int pageid, int dirty)
{
	return BufRelease(fileid, pageid, dirty);
}