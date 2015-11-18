#ifndef _SM_H_
#define _SM_H_
class Sysbuffer{
public:
	DBData *Database;
	OperationSource source;
	string *Dbnames;
	int Dbcnt;
	Sysbuffer(){
		Database=NULL;
		Readsysini();
	}
	int Writesysini(){
		int i;
		ofstream inifile("system.ini",ios::trunc);
		inifile<<Dbcnt<<endl;
		for (i=0;i<Dbcnt;i++){
			inifile<<Dbnames[i]<<endl;
		}
		inifile.close();
	}
	int Readsysini(){
		int i;
		ifstream inifile("system.ini",ios::in);
		string buffer;
		if (inifile.is_open()){
			getline(inifile,buffer,'\n');
			Dbcnt=atoi(buffer.c_str());//cout<<Dbcnt<<endl;
			Dbnames=new string[Dbcnt];
			for (i=0;i<Dbcnt;i++){
				getline(inifile,Dbnames[i],'\n');//cout<<Dbnames[i]<<endl;
			}
		}
		else{
			Dbcnt=0;
			Dbnames=NULL;
		}
	}
	int Newdbname(string dbname){
		int i;
		string *newlist=new string[Dbcnt+1];
		for (i=0;i<Dbcnt;i++){
			newlist[i]=Dbnames[i];
		}
		for (i=0;i<Dbcnt;i++){
			delete (Dbnames+i);
		}//just like realloc.
		newlist[Dbcnt++]=dbname;
		Dbnames=newlist;
	}
	int Dropdbname(string dbname){
		int i,j;
		string *newlist=new string[Dbcnt-1];
		for(i=0,j=0;i<Dbcnt	;i++){
			if (Dbnames	[i]!=dbname){
				newlist[j++]=Dbnames[i];
			}
		}
		for (i=0;i<	Dbcnt;i++){
			delete (Dbnames+i);
		}
		Dbcnt--;
		Dbnames=newlist;
	}
	void Showdatabase(){
		int i;
		for (i=0;i<Dbcnt;i++){
			cout<<Dbnames[i]<<":";
			DBData *buffer=new DBData(const_cast<char*>(Dbnames[i].c_str()),0);
			buffer->Showtablenames();
			cout<<endl;
		}
	}
	void Showtable(){
		int i,j;
		for (i=0;i<Database->Filecnt;i++){
			cout<<Database->Tablenames[i]<<":"<<endl;
			DBFile *buffer=new DBFile(const_cast<char*>(Database->DBname.c_str()),const_cast<char*>(Database->Tablenames[i].c_str()));
			for(j=0;j<buffer->Fileini.Attr_cnt;j++){
				cout<<buffer->Fileini.Attributes[j].Attr_name<<":";
				switch (buffer->Fileini.Attributes[j].Type){
					case DB_INT:cout<<"int";break;
					case DB_FLOAT:cout<<"float";break;
					case DB_CHAR:cout<<"char";break;
					case DB_STRING:cout<<"string";break;
				}
				switch (buffer->Fileini.Attributes[j].Status){
					case UNIQUE:cout<<",unique";break;
					case KEY:cout<<",main key";break;
					case FOREIGN:cout<<",foreign key:";break;
				}
				if (buffer->Fileini.Attributes[j].Status==FOREIGN){
					DBFileINI foreigntable(const_cast<char*>(Database->DBname.c_str()),const_cast<char*>(buffer->Fileini.Attributes[j].Foreigntable.c_str()));
					cout<<buffer->Fileini.Attributes[j].Foreigntable<<"."<<foreigntable.Attributes[buffer->Fileini.Attributes[j].Foreignkey].Attr_name;
				}
				cout<<endl;
			}
			cout<<endl;
		}
	}
	Attribute *Getattributes(int &attr_cnt,string tablename){
		int i,j;
		Attribute *Attributes;
		string *buffer;
		//cout<<source;
		source.Deletesparespaces();
		source.Formatsymbols();
		//attr_cnt=count(source.begin(),source.end(),',')+1;
		attr_cnt=source.Countattributes();
		//cout<<"attr_cnt:"<<attr_cnt<<endl;
		Attributes=new Attribute[attr_cnt];
		for (i=0;i<attr_cnt;i++){
			Attributes[i].Status=PLAIN;
			Attributes[i].Depended=0;
			Attributes[i].Indexed=0;
			Attributes[i].Attr_name=*source.Getvalue();
			for (j=0;j<i;j++){
				if (Attributes[i].Attr_name==Attributes[j].Attr_name) return NULL;
			}
			buffer=source.Getkeyword();
			if (*buffer=="int") Attributes[i].Type=DB_INT;
			else if (*buffer=="string") Attributes[i].Type=DB_STRING;
			else if (*buffer=="char") Attributes[i].Type=DB_CHAR;
			else if (*buffer=="float") Attributes[i].Type=DB_FLOAT;//can't use switch
			//Offset is written when writing into ini file.
			
			delete buffer;
			
			do {
				buffer=source.Getkeyword(); 
				//cout<<"buffer:"<<*buffer<<endl;
				if (*buffer==","||*buffer==")") break;
				else if (*buffer=="unique"){
					Attributes[i].Status=UNIQUE;
				}
				else if (*buffer=="main"&& *source.Getkeyword()=="key") Attributes[i].Status=KEY;
				else if (*buffer=="foreign"&& *source.Getkeyword()=="key"){
					Attributes[i].Status=FOREIGN;
					buffer=source.Getvalue();
					Attributes[i].Foreigntable=buffer->substr(0,buffer->find("."));//tablename
					//cout<<"foreign table:"<<Attributes[i].Foreigntable<<endl;
					if (Database->Checktable(const_cast<char*>(Attributes[i].Foreigntable.c_str()))){
						
						string foreignattr=buffer->substr(buffer->find(".")+1);//attribute name
						//cout<<"foreign key:"<<foreignattr<<endl;
						DBFile *table= new DBFile(const_cast<char*>(Database->DBname.c_str()),const_cast<char*>(Attributes[i].Foreigntable.c_str()));
						//cout<<table->Fileid<<endl; 
						//cout<<"attr_count:"<<table->Fileini.Attr_cnt<<endl;
						int attrorder_foreign;
						//for(j=0;j<table->Fileini.Attr_cnt&& *buffer!=table->Fileini.Attributes[j].Attr_name;j++);//search for the required foreign key
						attrorder_foreign=table->Fileini.Checkattribute(foreignattr);
						if (attrorder_foreign<0){
							//cout<<"attribute not found."<<endl;
							return NULL;
						}
						//cout<<"attrorder:"<<attrorder_foreign<<endl;
						if (table->Fileini.Attributes[attrorder_foreign].Status&UNIQUE && table->Fileini.Attributes[attrorder_foreign].Type==Attributes[i].Type){
						//ensure the foreign key is of the same type, and the status is available.
							Attributes[i].Foreignkey=attrorder_foreign;
							//cout<<"foreign key:"<<attrorder_foreign<<endl;
							/*
							table->Fileini.Attributes[j].Depended++;
							table->Fileini.Attributes[j].Dependingtable=tablename;
							table->Fileini.Attributes[j].Dependingkey=i;*/
							table->Fileini.Adddependence(attrorder_foreign,tablename,i);
						}
						else{
							cout<<"attribute is not available."<<endl;
							return NULL;
						}
					}
					else{
						cout<<"table not found."<<endl;
						return NULL;
					}
				}
				else {
					cout<<"invalid key."<<endl;
					return NULL;
				}
			}while(source.Getlength());
		}
		return Attributes;
	}
	
