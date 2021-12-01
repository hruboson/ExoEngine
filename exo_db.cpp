#include "exo_db.h"

#include <stdexcept>
#include <iostream>
#include <string>
#include <array>
#include <utility>

using queryRow = std::vector<std::pair<std::string, std::string>>;
using queryData = std::vector<queryRow>;

namespace exo{
	queryData ExoDB::planetData; // no idea why i have to declare it in implementation ¯\_(ツ)_/¯ (https://stackoverflow.com/questions/6140537/c-vector-issue-lnk2001-unresolved-external-symbol-private-static)

	ExoDB::ExoDB() {
		int err = sqlite3_open(db_path, &db);
		if (err != SQLITE_OK) {
			throw std::runtime_error("Couldn't connect to SQLite database");
		}
		queryPlanetData();
	}
	ExoDB::~ExoDB() {
		sqlite3_close(db);
	}

	int ExoDB::callback(void* NotUsed, int argc, char** argv, char** azColName)
	{
		for (int i = 0; i < argc; i++) {
			// column name and value
			queryRow row;
			row.push_back(std::make_pair(azColName[i], argv[i]));
			planetData.push_back(row);
		}

		std::cout << std::endl;

		return 0;
	}

	int ExoDB::queryPlanetData(){
		sqlite3* DB;
		char* messageError;

		std::string sql = u8"SELECT planet_id, planet_name, planet_desc AS 'Popis', planet_type, planet_size AS `Poloměr`, planet_distance AS 'Vzdálenost' FROM planets;";

		int exit = sqlite3_open(db_path, &DB);
		/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here*/
		exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);

		if (exit != SQLITE_OK) {
			std::cerr << "Error in queryPlanetData function." << std::endl;
			sqlite3_free(messageError);
		}
		else
			std::cout << "Records selected Successfully!" << std::endl;

		sqlite3_close(DB);
		return 0;
	}
}