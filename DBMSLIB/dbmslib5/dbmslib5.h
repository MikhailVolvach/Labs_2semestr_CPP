//файл dbmslib5.h
#pragma once
#ifndef _dbmsLib5_
#define _dbmsLib5_
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <strstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <sstream>

using namespace std;
namespace dbmsLib5
{
	enum Errors {
		EMPTY_ARGUMENT,
		INCORRECT_INPUT,
		EMPRTY_CLASS_FIELD,
		NOTHING_TO_PRINT,
	};

	static const int arrDays[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	//====================класс DBDate5======================
	class DBDate5
	{
		friend string DateToStr(DBDate5& date);
		friend ostream& operator<<(ostream& out, DBDate5& date);
		int day, month, year;
		bool IsLeapYear(int year); //год високосный?
		int GetDaysInMonth(int month, int year);//Количество дней в месяце
		int DaysInCurYear();//Количество дней от начала года до текущей даты
	public:
		DBDate5(string date);//формат строки: dd.mm.yyyy
		DBDate5(int d, int m, int y);
		DBDate5() :day(0), month(0), year(0) {};//???конструктор по умолчанию
		DBDate5(DBDate5& dat) :day(dat.day), month(dat.month), year(dat.year) {}
		int GetDay();
		int GetMonth();
		int GetYear();
		bool operator==(DBDate5& date);
		bool operator<(DBDate5& date);
		bool operator>(DBDate5& date);
		bool operator<= (DBDate5& date);
		bool operator>= (DBDate5& date);
		bool operator!= (DBDate5& date);
		const DBDate5& operator+=(int days);//Прибавляет к текущей дате days дней
		DBDate5& operator-=(int days);//Вычитает из текущей даты days дней
		int operator-(DBDate5& date);	//Количество дней между текущей датой и date
		//Если текущая дата > date, результат < 0.
	};
	//DBType-перечисление типов полей таблиц БД:
	enum DBType { NoType, Int32, Double, String, Date };
	//typeNames - массив имен типов. 
	//Используется для преобразования имени типа (char*) в объект DBType 
	static const char* typeNames[]={
		"NoType",
		"Int32",
		"Double",
		"String",
		"DBDate5"
	};
	//Condition-перечисление условий, по которым могут сравниваються поля таблиц БД:
	enum Condition { Undefined, Equal, NotEqual, Less, Greater, LessOrEqual, GreaterOrEqual };
	const int LENGTH = 24;//длина имени таблицы и имени столбца.
	//ColumnDesc - описание данных в столбце таблицы 
	//данные типа string имеют одинаковую длину (length) внутри одного столбца, 
	//но в разных столбцах их длина может отличаться
	struct ColumnDesc {
		char colName[LENGTH];//имя столбца
		DBType colType;//тип данных в столбце таблицы
		int length; //максимальное число символов, допустимое 
		//для представления данных в столбце
	};
	struct Strip {//полоса распечатки таблицы
		int nField;//число полей 
		int* fieldWidth;//ширина полей в полосе (массив)
	};
	typedef map<string, void*> Row;
	typedef map<string, ColumnDesc> Header;
	//-----------------класс DBDate5----------------------------
	class DBTable5 {
	private:
		Header columnHeaders;
		vector<Row> data;
		string tableName;
	public:
		DBType GetType(char* columnName);
		const char* TypeName(DBType type);
		virtual ~DBTable5() {};//виртуальный деструктор
		virtual Header GetHeader() = 0;
		virtual vector<int> IndexOfRecord(void* keyValue, string keyColumnName) = 0;
		//по значению ключа определяет 
		//порядковый номер (индекс) строки (записи) в data.
		// keyValue-значение ключа (указатель на string, int или DBDate5)
		// keyColumnName-имя ключевого столбца (первичный или вторичный ключ)
		//в таблице может быть несколько записей с одинаковым значением вторичного ключа.
		virtual Row GetRow(int index) = 0;
		virtual Row operator[](int index) = 0;
		virtual string valueToString(Row& row, string columnName) = 0;
		virtual int GetSize() = 0;
		virtual void SetHeader(Header& hdr) = 0;
		virtual void ReadDBTable5(string fileName) = 0;//fileName="..\\"+dbName+"\\"+tableName+".bin"|".txt"	
		virtual void PrintTable5(int screenwidth);

		virtual void WriteDBTable5(string fileName) = 0;
		virtual void SetTableName(string tName) = 0;
		virtual void SetPrimaryKey(string key) = 0;
		virtual string GetTableName() = 0;
		virtual string GetPrimaryKey() = 0;
		virtual void SetFileName(string path) = 0;
		virtual string GetFileName() = 0;
		//virtual void PrintTable(int screenWidth) = 0;
		virtual Row CreateRow() = 0;
		virtual void AddRow(int index) = 0;
		virtual DBTable5* SelfRows(string colName, Condition cond, void* value) = 0;
		virtual void CreateTableMaket(Strip*& strips, int& nStrip, int screenWidth) = 0;
	};

	//===============class DBTableTxt5=============================
	class DBTableTxt5 : public DBTable5 {
	private:
		Header columnHeaders;
		string tableName;
		string primaryKey;
		vector<Row> data;
		string fileName;
	public:
		DBTableTxt5() {}
		DBTableTxt5(string tabName);/*: DBTable5(tabName){}*/
		DBTableTxt5(string tabName, Header hdr, string primKey);/*:DBTable5(tabName,hdr,primKey){}*/
		~DBTableTxt5();//!!!деструктор перегружен,чтобы память,выделенная void*,не утекала
		vector<int> IndexOfRecord(void* keyValue, string keyColumnName);
		string valueToString(Row& row, string columnName);
		void ReadDBTable5(string tabName);//tabName=path+tableName
		void PrintTable5(int screenWidth);
		void WriteDBTable5(string tabName);//tabName=path+tableName	
		int GetSize();
		Row operator[](int ind) { return this->data[ind]; };
		void SetFileName(string path);
		void SetTableName(string tName);
		void SetPrimaryKey(string key);
		string GetFileName();
		string GetTableName();
		string GetPrimaryKey() { return primaryKey; }
		Header GetHeader();
		void SetHeader(Header& hdr);
		Row CreateRow() { return Row(); };
		Row GetRow(int index);
		void AddRow(int index);
		DBTable5* SelfRows(string columnName, Condition cond, void* value);
		//Макет распечатки таблицы:
		//screenWidth - ширина экрана (входной параметр)
		//nStrips - число полос в распечатке (выходной параметр)
		//strips[nStrips] - описание полос таблицы: 
		//число столбцов и ширина каждого столбца в полосе (выходной параметр)
		void CreateTableMaket(Strip*& strips, int& nStrips, int screenWidth);

		friend void WriteTableBin5(DBTableTxt5& tab, string fileName);
		friend void ReadTableBin5(DBTableTxt5& tab, string tabName);
	};
	//=====================Класс DBTableBin5=====================================
	class DBTableBin5 : public DBTable5 {
		Header columnHeaders;
		string tableName;
		string primaryKey;//имя столбца со значениями первичного ключа
		int nColumn;
		//ColumnDesc* header;
		int nRows;
		int RowLength();
		int DataBegin();
		int FieldPosition(string colName);//смещение поля colName от начала строки в байтах
		int FieldLength(string colName);//длина поля colName в байтах
		void ResizeData(int deltaRows);
		vector<Row> data;
		int maxRows;//число строк в data с учетом DELTA (при считывании таблицы//????int maxRows()
		//из файла устанавливается равным nRows+DELTA).В файл не записывается
		char fileName[80];//В файл не записывается. Используется в методах для
		//обращения к файлу
	public:
		DBTableBin5() {}
		DBTableBin5(string tableName);
		DBTableBin5(string tableName, Header hdr, string primKey);
		~DBTableBin5();
		Header GetHeader();
		vector<int> IndexOfRecord(void* keyPtr, string keyName);
		//по значению ключа определяет 
		//порядковый номер (индекс) строки (записи) в data.
		// keyPtr-значение ключа (указатель на string, int или DBDate5)
		// keyName-имя ключевого столбца (первичный или вторичный ключ)
		//в таблице может быть несколько записей с одинаковым значением вторичнего ключа.
		int FieldPosInFile(int rowIndex, string columnName);
		Row GetRow(int index);
		Row operator[](int index);
		string valueToString(Row& row, string columnName);
		int GetSize();
		void SetHeader(Header& hdr);
		void ReadDBTable5(string fileName);//fileName="..\\"+dbName+"\\"+tableName+".bin"											
		void WriteDBTable5(string fileName);
		void SetTableName(string tName);
		void SetPrimaryKey(string key);
		string GetTableName();
		string GetPrimaryKey();
		void SetFileName(string path);
		string GetFileName();
		void PrintTable5(int screenWidth);
		Row CreateRow();
		void AddRow(int index);
		DBTable5* SelfRows(string colName, Condition cond, void* value);

		void CreateTableMaket(Strip*& strips, int& nStrips, int screenWidth);
	};
	//==============Класс DBTableSet5============================
	struct Relation {//описывает связь двух таблиц
		string relationName;
		string parentTable;
		string parentPrimaryKey;
		string childTable;
		string childSecondaryKey;
	};
	void PrintRow(Row row, DBTable5* tab);
	//-------------------------класс DBTableSet5---------------------
	class DBTableSet5
	{
	private:
		string dbName;
		map<string, DBTable5*> db;
	public:
		DBTableSet5() {};
		DBTableSet5(string name);
		void ReadDB5();
		void PrintDB5(int numcol);
		void WriteDB5();
		string GetDBName() { return dbName; }
		DBTable5* operator[](string tableName);
		Relation GetRelation(string relationName);
		Row ParentRow(Relation& relation, Row& childRow);
		DBTable5* ChildRows(Relation& relation, Row& parentRow);

		friend string FileType(string dbName);

		int initNewDB(vector<string> tableNames);
	};

	const int DELTA = 10;//запас в 10 строк для новых записей в таблицу. 
	//Если запас израсходован, то для его восстановления нужно увеличить
	//на DELTA размер динамического массива char* data[]. 
	//const int LENGTH = 24;//длина имени таблицы и имени столбца.

	string GetTabNameFromPath(string path);
	string ignoreBlanc(const string str);
	void* GetValue(string value, string columnName, Header hdr);
	void* SetValue(string value, string columnName, Header hdr);
	bool comparator(/*TableDataType*/DBType type, void* obj1, Condition condition, void* obj);
	int GetLength(ColumnDesc colDesc);
}

#endif //конец _dbmsLib_