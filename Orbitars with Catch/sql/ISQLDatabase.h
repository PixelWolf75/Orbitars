#pragma once
#include "sqlite3.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>
//#include <map>
#include <variant>

namespace CODE {
	namespace SQL {

		/*namespace ENUMS {
			
			enum SQL_TYPES {
				INT,
				BOOLEAN,
				CHAR,
				REAL,
				TEXT,
				VARCHAR
			};

			enum SQL_PROPERTIES {
				PRIMARY_KEY_NOT_NULL,
				FOREIGN_KEY,
				NOT_NULL,
				CAN_BE_NULL
			};

			enum SQL_SEARCHS {
				SELECT,
				FROM,
				WHERE,
				ORDER_BY
			};

		}

		namespace STRUCTURES {
			struct TABLE_PROPERTIES {
						char* szName;
						ENUMS::SQL_TYPES data_type;
						ENUMS::SQL_PROPERTIES type_properties;
						union {
							int iData_Size;
							std::vector<char*> szCompund_key;
						};
					};

			struct LOCATION {
				ENUMS::SQL_SEARCHS Search_Type;
				std::vector<char*> szSearch_Method;
			};
		}
		

		typedef std::vector<char*> RECORD;
		typedef std::vector<STRUCTURES::TABLE_PROPERTIES> TABLE_STRUCTURE;
		typedef std::vector<STRUCTURES::LOCATION> SEARCH_LOCATION;

		class ISQLDatabase
		{
		public:
			//virtual ~ISQLDatabase()=0;
			virtual void SQL_CREATE_TABLE(TABLE_STRUCTURE, char*) = 0;
			virtual void SQL_WRITE(RECORD) = 0;
			virtual void SQL_DELETE(SEARCH_LOCATION) = 0;
			virtual void SQL_SEARCH(SEARCH_LOCATION) = 0;
		};

		class SQLFactory {
			static std::unique_ptr<ISQLDatabase> open(char * szDatabase_Name);

		};

		*/

		//typedef std::function<int(void * pContext, int ArgCount, char** ArgVariable, char** azcolName)> database_callback;

		class IDBCol {
		public:
			std::vector<char> raw;
			std::string get_string();
			int get_int();
		};
		class IDBRow {
		public:
			std::vector<IDBCol> get_columns();
			IDBCol& operator[](int i);
		private:
			std::vector<IDBCol> data;
		};
		class IDBRowCollection {
		public:
			std::vector<IDBRow> get_rows();
			IDBRow& operator[](int i);

			class Iterator {
			public:
				Iterator(IDBRowCollection& collection, int iIndex);
				const IDBRow& operator*(); // when the iterators is derefenced to a row
				Iterator& operator++(); // Makes the iterator point to the next iterator in the collection
				bool operator!=(const Iterator& other) const; //Compares the cuurent iterator to the other one and alters it if it isn't the end
			private:
				int iPos;
				IDBRowCollection& collection;
				

			};
			Iterator begin();
			Iterator end();
		private:
			std::vector<IDBRow> data;
		};
		class IDB {
		public:
			virtual IDBRowCollection&& execute(const char* sql, sqlite3* db, std::function<int(void* pContext, int ArgCount, char** ArgVariable, char** azcolName)> database_callback) = 0;
		};
	}
}

