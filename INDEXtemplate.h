#ifndef _INDEXTEMPLATE_
#define _INDEXTEMPLATE_

#define BPNODE_CAPACITY 3
#define BPNODE_MINKEYCNT 2 
using namespace std;

template <class Datatype> class  BPnode;
template <class Datatype> class  BPkey{
public:
	//int I;
	Datatype Value;
	BPkey<Datatype> *Next;
	BPkey<Datatype>(Datatype value):Value(value){
		Next=NULL;
	}
	//BPkey<Datatype>():Value(NULL),Next(NULL){}
	BPkey<Datatype>():Next(NULL){}
	virtual BPnode<Datatype>* Getleftpointer(){};
	virtual BPnode<Datatype>* Getrightpointer(){};
	virtual int Savekey(FILE *pfile){};
};

template <class Datatype> class  BPleafkey:public BPkey<Datatype>{
public:
	RecordID *Record;
	//BPleafkey<Datatype>():BPkey<Datatype>(NULL),Record(NULL){}
	BPleafkey<Datatype>(){Record=NULL;}
	BPleafkey<Datatype>(Datatype value,RecordID *record):BPkey<Datatype>(value){Record=record;}
	//BPleafkey<Datatype>(Datatype value):BPkey<Datatype>(value),Record(NULL){}
	BPleafkey<Datatype>(FILE *pfile){//read from pfile
		//cout<<"read leaf key"<<endl;
		fread(&this->Value,sizeof(Datatype),1,pfile);
		//cout<<this->Value<<endl;
		Record=new RecordID;
		fread(Record,sizeof(RecordID),1,pfile);
		//cout<<Record->pageid<<endl;
		this->Next=NULL;
	}
	int Saverecord(FILE *pfile){
		fwrite(Record,sizeof(RecordID),1,pfile);
	}
	int Savekey(FILE *pfile){
		fwrite(&this->Value,sizeof(Datatype),1,pfile);
		Saverecord(pfile);
	}
};

