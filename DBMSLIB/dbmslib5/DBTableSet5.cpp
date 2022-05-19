#include "dbmslib5.h"

namespace dbmsLib5 {
	DBTableSet5::DBTableSet5(string name) {
		this->dbName = name;
	}

	void DBTableSet5::ReadDB5() {
		vector<string> NamesOfTables;

		string folderName = this->dbName;
		string DBTablesTxt = "DBTables.txt";

		ifstream FILE("../" + folderName + "/" + DBTablesTxt);
		string fileStr;
		getline(FILE, fileStr);

		const string delims = "|";

		string word;

		for (string::iterator iter = fileStr.begin(); iter != fileStr.end(); iter++)
		{
			if (delims.find(*iter) != string::npos)
			{
				*iter = '\n';
			}
		}
		FILE.close();

		istringstream istr(fileStr);
		while (getline(istr, fileStr))
		{
			istringstream tmp(fileStr);
			string word;
			tmp >> word;
			NamesOfTables.push_back(word);
		}

		this->initNewDB(NamesOfTables);
	}

	void DBTableSet5::PrintDB5(int screenwidth) {
		for (auto iter = this->db.begin(); iter != this->db.end(); ++iter)
		{
			iter->second->PrintTable5(screenwidth);
		}
	}

	string FileType(string dbName) {
		return "." + dbName.substr(dbName.length() - 3);
	}

	void DBTableSet5::WriteDB5() {
		for (auto iter = this->db.begin(); iter != this->db.end(); ++iter)
		{
			iter->second->WriteDBTable5(iter->first + FileType(this->dbName));
		}
	}

	int DBTableSet5::initNewDB(vector<string> tableNames) {
		string folderName = this->dbName;
		//string folderName = "LibraryTxt";
		string fileType = FileType(this->dbName);//
		//string type = "";

		this->db.clear();

		for (size_t i = 0; i < tableNames.size(); i++) {

			if (fileType == ".txt")
			{
				this->db[tableNames[i]] = new DBTableTxt5(tableNames[i]);
			}
			else {
				if (fileType == ".bin")
				{
					this->db[tableNames[i]] = new DBTableBin5(tableNames[i]);
				}
				else {
					cout << "ReadDB1:Ошибка имени БД " << tableNames[i] << endl;
					return -1;
				}
			}

			//полиморфный вызов виртуальной функции
			this->db[tableNames[i]]->ReadDBTable5("../" + folderName + "/" + tableNames[i] + fileType);
		}
		return 0;
	}

	map<string, DBTable5*> DBTableSet5::GetDB()
	{
		if (!this->db.size())
		{
			exit(EMPRTY_CLASS_FIELD);
		}
		return this->db;
	}
}

