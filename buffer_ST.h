#ifndef _BUFFER_ST_H_
#define _BUFFER_ST_H_

int Checkdigit(char x){
	return (x>='0'&&x<='9');
}
int Checkint(string source){
	int i;
	if (source[0]!='-'&&!Checkdigit(source[0])) return 0;
	for (i=1;i<source.length();i++){
		if (!Checkdigit(source[i])) return 0;
	}
	if (abs(atoi(source.c_str()))>MAX_INTABS) return 0;
	return DB_INT;
}
int Checkfloat(string source){
	int i;
	double temp;
	if (source[0]!='-'&&!Checkdigit(source[0])) return -1;
	if (count(source.begin(),source.end(),'.')>1) return -2;
	for (i=1;i<source.length()-1;i++){
		if (!Checkdigit(source[i])&&source[i]!='.') return -3;
	}
	if (!Checkdigit(source[source.length()-1])) return -4;
	sscanf(source.c_str(),"%lf",&temp);
	if (abs(temp)>MAX_FLOATABS) return -5;
	return DB_FLOAT;
}
int Checkchar(string source){
	int i;
	if (source.length()!=3) return 0;
	if (source[0]!='\''||source[2]!='\'') return 0;
	return DB_CHAR;
}
int Checkstring(string source){
	if (source.length()>DB_STRING+2||source.length()<2) return 0;//2 for '\''
	if (source[0]!='\"'||source[source.length()-1]!='\"') return 0;//begin and end with '\''
	return DB_STRING;
}
int Checkconst(string x){
	int datatype;
	if ((datatype=Checkchar(x))>0) return datatype;
	else if ((datatype=Checkint(x))>0) return datatype;
	else if ((datatype=Checkfloat(x))>0) return datatype;
	else if ((datatype=Checkstring(x))>0) return datatype;
	return 0;
}

class OperationSource{
public:
	string keysource;
	void Getoperations(){
		getline(cin,keysource);
		Formatsymbols();
	}
	void Testoperation(char* source){
		keysource=source;
		Formatsymbols();
	}
	int Getlength(){
		return keysource.length();
	}
	int Deletesparespaces(){
		int i=1;
		if (keysource[0]=='\"'){
			for (;keysource[i]!='\"';i++);
		}
		for (;i<keysource.length();i++){
			if (keysource[i]=='\"'){
				i++;
				for (;keysource[i]!='\"';i++);
			}
			else if ((keysource[i]==' ')&&keysource[i-1]==' '){
				keysource=keysource.substr(0,i)+keysource.substr(i+1);
				i--;
			}
		}
		if (keysource[0]==' ') keysource=keysource.substr(1);
		//cout<<"after deleting:"<<keysource<<endl;
	}
	int Formatsymbols(){
		int i;
		int trigger=1;
		for (i=0;i<keysource.length();i++){
			if ((keysource[i]=='>'&&keysource[i+1]=='=')||
				(keysource[i]=='<'&&(keysource[i+1]=='='||keysource[i+1]=='>'))){
				if (keysource[i+2]!=' '){
					keysource=keysource.substr(0,i+2)+" "+keysource.substr(i+2);
				}
				if (keysource[i-1]!=' '){
					keysource=keysource.substr(0,i)+" "+keysource.substr(i);
					i++;
				}
				i++;
			}
			else if (keysource[i]==','||keysource[i]==')'||keysource[i]=='('||keysource[i]=='\"'||
				keysource[i]=='='||keysource[i]=='+'||keysource[i]=='-'||
				keysource[i]=='*'||keysource[i]=='/'||keysource[i]=='<'||keysource[i]=='>'){//add ' ' at both sides of these symbols.
				if (keysource[i+1]!=' '){
					keysource=keysource.substr(0,i+1)+" "+keysource.substr(i+1);
				}
				if (keysource[i-1]!=' '){
					keysource=keysource.substr(0,i)+" "+keysource.substr(i);
					i++;
				}
				if (keysource[i]=='\"'){
					i++;
					for (;keysource[i]!='\"';i++);
					if (keysource[i+1]!=' '){
						keysource=keysource.substr(0,i+1)+" "+keysource.substr(i+1);
					}
					if (keysource[i-1]!=' '){
						keysource=keysource.substr(0,i)+" "+keysource.substr(i);
						i++;
					}
					i++; 
				}
			}
			else if (keysource[i]=='.'){//delete ' ' at both sides of these symbols.
				if (keysource[i+1]==' '){
					keysource=keysource.substr(0,i+1)+keysource.substr(i+2);
				}
				if (keysource[i-1]==' '){
					keysource=keysource.substr(0,i-1)+keysource.substr(i);
					i--;
				}
			}
		}
		//cout<<"formated:"<<keysource<<endl;
	}
	int Countattributes(){
		int count_attr=0;
		int i;
		for (i=0;i<keysource.length()&&keysource[i]!=')';i++){
			if (keysource[i]==',') count_attr++; 
		}
		return ++count_attr;
	}
	string* Getvalue(){
		int i;
		string *out=new string;
		Deletesparespaces();
		if (keysource[0]=='\"'){
			int n;
			for(n=1;n<keysource.length()&&keysource[n]!='\"';n++);
			*out="\""+keysource.substr(2,n-3)+"\"";
			keysource=keysource.substr(n+1);
			cout<<*out<<endl;
			//cout<<"keysource:"<<keysource<<endl;
			return out;
		}
		for (i=0;i<keysource.length();i++){
			if (keysource[i]==' '){
				*out=keysource.substr(0,i);
				keysource=keysource.substr(i+1);
				cout<<*out<<endl;
				return out;
			}
		}
		*out=keysource;
		keysource="";
		return out;
	}
	string* Getkeyword(){
		int j;
		string *out=Getvalue();
		for (j=0;j<out->length();j++){
			//cout<<j<<endl; 
			(*out)[j]|=0x20;//transform into small letters 
		}
		return out;
	}
};

