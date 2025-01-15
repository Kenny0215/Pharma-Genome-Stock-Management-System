#ifndef DB_H
#define DB_H
#include <mysql/jdbc.h>
#include <string>

class DBConnection {


private:

	sql::Connection* connection;

public:

	DBConnection(); //Constructor
	~DBConnection(); //Destructor

	void prepareStatement(std::string query);

	sql::PreparedStatement* stmt;
	sql::ResultSet* res;

	void QueryStatement(); //no result
	void QueryResult(); //have result

	// Additional getters for direct access
	sql::PreparedStatement* getStatement() { return stmt; }
	sql::ResultSet* getResult() { return res; }

};	 
#endif