template <class Datatype> class  BPinteriorkey:public BPkey<Datatype>{
public:
	BPnode<Datatype> *Leftpointer, *Rightpointer;
	BPinteriorkey<Datatype>(BPnode<Datatype> *leftpointer,BPnode<Datatype> *rightpointer,Datatype value):BPkey<Datatype>(value),Leftpointer(leftpointer),Rightpointer(rightpointer){}
	BPinteriorkey<Datatype>(FILE *pfile){
		//cout<<"read interiorkey"<<endl;
		fread(&this->Value,sizeof(Datatype),1,pfile);
		//cout<<this->Value;
		this->Next=NULL;
	}
	BPnode<Datatype> *Getleftpointer(){return Leftpointer;}
	BPnode<Datatype> *Getrightpointer(){return Rightpointer;}
	int Savekey(FILE *pfile){
		fwrite(&this->Value,sizeof(Datatype),1,pfile);
	}
};
template <class Datatype> class  BPinteriornode;
template <class Datatype> class  BPnode{
public:
	int keycnt;
	BPinteriornode<Datatype> *parent;
	BPkey<Datatype> *keys; 
	BPkey<Datatype> *maxkey,*minkey;
	BPnode<Datatype>(){
		parent=NULL;
		maxkey=minkey=keys=NULL;
		keycnt=0;
	}
	virtual int Insertkey(BPkey<Datatype> *key_x){};
	virtual BPnode<Datatype> *Findnextnode(Datatype x){};
	virtual BPnode<Datatype> *Searchnextnode(Datatype x){};
	virtual BPnode<Datatype>* Getnext(){};
	void showkeys(){
		BPkey<Datatype> *p;
		for (p=keys;p;p=p->Next){
			//printf("%d",p);
			cout<<p->Value<<endl;
		}
		cout<<"min:"<<minkey->Value<<endl;
		cout<<"max:"<<maxkey->Value<<endl;
	}
	virtual int Savenode(FILE *pfile){};
	int Isoverflow(){return keycnt>BPNODE_CAPACITY;}

};
template <class Datatype> class  BPinteriornode:public BPnode<Datatype>{
public:
	BPinteriornode<Datatype>(BPnode<Datatype> *leftnode,BPnode<Datatype> *rightnode,Datatype newkey){
		this->minkey=this->maxkey=this->keys=new BPinteriorkey<Datatype>(leftnode,rightnode,newkey);
		this->parent=NULL;
		this->keycnt++;
	}
	BPinteriornode<Datatype>(){
		this->minkey=this->maxkey=this->keys=NULL;
		this->parent=NULL;
		this->keycnt=0;
	}
	BPinteriornode<Datatype> (FILE *pfile){
		int i;
		BPkey<Datatype> *p_keys;
		fread(&this->keycnt,sizeof(int),1,pfile);
		this->minkey=this->keys=new BPinteriorkey<Datatype>(pfile);
		
		p_keys=this->keys;
		for(i=0;i<this->keycnt-1;i++){
			p_keys->Next=new BPinteriorkey<Datatype>(pfile);
			p_keys=p_keys->Next;
		}
		this->maxkey=p_keys;
	} 
	int Savenode(FILE *pfile){
		int i;
		BPinteriorkey<Datatype> *p_keys=dynamic_cast<BPinteriorkey<Datatype>*>(this->keys); 
		fwrite(&this->keycnt,sizeof(int),1,pfile);
		for (i=0;i<this->keycnt;i++){
			p_keys->Savekey(pfile);
			p_keys=dynamic_cast<BPinteriorkey<Datatype>*>(p_keys->Next);
		}
	}
	
	int Insertkey(Datatype newkeyvalue,BPnode<Datatype> *newnode){
		BPkey<Datatype> *p;
		BPinteriorkey<Datatype> *newkey,*nextkey;
		p=dynamic_cast<BPinteriorkey<Datatype>*>(this->keys);
		for (;p->Next&&p->Next->Value<newkeyvalue;p=p->Next);//find the plot to insert
		newkey=new BPinteriorkey<Datatype>(p->Getrightpointer(),newnode,newkeyvalue);//set up new key
		if (p->Next){
			nextkey=dynamic_cast<BPinteriorkey<Datatype>*>(p->Next);
			nextkey->Leftpointer=newkey->Rightpointer;//set up next key
		}
		//insert.
		newkey->Next=p->Next;
		p->Next=newkey;
		this->keycnt++;
		//update maxkey.
		if (!newkey->Next){
			this->maxkey=p->Next;
		}
		//this->showkeys();
	}
	int Insertkey(Datatype newkeyvalue,BPnode<Datatype> *leftnode,BPnode<Datatype> *rightnode){//used only for inserting the smallest key
		BPinteriorkey<Datatype> *p=dynamic_cast<BPinteriorkey<Datatype>*>(this->keys);
		BPinteriorkey<Datatype> *newkey= new BPinteriorkey<Datatype>(leftnode,rightnode,newkeyvalue);//set up new key
		p->Leftpointer=rightnode;
		newkey->Next=this->keys;
		this->minkey=newkey;
		this->keys=dynamic_cast<BPkey<Datatype>*>(newkey);
		this->keycnt++;
		//this->showkeys();
	}
	BPnode<Datatype> *Findnextnode(Datatype x){//for inserting a new key into the tree.
		BPkey<Datatype> *p;
		if(this->minkey->Value>x){
			return this->minkey->Getleftpointer();
		}
		else if(this->maxkey->Value<x){
			return this->maxkey->Getrightpointer();
		}
		else if (this->minkey->Value==x||this->maxkey->Value==x) return NULL;
		else{
			for(p=this->keys;p->Next;p=p->Next){
				if (x==p->Value||x==p->Next->Value) return NULL;
				else if (x>p->Value&&x<p->Next->Value) return p->Getrightpointer();
			}
		} 
	}
	BPnode<Datatype> *Searchnextnode(Datatype x){//for searching a key.
		BPkey<Datatype> *p;
		if (this->minkey->Value>x){
			return this->minkey->Getleftpointer();
		}
		else if(this->maxkey->Value<=x){
			return this->maxkey->Getrightpointer();
		}
		else{
			for(p=this->keys;p->Next;p=p->Next){
				if (x>=p->Value&&x<p->Next->Value) {return p->Getrightpointer();}
			}
		}
	}
};

