#include <mysql/jdbc.h>
#include "DB.h"
#include <iostream>
#include <conio.h>

using namespace sql;
using namespace std;

DBConnection::DBConnection() {
	try
	{
		mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();

		connection = driver->connect("localhost", "root", "abc123");
		connection->setSchema("pgsms");
		stmt = nullptr;
		res = nullptr;
	}
	catch (sql::SQLException& e)
	{
		if (e.getErrorCode() == 0) {
			system("cls");
			cout << "Unable to connect to database";
			_getch();
			exit(0);
			return;
		}
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line" << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		_getch();
	}
}

DBConnection::~DBConnection() {

	try {
		if (connection) {
			connection->close();
		}

		if (stmt) {
			stmt->close();
		}

		if (res) {
			res->close();
		}
	}

	catch (sql::SQLException& e) {

		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line" << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		_getch();

		}
	}

void DBConnection::prepareStatement(string query) {

	try {
		if (!connection) {
			throw sql::SQLException("Database connection is not initialized.");
		}

		stmt = connection->prepareStatement(query);

		if (!stmt) {
			throw sql::SQLException("Failed to initialize prepared statement.");
		}
	}
	catch (sql::SQLException& e) {
		cout << "SQL error in prepareStatement: " << e.what() << endl;
		cout << "Error code: " << e.getErrorCode() << " SQLState: " << e.getSQLState() << endl;
	}
}

void DBConnection::QueryStatement() {

	try {
		stmt->executeUpdate();
	}
	catch (sql::SQLException& e) {};
}

void DBConnection::QueryResult() {

	try {
		res = stmt->executeQuery();
	}
	catch (sql::SQLException& e) {
	}
}