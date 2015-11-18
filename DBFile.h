#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "DBFileType.h"
#include "DBPageType.h"
#include "DBHash.h"

extern int FileRelease(int fileid);
DB_FILEHEADER DBGetFileHeader(int fileid);
void DBUpdateFileHeader(int fileid, DB_FILEHEADER fileheader, short headstat);
int DBFileIsOpen(char* filename);
int DBFileEntryFull();

void DBInit();
int DBCreateFile(char* filename);
int DBDestroyFile(char* filename);
int DBOpenFile(char* filename);
int DBCloseFile(int fileid);
int DBReadFile(int fileid, int pageid, DB_PAGE* bufpage);
int DBWriteFile(int fileid, int pageid, DB_PAGE* bufpage);