template <class Datatype> class  BPleafnode:public BPnode<Datatype>{
public:
	BPleafnode<Datatype> *next;
	BPleafnode<Datatype>(){
		next=NULL;
	}
	BPleafnode<Datatype> (FILE *pfile){
		int i;
		BPkey<Datatype> *p_keys;
		next=NULL;
		fread(&this->keycnt,sizeof(int),1,pfile);
		//cout<<this->keycnt;
		this->minkey=this->keys=new BPleafkey<Datatype>(pfile);
		p_keys=this->keys;
		
		for(i=0;i<this->keycnt-1;i++){
			p_keys->Next=new BPleafkey<Datatype>(pfile);
			p_keys=p_keys->Next;
		}
		this->maxkey=p_keys;
	} 
	int Savenode(FILE *pfile){
		int i;
		BPleafkey<Datatype> *p_keys=dynamic_cast<BPleafkey<Datatype>*>(this->keys); 
		fwrite(&this->keycnt,sizeof(int),1,pfile);
		for (i=0;i<this->keycnt;i++){
			p_keys->Savekey(pfile);
			p_keys=dynamic_cast<BPleafkey<Datatype>*>(p_keys->Next);
		}
	}
	
	BPnode<Datatype>* Getnext(){return next;}
	int Insertkey(BPkey<Datatype> *key_x){
		BPkey<Datatype> *p;
		if (this->keycnt==0){
			this->keys=this->minkey=this->maxkey=key_x;
			
		}
		else if (key_x->Value<this->keys->Value){
			key_x->Next=this->keys;
			this->keys=key_x;
			this->minkey=key_x;
		}
		else {
			for (p=this->keys;p;p=p->Next){
				if ( (key_x->Value>p->Value)&&(!p->Next||key_x->Value<p->Next->Value)){
					key_x->Next=p->Next;
					p->Next=key_x;
					break;
				}
			}
			if (!key_x->Next) this->maxkey=key_x;
		}
		this->keycnt++;
		//this->showkeys();
	}
};