class Slot{
public:
	int type;
	virtual void showdata(){};
	virtual string Datatostring(){};
	virtual int Isnull(){}; 
	virtual void Readdata(string source){};
	virtual bool operator == (const Slot &x){}; 
	virtual bool operator > (const Slot &x){};
	virtual bool operator < (const Slot &x){};
	virtual bool operator >= (const Slot &x){};
	virtual bool operator <= (const Slot &x){};
	virtual bool operator != (const Slot &x){};
	virtual Slot* operator + (const Slot &x){};
	virtual Slot* operator - (const Slot &x){};
	virtual Slot* operator * (const Slot &x){};
	virtual Slot* operator / (const Slot &x){};
};
class Intslot:public Slot{
public:
	int data;
	Intslot (int x){
		data=x;
		type=DB_INT;
	}
	Intslot(){
		data=NULL_INT;
		type=DB_INT;
	}
	Intslot(const Intslot &source){
		data=source.data;
		type=DB_INT;
	}
	Intslot(string source){
		Readdata(source);
	}
	void Readdata(string source){
		data=atoi(source.c_str());
		type=DB_INT;
	}
	void showdata(){cout<<data;}
	string Datatostring(){
		char buffer[DB_INT+1];
		sprintf(buffer,"%09d",data);
		return buffer;
	}
	int Isnull(){return (data==NULL_INT);}
	bool operator == (const Slot &x){Intslot *buffer=static_cast<Intslot*>(const_cast<Slot*>(&x)); return data==buffer->data;} 
	bool operator >= (const Slot &x){Intslot *buffer=static_cast<Intslot*>(const_cast<Slot*>(&x)); return data>=buffer->data;} 
	bool operator <= (const Slot &x){Intslot *buffer=static_cast<Intslot*>(const_cast<Slot*>(&x)); return data<=buffer->data;} 
	bool operator != (const Slot &x){Intslot *buffer=static_cast<Intslot*>(const_cast<Slot*>(&x)); return data!=buffer->data;}
	bool operator > (const Slot &x){Intslot *buffer=static_cast<Intslot*>(const_cast<Slot*>(&x)); return data>buffer->data;}
	bool operator < (const Slot &x){Intslot *buffer=static_cast<Intslot*>(const_cast<Slot*>(&x)); return data<buffer->data;}
	Slot* operator + (const Slot &x){
		Intslot *buffer=static_cast<Intslot*>(const_cast<Slot*>(&x));
		Intslot *out=new Intslot(data+buffer->data);
		return out;
	};
	Slot* operator - (const Slot &x){
		Intslot *buffer=static_cast<Intslot*>(const_cast<Slot*>(&x));
		Intslot *out=new Intslot(data-buffer->data);
		return out;
	}
	Slot* operator * (const Slot &x){
		Intslot *buffer=static_cast<Intslot*>(const_cast<Slot*>(&x));
		Intslot *out=new Intslot(data*buffer->data);
		return out;
	}
	Slot* operator / (const Slot &x){
		Intslot *buffer=static_cast<Intslot*>(const_cast<Slot*>(&x));
		Intslot *out=new Intslot(data/buffer->data);
		return out;
	}
};
class Charslot:public Slot{
public:
	char data;
	Charslot(){
		data='\3';
		type=DB_CHAR;
	}
	Charslot(const Charslot &source){
		data=source.data;
		type=DB_CHAR;
	}
	Charslot(string source){
		Readdata(source);
	} 
	void Readdata(string source){
		data=source[1];
		type=DB_CHAR;
	}
	void showdata(){cout<<data;}
	string Datatostring(){
		return string(1,data);
	}
	int Isnull(){return (data==NULL_CHAR);}
	bool operator == (const Slot &x){Charslot *buffer=static_cast<Charslot*>(const_cast<Slot*>(&x)); return data==buffer->data;} 
	bool operator >= (const Slot &x){Charslot *buffer=static_cast<Charslot*>(const_cast<Slot*>(&x)); return data>=buffer->data;} 
	bool operator <= (const Slot &x){Charslot *buffer=static_cast<Charslot*>(const_cast<Slot*>(&x)); return data<=buffer->data;} 
	bool operator != (const Slot &x){Charslot *buffer=static_cast<Charslot*>(const_cast<Slot*>(&x)); return data!=buffer->data;}
	bool operator > (const Slot &x){Charslot *buffer=static_cast<Charslot*>(const_cast<Slot*>(&x)); return data>buffer->data;}
	bool operator < (const Slot &x){Charslot *buffer=static_cast<Charslot*>(const_cast<Slot*>(&x)); return data<buffer->data;}
};
class Stringslot:public Slot{
public:
	string data;
	Stringslot (){
		type=DB_STRING;
	}
	Stringslot(const Stringslot &source){
		data=source.data;
		type=DB_STRING;
	}
	Stringslot(string source){
		Readdata(source);
	}
	void Readdata(string source){
		data=source.substr(1,source.length()-2);//remove '\'' at the head and end of the string.
		type=DB_STRING;
	}
	void showdata(){cout<<data;}
	string Datatostring(){
		string filler=string(DB_STRING-data.length(),'\3'); 
		return data+filler;
	}
	int Isnull(){return (!data.length());}
	bool operator == (const Slot &x){Stringslot *buffer=static_cast<Stringslot*>(const_cast<Slot*>(&x)); return data==buffer->data;} 
	bool operator >= (const Slot &x){Stringslot *buffer=static_cast<Stringslot*>(const_cast<Slot*>(&x)); return data>=buffer->data;} 
	bool operator <= (const Slot &x){Stringslot *buffer=static_cast<Stringslot*>(const_cast<Slot*>(&x)); return data<=buffer->data;} 
	bool operator != (const Slot &x){Stringslot *buffer=static_cast<Stringslot*>(const_cast<Slot*>(&x)); return data!=buffer->data;}
	bool operator > (const Slot &x){Stringslot *buffer=static_cast<Stringslot*>(const_cast<Slot*>(&x)); return data>buffer->data;}
	bool operator < (const Slot &x){Stringslot *buffer=static_cast<Stringslot*>(const_cast<Slot*>(&x)); return data<buffer->data;}
};
class Floatslot:public Slot{
public:
	double data;
	Floatslot(double x){
		data=x;
		type=DB_FLOAT;
	}
	Floatslot (){
		data=NULL_FLOAT;
		type=DB_FLOAT;
	}
	Floatslot(const Floatslot &source){
		data=source.data;
		type=DB_FLOAT;
	}
	Floatslot (string source){
		Readdata(source);
	}
	void Readdata(string source){
		double valuebuff;
		sscanf(source.c_str(),"%lf",&valuebuff);
		data=valuebuff;
		type=DB_FLOAT;
	}
	void showdata(){cout<<data;}
	string Datatostring(){
		char buffer[DB_FLOAT+1];
		sprintf(buffer,"%017.7f",data);
		return buffer;
	}
	int Isnull(){return (data==NULL_FLOAT);}
	bool operator == (const Slot &x){Floatslot *buffer=static_cast<Floatslot*>(const_cast<Slot*>(&x)); return data==buffer->data;} 
	bool operator >= (const Slot &x){Floatslot *buffer=static_cast<Floatslot*>(const_cast<Slot*>(&x)); return data>=buffer->data;} 
	bool operator <= (const Slot &x){Floatslot *buffer=static_cast<Floatslot*>(const_cast<Slot*>(&x)); return data<=buffer->data;} 
	bool operator != (const Slot &x){Floatslot *buffer=static_cast<Floatslot*>(const_cast<Slot*>(&x)); return data!=buffer->data;}
	bool operator > (const Slot &x){Floatslot *buffer=static_cast<Floatslot*>(const_cast<Slot*>(&x)); return data>buffer->data;}
	bool operator < (const Slot &x){Floatslot *buffer=static_cast<Floatslot*>(const_cast<Slot*>(&x)); return data<buffer->data;}
	Slot* operator + (const Slot &x){
		Floatslot *buffer=static_cast<Floatslot*>(const_cast<Slot*>(&x));
		Floatslot *out=new Floatslot(data+buffer->data);
		return out;
	};
	Slot* operator - (const Slot &x){
		Floatslot *buffer=static_cast<Floatslot*>(const_cast<Slot*>(&x));
		Floatslot *out=new Floatslot(data-buffer->data);
		return out;
	};
	Slot* operator * (const Slot &x){
		Floatslot *buffer=static_cast<Floatslot*>(const_cast<Slot*>(&x));
		Floatslot *out=new Floatslot(data*buffer->data);
		return out;
	};
	Slot* operator / (const Slot &x){
		Floatslot *buffer=static_cast<Floatslot*>(const_cast<Slot*>(&x));
		Floatslot *out=new Floatslot(data/buffer->data);
		return out;
	};
};

