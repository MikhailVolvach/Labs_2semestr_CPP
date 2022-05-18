#include "dbmslib5.h"

namespace dbmsLib5 {

	/* Constructors */
	DBTableBin5::DBTableBin5(string tabName) 
	{
		this->tableName = tabName;
	}
	DBTableBin5::DBTableBin5(string tableName, Header hdr, string primKey) 
	{
		this->tableName = tableName;
		this->columnHeaders = hdr;
		this->primaryKey = primKey;
	}

	/* Destructor */
	DBTableBin5::~DBTableBin5() 
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
	void DBTableBin5::ReadDBTable5(string tabName)
	{
		ifstream fin;
		fin.open((tabName).c_str(), ios::binary | ios::in);
		if (!fin.is_open())
		{
			cout << "������ �������� �����\n" << endl;
			system("pause");
			return;
		}
		//��������� ��� ������� � ��� ������� � primaryKey �� ������ ������ �����
		int len;
		char buf[80];
		fin.read((char*)&len, 4);
		fin.read((char*)buf, len);
		if (len > 79) {
			cout << "������: ����� ����� ������� " << tabName << endl;
			return;
		}
		buf[len] = '\0';
		this->tableName = buf;
		//��������� ��� ������� � primaryKey

			//��������� �� ����� ��������� ������� � �������� ��� � table.columnHeaders
		int size = 0;	//����� �������� � ��������� �������
		fin.read((char*)&size, sizeof(int));
		len = sizeof(ColumnDesc);
		ColumnDesc colDesc;
		this->columnHeaders.clear();
		for (int i = 0; i < size; i++) {
			fin.read((char*)&colDesc, len);
			this->columnHeaders[colDesc.colName] = colDesc;
		}
		//������ ����� �������
		Header::iterator hdrIter;

		this->data.clear();
		int nRows;
		fin.read((char*)&nRows, 4);
		len = sizeof(colDesc.colName);//����� ����� ������� (=24 �����) 
		for (int i = 0; i < nRows; i++)
		{
			Row row;//����� ��� ������������ ������ �������
			pair<string, void*> pr;
			for (hdrIter = this->columnHeaders.begin(); hdrIter != this->columnHeaders.end(); hdrIter++) {
				pr.first = hdrIter->first;
				switch ((hdrIter->second).colType) {//������ ������
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
						cout << "������: ����� ���� " << hdrIter->second.colName << " ������� " << tabName << endl;
						return;
					}
					fin.read(buf, hdrIter->second.length);
					buf[hdrIter->second.length] = '\0';
					pr.second = new string(ignoreBlanc(buf));
					row.insert(pr);
					//cout<<*(string*)row[pr.first]<<endl;
					break;
				default: cout << "������������ ��� ������ � ��\n";
					return;
				}
			}
			this->data.push_back(row);
		}
		fin.close();
	}

	void DBTableBin5::PrintTable5(int screenwidth) 
	{
		// ��������� ������ ��������
		dbmsLib5::Strip* strips;
		int nStrip;
		this->CreateTableMaket(strips, nStrip, screenwidth);

		if (nStrip < 1)
		{
			cerr << "������ ���" << endl;
			exit(NOTHING_TO_PRINT);
		}

		cout << "������� " << this->tableName << endl;

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
				cout << "����������� ������� " << this->tableName << endl;
			}
			cout << setfill('=') << setw(screenwidth) << "=" << setfill(' ') << endl;
			// ����� ����� �������
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

			cout << setfill('-') << setw(screenwidth) << "-" << setfill(' ') << endl;

			// ����� ����������� �������
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
			cout << setfill('=') << setw(screenwidth) << "=" << setfill(' ') << endl;
		}
	}
	void DBTableBin5::WriteDBTable5(string fileName)
	{
		ofstream fout;
		fout.open((fileName).c_str(), ios::binary | ios::out);
		if (!fout.is_open())
		{
			cout << "������ �������� ����� " << fileName << endl;
			system("pause");
			return;
		}
		Header::iterator iter;
		Row::iterator dataIter;
		//������ � �������� ���� ������ ������ .txt-�����:
		//����� �������  � 
		//(����� ��� ������ � �������� ����������� � ����� 24 ������, ������� \0) 
		const int BUF_SIZE = 100;
		char buf[BUF_SIZE];
		strcpy_s(buf, BUF_SIZE, this->tableName.c_str());
		int len = LENGTH;
		fout.write((char*)&len, sizeof(int));
		fout.write(buf, len);

		int size = this->columnHeaders.size();
		fout.write((char*)&size, sizeof(int));
		size = sizeof(ColumnDesc);
		for (iter = this->columnHeaders.begin(); iter != this->columnHeaders.end(); iter++)
			fout.write((char*)&(iter->second), size);
		//������ ����� �������.
		int nRows = (int)this->data.size();
		fout.write((char*)&nRows, sizeof(int));
		for (int i = 0; i < nRows; i++) {
			dataIter = this->data[i].begin();
			for (iter = this->columnHeaders.begin(); iter != this->columnHeaders.end(); dataIter++, iter++)
			{//iter->second ��������� �� ColumnDesc, dataIter->second ��������� �� void*.
				switch ((iter->second).colType) {
				case Int32:
					fout.write((char*)(dataIter->second), sizeof(int));
					break;
				case Double:
					fout.write((char*)(dataIter->second), sizeof(double));
					break;
				case Date:
					fout.write((char*)(dataIter->second), sizeof(Date));
					break;
				case String:
					strcpy_s(buf, BUF_SIZE, (*(string*)(dataIter->second)).c_str());
					fout.write(buf, iter->second.length); // ������� buf+1 �� buf, �.�. �������� ������ ������� ����!!!!!!!!!!!!!!!!!!!!!! 
					//buf+1 - ��� ����, ����� length �� ������� ����� ������ '\0'
					break;
				default: cout << "������������ ��� ������ � ��\n";
					return;
				}
			}
		}

	}

	/* Getters */
	Header DBTableBin5::GetHeader()		{ return this->columnHeaders; }
	int DBTableBin5::GetSize()			{ return this->nRows; }
	string DBTableBin5::GetTableName()	{ return this->tableName; }
	string DBTableBin5::GetPrimaryKey() { return this->primaryKey; }
	string DBTableBin5::GetFileName()	{ return this->fileName; }
	Row DBTableBin5::GetRow(int index)	{ return this->data[index]; }

	/* Setters */
	void DBTableBin5::SetHeader(Header& hdr)		{ this->columnHeaders = hdr; }
	void DBTableBin5::SetTableName(string tName)	{ this->tableName = tName; }
	void DBTableBin5::SetPrimaryKey(string key)		{ this->primaryKey = key; }
	void DBTableBin5::SetFileName(string path)		{ strcpy_s(this->fileName, 80, (path + "//" + this->tableName + ".bin").c_str()); }

	/* Other methods */
	void DBTableBin5::AddRow(int index) 
	{
		cout << "������� " << this->GetTableName() << endl;
		cout << "ID ��������� ������: " << this->data.size() << endl;
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
			cerr << "������������ ���� ������" << endl;
			return;
		}
		this->data.push_back(row);
	}
	Row DBTableBin5::CreateRow() 
	{
		return Row();
	}
	int FieldPosInFile(int rowIndex, string columnName) 
	{
		return 0;
	}
	vector<int> DBTableBin5::IndexOfRecord(void* keyPtr, string keyName)
	{
		return vector<int>();
	};
	DBTable5* DBTableBin5::SelfRows(string colName, Condition cond, void* value) 
	{
		return nullptr;
	}
	string DBTableBin5::valueToString(Row& row, string columnName) 
	{
		return string();
	}

	/* Operators */
	Row DBTableBin5::operator[](int index) 
	{
		return this->data[index];
	}

	void DBTableBin5::CreateTableMaket(Strip*& strips, int& nStrip, int screenWidth)
	{
		Header::iterator headerIter, contHeaderIter;
		int nColumn = (int)columnHeaders.size();
		//���������� ������� ������ ����� ��� ������ ������� 
		int* fieldW = new int[nColumn];
		headerIter = columnHeaders.begin();
		for (int k = 0; k < nColumn; k++) {
			//��� ����: ��������� ��� ������?
			fieldW[k] = (headerIter->second.length > headerIter->first.size()) ?
				headerIter->second.length + 2 : headerIter->first.size() + 2;
			headerIter++;
		}
		int currCol = 0;//���������� ����� ������� � �������
		nStrip = 1;//����� ����� � ���������� �������
		int sumWidth = 0;//��������� ������ �������� � ������
		int n = 0;//����� �������� � ������
		int buff[40] = { 0 };//���������� � ��������� ������ ��� ���������� 
		//�������� ����� �������� � ������ (n<40)
		for (currCol = 0; currCol < nColumn; currCol++) {
			if (fieldW[currCol] >= screenWidth - 1) {
				cout << "������ ������� " << currCol << " ������ ������ ������\n";
				cout << "������� ������ �����������" << endl;
				nStrip = 0;
				return;
			}
			sumWidth += fieldW[currCol];
			if ((sumWidth < screenWidth - 1) && (currCol < nColumn - 1)) {
				n++;
				continue;
			}
			if ((sumWidth >= screenWidth - 1)) {//����� �� ������� ������ 
				currCol--;
				buff[nStrip - 1] = n;
				nStrip++;
				n = 0;
				sumWidth = 0;
				continue;
			}
			if (currCol == nColumn - 1) {//��������� ������
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
}