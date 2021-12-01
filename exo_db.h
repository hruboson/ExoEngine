#pragma once

#include <stdio.h>
#include <sqlite3.h>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>

using queryData = std::vector<std::vector<std::pair<std::string, std::string>>>;

namespace exo {


	class ExoDB {
	public:
		ExoDB();
		~ExoDB();

		static int callback(void* NotUsed, int argc, char** argv, char** azColName);

		static queryData planetData;
	private:
		sqlite3* db;

		const char* db_path = "sqlite/exo.db";

		// Query functions called in constructor
		int queryPlanetData();
		


	};

}