class Recordbuffer{
public:
	int attr_cnt;
	RecordID recordid;
	Slot **Values;
	int *attr_types;
	Recordbuffer (Recordbuffer *a, Recordbuffer	*b){
		int i;
		attr_cnt=a->attr_cnt+b->attr_cnt;
		Values=new Slot*[attr_cnt];
		attr_types=new int[attr_cnt];
		for (i=0;i<a->attr_cnt;i++){
			attr_types[i]=a->attr_types[i];
			Values[i]=a->Values[i];
		}
		for (i=0;i<b->attr_cnt;i++){
			attr_types[i+a->attr_cnt]=b->attr_types[i];
			Values[i+a->attr_cnt]=b->Values[i];
		}
	} 
	Recordbuffer (DBFileINI *tableini){
		int i;
		attr_cnt=tableini->Attr_cnt;
		Values=new Slot*[attr_cnt];
		attr_types=new int[attr_cnt];
		for (i=0;i<attr_cnt;i++){
			attr_types[i]=tableini->Attributes[i].Type;
			Values[i]=NULL;
		}
	}
	Recordbuffer (DBFileINI	*tableini, string &record, RecordID id){
		Readrecord(tableini, record, id);
	}
	Recordbuffer(){}
	int Readrecord(DBFileINI *tableini, string &record, RecordID id){
		int i;
		attr_cnt=tableini->Attr_cnt;
		Values=new Slot*[attr_cnt];
		attr_types=new int[attr_cnt];
		recordid=id;
		for (i=0;i<attr_cnt;i++){
			attr_types[i]=tableini->Attributes[i].Type;
			Values[i]=Readvalue(attr_types[i],record);
		}
	}

