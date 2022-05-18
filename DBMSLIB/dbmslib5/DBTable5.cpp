#include "dbmslib5.h"

namespace dbmsLib5 {
	void DBTable5::PrintTable5(int screenwidth) {
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
	const char* DBTable5::TypeName(DBType type) {
		return typeNames[type];
	}
	string DBTable5::valueToString(Row& row, string columnName) {
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
	void DBTable5::CreateTableMaket(Strip*& strips, int& nStrip, int screenWidth)
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
	DBType DBTable5::GetType(char* columnName)
	{
		return this->GetHeader()[columnName].colType;
	}
}