template <class Datatype> class  BPtree{
public:
	BPnode<Datatype> *root;
	BPnode<Datatype> *leaves;
	BPtree<Datatype>(){
		leaves=root=new BPleafnode<Datatype>;
	}
	BPtree<Datatype>(FILE *pfile){
		leaves=NULL;
		root=Loadtree(pfile);
	}
	BPnode<Datatype>* Findtargetnode(BPnode<Datatype> *currentnode,Datatype x){
		//BPkey<Datatype> *key_x=new BPkey<Datatype>(x);
		BPkey<Datatype> *p;
		if (!currentnode) return NULL;
		else if (typeid(*currentnode)==typeid(BPleafnode<Datatype>)){
			for (p=currentnode->keys;p;p=p->Next){
				if (p->Value==x) return NULL;
			}
			return currentnode;
		}
		else {
			return Findtargetnode(currentnode->Findnextnode(x),x);
		}
	}
	RecordID *Search(Datatype x){
		return Search(root,x);
	}
	RecordID *Search(BPnode<Datatype> *currentnode,Datatype x){
		BPkey<Datatype> *p;
		if (!currentnode) return NULL;
		else if (typeid(*currentnode)==typeid(BPleafnode<Datatype>)){
			for (p=currentnode->keys;p;p=p->Next){
				if (p->Value==x){
					BPleafkey<Datatype> *out=dynamic_cast<BPleafkey<Datatype>*>(p);
					return out->Record;
				}
			}
			return NULL;
		}
		else {
			return Search(currentnode->Searchnextnode(x),x);
		}
	}
	int Insertx(Datatype x,RecordID	*record){
		BPkey<Datatype> *key_x=static_cast<BPkey<Datatype>*>(new BPleafkey<Datatype>(x,record));
		BPnode<Datatype> *p=root;
		if (typeid(*p)!=typeid(BPleafnode<Datatype>)){
			p=Findtargetnode(p,x);
		}
		if (!p){//new key already exists.
			cout<<"key "<<x<<" already exists."<<endl;
			return -1;
		}
		p->Insertkey(key_x);
		//cout<<"insert "<<x<<endl;p->showkeys();
		if (p->Isoverflow()){
			Dividenode(p);
			//cout<<"parent update."<<endl;
			//p->parent->showkeys();
		}
		return 0;
	}
	int Insertx(Datatype x){
		Insertx(x,NULL);
	}
	int Dividenode(BPnode<Datatype> *x){
		int i;
		BPnode<Datatype> *currentnode=x;
		BPkey<Datatype> *p_key;
		BPnode<Datatype> *newnode;
		if (typeid(*x)==typeid(BPleafnode<Datatype>)){//if this node is a leaf node.
			BPleafnode<Datatype> *currentnode_leaf=dynamic_cast<BPleafnode<Datatype>*>(x);
			BPleafnode<Datatype> *newnode_leaf=new BPleafnode<Datatype>;
			newnode=dynamic_cast<BPnode<Datatype>*>(newnode_leaf);
			//link the leaves together.
			newnode_leaf->next=currentnode_leaf->next;
			currentnode_leaf->next=newnode_leaf;
		}
		else {//if this node is an interior node.
			newnode=new BPinteriornode<Datatype>;
		}
		p_key=currentnode->keys;	
		for (i=0;i<BPNODE_MINKEYCNT-1;i++){
			p_key=p_key->Next;
			//cout<<"current value:"<<p_key->Value<<endl;
		}
		//set up new leafnode.
		newnode->keys=p_key->Next;
		p_key->Next=NULL;
		newnode->keycnt=currentnode->keycnt-BPNODE_MINKEYCNT;
		currentnode->keycnt=BPNODE_MINKEYCNT;
		//update minkey and maxkey.
		newnode->minkey=newnode->keys;
		newnode->maxkey=currentnode->maxkey;
		currentnode->maxkey=p_key;
			
		if (!currentnode->parent){//new root node
			currentnode->parent=new BPinteriornode<Datatype>(currentnode,newnode,newnode->minkey->Value);
			root=dynamic_cast<BPnode<Datatype>*>(currentnode->parent);
		}
		else if(currentnode==currentnode->parent->keys->Getleftpointer()){//currentnode is the smallest NODE
			currentnode->parent->Insertkey(newnode->minkey->Value,currentnode,newnode);
		}
		else{
			currentnode->parent->Insertkey(newnode->minkey->Value,newnode);
		}
		newnode->parent=currentnode->parent;
		
		if (currentnode->parent->Isoverflow()){
			Dividenode(currentnode->parent);
		}
		return 0;
	}
	int Savetree(FILE *pfile){
		Savenodes(root,pfile);
	}
	int Savenodes(BPnode<Datatype> *currentnode,FILE *pfile){
		int i;
		int *LEAF=new int(1);
		int *INTERIOR=new int(2);
		if (typeid(*currentnode)==typeid(BPleafnode<Datatype>)){//if this node is a leaf node.
			fwrite(LEAF,sizeof(int),1,pfile);
			//cout<<"save leaf node:"<<endl;
			//currentnode->showkeys(); 
			currentnode->Savenode(pfile);
		}
		else{
			BPinteriorkey<Datatype> *p_key=dynamic_cast<BPinteriorkey<Datatype>*>(currentnode->keys);
			fwrite(INTERIOR,sizeof(int),1,pfile);
			//cout<<"save interior node:"<<endl;
			//currentnode->showkeys();
			currentnode->Savenode(pfile);
			Savenodes(p_key->Getleftpointer(),pfile);
			for (i=0;i<currentnode->keycnt;i++){
				Savenodes(p_key->Getrightpointer(),pfile);
				p_key=dynamic_cast<BPinteriorkey<Datatype>*>(p_key->Next);
			}
		}
	}
	BPnode<Datatype>* Loadtree(FILE *pfile){
		int *flag=new int;
		BPnode<Datatype> *currentnode;
		fread(flag,sizeof(int),1,pfile);
		if (*flag==1){//1 for leaf, 2 for interior
			BPleafnode<Datatype> *p_node;
			currentnode= new BPleafnode<Datatype>(pfile);
			//cout<<"loaded leaf:"<<endl; currentnode->showkeys(); 
			if (leaves) {//not the first leaf node
				p_node=dynamic_cast<BPleafnode<Datatype>*>(leaves);
				for(;p_node->next;p_node=p_node->next);//to the end of queue of leafnodes
				p_node->next=dynamic_cast<BPleafnode<Datatype>*>(currentnode);
			}
			else{//the first leafnode
				leaves=currentnode;
			}
		}
		else if (*flag==2){
			BPinteriorkey<Datatype> *p_key;
			currentnode= new BPinteriornode<Datatype>(pfile);
			//cout<<"loaded interior:"<<endl; currentnode->showkeys(); 
			p_key=dynamic_cast<BPinteriorkey<Datatype>*>(currentnode->keys);
			p_key->Leftpointer=Loadtree(pfile);
			for(;p_key;p_key=dynamic_cast<BPinteriorkey<Datatype>*>(p_key->Next)){
				p_key->Rightpointer=Loadtree(pfile);
			}
		}
		return currentnode;
	}
};