	Slot *Intoslot (int attrorder, string source){
		int datatype=attr_types[attrorder];
		//cout<<"datatype:"<<datatype<<endl;
		Slot *newslot;
		switch (datatype){
			case DB_CHAR:{
				if (!Checkchar(source)) return NULL;
				newslot=new Charslot(source);
				break;
			}
			case DB_INT:{
				if (!Checkint(source)) return NULL;
				newslot=new Intslot(source);
				break;
			}
			case DB_FLOAT:{
				if (!Checkfloat(source)) return NULL;
				newslot=new Floatslot(source);
				break;
			}
			case DB_STRING:{
				if (!Checkstring(source)) return NULL;
				newslot=new Stringslot(source);
				break;
			}
		}
		return newslot;
	}
	int Insertslot(int attrorder,Slot *source){
		Values[attrorder]=source;
	}
	int Insertnull(int recordorder){
		int datatype=attr_types[recordorder];
		//cout<<"datatype:"<<datatype<<endl;
		switch (datatype){
			case DB_CHAR:{ 
				Charslot *newslot=new Charslot;
				Values[recordorder]=newslot;
				break;
			}
			case DB_INT:{
				Intslot *newslot=new Intslot;
				Values[recordorder]=newslot;
				break;
			}
			case DB_FLOAT:{
				Floatslot *newslot=new Floatslot;
				Values[recordorder]=newslot;
				break;
			}
			case DB_STRING:{
				Stringslot *newslot=new Stringslot;
				Values[recordorder]=newslot;
				break;
			}
		}
	}
	Slot *Readvalue(int datatype,string &record){
		switch (datatype){
			case DB_CHAR:{
				char value=record[0];
				record=record.substr(DB_CHAR);
				Charslot *slot=new Charslot;
				slot->type=DB_CHAR;
				slot->data=value;
				return slot;
				break;
			}
			case DB_INT:{
				int value=atoi(record.substr(0,DB_INT).c_str());
				record=record.substr(DB_INT);
				Intslot *slot=new Intslot;
				slot->type=DB_INT;
				slot->data=value;
				return slot;
				break;
			}
			case DB_FLOAT:{
				double value;
				sscanf(record.substr(0,DB_FLOAT).c_str(),"%lf",&value);
				record=record.substr(DB_FLOAT);
				Floatslot *slot=new Floatslot;
				slot->type=DB_FLOAT;
				slot->data=value;
				return slot;
				break;
			}
			case DB_STRING:{
				int eos;
				for (eos=0;record[eos]!='\3'&&eos<DB_STRING;eos++);
				string value=record.substr(0,eos);
				record=record.substr(DB_STRING);
				Stringslot *slot=new Stringslot;
				slot->type=DB_STRING;
				slot->data=value;
				return slot;
				break;
			}
		}
	}
	