	int Createindex(){
		string tablename=*source.Getvalue();
		if (*source.Getvalue()!="(") return INVALID_KEY;
		string attrname=*source.Getvalue();
		Tablebuffer *table=new Tablebuffer(Database->DBname,tablename);
		int attrorder=table->Checkattr(attrname);
		if (attrorder<0) return ATTRIBUTE_NOTFOUND;
		if (!(table->Tablefile->Fileini.Attributes[attrorder].Status&UNIQUE)) return INDEX_NOTUNIQUE;
		if (!table->Recordcnt) return TABLE_ISEMPTY;
		int datatype=table->Records[0]->Values[attrorder]->type;
		DBIndex index(datatype);
		index.Makeindex(table,attrorder);
		table->Tablefile->Fileini.Attributes[attrorder].Indexed=1;
		Database->Newindexname(tablename+"."+attrname);
		return 0;
	}
	int Dropindex(){
		string tablename=*source.Getvalue();
		if (*source.Getvalue()!="(") return INVALID_KEY;
		string attrname=*source.Getvalue();
		DBFile table(const_cast<char*>(Database->DBname.c_str()),const_cast<char*>(tablename.c_str()));
		int attrorder=table.Fileini.Checkattribute(attrname);
		if (attrorder<0) return ATTRIBUTE_NOTFOUND;
		string idxpath=Database->DBname+"\\"+tablename+"."+attrname+".idx";
		remove(idxpath.c_str());
		table.Fileini.Attributes[attrorder].Indexed=0;
		Database->Dropindexname(tablename+"."+attrname);
		return 0;
	}
	int Dropdatabase(string* name){
		int i;
		if (Database) delete Database;
		Database=new DBData(const_cast<char*>((*name).c_str()),0);
		string DBIpath=Database->DBname+"//"+Database->DBname+".dbi";
		for (;Database->Filecnt;){
			cout<<Database->Tablenames[0];
			Database->DBDeletetable(const_cast<char*>(Database->Tablenames[0].c_str()));
		}
		remove(DBIpath.c_str());
		RemoveDirectory(Database->DBname.c_str());
		Dropdbname(*name);
		Writesysini();
		return 0;
	}
	int Insert(){
		//resolute the tablename and attributes
		string tablename;
		int attr_cnt;//the sum of attributes in the key words, not attributes in the table in all.
		int *attr_orders;
		int i;
		string *buffer;
		tablename = *source.Getkeyword();
		if (!Database->Checktable(const_cast<char*>(tablename.c_str()))) return TABLE_NOTFOUND;//table not found.
		//load table
		DBFile *Table=new DBFile(const_cast<char*>(Database->DBname.c_str()),const_cast<char*>(tablename.c_str()));
		Recordbuffer Record(&(Table->Fileini));
		Attribute *Attributes=Table->Fileini.Attributes;
		buffer=source.Getkeyword();
		if (*buffer=="("){//specified attributes.
			attr_cnt=source.Countattributes();
			attr_orders=new int[attr_cnt];
			
			if (!attr_cnt) return ZERO_ATTRIBUTE;
			for (i=0;i<attr_cnt;i++){
				 attr_orders[i]=Table->Fileini.Checkattribute(*source.Getkeyword());
				 if (attr_orders[i]<0) return ATTRIBUTE_NOTFOUND;//check for attributes.
				 //cout<<"checked attr:"<<attr_orders[i]<<endl;
				 buffer=source.Getkeyword();
				 if (*buffer!=","&&*buffer!=")") return INVALID_KEY;
			}
			buffer=source.Getkeyword();//for "values"
		}
		else {//default, all attributes. 
			attr_cnt=Table->Fileini.Attr_cnt;
			attr_orders=new int[attr_cnt];
			for (i=0;i<attr_cnt;i++){
				attr_orders[i]=i;
				//cout<<"checked attr:"<<attr_orders[i]<<endl;
			}
		}
		
		if (*buffer=="values"&& *source.Getkeyword()=="("){
			for (i=0;i<attr_cnt;i++){//insert the values given in the key words.
				Slot *newslot;
				buffer=source.Getkeyword();
				newslot=Record.Intoslot(attr_orders[i],*buffer);//put value into slot.
				if (!newslot) return INVALID_VALUETYPE;//value type error.
				if (Attributes[attr_orders[i]].Status&UNIQUE){
					Tablebuffer *tablebuffer=new Tablebuffer(Database->DBname,tablename);
					if (tablebuffer->Recordcnt){
						DBvalueset *target=new DBvalueset(tablebuffer,Attributes[attr_orders[i]].Attr_name);
						if (Isin(newslot,target)) return VALUE_NOTUNIQUE;
					}
				}
				if (Attributes[attr_orders[i]].Status==FOREIGN){
					Tablebuffer *tablebuffer=new Tablebuffer(Database->DBname,Attributes[attr_orders[i]].Foreigntable);
					if (!tablebuffer->Recordcnt) return VALUE_NOTFOREIGN;
					DBvalueset *target=new DBvalueset(tablebuffer,tablebuffer->attr_names[Attributes[attr_orders[i]].Foreignkey]);
					if (!Isin(newslot,target)) return VALUE_NOTFOREIGN;
				}
				Record.Insertslot(attr_orders[i],newslot);
				
				//cout<<"inserted attr"<<attr_orders[i]<<" ";Record.Values[attr_orders[i]]->showdata();cout<<endl;
				buffer=source.Getkeyword();
				if (*buffer!=","&&*buffer!=")") return INVALID_KEY;
			}
		}
		else return INVALID_KEY;
		if (attr_cnt<Table->Fileini.Attr_cnt){//fill in NULL values.
			for (i=0;i<Table->Fileini.Attr_cnt;i++){
				if (Table->Fileini.Attributes[i].Status==KEY) return MAINKEY_ISNULL;
				if (!Record.Values[i]){
					Record.Insertnull(i);
				}
			}
		}
		//cout<<Record.Writerecord()<<endl;
		Record.recordid=Table->Insertrecord(const_cast<char*>(Record.Writerecord().c_str()));
		//cout<<Record.recordid.pageid<<"  "<<Record.recordid.slot<<endl;
		return 0;
	}
	int Isin(Slot *aim, DBvalueset *target){
		int i;
		for(i=0;i<target->Slotcnt;i++){
			if (*target->Values[i]== *aim) return 1;
		}
		return 0;
	}
	int Updatedependingtable(string dependedtable){
		int i,j,k;
		Tablebuffer *table=new Tablebuffer(Database->DBname,dependedtable);
		Attribute *attributes=table->Tablefile->Fileini.Attributes;
		for(i=0;i<table->attr_cnt;i++){
			if (attributes[i].Depended){
				DBvalueset *target=new DBvalueset(table,attributes[i].Attr_name);
				for (j=0;j<attributes[i].Depended;j++){
					Tablebuffer *dependingtable=new Tablebuffer(Database->DBname,attributes[i].Dependingtable[j]);
					for (k=0;k<dependingtable->Recordcnt;k++){
						if (!Isin(dependingtable->Records[k]->Values[attributes[i].Dependingkey[j]],target)){
							dependingtable->Tablefile->Deleterecord(table->Records[k]->recordid);
						}
					}
					Updatedependingtable(dependingtable->Tablename);
					delete dependingtable;
				}
				delete target;
			}
		}
	}
	int Update(Tablebuffer *table,OperationSource *source){
		int i;
		int datatype[2];
		string operatr,setnames[2];
		string attr_name;
		int attr_order;
		DBvalueset **sets;
		sets=new DBvalueset*[2];
		attr_name=*source->Getvalue();
		if (*source->Getvalue()!="=") return INVALID_KEY;//must be "="
		attr_order=table->Tablefile->Fileini.Checkattribute(attr_name);
		if (attr_order<0) return ATTRIBUTE_NOTFOUND;//attribute not found.
		
		setnames[0]=*source->Getvalue();
		operatr=*source->Getkeyword();
		setnames[1]=*source->Getvalue();
		if (!setnames[0].length()) return INVALID_KEY;//invalid key, no value given. 
		else if (!setnames[1].length()&&!operatr.length()){
			operatr="+";setnames[1]="0";//only lvalue is given, equals to lvalue+0;
		}
		else if (!setnames[1].length()||!operatr.length())return INVALID_KEY;//with signal, but without rvalue.
		for (i=0;i<2;i++){//set two valuesets to calculate.
			if ((datatype[i]=Checkconst(setnames[i]))>0){
				sets[i]=new DBvalueset(table->Recordcnt,datatype[i],setnames[i]);
			}
			else{
				sets[i]=new DBvalueset(table,setnames[i]);
				datatype[i]=sets[i]->Datatype;
			}
		}
		if (datatype[0]!=datatype[1]||datatype[0]!=table->Tablefile->Fileini.Attributes[attr_order].Type) return INVALID_VALUETYPE; //different datatype.
		if (datatype[0]!=DB_INT&&datatype[0]!=DB_FLOAT) return INVALID_VALUETYPE;//string and char is not supported to be updated.
		if (operatr=="+"){
			for (i=0;i<table->Recordcnt;i++){
				table->Records[i]->Values[attr_order]= *sets[0]->Values[i]+*sets[1]->Values[i];
			}
		}
		else if(operatr=="-"){
			for (i=0;i<table->Recordcnt;i++){
				table->Records[i]->Values[attr_order]= *sets[0]->Values[i]-*sets[1]->Values[i];
			}
		}
		else if(operatr=="*"){
			for (i=0;i<table->Recordcnt;i++){
				table->Records[i]->Values[attr_order]= *sets[0]->Values[i]*(*sets[1]->Values[i]);
			}
		}
		else if(operatr=="/"){
			for (i=0;i<table->Recordcnt;i++){
				table->Records[i]->Values[attr_order]= *sets[0]->Values[i]/(*sets[1]->Values[i]);
			}
		}
		else return INVALID_KEY;
		table->Showtable();
		table->Updateall();
		Updatedependingtable(table->Tablename);
		return 0;
	}
	int Where(Tablebuffer *table,OperationSource *whereclause){
		int i;
		int datatype[2];
		string operatr,setnames[2];
		DBvalueset **sets;
		sets=new DBvalueset*[2];
		setnames[0]=*whereclause->Getvalue();
		operatr=*whereclause->Getkeyword();
		setnames[1]=*whereclause->Getvalue();
		if (!setnames[0].length()||!setnames[1].length()||!operatr.length()) return INVALID_KEY;//invalid key
		for (i=0;i<2;i++){
			if ((datatype[i]=Checkconst(setnames[i]))>0){
				sets[i]=new DBvalueset(table->Recordcnt,datatype[i],setnames[i]);
			}
			else{
				sets[i]=new DBvalueset(table,setnames[i]);
				datatype[i]=sets[i]->Datatype;
			}
		}
		if (datatype[0]!=datatype[1]) return INVALID_VALUETYPE; //different datatypes.
		//divide by symbols
		//put up a set to hold the chosen records
		int chosen[table->Recordcnt];
		int chosencnt=0;
		if (operatr=="=") {
			for (i=0;i<table->Recordcnt;i++) if(*sets[0]->Values[i]==*sets[1]->Values[i]) chosen[chosencnt++]=i;
		}
		else if(operatr==">"){
			for (i=0;i<table->Recordcnt;i++) if(*sets[0]->Values[i]>*sets[1]->Values[i]) chosen[chosencnt++]=i;
		}
		else if(operatr=="<"){
			for (i=0;i<table->Recordcnt;i++) if(*sets[0]->Values[i]<*sets[1]->Values[i]) chosen[chosencnt++]=i;
		}
		else if(operatr==">="){
			for (i=0;i<table->Recordcnt;i++) if(*sets[0]->Values[i]>=*sets[1]->Values[i]) chosen[chosencnt++]=i;
		}
		else if(operatr=="<="){
			for (i=0;i<table->Recordcnt;i++) if(*sets[0]->Values[i]<=*sets[1]->Values[i]) chosen[chosencnt++]=i;
		}
		else if(operatr=="<>"){
			for (i=0;i<table->Recordcnt;i++) if(*sets[0]->Values[i]!=*sets[1]->Values[i]) chosen[chosencnt++]=i;
		}
		else return INVALID_KEY;//invalid key.
		
		table->Filter(chosencnt,chosen);
		
		return 0;
	}
	int Select(){
		int i,j;
		int selectcnt;
		int tablecnt=0;
		int Errbuf;
		string selections[20];
		string *buffer;
		string table1,table2;
		i=0;
		do{
			selections[i++]=*source.Getvalue();
			buffer=source.Getkeyword();
		}while(*buffer==",");
		selectcnt=i;
		if (*buffer!="from") return INVALID_KEY;
		table1=*source.Getvalue();
		if (!Database->Checktable(const_cast<char*>(table1.c_str()))) return TABLE_NOTFOUND;
		tablecnt++;
		if (*source.Getkeyword()==","){
			table2=*source.Getvalue();
			if (!Database->Checktable(const_cast<char*>(table2.c_str()))) return TABLE_NOTFOUND;
			tablecnt++;
		}
		Tablebuffer *outputtable;
		if (tablecnt==1){
			outputtable=new Tablebuffer(Database->DBname,table1);
			for (i=0;i<selectcnt;i++){
				selections[i]=selections[i].substr(selections[i].find(".")+1);//cut the tablename before every selections.
			}
		}
		else if (tablecnt==2){
			Tablebuffer *a=new Tablebuffer(Database->DBname,table1);
			Tablebuffer *b=new Tablebuffer(Database->DBname,table2);
			outputtable=new Tablebuffer(a,b);
		}
		if (*source.Getkeyword()=="where"){
			Errbuf=Where(outputtable,&source);
			if (Errbuf<0) return Errbuf;
		}
		if (selections[0]=="*") {
			outputtable->Showtable();
			return 0;
		}
		else{
			return outputtable->Select(selectcnt,selections);
		}
	}
	int DBCommandResolution(){
		if (Database) cout<<Database->DBname<<">";
		else cout<<">";
		source.Getoperations();
		string *buffer;
		buffer=source.Getkeyword();
		if (*buffer=="show"){
			buffer=source.Getkeyword();
			if (*buffer=="database"){
				Showdatabase();
				return 0;
			}
			else if(*buffer=="table"){
				if (!Database) return DB_NOTUSING;
				Showtable();
				return 0;
			}
			else if(*buffer=="index"){
				if (!Database) return DB_NOTUSING;
				Database->Showindex();
				return 0;
			}
			else return INVALID_KEY;
		}
		if (*buffer=="create"){//create database, create table, create index
			delete buffer;
			buffer=source.Getkeyword();
			if (*buffer=="database"){//create database
				delete buffer;
				buffer=source.Getvalue();//database name
				DBData *newdatabase=new DBData(const_cast<char*>((*buffer).c_str()),1);
				Newdbname(*buffer);
				Writesysini();
				delete newdatabase;
			}
			else if (*buffer=="table"){//create table
				int attr_cnt;
				Attribute *attributes;
				delete buffer;
				if (!Database) return DB_NOTUSING;//no database using.
				buffer=source.Getvalue();//table name
				if(*source.Getkeyword()=="("){
					if (!(attributes=Getattributes(attr_cnt,*buffer))){
						return INVALID_ATTR;
					}
					Database->DBCreateTable(const_cast<char*>(buffer->c_str()),attr_cnt,attributes);
				}
				else return -1;
			}
			else if (*buffer=="index"){//create index 
				int i;
				if (!Database) return DB_NOTUSING;//no database using
				return Createindex();
			}
			else return -1;
		}
		else if(*buffer=="use"&& *source.Getkeyword()=="database"){//use database	
			delete buffer;
			buffer=source.Getvalue();
			delete Database;
			Database=new DBData(const_cast<char*>((*buffer).c_str()),0);
		}
		else if(*buffer=="drop"){//drop database, drop table, drop index
			delete buffer;
			buffer=source.Getkeyword();
			if (*buffer=="table"){//drop table
				int i;
				delete buffer;
				buffer=source.Getvalue();
				if (!Database) return DB_NOTUSING;//no database using
				Database->DBDeletetable(const_cast<char*>((*buffer).c_str()));
				for (i=0;i<Database->Indexcnt;i++){
					if (Database->Indexes[i].substr(0,Database->Indexes[i].find("."))==*buffer){
						Database->Dropindexname(Database->Indexes[i]);
					}
				}
				return 0;
			}
			else if (*buffer=="database"){//drop database
				delete buffer;
				buffer=source.Getvalue();
				return Dropdatabase(buffer);
			}
			else if (*buffer=="index"){//drop index
				if (!Database) return DB_NOTUSING;//no database using
				return Dropindex();
			}
		}
		else if (*buffer=="insert"&& *source.Getkeyword()=="into"){//insert
			return Insert();
		}
		else if (*buffer=="update"){
			string tablename=*source.Getkeyword();
			if (!Database->Checktable(const_cast<char*>(tablename.c_str()))) return TABLE_NOTFOUND;//table not found.
			if (*source.Getkeyword()!="set") return INVALID_KEY;//invalid key
			Tablebuffer *table=new Tablebuffer(Database->DBname,tablename);
			return Update(table,&source);
		}
		else if (*buffer=="delete"&& *source.Getkeyword()=="from"){
			int Errbuf;
			string tablename=*source.Getkeyword();
			if (!Database->Checktable(const_cast<char*>(tablename.c_str()))) return TABLE_NOTFOUND;//table not found
			Tablebuffer *table=new Tablebuffer(Database->DBname,tablename);
			if (*source.Getkeyword()=="where"){
				Errbuf=Where(table,&source);
				if (Errbuf<0) return Errbuf;
			}
			table->Deleteall();
			Updatedependingtable(tablename);
			return 0;
		}
		else if (*buffer=="select"){
			return Select();
		}
		else if (*buffer=="exit"){
			exit(0);
		}
		else return INVALID_KEY;//invalid key.
	}
	~Sysbuffer(){delete Database;}
};
#endif