class Treeslot{
public:
	virtual int Insertx(Slot *x,RecordID *record){};
	virtual int Savetree(FILE *pfile){};
};
class Treeslot_int:public Treeslot{
public:
	BPtree<int> Tree;
	Treeslot_int():Tree(){}
	Treeslot_int(FILE *pfile):Tree(pfile){}
	int Insertx(Slot *x,RecordID *record){
		Intslot *slot=dynamic_cast<Intslot*>(x);
		Tree.Insertx(slot->data,record);
	}
	int Savetree(FILE *pfile){
		return Tree.Savetree(pfile);
	}
};
class Treeslot_float:public Treeslot{
public:
	BPtree<double> Tree;
	Treeslot_float():Tree(){}
	Treeslot_float(FILE *pfile):Tree(pfile){}
	int Insertx(Slot *x,RecordID *record){
		Floatslot *slot=dynamic_cast<Floatslot*>(x);
		Tree.Insertx(slot->data,record);
	}
	int Savetree(FILE *pfile){
		return Tree.Savetree(pfile);
	}
};
class Treeslot_char:public Treeslot{
public:
	BPtree<char> Tree;
	Treeslot_char():Tree(){}
	Treeslot_char(FILE *pfile):Tree(pfile){}
	int Insertx(Slot *x,RecordID *record){
		Charslot *slot=dynamic_cast<Charslot*>(x);
		Tree.Insertx(slot->data,record);
	}
	int Savetree(FILE *pfile){
		return Tree.Savetree(pfile);
	}
};

class DBIndex{
public:
	Treeslot *treeslot;
	DBIndex(int datatype){//create a new index
		switch (datatype){
			case DB_INT:{
				treeslot=new Treeslot_int;break;
			}
			case DB_FLOAT:{
				treeslot=new Treeslot_float;break;
			}
			case DB_CHAR:{
				treeslot=new Treeslot_char;break;
			}
		}
	}
	DBIndex(int datatype,string dbname,string tablename,string attrname){//load an index
		string path=dbname+"\\"+tablename+"."+attrname+".idx";
		FILE *pfile=fopen(const_cast<char*>(path.c_str()),"rb");
		switch (datatype){
			case DB_INT:{
				treeslot=new Treeslot_int(pfile);break;
			}
			case DB_FLOAT:{
				treeslot=new Treeslot_float(pfile);break;
			}
			case DB_CHAR:{
				treeslot=new Treeslot_char(pfile);break;
			}
		}
	}
	
	int Insertx(Slot *x,RecordID *record){
		return treeslot->Insertx(x,record);
	}
	int Makeindex(Tablebuffer *table,int attrorder){
		int i;
		for (i=0;i<table->Recordcnt;i++){
			Insertx(table->Records[i]->Values[attrorder],&table->Records[i]->recordid);
		}
		Savetree(table->DBname,table->Tablename,table->attr_names[attrorder]);
	}
	RecordID *Search(Slot *x){
		switch (x->type){
			case DB_INT:{
				Treeslot_int *treebuffer=dynamic_cast<Treeslot_int*>(treeslot);
				Intslot *slot=dynamic_cast<Intslot*>(x);
				return treebuffer->Tree.Search(slot->data);
				break;
			}
			case DB_FLOAT:{
				Treeslot_float *treebuffer=dynamic_cast<Treeslot_float*>(treeslot);
				Floatslot *slot=dynamic_cast<Floatslot*>(x);
				return treebuffer->Tree.Search(slot->data);
				break;
			}
			case DB_CHAR:{
				Treeslot_char *treebuffer=dynamic_cast<Treeslot_char*>(treeslot);
				Charslot *slot=dynamic_cast<Charslot*>(x);
				return treebuffer->Tree.Search(slot->data);
				break;
			} 
		}
	}
	int Savetree(string dbname,string tablename,string attrname){
		string path=dbname+"\\"+tablename+"."+attrname+".idx";
		FILE *pfile=fopen(const_cast<char*>(path.c_str()),"wb");
		treeslot->Savetree(pfile);
	}
	
};

#endif