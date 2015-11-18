#include "DBFBuf.h"

int bufpagecnt = 0;
BUF_PAGE* firstpage = NULL;
BUF_PAGE* lastpage = NULL;
BUF_PAGE* freepage = NULL;

void InsertFree(BUF_PAGE* page)
{
	page->nextpage = freepage;
	freepage = page;
}

void DeleteFree()
{
	freepage = freepage->nextpage;
}

void InsertUsed(BUF_PAGE* page)
{
	page->nextpage = firstpage;
	page->prevpage = NULL;
	if(firstpage != NULL)
	{
		firstpage->prevpage = page;
	}
	else
	{
		lastpage = page;
	}
	firstpage = page;
}

void DeleteUsed(BUF_PAGE* page)
{
	if(firstpage == page)
		firstpage = firstpage->nextpage;
	if(lastpage == page)
		lastpage = lastpage->prevpage;
	if(page->nextpage != NULL)
		page->nextpage->prevpage = page->prevpage;
	if(page->prevpage != NULL)
		page->prevpage->nextpage = page->nextpage;
	page->nextpage = NULL;
	page->prevpage = NULL;
}

int AllocPages(BUF_PAGE** bufpage)
{
	BUF_PAGE* page;
	int error;

	if (freepage != NULL)
	{
		*bufpage = freepage;
		freepage = (*bufpage)->nextpage;
	}
	else if (bufpagecnt < DB_MAX_BUF_PAGENUM)
	{
		if ((*bufpage = (BUF_PAGE*)malloc(sizeof(BUF_PAGE))) == NULL)
		{
			*bufpage = NULL;
			return DB_ERROR_MEM_OVERFLOW;
		}
		bufpagecnt++;
	}
	else 
	{
		HANDLE hThread = FreeCheck();
		WaitForSingleObject(hThread, INFINITE);
		*bufpage = freepage;
		freepage = (*bufpage)->nextpage;
	}
	InsertUsed(*bufpage);
	return DB_OK;
}

int BufRead(int fileid, int pageid, DB_PAGE** filepage)
{
	BUF_PAGE* bufpage;
	int error;

	if ((bufpage = DBHashFind(fileid, pageid)) == NULL)
	{
		if ((error = AllocPages(&bufpage)) != DB_OK)
		{
			*filepage = NULL;
			return(error);
		}
		if ((error = DBReadFile(fileid, pageid, &bufpage->page)) != DB_OK)
		{
			DeleteUsed(bufpage);
			InsertFree(bufpage);
			*filepage = NULL;
			return error;
		}
		if ((error = DBHashInsert(fileid, pageid, bufpage)) != DB_OK)
		{
			DeleteUsed(bufpage);
			InsertFree(bufpage);
			return error;
		}

		bufpage->fileid = fileid;
		bufpage->pageid = pageid;
		bufpage->dirty = FALSE;
		bufpage->lrucnt = LRU_TIMER;
	}
	else if (bufpage->used)
	{
		*filepage = &bufpage->page;
		return DB_ERROR_PAGE_USED;
	}
	else if(bufpage->lrucnt < LRU_TIMER)
	{
		bufpage->lrucnt++;
	}
	bufpage->inbuf = TRUE;
	bufpage->used = FALSE;
	*filepage = &bufpage->page;
	return DB_OK;
}

int BufWrite(int fileid, int pageid, BUF_PAGE* bufpage)
{
	int error;

	if (bufpage == NULL)
	{
		return DB_ERROR_BUF_OVERFLOW;
	}
	if(bufpage->dirty)
		if ((error = DBWriteFile(bufpage->fileid, bufpage->pageid, &bufpage->page) != DB_OK))
			return error;
	bufpage->dirty = FALSE;
	bufpage->inbuf = FALSE;
	bufpage->used = FALSE;
	if ((error = DBHashDelete(bufpage->fileid, bufpage->pageid)) != DB_OK)
		return error;
	DeleteUsed(bufpage);
	InsertFree(bufpage);

	return DB_OK;
}

