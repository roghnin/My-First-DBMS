###################################################
This is a simple DBMS I wrote about one year ago, when I just finished learning basic C++ and database.
Now I can see some stupid errors in the codes, but the design is still valuable for refernces.
Also, some realizes of data structure, like the B+ tree in index module, can be useful after a little revise.

It's really a good memorial of my old passion. I miss those days.

Hs.
11-18-2015
###################################################
3-8-2015

available SQL commands：
create database: CREATE DATABASE DBname
drop database: DROP DATABASE DBname
change database: USE DATABASE DBname
show all the databases and the tables in them: SHOW DATABASE DBname
*DBname stands for a custom database name

create a table: CREATE TABLE tableName(attrName1 Type1, attrName2 Type2,…, attrNameN TypeN)
*foreign key, main key and unique attribute are available.
e.g: create table table1(attr1 int main key, attr2 string unique, attr3 int foreign key table0.attr1, attr4 char)

drop a table: DROP TABLE tableName
list all the tables and their status: SHOW TABLE
create index: CREATE INDEX tableName(attrName)
drop index: DROP INDEX tableName(attrName)
list all the indexes and their corresponding tables and status: SHOW INDEX

insert a record: INSERT INTO  [tableName(attrName1, attrName2,…, attrNameN)] VALUES (attrValue1, attrValue2,…, attrValueN)
tableName is required, attrNames are optional. if you choose some attrNames，then the unchosen ones will be set NULL (NOT for main keys).
put strings in double quotation marks, and chars in single quotation marks.

DELETE FROM  tableName  WHERE  whereClauses
UPDATE  tableName  SET  tableName.attrName = attExpression
set supports constant value or four basic arithmatic operations. for now, only int and float are available.

SELECT  tableName. AttrName  FROM  tableName  WHERE  whereClauses
select supports * to stand for all attributes. only the cartesian product of two tables is avilable.
e.g select table1.attr1,table2.attr2 from table1,table2 where table1.attr1=table2.attr1


there is an empty database given called Mydatabase, with four tables in it for tests.

ps. this project is developed in c-free 5.