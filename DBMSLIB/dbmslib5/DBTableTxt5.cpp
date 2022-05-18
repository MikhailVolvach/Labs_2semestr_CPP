#include "dbmslib5.h"

namespace dbmsLib5 {

	/* Consturctor */
	DBTableTxt5::DBTableTxt5(string tabName)
	{
		this->tableName = tabName;
	}
	DBTableTxt5::DBTableTxt5(string tabName, Header hdr, string primKey)
	{
		this->tableName = tabName;
		this->columnHeaders = hdr;
		this->primaryKey = primKey;
	}

	/* Destructor */
	DBTableTxt5::~DBTableTxt5()
	{
		for (size_t i = 0; i < this->data.size(); i++)
		{
			for (auto iter = this->data[i].begin(); iter != this->data[i].end(); iter++)
			{
				delete[] iter->second;
			}
		}
	}

	/* Main methods */
	void DBTableTxt5::PrintTable5(int screenWidth) {
		// Получение макета страницы
		dbmsLib5::Strip* strips;
		int nStrip;
		this->CreateTableMaket(strips, nStrip, screenWidth);

		if (nStrip < 1)
		{
			cerr << "Данных нет" << endl;
			exit(NOTHING_TO_PRINT);
		}

		cout << "Таблица " << this->tableName << endl;

		auto colHeadersIterSave = this->columnHeaders.begin();

		string keySave = this->data.begin()->begin()->first;
		size_t rowCnt = 0;
		for (size_t i = 0; i < nStrip; i++)
		{
			auto colHeadersIt = colHeadersIterSave;
			auto colHeadersIterTmp = colHeadersIterSave;
			auto rowIt = this->data.begin()->begin();
			if (i > 0)
			{
				cout << "Продолжение таблицы " << this->tableName << endl;
			}
			cout << setfill('=') << setw(screenWidth) << "=" << setfill(' ') << endl;
			// Вывод шапки таблицы
			size_t j = 0;
			for (colHeadersIt = colHeadersIterTmp; colHeadersIt != this->columnHeaders.end() && j < strips[i].nField; ++colHeadersIt)
			{
				cout << setw(strips[i].fieldWidth[j]) << colHeadersIt->second.colName;
				++colHeadersIterSave;
				++j;
			}
			cout << endl;
			j = 0;
			for (colHeadersIt = colHeadersIterTmp; colHeadersIt != this->columnHeaders.end() && j < strips[i].nField; ++colHeadersIt)
			{
				cout << setw(strips[i].fieldWidth[j]) << this->TypeName(colHeadersIt->second.colType);
				j++;
			}
			cout << endl;

			cout << setfill('-') << setw(screenWidth) << "-" << setfill(' ') << endl;

			// Вывод содержимого таблицы
			for (Row row : this->data)
			{
				j = 0;
				for (colHeadersIt = colHeadersIterTmp; colHeadersIt != this->columnHeaders.end() && j < strips[i].nField; ++colHeadersIt)
				{
					cout << setw(strips[i].fieldWidth[j]) << this->valueToString(row, colHeadersIt->second.colName);
					++j;
				}
				cout << endl;
				++rowCnt;
			}
			cout << setfill('=') << setw(screenWidth) << "=" << setfill(' ') << endl;
		}
	}
	void DBTableTxt5::ReadDBTable5(string tabName)
	{
		this->columnHeaders.clear();

		ifstream FILE(tabName);
		string line;
		getline(FILE, line);
		this->tableName = line.substr(0, line.find_first_of('|'));
		this->primaryKey = line.substr(line.find_first_of('|') + 1);

		getline(FILE, line);
		ColumnDesc Title;
		int i = 0;
		int cnt = 0;
		string word = "";
		line.push_back('|');
		while (i < line.length())
		{
			if (line[i] != '|' && line[i] != '\0' && line[i] != '\n')
			{
				word += line[i];
			}
			else
			{
				switch (cnt)
				{
				case 0: {
					strcpy(Title.colName, word.c_str());
					word = "";
					cnt++;
					break;
				}
				case 1: {
					if (word == "NoType")
					{
						Title.colType = NoType;
					}
					if (word == "Int32")
					{
						Title.colType = Int32;
					}
					if (word == "Double")
					{
						Title.colType = Double;
					}
					if (word == "String")
					{
						Title.colType = String;
					}
					if (word == "Date")
					{
						Title.colType = Date;
					}
					word = "";
					cnt++;
					break;

				}
				case 2: {
					Title.length = stoi(word);
					this->columnHeaders[Title.colName] = Title;
					word = "";
					cnt++;
					break;
				}
				default:
					break;
				}
				cnt %= 3;
			}
			i++;
		}

		pair<string, void*> parRow;
		while (!FILE.eof())
		{
			Row row = *(new Row);
			getline(FILE, line);
			i = 0;
			for (auto it = this->columnHeaders.begin(); it != this->columnHeaders.end(); it++)
			{
				string word = "";
				while (line[i] != '|' && line[i] != '\0' && line[i] != '\n')
				{
					if (line[i] != ' ')
					{
						word += line[i];
					}
					i++;
				}

				if (line[i] == '|')
				{
					i++;
				}

				if (word.size())
				{
					parRow.first = it->first;
					parRow.second = GetValue(word, it->second.colName, this->columnHeaders);

					row.insert(parRow);
				}
			}
			if (row.size())
			{
				this->data.push_back(row);
			}
		}
		FILE.close();
	}
	void DBTableTxt5::WriteDBTable5(string fileName)
	{
		ofstream File(fileName);

		File << this->tableName << "|" << this->primaryKey << endl;
		for (auto colHeadersIt = this->columnHeaders.begin(); colHeadersIt != this->columnHeaders.end(); ++colHeadersIt)
		{
			auto colHeadersItVal = colHeadersIt->second;
			File << colHeadersItVal.colName << "|" << this->TypeName(colHeadersItVal.colType) << "|" << colHeadersItVal.length << "|";
		}
		File << endl;

		for (Row row : this->data)
		{
			for (auto rowIt = row.begin(); rowIt != row.end(); ++rowIt)
			{
				auto colHeadersItVal = rowIt->second;
				File << setfill(' ') << setw(this->columnHeaders[rowIt->first].length) << this->valueToString(row, rowIt->first) << "|";
			}
			File << endl;
		}
		File.close();
	}