DWORD WINAPI BufScan(PVOID pvParam)
{
	int error;
	int pagecnt;
	BUF_PAGE* buf;
	BUF_PAGE* tbuf = (BUF_PAGE*)malloc(sizeof(BUF_PAGE));
	DB_FILEHEADER fileheader;

	while(1)
	{
		pagecnt = 0;
		Sleep(1000);
		for(buf = firstpage; buf != NULL; buf = buf->nextpage)
		{
			pagecnt++;
			//			printf("%d  %d  %d  %d\n",buf->fileid, buf->pageid, buf->used, buf->lrucnt);
			if(!buf->used && buf->inbuf)
			{
				buf->lrucnt--;
				if(buf->lrucnt == 0)
				{
					if (buf == NULL)
					{
						return DB_ERROR_BUF_OVERFLOW;
					}
					if(buf->dirty)
						if ((error = DBWriteFile(buf->fileid, buf->pageid, &buf->page) != DB_OK))
							return error;
					buf->dirty = FALSE;
					buf->inbuf = FALSE;
					buf->used = FALSE;
					if ((error = DBHashDelete(buf->fileid, buf->pageid)) != DB_OK)
						return error;
					*tbuf = *buf;
					DeleteUsed(buf);
					InsertFree(buf);
					buf = tbuf;
					//					printf("FileID:  %d  PageID: %d.  Buffer --> File.\n", buf->fileid, buf->pageid);
				}
			}
		}
		//		printf("%d\n", pagecnt);
	}

	free(tbuf);
	return 0;
}

DWORD WINAPI FreeScan(PVOID pvParam)
{

	BUF_PAGE* buf = MinLRUCheck();
	//	printf("FileID:   %d   PageID:   %d.  Buffer --> File.\n", buf->fileid, buf->pageid);
	DBDisposePage(buf->fileid, buf->pageid);
	return 0;
}

HANDLE BufCheck()
{
	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, BufScan, NULL, 0, &dwThreadID);
	return hThread;
}

HANDLE FreeCheck()
{
	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, FreeScan, NULL, 0, &dwThreadID);
	return hThread;
}

BUF_PAGE* MinLRUCheck()
{
	BUF_PAGE* buf;
	BUF_PAGE* bufpage;
	int minlru = lastpage->lrucnt;
	bufpage = lastpage;
	for(buf = lastpage; buf != NULL; buf = buf->prevpage)
	{
		if(buf->lrucnt < minlru && !buf->used)
		{
			minlru = buf->lrucnt;
			bufpage = buf;
		}
	}
	return bufpage;
}

int BufAlloc(int fileid, int pageid, DB_PAGE** filepage)
{
	BUF_PAGE* bufpage;
	int error;
	*filepage = NULL;

	if((bufpage = DBHashFind(fileid, pageid)) != NULL)
		return DB_ERROR_PAGE_INBUF;
	if((error = AllocPages(&bufpage)) != DB_OK)
		return error;
	if((error = DBHashInsert(fileid, pageid, bufpage)) != DB_OK)
	{
		DeleteUsed(bufpage);
		InsertFree(bufpage);
		return error;
	}
	bufpage->fileid = fileid;
	bufpage->pageid = pageid;
	bufpage->inbuf = TRUE;
	bufpage->used = FALSE;
	bufpage->dirty = FALSE;
	bufpage->lrucnt = LRU_TIMER;
	*filepage = &bufpage->page;

	return DB_OK;
}

int BufRelease(int fileid, int pageid, BOOL dirty)
{
	BUF_PAGE* bufpage;
	if((bufpage = DBHashFind(fileid, pageid)) == NULL)
		return DB_ERROR_HASH_NOTFOUND;
	bufpage->used = FALSE;
	if(dirty)
		bufpage->dirty = TRUE;
	else
		bufpage->dirty = FALSE;
	return DB_OK;
}

int BufUse(int fileid, int pageid)
{
	BUF_PAGE* bufpage;
	if((bufpage = DBHashFind(fileid, pageid)) == NULL)
		return DB_ERROR_HASH_NOTFOUND;
	bufpage->used = TRUE;
	//	bufpage->lrucnt++;
	return DB_OK;
}

int FileRelease(int fileid)
{
	BUF_PAGE* buf;
	BUF_PAGE* tbuf;
	int error;

	buf = firstpage;
	while (buf != NULL)
	{
		if (buf->fileid == fileid)
		{
			if (buf->used)
			{
				return DB_ERROR_PAGE_USED;
			}

			if(buf->dirty)
				if ((error = DBWriteFile(buf->fileid, buf->pageid, &buf->page)) != DB_OK)
					return error;
			buf->dirty = FALSE;
			buf->inbuf = FALSE;

			if ((error = DBHashDelete(buf->fileid,buf->pageid)) != DB_OK)
			{
				printf("Internal error: DBFBufReleaseFile()\n");
				exit(1);
			}

			tbuf = buf;
			buf = buf->nextpage;
			DeleteUsed(tbuf);
			InsertFree(tbuf);
		}
		else
		{
			buf = buf->nextpage;
		}
	}

	return DB_OK;
}

int BufSetTimer(int fileid, int pageid, int lrutimer)
{
	BUF_PAGE* bufpage;

	if((bufpage = DBHashFind(fileid, pageid)) == NULL)
		return DB_ERROR_PAGE_NOTINBUF;
	bufpage->lrucnt = lrutimer;
	return DB_OK;
}