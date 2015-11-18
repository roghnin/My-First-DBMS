#include"DBError.cpp"
#include"DBFBuf.cpp"
#include"DBFile.cpp"
#include"DBHash.cpp"
#include"DBPageManager.cpp"
#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdlib>
using namespace std;

//Error infos
#define INVALID_KEY -1
#define TABLE_NOTFOUND -2
#define ZERO_ATTRIBUTE -3
#define ATTRIBUTE_NOTFOUND -4
#define INVALID_VALUETYPE -5
#define VALUE_NOTUNIQUE -6
#define VALUE_NOTFOREIGN -7
#define INDEX_NOTUNIQUE -8
#define INVALID_ATTR -9
#define MAINKEY_ISNULL -10
#define DB_NOTUSING -11
#define TABLE_ISEMPTY -12

#define FULL_SLOT 9999
//Data types
#define DB_PGDATASIZE 8096
#define DB_INT 9
#define DB_CHAR 1
#define DB_FLOAT 17
#define DB_STRING 20
//edgs of types
#define MAX_INTABS 99999998
#define NULL_INT -99999999
#define MAX_FLOATABS 99999999.9999998
#define NULL_FLOAT -99999999.9999999
#define NULL_CHAR '\3'
//Statuses of Attributes
#define PLAIN 0
#define UNIQUE 1
#define KEY 3
#define FOREIGN 8

#include"RM.h"
#include"buffer_ST.h"
#include"INDEXtemplate.h"
#include"SM.h"

void Errout(int errinfo){
	switch (errinfo){
		case INVALID_KEY: cout<<"Wrong keyword(s). Please check again."<<endl;break;
		case TABLE_NOTFOUND: cout<<"Table not found."<<endl;break;
		case ZERO_ATTRIBUTE: cout<<"No attributes inputed."<<endl;break;
		case ATTRIBUTE_NOTFOUND: cout<<"Attribute(s) not found."<<endl;break;
		case INVALID_VALUETYPE: cout<<"Required valuetype is not supported."<<endl;break;
		case VALUE_NOTUNIQUE: cout<<"A unique value is needed."<<endl;break;
		case VALUE_NOTFOREIGN: cout<<"A value in foreign key is needed."<<endl;break;
		case INDEX_NOTUNIQUE: cout<<"Index must be built on unique values."<<endl;break;
		case INVALID_ATTR: cout<<"Required attribute is not supported."<<endl;break;
		case MAINKEY_ISNULL: cout<<"Main key cannot be set NULL."<<endl;break;
		case DB_NOTUSING: cout<<"No database using."<<endl;break;
		case TABLE_ISEMPTY: cout<<"The table required is empty."<<endl;break;
	}
} 

int main(){
	Sysbuffer System;
	while (1){
		Errout(System.DBCommandResolution());
	}
}