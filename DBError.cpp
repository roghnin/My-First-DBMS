#include "DBError.h"
#include <stdio.h>
void DBPrintError(int error)
{
	printf("Error: ");
	printf(DB_ERROR[-1*error]);
	printf(".\n");
}
