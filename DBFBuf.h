#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include "DBPageType.h"
#include "DBFileType.h"
#include "DBFile.h"
#include "DBHash.h"

void InsertFree(BUF_PAGE* page);
void DeleteFree();
void InsertUsed(BUF_PAGE* page);
void DeleteUsed(BUF_PAGE* page);
int AllocPages(BUF_PAGE** bufpage);

extern int DBDisposePage(int fileid, int pageid);


int BufSetTimer(int fileid, int pageid, int lrutimer);
int BufRead(int fileid, int pageid, DB_PAGE** filepage);
int BufWrite(int fileid, int pageid, BUF_PAGE* bufpage);
DWORD WINAPI BufScan(PVOID pvParam);
HANDLE BufCheck();
DWORD WINAPI FreeScan(PVOID pvParam);
HANDLE FreeCheck();
BUF_PAGE* MinLRUCheck();
int BufAlloc(int fileid, int pageid, DB_PAGE** filepage);
int BufUse(int fileid, int pageid);
int BufRelease(int fileid, int pageid, BOOL dirty);
int FileRelease(int fileid);