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
		//cout << "../" + folderName + "/" + DBTablesTxt << endl;
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

		cout << "\tИмена таблиц БД:\n";
		for (size_t i = 0; i < NamesOfTables.size(); ++i)
		{
			cout << i + 1 << " " << NamesOfTables[i] << endl;
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
		//size_t type = 1;
		/*cout << "Chose file type: " << endl << "1 - bin." << endl << "2 - txt" << endl;
		cin >> type;
		if (!(type == 1 || type == 2))
		{
			cout << "Wrong file type. Setted default file type (bin)" << endl;
			type = 1;
		}*/
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

			if (fileType == ".Txt")
			{
				this->db[tableNames[i]] = new DBTableTxt5(tableNames[i]);
			}
			else {
				if (fileType == ".Bin")
				{
					this->db[tableNames[i]] = new DBTableBin5(tableNames[i]);
				}
				else {
					cout << "ReadDB1:Ошибка имени БД" << endl;
					return -1;
				}
			}

			//полиморфный вызов виртуальной функции
			cout << "../" + folderName + "/" + tableNames[i] + fileType << endl;
			this->db[tableNames[i]]->ReadDBTable5("../" + folderName + "/" + tableNames[i] + fileType);
		}
		return 0;
	}
}

