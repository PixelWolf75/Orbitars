#include "ISQLDatabase.h"
using namespace CODE;
using namespace SQL;
using namespace std;



/*class SQLDatabaseController : public CODE::SQL::ISQLDatabase
{
public:

	SQLDatabaseController(char* szTitle);
	~SQLDatabaseController();

	sqlite3* sqlDB;

	// Inherited via ISQLDatabase
	virtual void SQL_CREATE_TABLE(TABLE_STRUCTURE, char*) override;

	virtual void SQL_WRITE(RECORD) override;

	virtual void SQL_DELETE(SEARCH_LOCATION) override;

	virtual void SQL_SEARCH(SEARCH_LOCATION) override;

};

std::unique_ptr<CODE::SQL::ISQLDatabase> CODE::SQL::SQLFactory::open(char* szDatabase_Name)
{
	return std::make_unique<SQLDatabaseController>(szDatabase_Name);
}

SQLDatabaseController::SQLDatabaseController(char* szTitle)
{
	int exit = 0;
	exit = sqlite3_open(szTitle, &sqlDB);
	
	//test with catch
}

SQLDatabaseController::~SQLDatabaseController()
{
	sqlite3_close(sqlDB);
}

void SQLDatabaseController::SQL_CREATE_TABLE(TABLE_STRUCTURE tbTable, char* szTableName)
{

}

void SQLDatabaseController::SQL_WRITE(RECORD)
{
}

void SQLDatabaseController::SQL_DELETE(SEARCH_LOCATION)
{
}

void SQLDatabaseController::SQL_SEARCH(SEARCH_LOCATION)
{
}

*/


CODE::SQL::IDBRowCollection::Iterator::Iterator(IDBRowCollection& _collection, int iIndex)
	: collection(collection)
	, iPos(iIndex)
{
}

const IDBRow& CODE::SQL::IDBRowCollection::Iterator::operator*()
{
	// TODO: insert return statement here
	return this->collection[this->iPos];
}

CODE::SQL::IDBRowCollection::Iterator& CODE::SQL::IDBRowCollection::Iterator::operator++()
{
	// TODO: insert return statement here
	this->iPos++;
	return *this;
}

bool CODE::SQL::IDBRowCollection::Iterator::operator!=(const Iterator& other) const
{
	return other.iPos != this->iPos;
}

IDBRowCollection&& CODE::SQL::IDB::execute(const char* sql, sqlite3* db,
	std::function<int(void* pContext, int ArgCount, char** ArgVariable, char** azcolName)> database_callback)
{
	struct context_wrapper {
		std::function<int(void* pContext, int ArgCount, char** ArgVariable, char** azcolName)> database_callback;
		void* pContext;
	};
	char* szErrMsg = 0; 
	context_wrapper context_wrapped = { database_callback, szErrMsg };
	
	int rc = sqlite3_exec(db, sql, [](void* pContext, int ArgCount, char** ArgVariable, char** azcolName) -> int {
		context_wrapper* pWrapped = (context_wrapper*)pContext;
		return pWrapped->database_callback(pWrapped->pContext, ArgCount, ArgVariable, azcolName);
	} , 0, (char**)&context_wrapped);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", szErrMsg);
		sqlite3_free(szErrMsg);
	}
	IDBRowCollection col;
	return move(col);
}

std::vector<IDBRow> CODE::SQL::IDBRowCollection::get_rows()
{
	return this->data;
}

IDBRow& CODE::SQL::IDBRowCollection::operator[](int i)
{
	// TODO: insert return statement here
	return this->data[i];
}

CODE::SQL::IDBRowCollection::Iterator CODE::SQL::IDBRowCollection::begin()
{
	return Iterator(*this, 0);
}

CODE::SQL::IDBRowCollection::Iterator CODE::SQL::IDBRowCollection::end()
{
	return Iterator(*this, this->data.size());
}

std::vector<IDBCol> CODE::SQL::IDBRow::get_columns()
{
	return this->data;
}

IDBCol& CODE::SQL::IDBRow::operator[](int i)
{
	// TODO: insert return statement here
	return this->data[i];
}

std::string CODE::SQL::IDBCol::get_string()
{
	string str = string(raw.begin(), raw.end());
	return str;
}

int CODE::SQL::IDBCol::get_int()
{
	string str = string(raw.begin(), raw.end());
	int i = stoi(str, nullptr, 10);
	return i;
}