	string Writerecord(){
		int i;
		string buffer;
		for (i=0;i<attr_cnt;i++){
			buffer+=Values[i]->Datatostring();
		}
		return buffer;
	}
	void Showrecord(){
		int i;
		for (i=0;i<attr_cnt;i++){
			Values[i]->showdata();
			cout<<'\t';
		}
		cout<<endl;
	}
	void Showrecord(int selectcnt,int *outputorder){
		int i;
		for (i=0;i<selectcnt;i++){
			Values[outputorder[i]]->showdata();
			cout<<'\t';
		}
		cout<<endl;
	}//to be checked.
};

class Tablebuffer{
public:
	string DBname,Tablename;
	int Recordcnt;
	Recordbuffer **Records;
	DBFile *Tablefile;
	string *attr_names;
	int attr_cnt;
	Tablebuffer(string dbname,string tablename){
		int i;
		string *Record_strings;
		RecordID *ids;
		RecordID **Record_ids=&ids;
		Tablefile=new DBFile(const_cast<char*>(dbname.c_str()),const_cast<char*>(tablename.c_str()));
		Tablename=tablename;
		DBname=dbname;
		attr_cnt=Tablefile->Fileini.Attr_cnt;
		attr_names=new string[attr_cnt];
		for (i=0;i<attr_cnt;i++){
			attr_names[i]=Tablefile->Fileini.Attributes[i].Attr_name;
		}
		Recordcnt=Tablefile->Fileini.Recordcnt;
		Record_strings=Tablefile->Getallrecords(Record_ids);
		/*
		for (i=0;i<Recordcnt;i++){
			cout<<"pageid:"<<(*Record_ids)[i].pageid<<endl;
			cout<<"slot:"<<(*Record_ids)[i].slot<<endl;
		}
		*/
		Records=new Recordbuffer*[Recordcnt];
		for (i=0;i<Recordcnt;i++){
			Records[i]=new Recordbuffer(&Tablefile->Fileini,Record_strings[i],(*Record_ids)[i]);
			//cout<<Records[i]->recordid.pageid<<endl;
			//cout<<Records[i]->recordid.slot<<endl;
		}
	}
	Tablebuffer(Tablebuffer *a, Tablebuffer *b){//Cartesian product.
		int i,j;
		Tablename=a->Tablename+"*"+b->Tablename;
		Recordcnt=a->Recordcnt*b->Recordcnt;
		Records=new Recordbuffer*[Recordcnt];
		attr_cnt=a->attr_cnt+b->attr_cnt; 
		attr_names=new string[attr_cnt];
		for (i=0;i<a->attr_cnt;i++){
			attr_names[i]=a->Tablename+"."+a->attr_names[i];
		}
		for (i=0;i<b->attr_cnt;i++){
			attr_names[a->attr_cnt+i]=b->Tablename+"."+b->attr_names[i];
		}
		for (i=0;i<a->Recordcnt;i++){
			for (j=0;j<b->Recordcnt;j++){
				Records[i*b->Recordcnt+j]=new Recordbuffer(a->Records[i],b->Records[j]);
				Records[i*b->Recordcnt+j]->Showrecord();
			}
		}
	}
	int Checkattr(string x){
		int i;
		for (i=0;i<attr_cnt;i++){
			if (x==attr_names[i]) return i;
		}
		return -1;
	}
	int Deleteall(){
		int i;
		for (i=0;i<Recordcnt;i++){
			Tablefile->Deleterecord(Records[i]->recordid);
		}
	}
	int Updateall(){
		int i;
		for (i=0;i<Recordcnt;i++){
			Tablefile->Overwriterecord(Records[i]->recordid,const_cast<char*>(Records[i]->Writerecord().c_str())); 
		} 
	}
	int Filter(int newrecordcnt, int* chosen){
		int i,counter=0;
		Recordbuffer **Newrecords;
		Newrecords=new Recordbuffer*[newrecordcnt];
		for (i=0;i<Recordcnt;i++){
			if (i==chosen[counter]){
				Newrecords[counter]=Records[i];
				counter++;
			}
			else delete Records[i];
		}
		for (i=0;i<Recordcnt;i++) delete (Records+i);
		Records=Newrecords;
		Recordcnt=newrecordcnt;
	}
	int Select(int selectcnt,string *selections){
		int i,j;
		int *outputorder=new int[selectcnt];
		for (i=0;i<selectcnt;i++){
			for (j=0;j<attr_cnt;j++){
				if (attr_names[j]==selections[i]){
					outputorder[i]=j;
					break;
				}
			}
			if (j==attr_cnt) return ATTRIBUTE_NOTFOUND;
		}
		for (i=0;i<selectcnt;i++){
			cout<<attr_names[outputorder[i]]<<'\t';
		}
		cout<<endl;
		for (i=0;i<Recordcnt;i++){
			Records[i]->Showrecord(selectcnt,outputorder);
		}
		return 0;
	}//to be checked.
	void Showtable(){
		int i;
		for (i=0;i<attr_cnt;i++){
			cout<<attr_names[i]<<'\t';
		}
		cout<<endl;
		for (i=0;i<Recordcnt;i++){
			Records[i]->Showrecord();
		}
	}
};
class DBvalueset{
public:
	int Datatype;
	Slot **Values;
	int Slotcnt;
	DBvalueset(Tablebuffer *Table, string attr_name){
		int i;
		int recordorder;
		Slotcnt=Table->Recordcnt;
		recordorder=Table->Checkattr(attr_name);
		Values=new Slot*[Slotcnt];
		Datatype=Table->Records[0]->Values[recordorder]->type;
		switch (Datatype){
			case DB_INT:{
				Intslot *source;
				for (i=0;i<Slotcnt;i++){
					source=dynamic_cast<Intslot*>(Table->Records[i]->Values[recordorder]);
					Values[i]=new Intslot(*source);
				}
				break;
			}
			case DB_CHAR:{
				Charslot *source;
				for(i=0;i<Slotcnt;i++){
					source=dynamic_cast<Charslot*>(Table->Records[i]->Values[recordorder]);
					Values[i]=new Charslot(*source);
				}
				break;
			}
			case DB_FLOAT:{
				Floatslot *source;
				for(i=0;i<Slotcnt;i++){
					source=dynamic_cast<Floatslot*>(Table->Records[i]->Values[recordorder]);
					Values[i]=new Floatslot(*source);
				}
				break;
			}
			case DB_STRING:{
				Stringslot *source;
				for(i=0;i<Slotcnt;i++){
					source=dynamic_cast<Stringslot*>(Table->Records[i]->Values[recordorder]);
					Values[i]=new Stringslot(*source);
				}
				break;
			}
		}
	}
	DBvalueset(int slotcnt,int datatype,string source):Slotcnt(slotcnt),Datatype(datatype){
		int i;
		Values=new Slot*[Slotcnt];
		switch (datatype){
			case DB_INT:{
				for (i=0;i<slotcnt;i++){
					Values[i]=new Intslot(source);
				}
				break;
			}
			case DB_CHAR:{
				for (i=0;i<slotcnt;i++){
					Values[i]=new Charslot(source);
				}
				break;
			}
			case DB_STRING:{
				for(i=0;i<slotcnt;i++){
					Values[i]=new Stringslot(source);	
				}
				break;
			}
			case DB_FLOAT:{
				for (i=0;i<slotcnt;i++){
					Values[i]=new Floatslot(source);
				}
				break;
			}
		}
	}
};

#endif