	/* Getters */
	string DBTableTxt5::GetFileName() { 
		if (this->fileName.empty())
		{
			exit(EMPRTY_CLASS_FIELD);
		}
		return this->fileName;
	}
	string DBTableTxt5::GetTableName() { 
		if (this->tableName.empty())
		{
			exit(EMPRTY_CLASS_FIELD);
		}
		return this->tableName; 
	}
	Header DBTableTxt5::GetHeader() { 
		if (this->columnHeaders.empty())
		{
			exit(EMPRTY_CLASS_FIELD);
		}
		return this->columnHeaders; 
	}
	string GetTabNameFromPath(string path) { 
		if (path.empty())
		{
			exit(INCORRECT_INPUT);
		}
		return path.substr(path.find_last_of('\\'), path.find_last_of('.') - path.find_last_of('\\') - 1); 
	}
	void* GetValue(string value, string columnName, Header hdr)
	{
		if (value.empty() || columnName.empty() || hdr.empty())
		{
			exit(INCORRECT_INPUT);
		}
		switch (hdr[columnName].colType)
		{
		case Int32:		return new int(stoi(value)); break;
		case Double:	return new double(stod(value)); break;
		case String:	return new string(value); break;
		case Date:		return new DBDate5(value); break;
		default: cout << "Недопустимый тип данных в столбце" << endl <<
			columnName << endl;
		}
		return 0;
	}
	int GetLength(ColumnDesc colDesc) { 
		if (!colDesc.length)
		{
			exit(EMPRTY_CLASS_FIELD);
		}
		return colDesc.length; 
	}
	int DBTableTxt5::GetSize() { 
		if (!this->data.size())
		{
			exit(EMPRTY_CLASS_FIELD);
		}
		return this->data.size(); 
	}
	Row DBTableTxt5::GetRow(int index) { 
		if (this->data[index].empty())
		{
			exit(EMPRTY_CLASS_FIELD);
		}
		return this->data[index]; 
	}
	/*DBType dbmsLib5::DBTableTxt5::GetType(char* columnName)
	{
		return DBType();
	}*/

	/* Setters */
	void DBTableTxt5::SetFileName(string path) { 
		if (path.empty())
		{
			exit(INCORRECT_INPUT);
		}
		this->fileName = path.substr(path.find_last_of('/')); 
	}
	void DBTableTxt5::SetTableName(string tName) { 
		if (tName.empty())
		{
			exit(INCORRECT_INPUT);
		}
		this->tableName = tName; 
	}
	void DBTableTxt5::SetPrimaryKey(string key) {
		if (key.empty())
		{
			exit(INCORRECT_INPUT);
		}
		this->primaryKey = key; 
	}
	void DBTableTxt5::SetHeader(Header& hdr) {
		if (hdr.empty())
		{
			exit(INCORRECT_INPUT);
		}
		this->columnHeaders = hdr; 
	}
	void* SetValue(string value, string columnName, Header hdr) { 
		return nullptr;
	}

	/* Other methods */
	void DBTableTxt5::AddRow(int index)
	{
		cout << "Таблица " << this->GetTableName() << endl;
		cout << "ID последней записи: " << this->data.size() << endl;
		pair<string, void*> parRow;
		Row row = (*new Row);

		string strData = "";
		for (auto it = this->columnHeaders.begin(); it != this->columnHeaders.end(); ++it)
		{
			cout << "Enter " << it->second.colName << ": "; cin >> strData;
			parRow.first = it->second.colName;
			parRow.second = GetValue(strData, it->second.colName, this->columnHeaders);

			row.insert(parRow);
		}
		if (!strData.size())
		{
			cerr << "Некорректный ввод данных" << endl;
			return;
		}
		this->data.push_back(row);
	}
	bool comparator(DBType type, void* obj1, Condition condition, void* obj2)
	{
		switch (type) {
		case Int32:
			switch (condition) {
			case	Equal:		return *(int*)obj1 == *(int*)obj2;
			case NotEqual:		return *(int*)obj1 != *(int*)obj2;
			case	 Less:		return *(int*)obj1 < *(int*)obj2;
			case  Greater:		return *(int*)obj1 > *(int*)obj2;
			case LessOrEqual:	return *(int*)obj1 <= *(int*)obj2;
			case GreaterOrEqual:	 return *(int*)obj1 >= *(int*)obj2;
			default: cout << "Недопустимая операция сравнения\n" << endl;
				return false;
			}
		case Double:
			switch (condition) {
			case	Equal:		return *(double*)obj1 == *(double*)obj2;
			case NotEqual:		return *(double*)obj1 != *(double*)obj2;
			case	 Less:		return *(double*)obj1 < *(double*)obj2;
			case  Greater:		return *(double*)obj1 > *(double*)obj2;
			case LessOrEqual:	return *(double*)obj1 <= *(double*)obj2;
			case GreaterOrEqual: 	return *(double*)obj1 >= *(double*)obj2;
			default: cout << "Недопустимая операция сравнения\n" << endl;
				return false;
			}
		case String: //пропуск ведущих пробелов в string
			switch (condition) {
			case	Equal:		return ignoreBlanc(*(string*)obj1) == ignoreBlanc(*(string*)obj2);
			case NotEqual:		return ignoreBlanc(*(string*)obj1) != ignoreBlanc(*(string*)obj2);
			case	 Less:		return ignoreBlanc(*(string*)obj1) < ignoreBlanc(*(string*)obj2);
			case  Greater:		return ignoreBlanc(*(string*)obj1) > ignoreBlanc(*(string*)obj2);
			case LessOrEqual:	return ignoreBlanc(*(string*)obj1) <= ignoreBlanc(*(string*)obj2);
			case GreaterOrEqual: 	return ignoreBlanc(*(string*)obj1) >= ignoreBlanc(*(string*)obj2);
			default: cout << "Недопустимая операция сравнения\n" << endl;
				return false;
			}
		case Date: switch (condition) {
		case	Equal:		return *(DBDate5*)obj1 == *(DBDate5*)obj2;
		case NotEqual:		return *(DBDate5*)obj1 != *(DBDate5*)obj2;
		case	 Less:		return *(DBDate5*)obj1 < *(DBDate5*)obj2;
		case  Greater:		return *(DBDate5*)obj1 > *(DBDate5*)obj2;
		case LessOrEqual:	return *(DBDate5*)obj1 <= *(DBDate5*)obj2;
		case GreaterOrEqual: return *(DBDate5*)obj1 >= *(DBDate5*)obj2;
		default: cout << "Недопустимая операция сравнения\n" << endl;
			return false;
		}

		default: cout << "Недопустимый тип данных\n" << endl;
			return false;
		}
	}
	string ignoreBlanc(const string str)
	{
		string bufStr = str;
		int begStr = bufStr.find_first_not_of(' ');
		return bufStr.substr(begStr);
	}
	vector<int> DBTableTxt5::IndexOfRecord(void* keyValue, string keyColumnName)
	{
		vector<int> result;
		size_t index = 0;
		for (Row row : this->data)
		{
			if (row[keyColumnName] == keyValue)
			{
				result.push_back(index);
			}
			index++;
		}
		return result;
	}
	DBTable5* dbmsLib5::DBTableTxt5::SelfRows(string columnName, Condition cond, void* value)
	{
		return nullptr;
	}

	string DBTableTxt5::valueToString(Row& row, string columnName) {
		char buf[80] = { 0 };
		if (this->columnHeaders.find(columnName) != this->columnHeaders.end())
		{
			void* value = row[columnName];//
			ostrstream os(buf, 80);
			switch (columnHeaders[columnName].colType) {
			case Int32: os << setw(8) << *(int*)value << '\0'; break;
			case Double: os << setw(12) << *(double*)value << '\0'; break;
			case String:os << setw(columnHeaders[columnName].length) <<
				*(string*)value << '\0'; break;
			case Date: os << setw(12) << *(DBDate5*)value << '\0'; break;
			default: cout << setw(8) << "NoType" << '\0';
				system("pause"); break;
			}
		}
		else
			cout << "column not found " << endl;
		return buf;
	}

	void DBTableTxt5::CreateTableMaket(Strip*& strips, int& nStrip, int screenWidth)
	{
		Header::iterator headerIter, contHeaderIter;
		int nColumn = (int)columnHeaders.size();
		//Заполнение массива ширины полей для печати таблицы 
		int* fieldW = new int[nColumn];
		headerIter = columnHeaders.begin();
		for (int k = 0; k < nColumn; k++) {
			//что шире: заголовок или данные?
			fieldW[k] = (headerIter->second.length > headerIter->first.size()) ?
				headerIter->second.length + 2 : headerIter->first.size() + 2;
			headerIter++;
		}
		int currCol = 0;//порядковый номер столбца в таблице
		nStrip = 1;//число полос в распечатке таблицы
		int sumWidth = 0;//суммарная ширина столбцов в полосе
		int n = 0;//число столбцов в полосе
		int buff[40] = { 0 };//объявление и обнуление буфера для временного 
		//хранения числа столбцов в полосе (n<40)
		for (currCol = 0; currCol < nColumn; currCol++) {
			if (fieldW[currCol] >= screenWidth - 1) {
				cout << "Ширина столбца " << currCol << " больше ширины экрана\n";
				cout << "Таблицу нельзя распечатать" << endl;
				nStrip = 0;
				return;
			}
			sumWidth += fieldW[currCol];
			if ((sumWidth < screenWidth - 1) && (currCol < nColumn - 1)) {
				n++;
				continue;
			}
			if ((sumWidth >= screenWidth - 1)) {//выход за границу экрана 
				currCol--;
				buff[nStrip - 1] = n;
				nStrip++;
				n = 0;
				sumWidth = 0;
				continue;
			}
			if (currCol == nColumn - 1) {//последняя полоса
				n++;
				buff[nStrip - 1] = n;
			}
		}
		strips = new Strip[nStrip];
		int col = 0;
		for (int i = 0; i < nStrip; i++) {
			strips[i].nField = buff[i];
			strips[i].fieldWidth = new int[strips[i].nField];
			for (int j = 0; j < strips[i].nField; j++)
				strips[i].fieldWidth[j] = fieldW[col++];
		}
	}

