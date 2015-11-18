#ifndef _DBFUTILITY_
#define _DBFUTILITY_

#define DB_PGSIZE			8192
#define DB_HASH_TAB_SIZE	1000
#define DB_TIMER_MAX		200
#define DB_MAX_FILENUM		20
#define DB_MAX_FILEPAGE		5000
#define DB_MAX_BUF_PAGENUM	5000

#define LRU_TIMER			10

#define DB_PAGE_LIST_END	-1
#define DB_FILE_PAGE_INBUF	-2

#define DB_PAGE_INFILE		0
#define DB_PAGE_INBUF		1
#define DB_PAGE_USED		2

#define DB_OK						0							
#define DB_ERROR_MEM_OVERFLOW		-1							
#define DB_ERROR_BUF_OVERFLOW		-2							
#define DB_ERROR_PAGE_INBUF 		-3					
#define DB_ERROR_PAGE_NOTINBUF		-4
#define DB_ERROR_OP					-5
#define DB_ERROR_READ_FAIL			-6
#define DB_ERROR_WRITE_FAIL			-7
#define DB_ERROR_HDR_READ_FAIL		-8
#define DB_ERROR_HDR_WRITE_FAIL		-9
#define DB_ERROR_INVALID_PAGE		-10
#define DB_ERROR_FILE_OPEN			-11
#define	DB_ERROR_TAB_FULL			-12
#define DB_ERROR_INVALID_FILE		-13
#define DB_ERROR_EOF				-14
#define DB_ERROR_PAGE_FREE			-15
#define DB_ERROR_PAGE_INFILE		-16
#define DB_ERROR_NEWPAGE_INBUF		-17
#define DB_ERROR_HASH_NOTFOUND		-18
#define DB_ERROR_HASH_PAGE_EXIST	-19
#define DB_ERROR_PAGE_USED			-20
#define DB_ERROR_DISPOSED			-21
#define BPTI_NOT_FOUND -1;
#define BPTI_FOUND 0;

#ifndef TRUE
#define TRUE 1		
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif