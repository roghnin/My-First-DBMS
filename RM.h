#ifndef _RM_H_
#define _RM_H_
double abs(double x){
	return x>=0?x:(x*-1);
}

typedef struct RecordID{
	int pageid;
	int slot;
}RecordID;

typedef struct Attribute{
	string Attr_name;
	int Type;
	int Offset;
	int Indexed;
	int Status;//to be designed with signatural digits
	string Foreigntable;
	int Foreignkey;
	int Depended;
	string *Dependingtable;
	int *Dependingkey;
}Attribute;
//class Recordbuffer;
class DBFileINI{
public:	
	string DBname;
	string Filename;
	//int Fileid;//not included in the ini file, since fileid varies from time to time due to the reaction of file system.
	int Recordlength;
	int Pagerecordcnt;
	int Attr_cnt;
	int Recordcnt;
	Attribute *Attributes;
	
	int Checkattribute(string x){
		int i;
		for (i=0;i<Attr_cnt;i++){
			if (Attributes[i].Attr_name==x) return i;
		}
		return -1;
	}
	int Adddependence(int attr_order,string dependingtable,int dependingkey){
		int i;
		if (Attributes[attr_order].Depended==0){
			Attributes[attr_order].Dependingtable=new string(dependingtable);
			Attributes[attr_order].Dependingkey=new int(dependingkey);
		}
		else{
			string *newtablelist=new string[Attributes[attr_order].Depended+1];
			int *newkeylist=new int [Attributes[attr_order].Depended+1];
			for (i=0;i<Attributes[attr_order].Depended;i++){
				newtablelist[i]=Attributes[attr_order].Dependingtable[i];
				newkeylist[i]=Attributes[attr_order].Dependingkey[i];
			}
			Attributes[attr_order].Dependingtable=newtablelist;
			Attributes[attr_order].Dependingkey=newkeylist;
		}
		Attributes[attr_order].Depended++;
		Write_ini();
	}
	int Dropalldependence(){
		int i;
		for(i=0;i<Attr_cnt;i++){
			if (Attributes[i].Status==FOREIGN){
				DBFileINI foreignfile(const_cast<char*>(DBname.c_str()),const_cast<char*>(Attributes[i].Foreigntable.c_str()));
				foreignfile.Dropdependence(Attributes[i].Foreignkey,Filename,i);
			}
		}
	}//To be checked.
	int Dropdependence(int attr_order,string dependingtable, int dependingkey){
		int i,j;
		if (Attributes[attr_order].Depended==1){
			delete Attributes[attr_order].Dependingtable;
			delete Attributes[attr_order].Dependingkey;
		}
		else{
			string *newtablelist=new string[Attributes[attr_order].Depended-1];
			int *newkeylist=new int[Attributes[attr_order].Depended-1];
			for (i=0,j=0;i<Attributes[attr_order].Depended;i++,j++){
				if (Attributes[attr_order].Dependingtable[i]==dependingtable&&
					Attributes[attr_order].Dependingkey[i]==dependingkey){
						j--;
					}
				else{
					newtablelist[j]=Attributes[attr_order].Dependingtable[i];
					newkeylist[j]=Attributes[attr_order].Dependingkey[i];
				}
			}
			Attributes[attr_order].Dependingtable=newtablelist;
			Attributes[attr_order].Dependingkey=newkeylist;
		}
		Attributes[attr_order].Depended--;
		Write_ini();
	}//to be checked.
	
	int Write_ini(){
		int i,j;
		string Path=DBname+"//"+Filename+".i";
		ofstream inifile(Path.c_str(),ios::trunc);
		inifile<<Filename<<'\n'<<Recordlength<<'\n'<<Pagerecordcnt<<'\n'<<Attr_cnt<<'\n'<<Recordcnt<<endl;
		for (i=0;i<Attr_cnt;i++){
			inifile<<Attributes[i].Attr_name<<endl;
			inifile<<Attributes[i].Type<<endl;
			Attributes[i].Offset=0;
			for (j=0;j<i;j++){
				Attributes[i].Offset+=Attributes[j].Type;
			}
			inifile<<Attributes[i].Offset<<endl;
			inifile<<Attributes[i].Indexed<<endl;
			inifile<<Attributes[i].Status<<endl;
			inifile<<Attributes[i].Depended<<endl;
			if (Attributes[i].Status==FOREIGN){
				inifile<<Attributes[i].Foreigntable<<endl;
				inifile<<Attributes[i].Foreignkey<<endl;
			}
			for (j=0;j<Attributes[i].Depended;j++){
				inifile<<Attributes[i].Dependingtable[j]<<endl;
				inifile<<Attributes[i].Dependingkey[j]<<endl;
			}
		}
		inifile.close();
	}
	int Read_ini(char* filename_x){
		int i,j;
		string filename(filename_x);
		string Path=DBname+"/"+filename+".i";
		string buffer;
		ifstream inifile(Path.c_str(),ios::in);
		//cout<<"ini path:"<<Path<<endl;
		getline(inifile,Filename,'\n');//cout<<Filename<<endl;
		getline(inifile,buffer,'\n');
		Recordlength=atol(buffer.c_str());//cout<<Recordlength<<endl;
		getline(inifile,buffer,'\n');
		Pagerecordcnt=atoi(buffer.c_str());//cout<<Pagerecordcnt<<endl;
		getline(inifile,buffer,'\n');
		Attr_cnt=atoi(buffer.c_str());//cout<<Attr_cnt<<endl;
		getline(inifile,buffer,'\n');
		Recordcnt=atoi(buffer.c_str());//cout<<Recordcnt<<endl;
		Attributes=new Attribute[Attr_cnt];
		for(i=0;i<Attr_cnt;i++){
			getline(inifile,Attributes[i].Attr_name,'\n');//cout<<Attributes[i].Attr_name<<endl;
			getline(inifile,buffer,'\n');
			Attributes[i].Type=atoi(buffer.c_str());//cout<<Attributes[i].Type<<endl;
			getline(inifile,buffer,'\n');
			Attributes[i].Offset=atoi(buffer.c_str());//cout<<Attributes[i].Offset<<endl;
			getline(inifile,buffer,'\n');
			Attributes[i].Indexed=atoi(buffer.c_str());//cout<<Attributes[i].Indexed<<endl;
			getline(inifile,buffer,'\n');
			Attributes[i].Status=atoi(buffer.c_str());//cout<<Attributes[i].Status<<endl;
			getline(inifile,buffer,'\n');
			Attributes[i].Depended=atoi(buffer.c_str());//cout<<Attributes[i].Depended<<endl;
			if (Attributes[i].Status==FOREIGN){
				getline(inifile,Attributes[i].Foreigntable,'\n');//cout<<Attributes[i].Foreigntable<<endl;
				getline(inifile,buffer,'\n');
				Attributes[i].Foreignkey=atoi(buffer.c_str());//cout<<Attributes[i].Foreignkey<<endl;
			}
			if (Attributes[i].Depended){
				Attributes[i].Dependingtable=new string[Attributes[i].Depended];
				Attributes[i].Dependingkey=new int[Attributes[i].Depended];
			}
			for (j=0;j<Attributes[i].Depended;j++){
				getline(inifile,Attributes[i].Dependingtable[j],'\n');//cout<<Attributes[i].Dependingtable[j]<<endl;
				getline(inifile,buffer,'\n');
				Attributes[i].Dependingkey[j]=atoi(buffer.c_str());//cout<<Attributes[i].Dependingkey[j]<<endl;
			}
		}
		inifile.close();
	}
	DBFileINI(char *dbname,char *filename,int recordlength,int attr_cnt,Attribute *attributes){//use when creating a new file.
		int i;
		DBname=dbname;
		Filename=filename;
		Recordlength=recordlength;
		Pagerecordcnt=(DB_PGDATASIZE/recordlength)-1;
		Attr_cnt=attr_cnt;
		Attributes=attributes;
		Recordcnt=0;
		//DBCreateFile(filename);
		//Fileid=DBOpenFile(filename);
		/*Attributes=new Attribute[Attr_cnt];
		for (i=0;i<Attr_cnt;i++){
			Attributes[i].Attr_name= *(attr_names+i);
			Attributes[i].Type= *(types+i);
		}*/
		Write_ini();
		//DBCloseFile(Fileid);
	}
	DBFileINI(char *dbname,char *filename){//use when ini file exists.
		DBname=dbname;
		Read_ini(filename);
		//Fileid=DBOpenFile(filename);
	}
	~DBFileINI(){
		Write_ini();
	} 
};

 
class DBFile{
public:	
	DBFileINI Fileini;
	int Fileid;
	int Pagecnt;
	DBFile(char *dbname,char *filename):Fileini(dbname,filename){
		string DBname(dbname);
		string Filename(filename);
		string Path=DBname+"//"+Filename;
		char *Path_char=new char[Path.length()+1];
		strcpy (Path_char,Path.c_str());
		Fileid=DBOpenFile(Path_char);
		//cout<<"fileid:"<<Fileid<<endl;
		Pagecnt=DBGetFileHeader(Fileid).pagecnt;
		//cout<<"pagecnt:"<<Pagecnt<<endl;
	}
	DBFile(char *dbname,char *filename,int recordlength,int attr_cnt,Attribute *attributes):
			Fileini(dbname,filename,recordlength,attr_cnt,attributes){
		int i;
		char *p,**buf;
		int pageid=0,*p_pageid=&pageid;
		DB_PAGE *page,**p_page;
		char pointer_buff[4];
		string DBname(dbname);
		string Filename(filename);
		string Path=DBname+"//"+Filename;
		char *Path_char=new char[Path.length()+1];
		strcpy (Path_char,Path.c_str());
		p_page=&page;buf=&p;//must act like this, don't know why.
		DBCreateFile(Path_char);
		Fileid=DBOpenFile(Path_char);
		//cout<<"create file id:"<<Fileid<<endl;
		DBAllocPage(Fileid,p_pageid,p_page);
		Pagecnt=1;
		DBGetFirstPage(Fileid,buf);
		DBPageUsed(Fileid,0);
		Formatpagedata(buf);
		/*for (i=0;i<=Fileini.Pagerecordcnt;i++){
			sprintf(pointer_buff,"%04d",i);
			//cout<<"test."<<pointer_buff;
			strcpy((*buf+(i*Fileini.Recordlength)),pointer_buff);
		}*/
		DBPageRelease(Fileid,0,1);
		DBDisposePage(Fileid,0);
	}
	~DBFile(){
		DBCloseFile(Fileid);
	}
	int Formatpagedata(char **buf){
		int i;
		char pointer_buff[5];
		for (i=0;i<=Fileini.Pagerecordcnt;i++){
			sprintf(pointer_buff,"%04d",i);
			//cout<<"test."<<pointer_buff;
			strcpy((*buf+(i*Fileini.Recordlength)),pointer_buff);
		}
	}
	
	int Deleterecord(RecordID target){
		char *p,**buf;
		buf=&p;
		DBGetPage(Fileid,target.pageid,buf);
		DBPageUsed(Fileid,target.pageid);
		Setslotempty(buf,target.slot);
		DBPageUnfix(Fileid,target.pageid);
		DBPageRelease(Fileid,target.pageid,1);
		DBDisposePage(Fileid,target.pageid);
		Fileini.Recordcnt--;
		Fileini.Write_ini();
	}
	int Overwriterecord(RecordID target,char *source){
		char *p,**buf;
		buf=&p;
		DBGetPage(Fileid,target.pageid,buf);
		DBPageUsed(Fileid,target.pageid);
		strcpy(*buf+(target.slot*Fileini.Recordlength+4),source);
		DBPageRelease(Fileid,target.pageid,1);
		DBDisposePage(Fileid,target.pageid);
	}
	int Setslotempty(char **buf,int slot){
		int i;
		char buffer_char[5];
		sprintf(buffer_char,"%04d",slot);
		for (i=0;i<4;i++){
			*(*buf+i+(slot+1)*Fileini.Recordlength)=*(*buf+i);
			*(*buf+i)=buffer_char[i];
		}
	} 
	int Clearslot (char **buf,int slot){
		int i;
		for (i=0;i<Fileini.Recordlength-4;i++){
			*(*buf+slot*Fileini.Recordlength+4+i)='\0';
		}
	}
	RecordID Insertrecord(char *source){
		RecordID id;
		int pageid=0,*p_pageid=&pageid;
		char *p,**buf;
		char buffer_char[5];
		//char *temp="source.";
		DB_PAGE *page,**p_page;
		int i;
		int slot;
		buf=&p;p_page=&page;
		
		DBAllocPage(Fileid,p_pageid,p_page);
		id.pageid=*p_pageid;
		DBGetPage(Fileid,*p_pageid,buf);
		DBPageUsed(Fileid,*p_pageid);
		if (Pagecnt<DBGetFileHeader(Fileid).pagecnt){
			//cout<<"new page allocated."<<endl;
			Formatpagedata(buf);
			//cout<<"formated."<<endl;
		}
		slot=Getemptyslot(buf);
		//cout<<"slot:"<<slot;
		id.slot=slot;
		Clearslot(buf,slot);
		strcpy(*buf+(slot*Fileini.Recordlength+4),source);
		Setslotfull(buf,slot);
		//cout<<Fileini.Pagerecordcnt;
		if (Getemptyslot(buf)>=Fileini.Pagerecordcnt){
			DBPageFix(Fileid,*p_pageid);
			//cout<<"page fixed.";
		} 
		DBPageRelease(Fileid,*p_pageid,1);
		DBDisposePage(Fileid,*p_pageid);
		Pagecnt=DBGetFileHeader(Fileid).pagecnt;
		Fileini.Recordcnt++;
		Fileini.Write_ini();
		return id;
	}
	string* Getallrecords(RecordID **ids){
		int i,j;
		char *p,**buf;
		buf=&p;
		string *out_data=new string[Fileini.Recordcnt];
		RecordID *out_id=new RecordID[Fileini.Recordcnt];
		int counter=0;
		string buffer;
		for (i=0;i<Pagecnt;i++){
			DBGetPage(Fileid,i,buf);
			for (j=0;j<Fileini.Pagerecordcnt;j++){
				//cout<<Getslotpointer(buf,j+1)<<endl;
				if (Getslotpointer(buf,j+1)==FULL_SLOT){
					out_data[counter]=Getslotdata(buf,j);
					out_id[counter].pageid=i;
					out_id[counter].slot=j;
					counter++;
				}
			}
		}
		if (Fileini.Recordcnt==counter){
			*ids=out_id;
			return out_data;
		}
		else return NULL;
	} 
	char *Getslotdata(RecordID *rid){
		char *p,**buf;
		buf=&p;
		DBGetPage(Fileid,rid->pageid,buf);
		//cout<<"get slot"<<rid->slot<<":"<<(*buf+(rid->slot)*Fileini.Recordlength+4)<<endl;
		return (*buf+(rid->slot)*Fileini.Recordlength+4);
	}
	char *Getslotdata(char **buf,int slot){
		//cout<<"get slot"<<slot<<":"<<(*buf+slot*Fileini.Recordlength+4)<<endl;
		return (*buf+slot*Fileini.Recordlength+4);
	}
	
	int Getemptyslot(char **buf){
		return Getslotpointer(buf,0);
	}
	int Getslotpointer(char **buf,int offset){
		char buffer_char[5];
		int i;
		for (i=0;i<4;i++){
			buffer_char[i]=*(*buf+i+offset*Fileini.Recordlength);
		}
		buffer_char[4]='\0';
		//cout<<"got pointer:"<<buffer_char<<endl;
		return (atoi(buffer_char));
	}
	int Setslotfull(char **buf,int dirtyslot){
		int i;
		char buffer_char[5];
		sprintf(buffer_char,"%04d",Getslotpointer(buf,dirtyslot+1));
		for (i=0;i<4;i++){
			*(*buf+i)=buffer_char[i];
			*(*buf+i+(dirtyslot+1)*Fileini.Recordlength)='9';
		}
	}
};

class DBData{
public:
	string DBname;
	int Filecnt;
	string *Tablenames;
	string *Indexes;
	int Indexcnt;
	DBData(char *dbname,int create):DBname(dbname){
		if (create==1){
			_mkdir(DBname.c_str());
			//DBInit();
			//BufCheck();
			Filecnt=0;
			Indexcnt=0;
		}
		else {
			Read_dbini(dbname);
			//DBInit();
			//BufCheck();
		}
	}
	void Showindex(){
		int i;
		for (i=0;i<Indexcnt;i++){
			cout<<Indexes[i]<<endl;
		}
	}
	void Showtablenames(){
		int i;
		for (i=0;i<Filecnt;i++){
			cout<<Tablenames[i]<<' ';
		}
	}
	int Checktable(char *tablename){
		string name=tablename;
		int i;
		for (i=0;i<Filecnt;i++){
			if (Tablenames[i]==name) return 1;
		}
		return 0;
	}
	int Newtablename(char *tablename){
		int i;
		string newname=tablename;
		string *newlist=new string[Filecnt+1];
		for (i=0;i<Filecnt;i++){
			newlist[i]=Tablenames[i];
		}
		for (i=0;i<Filecnt;i++){
			delete (Tablenames+i);
		}//just like realloc.
		newlist[Filecnt++]=newname;
		Tablenames=newlist;
	} 
	int Droptablename(char *tablename){
		int i,j;
		string currentname=tablename;
		string *newlist=new string[Filecnt-1];
		for(i=0,j=0;i<Filecnt;i++){
			if (Tablenames[i]!=currentname){
				newlist[j++]=Tablenames[i];
			}
		}
		for (i=0;i<Filecnt;i++){
			delete (Tablenames+i);
		}
		Filecnt--;
		Tablenames=newlist;
	}
	int Newindexname(string indexname){
		int i;
		string *newlist=new string[Indexcnt+1];
		for (i=0;i<Indexcnt;i++){
			newlist[i]=Indexes[i];
		}
		for (i=0;i<Indexcnt;i++){
			delete (Indexes+i);
		}//just like realloc.
		newlist[Indexcnt++]=indexname;
		Indexes=newlist;
	}
	int Dropindexname(string indexname){
		int i,j;
		string *newlist=new string[Indexcnt-1];
		for(i=0,j=0;i<Indexcnt;i++){
			if (Indexes[i]!=indexname){
				newlist[j++]=Indexes[i];
			}
		}
		for (i=0;i<	Indexcnt;i++){
			delete (Indexes+i);
		}
		Indexcnt--;
		Indexes=newlist;
	}
	int Write_dbini(){
		int i;
		string Path=DBname+"//"+DBname+".dbi";
		ofstream inifile(Path.c_str(),ios::trunc);
		inifile<<DBname<<'\n'<<Filecnt<<endl;
		for (i=0;i<Filecnt;i++){
			inifile<<Tablenames[i]<<endl;
		}
		inifile<<Indexcnt<<endl;
		for (i=0;i<	Indexcnt;i++){
			inifile<<Indexes[i]<<endl;
		}
		inifile.close();
	}
	int Read_dbini(char* dbname_x){
		int i;
		string dbname(dbname_x);
		string Path=dbname+"//"+dbname+".dbi";
		string buffer;
		ifstream inifile(Path.c_str(),ios::in);
		getline(inifile,DBname,'\n');//cout<<DBname<<endl;
		getline(inifile,buffer,'\n');
		Filecnt=atoi(buffer.c_str());//cout<<Filecnt<<endl;
		Tablenames=new string[Filecnt];
		for(i=0;i<Filecnt;i++){
			getline(inifile,Tablenames[i],'\n');//cout<<Tablenames[i]<<endl;
		}
		getline(inifile,buffer,'\n');
		Indexcnt=atoi(buffer.c_str());//cout<<Indexcnt<<endl;
		for(i=0;i<Indexcnt;i++){
			getline(inifile,Indexes[i],'\n');//cout<<Indexes[i]<<endl;
		}
		inifile.close();
	}
	int DBCreateTable(char *tablename,int attr_cnt,Attribute *attributes){
		char *DBname_char=new char[DBname.length()+1];
		strcpy(DBname_char,DBname.c_str());
		int i;
		int recordlength=5;//calculate recordlength. 4 for the length of pointer,1 for the '\0' in the end.
		for (i=0;i<attr_cnt;i++){
			recordlength+=attributes[i].Type;
		}
		DBFile *Tabledata=new DBFile(DBname_char,tablename,recordlength,attr_cnt,attributes);
		Newtablename(tablename);
		delete Tabledata; 
		Write_dbini();
	}
	int DBDeletetable(char *tablename){
		int i,j;
		string Filename(tablename);
		string Path=DBname+"//"+Filename;
		string INIPath=Path+".i";
		char *Path_char=new char[Path.length()+1];
		char *INIPath_char=new char[INIPath.length()+1];
		DBFileINI *tableini=new DBFileINI(const_cast<char*>(DBname.c_str()),tablename);
		for (i=0;i<tableini->Attr_cnt;i++){//delete all depending tables.
			if (tableini->Attributes[i].Depended>0){
				for (j=0;j<tableini->Attributes[i].Depended;j++){
					DBDeletetable(const_cast<char*>(tableini->Attributes[i].Dependingtable[j].c_str()));
				}
			}
		}//to be checked.
		tableini->Dropalldependence();
		strcpy (Path_char,Path.c_str());
		DBDestroyFile(Path_char);
		remove(INIPath.c_str());
		Droptablename(tablename);
		Write_dbini();
	}
	~DBData(){Write_dbini();} 
};

#endif