	//void WriteTableBin5(DBTableTxt5& tab, string fileName)
	//{
	//	ofstream fout;
	//	fout.open((fileName).c_str(), ios::binary | ios::out);
	//	if (!fout.is_open())
	//	{
	//		cout << "Ошибка открытия файла " << fileName << endl;
	//		system("pause");
	//		return;
	//	}
	//	Header::iterator iter;
	//	Row::iterator dataIter;
	//	//Запись в бинарный файл первой строки .txt-файла:
	//	//имени таблицы  и 
	//	//(длины имён таблиц и столбцов фиксированы и равны 24 байтам, включая \0) 
	//	const int BUF_SIZE = 100;
	//	char buf[BUF_SIZE];
	//	strcpy_s(buf, BUF_SIZE, tab.tableName.c_str());
	//	int len = LENGTH;
	//	fout.write((char*)&len, sizeof(int));
	//	fout.write(buf, len);
	//	int size = tab.columnHeaders.size();
	//	fout.write((char*)&size, sizeof(int));
	//	size = sizeof(ColumnDesc);
	//	for (iter = tab.columnHeaders.begin(); iter != tab.columnHeaders.end(); iter++)
	//		fout.write((char*)&(iter->second), size);
	//	//Запись строк таблицы.
	//	int nRows = (int)tab.data.size();
	//	fout.write((char*)&nRows, sizeof(int));
	//	for (int i = 0; i < nRows; i++) {
	//		dataIter = tab.data[i].begin();
	//		for (iter = tab.columnHeaders.begin(); iter != tab.columnHeaders.end(); dataIter++, iter++)
	//		{//iter->second указывает на ColumnDesc, dataIter->second указывает на void*.
	//			switch ((iter->second).colType) {
	//			case Int32:
	//				fout.write((char*)(dataIter->second), sizeof(int));
	//				break;
	//			case Double:
	//				fout.write((char*)(dataIter->second), sizeof(double));
	//				break;
	//			case Date:
	//				fout.write((char*)(dataIter->second), sizeof(Date));
	//				break;
	//			case String:
	//				strcpy_s(buf, BUF_SIZE, (*(string*)(dataIter->second)).c_str());
	//				fout.write(buf, iter->second.length); // Заменил buf+1 на buf, т.к. терялись первые символы слов!!!!!!!!!!!!!!!!!!!!!! 
	//				//buf+1 - для того, чтобы length не обрубал конец строки '\0'
	//				break;
	//			default: cout << "Недопустимый тип данных в БД\n";
	//				return;
	//			}
	//		}
	//	}
	//}

	void ReadTableBin5(DBTableTxt5& tab, string tabName)
	{
		ifstream fin;
		fin.open((tabName).c_str(), ios::binary | ios::in);
		if (!fin.is_open())
		{
			cout << "Ошибка открытия файла\n" << endl;
			system("pause");
			return;
		}
		//Прочитать имя таблицы и имя столбца с primaryKey из первой строки файла
		int len;
		char buf[80];
		fin.read((char*)&len, 4);
		fin.read((char*)buf, len);
		if (len > 79) {
			cout << "Ошибка: длина имени таблицы " << tabName << endl;
			return;
		}
		buf[len] = '\0';
		tab.tableName = buf;
		//Прочитать имя столбца с primaryKey

			//Прочитать из файла заголовок таблицы и записать его в table.columnHeaders
		int size = 0;	//число столбцов в заголовке таблицы
		fin.read((char*)&size, sizeof(int));
		len = sizeof(ColumnDesc);
		ColumnDesc colDesc;
		tab.columnHeaders.clear();
		for (int i = 0; i < size; i++) {
			fin.read((char*)&colDesc, len);
			tab.columnHeaders[colDesc.colName] = colDesc;
		}
		//Чтение строк таблицы
		Header::iterator hdrIter;

		tab.data.clear();
		int nRows;
		fin.read((char*)&nRows, 4);
		len = sizeof(colDesc.colName);//длина имени столбца (=24 байта) 
		for (int i = 0; i < nRows; i++)
		{
			Row row;//буфер для формирования строки таблицы
			pair<string, void*> pr;
			for (hdrIter = tab.columnHeaders.begin(); hdrIter != tab.columnHeaders.end(); hdrIter++) {
				pr.first = hdrIter->first;
				switch ((hdrIter->second).colType) {//чтение данных
				case Int32:	
					pr.second = new int;
					fin.read((char*)pr.second, sizeof(int));
					row.insert(pr);	break;
				case Double:
					pr.second = new double;
					fin.read((char*)pr.second, sizeof(double));
					row.insert(pr);	
					break;
				case Date:
					pr.second = new DBDate5();
					fin.read((char*)pr.second, sizeof(Date));
					row.insert(pr);	break;
				case String:
					if (hdrIter->second.length > 79) {
						cout << "Ошибка: длина поля " << hdrIter->second.colName << " таблицы " << tabName << endl;
						return;
					}
					fin.read(buf, hdrIter->second.length);
					buf[hdrIter->second.length] = '\0';
					pr.second = new string(ignoreBlanc(buf));
					row.insert(pr);
					//cout<<*(string*)row[pr.first]<<endl;
					break;
				default: cout << "Недопустимый тип данных в БД\n";
					return;
				}
			}
			tab.data.push_back(row);
		}
		fin.close();
	}
}