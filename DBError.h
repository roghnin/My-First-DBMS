#ifndef _DBERROR_H_
#define _DBERROR_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "DBFUtility.h"

	static char* DB_ERROR[] = {
		"DB_OK",
		"memory overflow",
		"buffer overflow",
		"page is already in buffer",
		"page is not in buffer",
		"system error",
		"read fail",
		"write fail",
		"invalid pageID",
		"file is already open",
		"file table is full",
		"invalid fileID",
		"eof",
		"page is already free",
		"page is in file",
		"the page allocated is already in buffer",
		"page is not in hash table",
		"page is already in hash table",
		"page is used",
		"page is already disposed"
	};

	void DBPrintError(int error);

#ifdef __cplusplus
}
#endif 

#endif