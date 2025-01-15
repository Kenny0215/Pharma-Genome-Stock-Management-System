#include "pharmacy.h"
#include "DB.h"
#include <iomanip>
#include <iostream>
#include <mysql/jdbc.h>
#include <stdexcept>
#include <conio.h>
#include <regex>

// Define color codes
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string CYAN = "\033[36m";
const string BLUE = "\033[34m";
const string WHITE_BOLD = "\033[1;37m";

using namespace std;

// Constructor
Admin::Admin(){}
Supplier::Supplier(){}

// Add an item to the stock
void Admin::addItem() {

    cout << "\n\n*----------------------------------------------------------*\n";
    cout << "|                                                          |\n";
    cout << "|                     ADD NEW STOCK ITEM                   |\n";
    cout << "|                                                          |\n";
    cout << "*----------------------------------------------------------*\n";
    cout << "*----------------------------------------------------------*\n";

    string ID, name, expiryDate, newID,category,description,status;
    int quantity, Categorychoice;
    double price;

    db.prepareStatement("SELECT stock_ID FROM stock ORDER BY stock_ID DESC");
    db.QueryResult();

    if (db.res->next()) {

        string latestStockID = db.res->getString("stock_ID");

        int numericPart = 0;
        for (int i = 1; i < latestStockID.length(); i++) {
            numericPart = numericPart * 10 + (latestStockID[i] - '0');
        }

        // Increment the numeric part
        numericPart++;

        // Build the new ID as "S" + zero-padded number
        newID = "S";

        if (numericPart < 10) {
            newID += "00"; // Pad with two zeros for numbers < 10
        }
        else if (numericPart < 100) {
            newID += "0";  // Pad with one zero for numbers < 100
        }
        newID += to_string(numericPart); // Append the incremented numeric part
    }
    else //if no data
    {
        newID = "S001";
    }

    cin.ignore();

    // Validate stock name
    while (true) {
        cout << "\n- Enter item name: ";
        getline(cin, name);

        // Check if stock name is empty
        if (name.empty()) {
            cout << YELLOW << "Stock name cannot be empty. Please try again.\n\n" << RESET;
            continue;
        }

        // Check if stock name is numeric
        bool isNumeric = true;
        for (char c : name) {
            if (!isdigit(c)) {
                isNumeric = false;
                break;
            }
        }

        if (isNumeric) {
            cout << YELLOW << "Stock name cannot be numeric. Please enter a valid name.\n\n" << RESET;
            continue;
        }

        // Valid stock name
        break;
    }

    cout << endl;

    while (true) {
        try {
            // Error handling for quantity input
            string input;

            cout << "\n- Enter quantity: ";
            cin >> input;

            // Attempt to convert the input to an integer
            size_t pos;
            quantity = stoi(input, &pos);

            // Check if the entire string was consumed
            if (pos != input.length()) {
                throw invalid_argument("Extra characters found");
            }

            // Check if quantity is negative
            if (quantity < 0) {
                throw out_of_range("Quantity cannot be negative");
            }

            // Check if quantity exceeds the maximum allowed limit
            const int MAX_QUANTITY = 100000;  // Define a maximum quantity limit
            if (quantity > MAX_QUANTITY) {
                throw out_of_range("Quantity exceeds the maximum allowed limit");
            }

            break;  // Exit the loop if input is valid
        }
        catch (const invalid_argument& e) {
            cout << YELLOW << "Invalid input. Please enter an integer to proceed.\n\n" << RESET;
        }
        catch (const out_of_range& e) {
            cout << YELLOW << "Error: " << e.what() << ". Please enter a valid quantity" << RESET;
        }
    }


    while (true) {

        cout << "\n- Enter price: RM";

        if (cin >> price && price >= 0) {
            break;
        }
        else {
            cout << YELLOW << "Invalid input. Please enter a non-negative value for price.\n" << RESET;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    cout << "\n- Enter expiry date (YYYY-MM-DD): ";
    cin >> expiryDate;

    cout << "\n- Enter category ID: "; 
    cin >> category; 
    cin.ignore();

    // Insert stock item with the selected category ID
    try {

        DBConnection dbConn;
        dbConn.prepareStatement("INSERT INTO stock (stock_ID,stock_Name, stock_Quantity, stock_Price, stock_ExpiryDate,category_id) VALUES (?, ?, ?, ?, ?, ?)");

        dbConn.stmt->setString(1, newID);
        dbConn.stmt->setString(2, name);
        dbConn.stmt->setInt(3, quantity);
        dbConn.stmt->setDouble(4, price);
        dbConn.stmt->setString(5, expiryDate);
        dbConn.stmt->setString(6, category);

        dbConn.stmt->executeUpdate();

        cout << GREEN << "\nItem added successfully!" << endl << RESET;
    }
    catch (sql::SQLException& e) {
        cout << RED << "\nSQL error while inserting stock item: " << e.what() << endl << RESET;
    }

    cout << "*----------------------------------------------------------*\n";
    _getch();
}

// Remove an item by name
void Admin::removeItem() {


    cout << "\n\n*----------------------------------------------------------*\n";
    cout << "|                                                          |\n";
    cout << "|                     REMOVE STOCK ITEM                    |\n";
    cout << "|                                                          |\n";
    cout << "*----------------------------------------------------------*\n";
    cout << "*----------------------------------------------------------*\n";

    string ID;

    while (true) {

        cin.ignore();
        cout << "\n- Enter stock ID (S001): ";
        getline(cin, ID);

        // Check if stock ID is empty
        if (ID.empty()) {
            cout << YELLOW << "Stock ID cannot be empty. Please enter a valid ID.\n" << RESET;
            continue;
        }

        // Check if stock ID is a numeric value
        if (std::all_of(ID.begin(), ID.end(), ::isdigit)) {
            cout << YELLOW << "Stock ID cannot be numeric. Please enter a valid ID.\n" << RESET;
            continue;
        }


        // Validate stock ID format using regex (e.g., starts with a letter followed by digits)
        regex idPattern("^[A-Za-z]\\d{3}$");
        if (!std::regex_match(ID, idPattern)) {

            cout << YELLOW << "Invalid stock ID format. It should be like S001\n" << RESET;
            continue;
        }

        try {

            DBConnection dbConn;
            dbConn.prepareStatement("DELETE FROM stock WHERE stock_ID = ?");
            dbConn.stmt->setString(1, ID);

            // Execute deletion
            int rowsAffected = dbConn.stmt->executeUpdate();

            if (rowsAffected > 0) {

                cout << GREEN << "[" << ID << "] has been removed successfully!" << endl << RESET;
                break;
            }
            else {

                cout << YELLOW << "Stock ID is not found! Please enter a valid ID.\n" << endl << RESET;

            }
        }

        catch (sql::SQLException& e) {

            cout << RED << "SQL error: " << e.what() << endl << RESET;
        }
    }

    cout << "*----------------------------------------------------------*\n";
    _getch();
}

// Modify functions
// Modify an item's name
void Admin::modifyItemName() {


        cout << "\n\n*-------------------- MODIFY ITEM NAME --------------------*\n";

        string ID, newName;

        while (true) {

            cin.ignore();
            cout << "\n- Enter stock ID (S001): ";
            getline(cin, ID);

            // Check if stock ID is empty
            if (ID.empty()) {

                cout << YELLOW << "Stock ID cannot be empty. Please enter a valid ID.\n\n" << RESET;
                continue;
            }

            // Check if stock ID is a numeric value
            if (std::all_of(ID.begin(), ID.end(), ::isdigit)) {
                cout << YELLOW << "Stock ID cannot be numeric. Please enter a valid ID.\n\n" << RESET;
                continue;
            }

            // Validate stock ID format using regex (e.g., starts with a letter followed by digits)
            regex idPattern("^[A-Za-z]\\d{3}$");
            if (!std::regex_match(ID, idPattern)) {
                cout << YELLOW << "Invalid stock ID format. It should be like S001.\n" << RESET;
                continue;
            }

            // Check if the stock ID exists in the database
            try {
                DBConnection dbConn;

                dbConn.prepareStatement("SELECT stock_ID FROM stock WHERE stock_ID = ?");
                dbConn.stmt->setString(1, ID);
                dbConn.QueryResult();

                if (!dbConn.res->next()) {
                    cout << YELLOW << "Stock item with ID [" << ID << "] does not exist in the database.\n" << RESET;
                    continue;
                }

            }
            catch (sql::SQLException& e) {
                cout << RED << "SQL error: " << e.what() << endl << RESET;
                continue;
            }
            break;
        }

        cout << "\n- Enter new item name: ";
        getline(cin, newName);

        try {
            DBConnection dbConn;
            dbConn.prepareStatement("UPDATE stock SET stock_Name = ? WHERE stock_ID = ?");
            dbConn.stmt->setString(1, newName);
            dbConn.stmt->setString(2, ID);
            int rowsAffected = dbConn.stmt->executeUpdate();
            if (rowsAffected > 0) {
                cout << GREEN << "\nStock item name updated successfully!" << endl << RESET;
            }
            else {
                cout << YELLOW << "\nStock item not found!" << endl << RESET;
            }
        }
        catch (sql::SQLException& e) {
            cout << RED << "\nSQL error: " << e.what() << endl << RESET;
        }
        cout << "*----------------------------------------------------------*\n";
        _getch();
    }

// Modify an item's quantity
void Admin::modifyItemQuantity() {

  
        cout << "\n\n*---------------- MODIFY ITEM QUANTITY ----------------*\n";

        string ID,status;
        int newQuantity;

        while (true) {

            cin.ignore();
            cout << "\n- Enter stock ID (S001): ";
            getline(cin, ID);

            // Check if stock ID is empty
            if (ID.empty()) {

                cout << YELLOW << "Stock ID cannot be empty. Please enter a valid ID.\n\n" << RESET;
                continue;
            }

            // Check if stock ID is a numeric value
            if (std::all_of(ID.begin(), ID.end(), ::isdigit)) {
                cout << YELLOW << "Stock ID cannot be numeric. Please enter a valid ID.\n\n" << RESET;
                continue;
            }

            // Validate stock ID format using regex
            regex idPattern("^[A-Za-z]\\d{3}$");
            if (!std::regex_match(ID, idPattern)) {
                cout << YELLOW << "Invalid stock ID format. It should be like S001.\n" << RESET;
                continue;
            }

            // Check if the stock ID exists in the database
            try {
                DBConnection dbConn;

                dbConn.prepareStatement("SELECT stock_ID FROM stock WHERE stock_ID = ?");
                dbConn.stmt->setString(1, ID);
                dbConn.QueryResult();

                if (!dbConn.res->next()) {
                    cout << YELLOW << "Stock item with ID [" << ID << "] does not exist in the database.\n" << RESET;
                    continue;
                }

            }
            catch (sql::SQLException& e) {
                cout << RED << "SQL error: " << e.what() << endl << RESET;
                continue;
            }
            break;
        }


        while (true) {
            try {
                string input;
                cout << "\n- Enter new quantity: ";
                cin >> input;

                size_t pos;
                newQuantity = stoi(input, &pos);

                if (pos != input.length()) {
                    throw invalid_argument("Extra characters found");
                }
                if (newQuantity < 0) {
                    throw out_of_range("Quantity cannot be negative");
                }
                break;
            }
            catch (const invalid_argument& e) {
                cout << YELLOW << "Invalid input. Please enter an integer to proceed.\n\n" << RESET;
            }
            catch (const out_of_range& e) {
                cout << YELLOW << "Quantity cannot be negative. Please try again.\n\n" << RESET;
            }
        }

        try {
            DBConnection dbConn;
            dbConn.prepareStatement("UPDATE stock SET stock_Quantity = ? WHERE stock_ID = ?");
            dbConn.stmt->setInt(1, newQuantity);
            dbConn.stmt->setString(2, ID);

            int rowsAffected = dbConn.stmt->executeUpdate();

            if (rowsAffected > 0) {
                cout << GREEN << "\nStock item quantity updated successfully!" << endl << RESET;
            }
            else {
                cout << CYAN << "\nStock item not found!" << endl << RESET;
            }
        }
        catch (sql::SQLException& e) {
            cout << RED << "\nSQL error: " << e.what() << endl << RESET;
        }
        cout << "*----------------------------------------------------------*\n";
        _getch();
    }

// Modify an item's price
void Admin::modifyItemPrice() {
   
        cout << "\n\n*------------------ MODIFY ITEM PRICE ------------------*\n";

        string ID;
        double newPrice;

        while (true) {

            cin.ignore();
            cout << "\n- Enter stock ID (S001): ";
            getline(cin, ID);

            // Check if stock ID is empty
            if (ID.empty()) {

                cout << "Stock ID cannot be empty. Please enter a valid ID.\n\n";
                continue;
            }

            // Check if stock ID is a numeric value
            if (std::all_of(ID.begin(), ID.end(), ::isdigit)) {
                cout << "Stock ID cannot be numeric. Please enter a valid ID.\n\n";
                continue;
            }

            // Validate stock ID format using regex (e.g., starts with a letter followed by digits)
            regex idPattern("^[A-Za-z]\\d{3}$");
            if (!std::regex_match(ID, idPattern)) {
                cout << "Invalid stock ID format. It should be like S001.\n";
                continue;
            }

            // Check if the stock ID exists in the database
            try {
                DBConnection dbConn;

                dbConn.prepareStatement("SELECT stock_ID FROM stock WHERE stock_ID = ?");
                dbConn.stmt->setString(1, ID);
                dbConn.QueryResult();

                if (!dbConn.res->next()) {
                    cout << "Stock item with ID [" << ID << "] does not exist in the database.\n";
                    continue;
                }

            }
            catch (sql::SQLException& e) {
                cout << "SQL error: " << e.what() << endl;
                continue;
            }
            break;
        }


        while (true) {
            cout << "\n- Enter new price: RM";
            if (cin >> newPrice && newPrice >= 0) {
                break;
            }
            else {
                cout << "Invalid input. Please enter a non-negative value for price.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }

        try {
            DBConnection dbConn;
            dbConn.prepareStatement("UPDATE stock SET stock_Price = ? WHERE stock_ID = ?");
            dbConn.stmt->setDouble(1, newPrice);
            dbConn.stmt->setString(2, ID);
            int rowsAffected = dbConn.stmt->executeUpdate();
            if (rowsAffected > 0) {
                cout << "\nStock item price updated successfully!" << endl;
            }
            else {
                cout << "\nStock item not found!" << endl;
            }
        }
        catch (sql::SQLException& e) {
            cout << "\nSQL error: " << e.what() << endl;
        }
        cout << "*----------------------------------------------------------*\n";
        _getch();
    }

// Modify an item's expiry date
void Admin::modifyItemExpiryDate() {

        cout << "\n\n*---------------- MODIFY ITEM EXPIRY DATE ----------------*\n";

        string ID, newExpiryDate;

        while (true) {

            cin.ignore();
            cout << "\n- Enter stock ID (S001): ";
            getline(cin, ID);

            // Check if stock ID is empty
            if (ID.empty()) {

                cout << YELLOW << "Stock ID cannot be empty. Please enter a valid ID.\n\n" << RESET;
                continue;
            }

            // Check if stock ID is a numeric value
            if (std::all_of(ID.begin(), ID.end(), ::isdigit)) {
                cout << YELLOW << "Stock ID cannot be numeric. Please enter a valid ID.\n\n" << RESET;
                continue;
            }

            // Validate stock ID format using regex (e.g., starts with a letter followed by digits)
            regex idPattern("^[A-Za-z]\\d{3}$");
            if (!std::regex_match(ID, idPattern)) {
                cout << YELLOW << "Invalid stock ID format. It should be like S001.\n" << RESET;
                continue;
            }

            // Check if the stock ID exists in the database
            try {
                DBConnection dbConn;

                dbConn.prepareStatement("SELECT stock_ID FROM stock WHERE stock_ID = ?");
                dbConn.stmt->setString(1, ID);
                dbConn.QueryResult();

                if (!dbConn.res->next()) {
                    cout << YELLOW << "Stock item with ID [" << ID << "] does not exist in the database.\n" << RESET;
                    continue;
                }

            }
            catch (sql::SQLException& e) {
                cout << RED << "SQL error: " << e.what() << endl << RESET;
                continue;
            }
            break;
        }

            cout << "\n- Enter new expiry date (YYYY-MM-DD): ";
            cin >> newExpiryDate;

            try {
                DBConnection dbConn;

                dbConn.prepareStatement("UPDATE stock SET stock_ExpiryDate = ? WHERE stock_ID = ?");
                dbConn.stmt->setString(1, newExpiryDate);
                dbConn.stmt->setString(2, ID);

                int rowsAffected = dbConn.stmt->executeUpdate();

                if (rowsAffected > 0) {
                    cout << GREEN << "\nStock item expiry date updated successfully!" << endl << RESET;
                }
                else {
                    cout << YELLOW << "\nStock item not found!" << endl << RESET;
                }
            }
            catch (sql::SQLException& e) {
                cout << RED << "\nSQL error: " << e.what() << endl << RESET;
            }
            cout << "*----------------------------------------------------------*\n";
            _getch();
}

// Modify an item's category
void Admin::modifyItemCategoryID() {

        cout << "\n\n*---------------- MODIFY ITEM CATEGORY ID ----------------*\n";

        string ID, newCategoryID;

        cout << "\n- Enter stock ID (S001): ";
        cin >> ID;
        cin.ignore();

        // Code to validate and check the stock ID...

        cout << "\n- Enter new category ID: ";
        cin >> newCategoryID;
        cin.ignore();

        // Check if the new category ID exists
        try {
            DBConnection dbConn;
            dbConn.prepareStatement("SELECT category_id FROM category WHERE category_id = ?");
            dbConn.stmt->setString(1, newCategoryID);
            dbConn.QueryResult();

            if (!dbConn.res->next()) {
                cout << YELLOW << "Category ID [" << newCategoryID << "] does not exist in the database.\n" << RESET;
                return;
            }
        }
        catch (sql::SQLException& e) {
            cout << RED << "\nSQL error: " << e.what() << endl << RESET;
            return;
        }

        // Update the stock category ID
        try {
            DBConnection dbConn;
            dbConn.prepareStatement("UPDATE stock SET category_id = ? WHERE stock_ID = ?");
            dbConn.stmt->setString(1, newCategoryID);
            dbConn.stmt->setString(2, ID);
            int rowsAffected = dbConn.stmt->executeUpdate();
            if (rowsAffected > 0) {
                cout << GREEN << "\nStock item category ID updated successfully!" << endl << RESET;
            }
            else {
                cout << YELLOW << "\nStock item not found!" << endl << RESET;
            }
        }
        catch (sql::SQLException& e) {
            cout << RED << "\nSQL error: " << e.what() << endl << RESET;
        }

        cout << "*----------------------------------------------------------*\n";
        _getch();
    }

// Display all items in the stock
void Admin::displayItems() {
    cout << "\n\n+-----------------------------------------------------------------------------------------------------------------+\n";
    cout << "|                                                                                                                 |\n";
    cout << "|                                                    DISPLAY STOCK ITEMS                                          |\n";
    cout << "|                                                                                                                 |\n";
    cout << "+-----------------------------------------------------------------------------------------------------------------+\n";

    try {
        DBConnection dbConn;
        db.prepareStatement("SELECT stock_ID, stock_Name, stock_Quantity, stock_Price, stock_ExpiryDate, category_id, category_name, category_description "
            "FROM stock JOIN category USING (category_id)"
            "ORDER BY stock_ID");
        db.QueryResult();

        if (db.res->next()) {
            cout << "+----------+--------------+----------+------------+----------------+--------------+---------------+--------------------------------+\n";
            cout << "| Stock ID | Stock Name   | Quantity | Price (RM) | Expiry Date    | Category ID  | Category      | Description                    |\n";
            cout << "+----------+--------------+----------+------------+----------------+--------------+---------------+--------------------------------+\n";

            do {
                string stockID = db.res->getString("stock_ID");
                string stockName = db.res->getString("stock_Name");
                int quantity = db.res->getInt("stock_Quantity");
                double price = db.res->getDouble("stock_Price");
                string expiryDate = db.res->getString("stock_ExpiryDate");
                string categoryID = db.res->getString("category_id");
                string categoryName = db.res->getString("category_name");
                string categoryDescription = db.res->getString("category_description");

                cout << "| " << setw(9) << left << stockID
                    << "| " << setw(13) << left << stockName
                    << "| " << setw(9) << left << quantity
                    << "| " << setw(11) << left << fixed << setprecision(2) << price
                    << "| " << setw(15) << left << expiryDate
                    << "| " << setw(13) << left << categoryID
                    << "| " << setw(14) << left << categoryName
                    << "| " << setw(31) << left << categoryDescription
                    << "|\n";

            } while (db.res->next());

            cout << "+----------+--------------+----------+------------+----------------+--------------+---------------+--------------------------------+\n";
        }
        else {
            cout << YELLOW << "\n* No stock items are currently available in the database. *\n" << RESET;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\n* SQL Error: " << e.what() << " *\n" << RESET;
    }
}

// Search for an item by name
void Admin::searchItem() {

    system("cls");
    cin.ignore();
    cin.clear();

    cout << "\n\n+------------------------------------------------------------------------------------------------------------------+\n";
    cout << "|                                                                                                                  |\n";
    cout << "|                                                      SEARCH STOCK ITEM                                           |\n";
    cout << "|                                                                                                                  |\n";
    cout << "+------------------------------------------------------------------------------------------------------------------+\n";

    string searchName;

    cout << "- Enter item name to search: ";
    getline(cin, searchName);

    cout << endl;

    try {
        DBConnection dbConn;
        dbConn.prepareStatement(
            "SELECT stock_ID, stock_Name, stock_Quantity, stock_Price, stock_ExpiryDate, category_name, category_description "
            "FROM stock "
            "JOIN category USING (category_ID) "
            "WHERE stock_Name LIKE ?");

        // Add wildcard (%) for partial matching
        dbConn.stmt->setString(1, "%" + searchName + "%");

        unique_ptr<sql::ResultSet> resultSet(dbConn.stmt->executeQuery());

        if (resultSet->next()) {
            cout << "+----------+--------------+----------+------------+----------------+------------------+--------------------------------+\n";
            cout << "| Stock ID | Stock Name   | Quantity | Price (RM) | Expiry Date    | Category         | Description                    |\n";
            cout << "+----------+--------------+----------+------------+----------------+------------------+--------------------------------+\n";

            do {
                string stockID = resultSet->getString("stock_ID");
                string stockName = resultSet->getString("stock_Name");
                int quantity = resultSet->getInt("stock_Quantity");
                double price = resultSet->getDouble("stock_Price");
                string expiryDate = resultSet->getString("stock_ExpiryDate");
                string categoryName = resultSet->getString("category_name");
                string categoryDescription = resultSet->getString("category_description");

                cout << "| " << setw(8) << left << stockID
                    << " | " << setw(12) << left << stockName
                    << " | " << setw(8) << left << quantity
                    << " | " << setw(10) << left << fixed << setprecision(2) << price
                    << " | " << setw(14) << left << expiryDate
                    << " | " << setw(16) << left << categoryName
                    << " | " << setw(30) << left << categoryDescription
                    << " |\n";

            } while (resultSet->next());

            cout << "+----------+--------------+----------+------------+----------------+------------------+--------------------------------+\n";;
        }
        else {
            cout << YELLOW << "\n* No stock items found matching the search criteria. *\n" << RESET;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\n* SQL Error: " << e.what() << " *\n" << RESET;
    }

    cout << "\nPress any key to return to the main menu...";
    _getch();
}

// Receive stock from restock
void Admin::receiveStock() {
    cout << "\n\n*-----------------------------------------------*\n";
    cout << "|                                               |\n";
    cout << "|                  RECEIVE STOCK                |\n";
    cout << "|                                               |\n";
    cout << "*-----------------------------------------------*\n";

    string stockID, supplierID;

    cout << "\n- Enter stock ID (S001): ";
    cin.ignore();
    getline(cin, stockID);

    cout << "\n- Enter supplier ID (SP001): ";
    getline(cin, supplierID);

    try {
        DBConnection dbConn;

        // Check for pending transactions for the given stock ID and supplier ID
        dbConn.prepareStatement(
            "SELECT SUM(quantity_supply) AS total_quantity, COUNT(*) AS pending_count, "
            "MAX(status = 'received') AS already_received "
            "FROM transaction "
            "WHERE stock_ID = ? AND supplier_id = ?"
        );
        dbConn.stmt->setString(1, stockID);
        dbConn.stmt->setString(2, supplierID);
        dbConn.QueryResult();

        if (dbConn.res->next()) {
            int totalQuantity = dbConn.res->getInt("total_quantity");
            int pendingCount = dbConn.res->getInt("pending_count");
            bool alreadyReceived = dbConn.res->getInt("already_received");

            // Check if there are no pending restock requests
            if (alreadyReceived) {
                cout << RED << "\nCannot receive this stock because it has already been received for the given Stock ID and Supplier ID." << RESET << endl;
                return;
            }

            if (pendingCount == 0) {
                cout << YELLOW << "\nNo pending restock requests found for the given Stock ID and Supplier ID." << RESET << endl;
                return;
            }

            // Update stock quantity
            dbConn.prepareStatement("UPDATE stock SET stock_quantity = stock_quantity + ? WHERE stock_ID = ?");
            dbConn.stmt->setInt(1, totalQuantity);
            dbConn.stmt->setString(2, stockID);
            dbConn.stmt->executeUpdate();

            // Mark all pending transactions as received
            dbConn.prepareStatement("UPDATE transaction SET status = 'received' WHERE stock_ID = ? AND supplier_id = ? AND status = 'pending'");
            dbConn.stmt->setString(1, stockID);
            dbConn.stmt->setString(2, supplierID);
            dbConn.stmt->executeUpdate();

            cout << GREEN << "\nStock received successfully! Total quantity added: " << totalQuantity << RESET << endl;
        }
        else {
            cout << YELLOW << "\nNo pending restock requests found for the given Stock ID and Supplier ID." << RESET << endl;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\nSQL error while receiving stock: " << e.what() << RESET << endl;
    }

    cout << "*------------------------------------------------------*\n";
    _getch();
}

// Return stock
void Admin::returnStock() {
    cout << "\n\n*-----------------------------------------------------------------*\n";
    cout << "|                                                                 |\n";
    cout << "|                          RETURN STOCK                           |\n";
    cout << "|                                                                 |\n";
    cout << "*-----------------------------------------------------------------*\n";

    string stockID, supplierID, supplyDate;
    int quantityReturn, newTransactionID;

    cout << "\n- Enter stock ID (S001): ";
    cin.ignore();
    getline(cin, stockID);

    cout << "\n- Enter supplier ID (SP001): ";
    getline(cin, supplierID);

    cout << "\n- Enter quantity to restock: ";
    while (!(cin >> quantityReturn) || quantityReturn <= 0) {
        cout << YELLOW << "Invalid input. Please enter a positive integer: " << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "\n- Enter supply date (YYYY-MM-DD): ";
    cin.ignore();
    getline(cin, supplyDate);

    try {
        DBConnection dbConn;


        // Retrieve the highest current transaction ID
        dbConn.prepareStatement("SELECT MAX(transaction_id) AS maxID FROM transaction");
        dbConn.QueryResult();

        if (dbConn.res->next()) {
            newTransactionID = dbConn.res->getInt("maxID") + 1;  // Increment by 1
        }
        else {
            newTransactionID = 1;  // Start with 1 if no records exist
        }

        // Check stock availability
        dbConn.prepareStatement("SELECT stock_quantity FROM stock WHERE stock_ID = ?");
        dbConn.stmt->setString(1, stockID);
        dbConn.QueryResult();

        if (dbConn.res->next()) {
            int availableQuantity = dbConn.res->getInt("stock_quantity");
            if (availableQuantity < quantityReturn) {
                cout << RED << "\nError: Insufficient stock for return." << RESET << endl;
                return;
            }
        }

        // Insert a return transaction
        dbConn.prepareStatement(
            "INSERT INTO transaction (transaction_id, stock_ID, supplier_id, quantity_return, return_date, status) "
            "VALUES (?, ?, ?, ?, ?, 'pending')"
        );
        dbConn.stmt->setInt(1, newTransactionID);
        dbConn.stmt->setString(2, stockID);
        dbConn.stmt->setString(3, supplierID);
        dbConn.stmt->setInt(4, quantityReturn);
        dbConn.stmt->setString(5, supplyDate);
        dbConn.stmt->executeUpdate();

        cout << GREEN << "\nNew stock record added successfully!" << endl << RESET;

        // Update the stock quantity in the main stock table
        dbConn.prepareStatement("UPDATE stock SET stock_quantity = stock_quantity - ? WHERE stock_ID = ?");
        dbConn.stmt->setInt(1, quantityReturn);
        dbConn.stmt->setString(2, stockID);
        dbConn.stmt->executeUpdate();
    }
    catch (sql::SQLException& e) {
        cout << RED << "\nSQL error while processing return stock: " << e.what() << endl << RESET;
    }

    cout << "*-----------------------------------------------------------------*\n";
    _getch();
}

// View stock report for all items
void Admin::viewStockReport() {
    system("cls");

    // Define color codes
    const string RESET = "\033[0m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string CYAN = "\033[36m";
    const string BLUE = "\033[34m";
    const string WHITE_BOLD = "\033[1;37m";

    cout << CYAN << "*-------------------------------------------------------------------------------------------------------------*" << RESET << "\n";
    cout << CYAN << "|                                                                                                             |" << RESET << "\n";
    cout << CYAN << "|                                                  STOCK REPORT                                               |" << RESET << "\n";
    cout << CYAN << "|                                                                                                             |" << RESET << "\n";
    cout << CYAN << "*-------------------------------------------------------------------------------------------------------------*" << RESET << "\n";

    try {
        DBConnection dbConn;

        // Query to fetch stock details
        dbConn.prepareStatement(
            "SELECT transaction_id,stock_ID, supplier_id, stock_Name, stock_Quantity, stock_Price, quantity_supply, supply_date, quantity_return, return_date, status "
            "FROM stock JOIN transaction USING(stock_ID)"
        );

        unique_ptr<sql::ResultSet> resultSet(dbConn.stmt->executeQuery());

        // Print the stock details
        cout << endl;
        cout << GREEN << "*--------------------------------------------------------------------------------------------------------------------------*\n";
        cout << "| " << WHITE_BOLD << setw(10) << left << "Trans ID" << GREEN << " | "
            << WHITE_BOLD << setw(7) << "StockID" << GREEN << " | "
            << WHITE_BOLD << setw(7) << "SuppID" << GREEN << " | "
            << WHITE_BOLD << setw(12) << "Name" << GREEN << " | "
            << WHITE_BOLD << setw(5) << "Qty" << GREEN << " | "
            << WHITE_BOLD << setw(7) << "Price" << GREEN << " | "
            << WHITE_BOLD << setw(7) << "SupQty" << GREEN << " | "
            << WHITE_BOLD << setw(10) << "SupDate" << GREEN << " | "
            << WHITE_BOLD << setw(7) << "RetQty" << GREEN << " | "
            << WHITE_BOLD << setw(10) << "RetDate" << GREEN << " | "
            << WHITE_BOLD << setw(8) << "Status" << GREEN << " |\n";
        cout << "*--------------------------------------------------------------------------------------------------------------------------*" << RESET << "\n";

        bool hasData = false;
        double totalStockPrice = 0.0;
        int totalStockQuantity = 0, totalSupplyQty = 0, totalReturnQty = 0;

        string highestStockName, lowestStockName;
        double highestStockPrice = -1, lowestStockPrice = DBL_MAX;
        int highestStockQty = -1, lowestStockQty = INT_MAX;
        int highestSupplyQty = -1, lowestSupplyQty = INT_MAX;
        int highestReturnQty = -1, lowestReturnQty = INT_MAX;

        // Loop through the result set
        while (resultSet->next()) {
            hasData = true;

            int transactionID = resultSet->getInt("transaction_id");
            string stockID = resultSet->getString("stock_ID");
            string supplierID = resultSet->getString("supplier_id");
            string stockName = resultSet->getString("stock_Name");
            int stockQuantity = resultSet->getInt("stock_Quantity");
            double stockPrice = resultSet->getDouble("stock_Price");
            int quantitySupply = resultSet->getInt("quantity_supply");
            string supplyDate = resultSet->getString("supply_date");
            int quantityReturn = resultSet->getInt("quantity_return");
            string returnDate = resultSet->getString("return_date");
            string status = resultSet->getString("status");

            // Add to totals
            totalStockPrice += stockPrice * stockQuantity;
            totalStockQuantity += stockQuantity;
            totalSupplyQty += quantitySupply;
            totalReturnQty += quantityReturn;

            // Check for highest and lowest
            if (stockPrice * quantitySupply > highestStockPrice) {
                highestStockPrice = stockPrice * quantitySupply;
                highestStockName = stockName;
            }
            if (stockPrice * quantitySupply < lowestStockPrice) {
                lowestStockPrice = stockPrice * quantitySupply;
                lowestStockName = stockName;
            }
            if (stockQuantity > highestStockQty) {
                highestStockQty = stockQuantity;
                highestStockName = stockName;
            }
            if (stockQuantity < lowestStockQty) {
                lowestStockQty = stockQuantity;
                lowestStockName = stockName;
            }
            if (quantitySupply > highestSupplyQty) {
                highestSupplyQty = quantitySupply;
                highestStockName = stockName;
            }
            if (quantitySupply < lowestSupplyQty) {
                lowestSupplyQty = quantitySupply;
                lowestStockName = stockName;
            }
            if (quantityReturn > highestReturnQty) {
                highestReturnQty = quantityReturn;
                highestStockName = stockName;
            }
            if (quantityReturn < lowestReturnQty) {
                lowestReturnQty = quantityReturn;
                lowestStockName = stockName;
            }

            // Print row
            cout << YELLOW << "| " << setw(10) << left << transactionID
                << " | " << setw(7) << left << stockID
                << " | " << setw(7) << left << supplierID
                << " | " << setw(12) << left << stockName
                << " | " << setw(5) << left << stockQuantity
                << " | " << setw(7) << left << fixed << setprecision(2) << stockPrice
                << " | " << setw(7) << left << quantitySupply
                << " | " << setw(10) << left << supplyDate
                << " | " << setw(7) << left << quantityReturn
                << " | " << setw(10) << left << returnDate
                << " | " << setw(9) << left << status << "|\n";
        }

        cout << GREEN << "*--------------------------------------------------------------------------------------------------------------------------*" << RESET << "\n";

        if (hasData){

            // Display summary
            cout << "\n" << WHITE_BOLD << "Summary:\n" << RESET;
            cout << GREEN << "- Total Stock Price: RM " << fixed << setprecision(2) << totalStockPrice << "\n";
            cout << "- Total Stock Quantity: " << totalStockQuantity << "\n";
            cout << "- Total Supply Quantity: " << totalSupplyQty << "\n";
            cout << "- Total Return Quantity: " << totalReturnQty << "\n" << RESET;

            cout << "\n" << YELLOW << "- Highest Stock Price: " << highestStockPrice << " (" << highestStockName << ")\n";
            cout << "- Lowest Stock Price: " << lowestStockPrice << " (" << lowestStockName << ")\n";
            cout << "- Highest Stock Quantity: " << highestStockQty << " (" << highestStockName << ")\n";
            cout << "- Lowest Stock Quantity: " << lowestStockQty << " (" << lowestStockName << ")\n";
            cout << "- Highest Supply Quantity: " << highestSupplyQty << " (" << highestStockName << ")\n";
            cout << "- Lowest Supply Quantity: " << lowestSupplyQty << " (" << lowestStockName << ")\n";
            cout << "- Highest Return Quantity: " << highestReturnQty << " (" << highestStockName << ")\n";
            cout << "- Lowest Return Quantity: " << lowestReturnQty << " (" << lowestStockName << ")\n";

            // Display bar chart
            cout << "\n" << WHITE_BOLD << "Bar Chart Visualization (1 # = 10 units, max 50 # per bar):\n" << RESET;

            const int maxBarLength = 50; // Maximum number of '#' symbols in a single bar

            auto drawBar = [&](const string& label, int value) {
                cout << CYAN << setw(25) << left << label << RESET << ": ";
                int barCount = value / 10;
                if (barCount > maxBarLength) {
                    for (int i = 0; i < maxBarLength; ++i) {
                        cout << GREEN << "#" << RESET;
                    }
                    cout << "..."; // Indicate overflow
                }
                else {
                    for (int i = 0; i < barCount; ++i) {
                        cout << GREEN << "#" << RESET;
                    }
                }
                cout << " (" << value << ")\n";
                };

            drawBar("Total Stock Quantity", totalStockQuantity);
            drawBar("Total Supply Quantity", totalSupplyQty);
            drawBar("Total Return Quantity", totalReturnQty);
            drawBar("Total Stock Price (RM)", (totalStockPrice));
        }
        else {
            cout << RED << "* No stock items available in the database.\n" << RESET;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "* SQL error: " << e.what() << RESET << endl;
    }

    cout << "\n" << WHITE_BOLD << "Press any key to return to the menu..." << RESET;
    _getch();
}

// View low stock items report (below the given threshold)
void Admin::viewLowStockReport() {
    system("cls");

    // Define color codes
    const string RESET = "\033[0m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string CYAN = "\033[36m";
    const string BLUE = "\033[34m";
    const string WHITE_BOLD = "\033[1;37m";

    // Title Header
    cout << CYAN << "*---------------------------------------------------------------------------------*\n";
    cout << "|                                                                                 |\n";
    cout << "|                                  LOW STOCK REPORT                               |\n";
    cout << "|                                                                                 |\n";
    cout << "*---------------------------------------------------------------------------------*" << RESET << "\n\n";

    int threshold;
    bool validInput = false;

    // Input validation loop for threshold
    while (!validInput) {

        cout << WHITE_BOLD << "Enter the threshold level for low stock (numeric value): " << RESET;
        string input;
        cin >> input;

        try {
            // Convert input to an integer
            size_t pos;
            threshold = stoi(input, &pos);

            // Ensure the entire input string was valid and positive
            if (pos != input.length() || threshold <= 0) {
                throw invalid_argument("Invalid threshold value. Please enter a positive numeric value.");
            }

            validInput = true;  // Input is valid, exit loop
        }
        catch (const invalid_argument& e) {

            cout << RED << "Error: " << e.what() << RESET << "\n";
            cout << YELLOW << "Please enter a valid positive integer for the threshold.\n\n" << RESET;
        }
        catch (const out_of_range& e) {
            cout << RED << "Error: Threshold value is too large. Please enter a smaller number.\n\n" << RESET;
        }
    }

    try {
        DBConnection dbConn;
        dbConn.prepareStatement(
            "SELECT stock_ID, stock_Name, stock_Quantity "
            "FROM stock WHERE stock_Quantity < ?"
        );
        dbConn.stmt->setInt(1, threshold);

        unique_ptr<sql::ResultSet> resultSet(dbConn.stmt->executeQuery());

        cout << GREEN << "\n*---------------------------------------------------------------*\n";
        cout << "| Stock ID  | Stock Name             | Quantity in Stock        |\n";
        cout << "*---------------------------------------------------------------*" << RESET << "\n";

        bool hasData = false;
        int totalLowStockQuantity = 0;
        int rowCount = 0;
        const int MAX_ROWS = 100;
        string stockNames[MAX_ROWS];
        int quantities[MAX_ROWS];

        // Loop through the result set
        while (resultSet->next()) {
            hasData = true;

            string stockID = resultSet->getString("stock_ID");
            string stockName = resultSet->getString("stock_Name");
            int stockQuantity = resultSet->getInt("stock_Quantity");

            stockNames[rowCount] = stockName;
            quantities[rowCount] = stockQuantity;
            totalLowStockQuantity += stockQuantity;
            ++rowCount;

            // Display each row
            cout << YELLOW << "| " << setw(9) << left << stockID
                << " | " << setw(22) << left << stockName
                << " | " << setw(24) << left << stockQuantity << " |" << RESET << "\n";
        }

        cout << GREEN << "*---------------------------------------------------------------*" << RESET << "\n";

        if (hasData) {

            cout << "\n" << WHITE_BOLD << "Summary:" << RESET << "\n\n";
            cout << BLUE << "- Total Low Stock Quantity: " << totalLowStockQuantity << RESET << "\n";

            cout << "\n" << CYAN << "*---------------------------------------------------------------*" << RESET;

            // Bar Chart for low stock quantities
            cout << "\n" << WHITE_BOLD << "Low Stock Quantity Distribution (1 # = 10 units, max 50 # per bar):" << RESET << "\n\n";
            const int maxBarLength = 50;  // Maximum length of the bar

            for (int i = 0; i < rowCount; ++i) {
                cout << CYAN << setw(22) << left << stockNames[i] << RESET << ": ";

                int barCount = quantities[i] / 10;

                // Boolean flag to check if the bar length exceeds the max length
                bool isLimitReached = false;

                // Cap the number of `#` to maxBarLength
                if (barCount > maxBarLength) {
                    barCount = maxBarLength;  // Limit the bar length to 50
                    isLimitReached = true;     // Set the flag to true if limit is reached
                }

                // Print the `#` symbols for the bar
                for (int j = 0; j < barCount; ++j) {
                    cout << GREEN << "#" << RESET;
                }

                // If the limit was reached, append a custom message (ellipsis)
                if (isLimitReached) {
                    cout << " ...";  // Indicating that the value exceeds the limit
                }
                else {
                    cout << " (" << quantities[i] << ")";
                }

                cout << "\n";
            }
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "* SQL error: " << e.what() << RESET << "\n";
    }

    cout << "\n" << WHITE_BOLD << "Press any key to return to the menu..." << RESET;
    _getch();
}

// View items report that are expired and expiring soon
void Admin::viewExpiredAndExpiringSoonReport() {
    system("cls");

    string targetDate;

    // Prompt user for the target expiry date
    cout << "\nEnter target expiry date (YYYY-MM-DD): ";
    cin >> targetDate;

    cout << "\033[36m*-----------------------------------------------------------------------------------------------*\n";
    cout << "|                                                                                               |\n";
    cout << "|                          EXPIRED AND EXPIRING SOON STOCK REPORT                               |\n";
    cout << "|                                                                                               |\n";
    cout << "*-----------------------------------------------------------------------------------------------*\033[0m\n";

    try {
        DBConnection dbConn;

        // Query for expired stocks (Expiry date earlier or equal to the target date)
        dbConn.prepareStatement(
            "SELECT stock_ID, stock_Name, stock_Quantity, stock_ExpiryDate "
            "FROM stock WHERE stock_ExpiryDate > ?"
        );
        dbConn.stmt->setString(1, targetDate);
        unique_ptr<sql::ResultSet> expiredResultSet(dbConn.stmt->executeQuery());

        // Query for expiring soon stocks (Expiry date greater than the target date)
        dbConn.prepareStatement(
            "SELECT stock_ID, stock_Name, stock_Quantity, stock_ExpiryDate "
            "FROM stock WHERE stock_ExpiryDate <= ?"
        );
        dbConn.stmt->setString(1, targetDate); // Target date for expiring soon stocks
        unique_ptr<sql::ResultSet> expiringSoonResultSet(dbConn.stmt->executeQuery());

        // Initialize quantities for expired and expiring soon stocks
        int totalExpiredQuantity = 0;
        int totalExpiringSoonQuantity = 0;

        // Section: Expired Stocks
        cout << "\n\033[1;37mExpired Stocks :\033[0m\n";
        cout << "\033[32m*--------------------------------------------------------------------------*\n";
        cout << "| Stock ID   | Stock Name           | Quantity in Stock  | Expiration Date |\n";
        cout << "*--------------------------------------------------------------------------*\033[0m\n";

        bool hasExpiredData = false;
        while (expiredResultSet->next()) {
            hasExpiredData = true;
            int stockQuantity = expiredResultSet->getInt("stock_Quantity");
            totalExpiredQuantity += stockQuantity;  // Add to total expired quantity
            cout << "\033[33m| " << setw(11) << left << expiredResultSet->getString("stock_ID")
                << "| " << setw(21) << left << expiredResultSet->getString("stock_Name")
                << "| " << setw(19) << left << stockQuantity
                << "| " << setw(16) << left << expiredResultSet->getString("stock_ExpiryDate")
                << "|\033[0m\n";
        }

        if (!hasExpiredData) {
            cout << "\033[31m* No expired stock items found before or on the target date.\033[0m\n";
        }

        cout << "\033[32m*--------------------------------------------------------------------------*\033[0m\n";

        // Section: Expiring Soon Stocks
        cout << "\n\033[1;37mExpiring Soon Stocks:\033[0m\n";
        cout << "\033[32m*--------------------------------------------------------------------------*\n";
        cout << "| Stock ID   | Stock Name           | Quantity in Stock  | Expiration Date |\n";
        cout << "*--------------------------------------------------------------------------*\033[0m\n";

        bool hasExpiringSoonData = false;
        while (expiringSoonResultSet->next()) {
            hasExpiringSoonData = true;
            int stockQuantity = expiringSoonResultSet->getInt("stock_Quantity");
            totalExpiringSoonQuantity += stockQuantity;  // Add to total expiring soon quantity
            cout << "\033[33m| " << setw(11) << left << expiringSoonResultSet->getString("stock_ID")
                << "| " << setw(21) << left << expiringSoonResultSet->getString("stock_Name")
                << "| " << setw(19) << left << stockQuantity
                << "| " << setw(16) << left << expiringSoonResultSet->getString("stock_ExpiryDate")
                << "|\033[0m\n";
        }

        if (!hasExpiringSoonData) {
            cout << "\033[31m* No expiring soon stock items found after the target date.\033[0m\n";
        }

        cout << "\033[32m*--------------------------------------------------------------------------*\033[0m\n\n";

        // Summary Section
        cout << "\033[33mSummary:\033[0m\n";
        // Display total quantities
        cout << "\033[32m*-------------------------------------------------------------------------*\n";
        cout << "| Total Expired Stock Quantity       : " << setw(35) << totalExpiredQuantity << "|\n";
        cout << "| Total Expiring Soon Stock Quantity : " << setw(35) << totalExpiringSoonQuantity << "|\n";
        cout << "*-------------------------------------------------------------------------*\033[0m\n";

    }
    catch (sql::SQLException& e) {
        cout << "\033[31m* SQL error: " << e.what() << "\033[0m\n";
    }

    cout << "\n\033[1;37mPress any key to return to the menu...\033[0m";
    _getch();
}

// View the most highly stock category report
void Admin::viewCategoryReport() {
    system("cls");

    // Define color codes
    const string RESET = "\033[0m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string WHITE_BOLD = "\033[1;37m";
    const string RED = "\033[31m";

    cout << GREEN << "*------------------------------------------------------------------------------------------------------------*\n";
    cout << "| Stock ID   | Stock Name             | Quantity Supply        | Quantity Return        | Category Name      |\n";
    cout << "*------------------------------------------------------------------------------------------------------------*\033[0m\n";

    try {
        DBConnection dbConn;

        // Query to fetch stock details along with category
        dbConn.prepareStatement(
            "SELECT s.stock_ID, s.stock_Name, IFNULL(SUM(sd.quantity_supply), 0) AS total_supply, "
            "IFNULL(SUM(sd.quantity_return), 0) AS total_return, c.category_name "
            "FROM stock s "
            "JOIN category c ON s.category_id = c.category_id "
            "LEFT JOIN transaction sd ON s.stock_ID = sd.stock_ID "
            "GROUP BY s.stock_ID, s.stock_Name, c.category_name"
        );

        unique_ptr<sql::ResultSet> resultSet(dbConn.stmt->executeQuery());

        int totalSupply = 0, totalReturn = 0;

        // Variables to track most and least favorable categories
        string mostFavorableCategory;
        int highestSupply = 0;
        string leastFavorableCategory;
        int highestReturn = 0;

        while (resultSet->next()) {
            string stockID = resultSet->getString("stock_ID");
            string stockName = resultSet->getString("stock_Name");
            int supply = resultSet->getInt("total_supply");
            int returns = resultSet->getInt("total_return");
            string categoryName = resultSet->getString("category_name");

            // Add to totals
            totalSupply += supply;
            totalReturn += returns;

            // Check for most and least favorable categories
            if (supply > highestSupply) {
                highestSupply = supply;
                mostFavorableCategory = categoryName;
            }
            if (returns > highestReturn) {
                highestReturn = returns;
                leastFavorableCategory = categoryName;
            }

            // Display stock details
            cout << YELLOW
                << "| " << setw(11) << left << stockID
                << "| " << setw(23) << left << stockName
                << "| " << setw(23) << left << supply
                << "| " << setw(23) << left << returns
                << "| " << setw(19) << left << categoryName << "|\n";
        }

        cout << GREEN << "*------------------------------------------------------------------------------------------------------------*\n";

        // Display the totals
        cout << WHITE_BOLD
            << "| " << setw(11) << left << "TOTAL"
            << "| " << setw(23) << left << "-"
            << "| " << setw(23) << left << totalSupply
            << "| " << setw(23) << left << totalReturn
            << "| " << setw(19) << left << "-" << "|\n";
        cout << "*------------------------------------------------------------------------------------------------------------*" << RESET << "\n";

        // Bar chart summary
        cout << "\n" << WHITE_BOLD << "Summary:\n" << RESET;
        cout << GREEN << "- Total Quantity Supplied: " << totalSupply << "\n";
        cout << "- Total Quantity Returned: " << totalReturn << "\n";

        // Define the maximum length for the bar chart
        const int maxBarLength = 50;  // Maximum number of `#` allowed

        cout << "\n" << WHITE_BOLD << "Bar Chart Visualization (1 bar = 10 units):\n" << RESET;

        // Lambda function to draw a bar with a limit on the number of `#`
        auto drawBar = [&](const string& label, int value) {
            cout << YELLOW << setw(25) << left << label << RESET << ": ";
            int barCount = value / 10;

            // Cap the bar length to maxBarLength
            if (barCount > maxBarLength) {
                barCount = maxBarLength;  // Limit the bar length to 50
            }

            // Print the `#` symbols for the bar
            for (int i = 0; i < barCount; ++i) {
                cout << GREEN << "#" << RESET;
            }

            // If the value exceeds the max bar length, display a "..." after the bar
            if (value / 10 > maxBarLength) {
                cout << " ...";  // Indicating that the value is too large for full display
            }

            cout << " (" << value << ")\n";
            };

        drawBar("Total Quantity Supplied", totalSupply);
        drawBar("Total Quantity Returned", totalReturn);

        // Display most and least favorable categories
        cout << YELLOW << "\n\n- Most Favorable Category: " << mostFavorableCategory
            << " (Highest Quantity Supplied: " << highestSupply << ")\n";
        cout << RED << "- Least Favorable Category: " << leastFavorableCategory
            << " (Highest Quantity Returned: " << highestReturn << ")\n" << RESET;
    }
    catch (sql::SQLException& e) {
        cout << RED << "* SQL error: " << e.what() << RESET << endl;
    }

    cout << "\nPress any key to return to the menu...";
    _getch();
}

//Admin Page
// Function to register an admin
void Account::registerAdmin() {

    string username, password, newAdminID;

    db.prepareStatement("SELECT admin_id FROM admin ORDER BY admin_id DESC");
    db.QueryResult();


    if (db.res->next()) {
        //Get the latest admin_id
        string latestAdminID = db.res->getString("admin_id");

        // Extract numeric part from the latest ID, starting from index 1
        int numericPart = 0;
        for (int i = 1; i < latestAdminID.length(); i++) {
            numericPart = numericPart * 10 + (latestAdminID[i] - '0');
        }

        // Increment the numeric part
        numericPart++;

        // Build the new ID as "A" + zero-padded number
        newAdminID = "A";

        if (numericPart < 10) {
            newAdminID += "00"; // Pad with two zeros for numbers < 10
        }
        else if (numericPart < 100) {
            newAdminID += "0";  // Pad with one zero for numbers < 100
        }
        newAdminID += to_string(numericPart); // Append the incremented numeric part
    }
    else //if no data
    {
        newAdminID = "A001";
    }

    cin.ignore();

    // Get username input with validation
    bool isValid = false;
    while (!isValid) {

        cout << "\nEnter username: ";
        getline(cin, username);

        if (username.empty()) {
            cout << RED << "Error: Username cannot be blank.\n" << RESET;
        }
        else if (username.length() < 8) {
            cout << RED << "Error: Username must be at least 8 characters long.\n" << RESET;
        }
        else if (username.find(' ') != string::npos) {
            cout << RED << "Error: Username must not contain spaces.\n" << RESET;
        }
        else if (regex_match(username, regex("[0-9]+"))) {
            cout << RED << "Error: Username cannot consist of only numeric characters.\n" << RESET;
        }
        else if (regex_match(username, regex(".*[^A-Za-z0-9].*"))) {
            cout << RED << "Error: Username must not contain special characters (e.g., !@#&*).\n" << RESET;
        }
        else {
            break;
        }
    }

    while (true) {
        password = getPasswordInput();

        if (password.empty()) {
            cout << RED << "Error: Password cannot be blank.\n" << RESET;
        }
        else if (password.length() < 6) {
            cout << RED << "Error: Password must be at least 6 characters long.\n" << RESET;
        }
        else if (!regex_search(password, regex("[A-Za-z]")) || !regex_search(password, regex("[0-9]"))) {
            cout << RED << "Error: Password must contain at least one letter and one number.\n" << RESET;
        }
        else {
            break;
        }
    }

    try {

        // Use DBConnection object to handle the connection
        DBConnection dbConn;
        dbConn.prepareStatement("INSERT INTO admin (admin_ID,admin_Name,admin_Password) VALUES (?,?,?)");
        dbConn.stmt->setString(1, newAdminID);
        dbConn.stmt->setString(2, username);
        dbConn.stmt->setString(3, password);

        // Execute the statement
        dbConn.stmt->execute();

        cout << GREEN << "\nAdmin registered successfully!. Your admin ID is " << newAdminID << endl << RESET;
        _getch();
    }

    catch (sql::SQLException& e) {
        cout << RED << "SQL error while registering admin: " << e.what() << endl << RESET;
    }
}

// Function to hide password as symbolic
string Account::getPasswordInput() {
    string password;
    char ch;

    cout << "\nEnter password: ";

    while (true) {

        ch = _getch();

        if (ch == '\r') { 
            cout << endl; 
            break;
        }
        else if (ch == '\b') { 
            if (!password.empty()) {
                cout << "\b \b";
                password.pop_back();
            }
        }
        else {
            password += ch;
            cout << '*'; 
        }
    }

    return password;
}

// Function to login an admin
bool Account::loginAdmin() {

    string username, password;

    cin.ignore();
    
    // Get username input with validation
    bool isValid = false;

    while (!isValid) {
        cout << "Enter username: ";
        getline(cin, username);

        if (username.find(' ') != string::npos) {
            cout << RED <<"Error: Username must not contain spaces.\n" << RESET;
        }
        else {
            isValid = true;
        }
    }

    password = getPasswordInput(); 

    try {

        DBConnection dbConn;
        dbConn.prepareStatement("SELECT * FROM admin WHERE BINARY admin_Name = ? AND BINARY admin_Password = ?");
        dbConn.stmt->setString(1, username);
        dbConn.stmt->setString(2, password);

        dbConn.QueryResult();

        if (dbConn.res->next()) {
            return true;

        }
        else {
            return false;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "SQL error: " << e.what() << endl << RESET;
    }

    return false;
}

//User Page
// Function to register an user
void Account::registerUser() {

    string password,newUserID,username;

    db.prepareStatement("SELECT user_id FROM user ORDER BY user_id DESC LIMIT 1");
    db.QueryResult();
    cin.ignore();

    if (db.res->next()) {

        string userId = db.res->getString("user_id");

        int numericPart = stoi(userId.substr(1));

        numericPart++;

        if (numericPart < 10) {
            newUserID = "U00" + to_string(numericPart); // Pad with two zeros for numbers < 10
        }
        else if (numericPart < 100) {
            newUserID = "U0" + to_string(numericPart);  // Pad with one zero for numbers < 100
        }
        else {
            newUserID = "U" + to_string(numericPart);  // No padding needed
        }

    }
    else {
        // No data in the supplier table, start with SP001
        newUserID = "U001";
    }

    // Username input validation
    while (true) {
        cout << "\nEnter username: ";
        getline(cin, username);

        if (username.empty()) {
            cout << RED << "Error: Username cannot be blank.\n" << RESET;
        }
        else if (username.length() < 8) {
            cout << RED << "Error: Username must be at least 8 characters long.\n" << RESET;
        }
        else if (username.find(' ') != string::npos) {
            cout << RED << "Error: Username must not contain spaces.\n" << RESET;
        }
        else if (regex_match(username, regex("[0-9]+"))) {
            cout << RED << "Error: Username cannot consist of only numeric characters.\n" << RESET;
        }
        else if (regex_match(username, regex(".*[^A-Za-z0-9].*"))) {
            cout << RED << "Error: Username must not contain special characters (!@#&*).\n" << RESET;
        }
        else {
            break;
        }
    }

    // Password input validation
    while (true) {
        password = getPasswordInput();

        if (password.empty()) {
            cout << RED << "Error: Password cannot be blank.\n" << RESET;
        }
        else if (password.length() < 6) {
            cout << RED << "Error: Password must be at least 6 characters long.\n" << RESET;
        }
        else if (!regex_search(password, regex("[A-Za-z]")) || !regex_search(password, regex("[0-9]"))) {
            cout << RED << "Error: Password must contain at least one letter and one number.\n" << RESET;
        }
        else {
            break;
        }
    }

    try {

        DBConnection dbConn;
        dbConn.prepareStatement("INSERT INTO user (user_id,username,password) VALUES (?,?,?)");
        dbConn.stmt->setString(1, newUserID);
        dbConn.stmt->setString(2, username);
        dbConn.stmt->setString(3, password);
        dbConn.QueryStatement();


        cout << GREEN << "\nUser registered successfully! User ID: " << newUserID << RESET << endl;
        _getch();
    }

    catch (sql::SQLException& e) {

        cout << RED << "SQL error while registering user: " << e.what() << endl << RESET;
        _getch();
    }
}

// Function to login an user
bool Account::loginUser() {

        string username, password;

        cin.ignore();

        // Get supplier ID input with validation
        bool isValid = false;

        while (!isValid) {
            cout << "Enter Username: ";
            getline(cin, username);

            // Validate supplier ID: must not be empty
            if (username.empty()) {
                cout << RED <<"Error: Username cannot be empty.\n" << RESET;
            }
            else {
                isValid = true;
            }
        }

        password = getPasswordInput();

        try {
           
            DBConnection dbConn;
            dbConn.prepareStatement("SELECT * FROM user WHERE BINARY username = ? AND BINARY password = ?");
            dbConn.stmt->setString(1, username);
            dbConn.stmt->setString(2, password);

            dbConn.QueryResult();

            if (dbConn.res->next()) {
                // Successfully logged in
                userID = dbConn.res->getString("user_id");

                return true;
            }
        }
        catch (sql::SQLException& e) {

            cout << RED <<"SQL error: " << e.what() << endl << RESET;
        }

        return false;
}

string Account::getUserID() {
    return userID;
}

// Category functions
// Add category
void Category::addCategory() {

    cout << CYAN << "\n\n*--------------------------------------------------------------------------------------------------*\n";
    cout << "|                                                                                                  |\n";
    cout << "|                                            ADD CATEGORY                                          |\n";
    cout << "|                                                                                                  |\n";
    cout << "*--------------------------------------------------------------------------------------------------*\n" << RESET;
    cout << "*--------------------------------------------------------------------------------------------------*\n";

    string categoryID, name, description, newID;

    // Generate category ID (auto-increment logic)
    db.prepareStatement("SELECT category_ID FROM category ORDER BY category_ID DESC");
    db.QueryResult();

    if (db.res->next()) {

        string categoryID = db.res->getString("category_ID");

        int numericPart = 0;
        for (int i = 1; i < categoryID.length(); i++) {
            numericPart = numericPart * 10 + (categoryID[i] - '0');
        }

        // Increment the numeric part
        numericPart++;

        // Build the new ID as "C" + zero-padded number
        newID = "C";

        if (numericPart < 10) {
            newID += "00"; // Pad with two zeros for numbers < 10
        }
        else if (numericPart < 100) {
            newID += "0";  // Pad with one zero for numbers < 100
        }
        newID += to_string(numericPart); // Append the incremented numeric part
    }
    else //if no data
    {
        newID = "C001";
    }

    cin.ignore();

    // Validate stock name
    while (true) {
        cout << "\n- Enter category name: ";
        getline(cin, name);

        // Check if stock name is empty
        if (name.empty()) {
            cout << YELLOW << "Category name cannot be empty. Please try again.\n\n" << RESET;
            continue;
        }

        // Check if stock name is numeric
        bool isNumeric = true;
        for (char c : name) {
            if (!isdigit(c)) {
                isNumeric = false;
                break;
            }
        }

        if (isNumeric) {
            cout << YELLOW << "Category name cannot be numeric. Please enter a valid name.\n\n" << RESET;
            continue;
        }

        // Valid stock name
        break;
    }

    cout << "\n- Enter category description: ";
    getline(cin, description);

    // Insert category into category table in database
    try {

        DBConnection dbConn;
        dbConn.prepareStatement("INSERT INTO category (category_id, category_name, category_description) VALUES (?, ?, ?)");

        dbConn.stmt->setString(1, newID);
        dbConn.stmt->setString(2, name);
        dbConn.stmt->setString(3, description);

        dbConn.stmt->executeUpdate();

        cout << GREEN << "Category item and stock entry added successfully!" << endl << RESET;
    }
    catch (sql::SQLException& e) {
        cout << RED << "SQL error while inserting category and stock item: " << e.what() << endl << RESET;
    }

    cout << "*--------------------------------------------------------------------------------------------------*\n";
    _getch();
}

// Remove category
void Category::removeCategory() {

    cout << "\n\n*----------------------------------------------------------*\n";
    cout << "|                                                          |\n";
    cout << "|                      REMOVE CATEGORY                     |\n";
    cout << "|                                                          |\n";
    cout << "*----------------------------------------------------------*\n";
    cout << "*----------------------------------------------------------*\n";

    string ID;

    while (true) {

        cin.ignore();
        cout << "\n- Enter category ID: ";
        getline(cin, ID);

        // Check if stock ID is empty
        if (ID.empty()) {
            cout << YELLOW << "Stock ID cannot be empty. Please enter a valid ID.\n" << RESET;
            continue;
        }

        // Check if stock ID is a numeric value
        if (all_of(ID.begin(), ID.end(), ::isdigit)) {
            cout << YELLOW << "Stock ID cannot be numeric. Please enter a valid ID.\n" << RESET;
            continue;
        }


        // Validate stock ID format using regex (e.g., starts with a letter followed by digits)
        regex idPattern("^[A-Za-z]\\d{3}$");
        if (!std::regex_match(ID, idPattern)) {

            cout << YELLOW << "Invalid stock ID format. It should be like S001\n" << RESET;
            continue;
        }

        try {

            DBConnection dbConn;
            dbConn.prepareStatement("DELETE FROM category WHERE category_ID = ?");
            dbConn.stmt->setString(1, ID);

            // Execute deletion
            int rowsAffected = dbConn.stmt->executeUpdate();

            if (rowsAffected > 0) {

                cout << GREEN << "[" << ID << "] has been removed successfully!" << endl << RESET;
                break;
            }
            else {

                cout << YELLOW << "Stock ID is not found! Please enter a valid ID.\n" << endl << RESET;

            }
        }

        catch (sql::SQLException& e) {

            cout << RED << "SQL error: " << e.what() << endl << RESET;
        }
    }

    cout << "*----------------------------------------------------------*\n";
    _getch();
}

// Modify category
void Category::modifyCategory() {

    cout << CYAN << "\n\n*----------------------------------------------------------*\n";
    cout << "|                                                          |\n";
    cout << "|                      MODIFY CATEGORY                     |\n";
    cout << "|                                                          |\n";
    cout << "*----------------------------------------------------------*\n" << RESET;
    cout << "*----------------------------------------------------------*\n";

    string ID,name,description;

    while (true) {

        cin.ignore();
        cout << "\n- Enter category ID (C001): ";
        getline(cin, ID);

        // Check if stock ID is empty
        if (ID.empty()) {

            cout << YELLOW << "Stock ID cannot be empty. Please enter a valid ID.\n" << RESET;
            continue;
        }

        // Check if stock ID is a numeric value
        if (std::all_of(ID.begin(), ID.end(), ::isdigit)) {
            cout << YELLOW << "Stock ID cannot be numeric. Please enter a valid ID.\n" << RESET;
            continue;
        }

        // Validate stock ID format using regex (e.g., starts with a letter followed by digits)
        regex idPattern("^[A-Za-z]\\d{3}$");
        if (!std::regex_match(ID, idPattern)) {
            cout << YELLOW << "Invalid stock ID format. It should be like S001.\n" << RESET;
            continue;
        }

        // Check if the stock ID exists in the database
        try {
            DBConnection dbConn;

            dbConn.prepareStatement("SELECT category_id FROM category WHERE category_id = ?");
            dbConn.stmt->setString(1, ID);
            dbConn.QueryResult();

            if (!dbConn.res->next()) {
                cout << YELLOW << "Category item with ID [" << ID << "] does not exist in the database.\n" << RESET;
                continue;
            }

        }
        catch (sql::SQLException& e) {
            cout << RED << "SQL error: " << e.what() << endl << RESET;
            continue;
        }
        break;
    }

    cin.ignore();
    cout << "\n- Enter new category name: ";
    getline(cin, name);

    cout << "\n- Enter new category description: ";
    getline(cin, description);

    try {

        DBConnection dbConn;

        dbConn.prepareStatement("UPDATE category SET category_name = ?, category_description = ? WHERE category_ID = ?");
        dbConn.stmt->setString(1, name);
        dbConn.stmt->setString(2, description);
        dbConn.stmt->setString(3, ID);

        int rowsAffected = dbConn.stmt->executeUpdate();

        if (rowsAffected > 0) {

            cout << GREEN << "Stock item with ID [" << ID << "] has been updated successfully!" << endl << RESET;

        }
        else {
            cout << YELLOW << "Stock item with ID [" << ID << "] was not found!" << endl << RESET;
        }
    }

    catch (sql::SQLException& e) {

        cout << RED << "SQL error: " << e.what() << endl << RESET;
    }

    cout << "*----------------------------------------------------------*\n";
    _getch();
}

// Display category
void Category::displayCategory() {

    cout << "*--------------------------------------------------------------------------------------------------*\n";
    cout << "|                                                                                                  |\n";
    cout << "|                                            DISPLAY CATEGORIES                                    |\n";
    cout << "|                                                                                                  |\n";
    cout << "*--------------------------------------------------------------------------------------------------*\n";

    try {
        DBConnection dbConn;
        dbConn.prepareStatement("SELECT category_id, category_name, category_description FROM category");
        dbConn.QueryResult();

        if (dbConn.res->next()) {
            cout << "+----------------+--------------------+-----------------------------------------+\n";
            cout << "| Category ID    | Category Name      | Description                             |\n";
            cout << "+----------------+--------------------+-----------------------------------------+\n";

            do {
                string categoryID = dbConn.res->getString("category_id");
                string categoryName = dbConn.res->getString("category_name");
                string categoryDescription = dbConn.res->getString("category_description");

                cout << "| " << setw(15) << left << categoryID
                    << "| " << setw(19) << left << categoryName
                    << "| " << setw(40) << left << categoryDescription
                    << "|\n";

            } while (dbConn.res->next());

            cout << "+----------------+--------------------+-----------------------------------------+\n";
        }
        else {
            cout << YELLOW << "\n* No categories available in the database. *\n" << RESET;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\n* SQL Error: " << e.what() << " *\n" << RESET;
    }
    _getch();
}

// Search category
void Category::searchCategory() {

    cin.ignore();
    cin.clear();

    cout << YELLOW << "\n\n*--------------------------------------------------------------------------------------------------*\n";
    cout << "|                                                                                                  |\n";
    cout << "|                                              SEARCH CATEGORY                                     |\n";
    cout << "|                                                                                                  |\n";
    cout << "*--------------------------------------------------------------------------------------------------*\n" << RESET;

    string searchName;

    cout << "- Enter category name to search: ";
    getline(cin, searchName);

    cout << endl;

    try {
        DBConnection dbConn;
        dbConn.prepareStatement(
            "SELECT category_id, category_name, category_description "
            "FROM category "
            "WHERE category_name LIKE ?");

        // Add wildcard (%) for partial matching
        dbConn.stmt->setString(1, "%" + searchName + "%");

        unique_ptr<sql::ResultSet> resultSet(dbConn.stmt->executeQuery());

        if (resultSet->next()) {
            cout << GREEN <<"+----------------+--------------------+-----------------------------------------+\n";
            cout << "| Category ID    | Category Name      | Description                             |\n";
            cout << "+----------------+--------------------+-----------------------------------------+\n" << RESET;

            do {
                string categoryID = resultSet->getString("category_id");
                string categoryName = resultSet->getString("category_name");
                string categoryDescription = resultSet->getString("category_description");

                cout << "| " << setw(14) << left << categoryID
                    << " | " << setw(18) << left << categoryName
                    << " | " << setw(37) << left << categoryDescription
                    << "   |\n";

            } while (resultSet->next());

            cout << "+----------------+--------------------+-----------------------------------------+\n";;
        }
        else {
            cout << YELLOW << "\n* No categories found matching the search criteria. *\n" << RESET;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\n* SQL Error: " << e.what() << " *\n" << RESET;
    }

    cout << "\nPress any key to return to the menu...";
    _getch();
}

// Supplier functions
void Supplier::setSupplierID(string userID) {
    loggedInUserID = userID;

    db.prepareStatement("SELECT supplier_id FROM supplier WHERE user_id = ?");
    db.stmt->setString(1, userID);
    db.QueryResult();
    db.res->next();
    loggedInSupplierID = db.res->getString("supplier_id");
}

// Add supplier details
void Supplier::addSupplier(string userID) {
    
    string loggedInUserID = userID;
    string newID, supplierName, stockID;
    int supplierStockQuantity;

    cout << GREEN << "\n\n*----------------------------------------------*\n";
    cout << "|                                              |\n";
    cout << "|                  ADD SUPPLIER                |\n";
    cout << "|                                              |\n";
    cout << "*----------------------------------------------*\n" << RESET;

    // Generate supplier ID (auto-increment logic)
    db.prepareStatement("SELECT supplier_id FROM supplier ORDER BY supplier_id DESC");
    db.QueryResult();

    if (db.res->next()) {

        string supplierID = db.res->getString("supplier_id");

        int numericPart = stoi(supplierID.substr(2));

        numericPart++;

        if (numericPart < 10) {
            newID = "SP00" + to_string(numericPart); // Pad with two zeros for numbers < 10
        }
        else if (numericPart < 100) {
            newID = "SP0" + to_string(numericPart);  // Pad with one zero for numbers < 100
        }
        else {
            newID = "SP" + to_string(numericPart);  // No padding needed
        }

    }
    else {
        // No data in the supplier table, start with SP001
        newID = "SP001";
    }

    cin.ignore();

    // Validate stock name
    while (true) {

        cout << "\n- Enter supplier name: ";
        getline(cin, supplierName);

        // Check if stock name is empty
        if (supplierName.empty()) {
            cout << YELLOW << "Supplier name cannot be empty. Please try again.\n\n" << RESET;
            continue;
        }

        // Check if stock name is numeric
        bool isNumeric = true;
        for (char c : supplierName) {
            if (!isdigit(c)) {
                isNumeric = false;
                break;
            }
        }

        if (isNumeric) {
            cout << YELLOW << "Supplier name cannot be numeric. Please enter a valid name.\n\n" << RESET;
            continue;
        }

        // Valid stock name
        break;
    }

    while (true) {

        try {
            //Error handling (Quantity cannot be -ve and must be int)
            string input;

            cout << "\n- Enter stock quantity: ";
            cin >> input;

            // Attempt to convert the input to an integer
            size_t pos;
            supplierStockQuantity = stoi(input, &pos);

            // Check if the entire string was consumed
            if (pos != input.length()) {
                throw invalid_argument("Extra characters found");
            }

            if (supplierStockQuantity < 0) {
                throw out_of_range("Quantity cannot be negative");
            }

            break;
        }
        catch (const invalid_argument& e) {
            cout << YELLOW << "Invalid input. Please enter an integer to proceed.\n\n" << RESET;
        }
        catch (const out_of_range& e) {
            cout << YELLOW << "Quantity cannot be negative. Please try again.\n\n" << RESET;
        }
    }

    cout << "\n- Enter stockID: ";
    cin >> stockID;

    try {
        DBConnection dbConn;

        dbConn.prepareStatement("INSERT INTO supplier (supplier_id, supplier_name,supplier_stock_quantity,user_id,stock_id) VALUES (?, ?, ?, ?,?)");
        dbConn.stmt->setString(1, newID);
        dbConn.stmt->setString(2, supplierName);
        dbConn.stmt->setInt(3, supplierStockQuantity);
        dbConn.stmt->setString(4, loggedInUserID);
        dbConn.stmt->setString(5, stockID);

        dbConn.stmt->executeUpdate();

        cout << GREEN << "\nSupplier details added successfully!" << RESET << endl;
    }
    catch (sql::SQLException& e) {
        cout << RED << "\nSQL error while adding supplier: " << e.what() << RESET << endl;
    }
    cout << "*----------------------------------------------------------*\n";
    _getch();
}

// Remove supplier details
void Supplier::removeSupplier() {

    cout << CYAN << "\n\n*----------------------------------------------*\n";
    cout << "|                                              |\n";
    cout << "|                REMOVE SUPPLIER               |\n";
    cout << "|                                              |\n";
    cout << "*----------------------------------------------*\n" << RESET;

    string supplierID;

    while (true) {
        cin.ignore();

        cout << "\n- Enter supplier ID (SP001): ";
        getline(cin, supplierID);

        // Check if supplier ID is empty
        if (supplierID.empty()) {
            cout << YELLOW << "\nSupplier ID cannot be empty. Please enter a valid ID." << RESET;
            continue;
        }

        // Check if the supplier ID exists in the database
        try {
            DBConnection dbConn;

            dbConn.prepareStatement("SELECT supplier_id FROM supplier WHERE supplier_id = ?");
            dbConn.stmt->setString(1, supplierID);
            dbConn.QueryResult();

            if (!dbConn.res->next()) {
                cout << YELLOW << "\nSupplier with ID [" << supplierID << "] does not exist in the database.\n" << RESET;
                continue;
            }

        }
        catch (sql::SQLException& e) {
            cout << RED << "\nSQL error: " << e.what() << endl << RESET;
            continue;
        }
        break;
    }

    try {
        DBConnection dbConn;

        dbConn.prepareStatement("DELETE FROM supplier WHERE supplier_ID = ?");
        dbConn.stmt->setString(1, supplierID);

        int rowsAffected = dbConn.stmt->executeUpdate();
        if (rowsAffected > 0) {
            cout << GREEN << "\nSupplier deleted successfully!" << endl << RESET;
        }
        else {
            cout << YELLOW << "\nSupplier not found!" << endl << RESET;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\nSQL error while deleting supplier: " << e.what() << endl << RESET;
    }
    cout << "*----------------------------------------------------------*\n";
    _getch();
}

// Modify supplier functions
// Modify supplier name
void Supplier::modifySupplierName() {
 
    cout << "\n\n*----------------------------------------------*\n";
    cout << "|                                              |\n";
    cout << "|             MODIFY SUPPLIER NAME             |\n";
    cout << "|                                              |\n";
    cout << "*----------------------------------------------*\n";

    string supplierID, newName;

    while (true) {
        cin.ignore();

        cout << "\n- Enter supplier ID (SP001): ";
        getline(cin, supplierID);

        // Check if supplier ID is empty
        if (supplierID.empty()) {
            cout << "\nSupplier ID cannot be empty. Please enter a valid ID.";
            continue;
        }

        // Check if the supplier ID exists in the database
        try {
            DBConnection dbConn;

            dbConn.prepareStatement("SELECT supplier_id FROM supplier WHERE supplier_id = ?");
            dbConn.stmt->setString(1, supplierID);
            dbConn.QueryResult();

            if (!dbConn.res->next()) {
                cout << "\nSupplier with ID [" << supplierID << "] does not exist in the database.\n";
                continue;
            }

        }
        catch (sql::SQLException& e) {
            cout << "\nSQL error: " << e.what() << endl;
            continue;
        }
        break;
    }

    cout << "\n- Enter new supplier name: ";
    getline(cin, newName);

    try {
        DBConnection dbConn;
        dbConn.prepareStatement("UPDATE supplier SET supplier_name = ? WHERE supplier_ID = ?");
        dbConn.stmt->setString(1, newName);
        dbConn.stmt->setString(2, supplierID);
        int rowsAffected = dbConn.stmt->executeUpdate();
        if (rowsAffected > 0) {
            cout << "\nSupplier name updated successfully!" << endl;
        }
        else {
            cout << "\nSupplier not found!" << endl;
        }
    }
    catch (sql::SQLException& e) {
        cout << "\nSQL error: " << e.what() << endl;
    }
    cout << "*----------------------------------------------*\n";
    _getch();
}

// Modify supplier stock quantity
void Supplier::modifySupplierStockQuantity() {

    cout << GREEN << "\n\n*----------------------------------------------*\n";
    cout << "|                                              |\n";
    cout << "|         MODIFY SUPPLIER STOCK QUANTITY       |\n";
    cout << "|                                              |\n";
    cout << "*----------------------------------------------*\n" << RESET;

    string supplierID;
    int newQuantity;

    while (true) {
        cin.ignore();
        cout << "\n- Enter supplier ID (SP001): ";
        getline(cin, supplierID);

        // Check if supplier ID is empty
        if (supplierID.empty()) {
            cout << YELLOW << "\nSupplier ID cannot be empty. Please enter a valid ID.\n\n" << RESET;
            continue;
        }

        // Check if the supplier ID exists in the database
        try {
            DBConnection dbConn;

            dbConn.prepareStatement("SELECT supplier_ID FROM supplier WHERE supplier_ID = ?");
            dbConn.stmt->setString(1, supplierID);
            dbConn.QueryResult();

            if (!dbConn.res->next()) {
                cout << YELLOW << "\nSupplier with ID [" << supplierID << "] does not exist in the database.\n" << RESET;
                continue;
            }

        }
        catch (sql::SQLException& e) {
            cout << RED << "\nSQL error: " << e.what() << endl << RESET;
            continue;
        }
        break;
    }

    while (true) {
        try {
            string input;
            cout << "\n- Enter new stock quantity: ";
            cin >> input;

            size_t pos;
            newQuantity = stoi(input, &pos);

            if (pos != input.length()) {
                throw invalid_argument("Extra characters found");
            }
            if (newQuantity < 0) {
                throw out_of_range("Quantity cannot be negative");
            }
            break;
        }
        catch (const invalid_argument& e) {
            cout << "\nInvalid input. Please enter an integer to proceed.\n\n";
        }
        catch (const out_of_range& e) {
            cout << "\nQuantity cannot be negative. Please try again.\n\n";
        }
    }

    try {
        DBConnection dbConn;
        dbConn.prepareStatement("UPDATE supplier SET supplier_stock_quantity = ? WHERE supplier_ID = ?");
        dbConn.stmt->setInt(1, newQuantity);
        dbConn.stmt->setString(2, supplierID);

        int rowsAffected = dbConn.stmt->executeUpdate();
        if (rowsAffected > 0) {
            cout << "\nSupplier stock quantity updated successfully!" << endl;
        }
        else {
            cout << "\nSupplier not found!" << endl;
        }
    }
    catch (sql::SQLException& e) {
        cout << "\nSQL error: " << e.what() << endl;
    }
    cout << "*----------------------------------------------*\n";
    _getch();
}

// Display supplier details
void Supplier::displaySupplier() {

    cout << "\n\n+---------------------------------------------------------------------------------------------+\n";
    cout << "|                                                                                             |\n";
    cout << "|                                     DISPLAY SUPPLIER DETAILS                                |\n";
    cout << "|                                                                                             |\n";
    cout << "+---------------------------------------------------------------------------------------------+\n";

    try {
        DBConnection dbConn;
        dbConn.prepareStatement(
            "SELECT u.user_id, s.supplier_id, u.username, s.supplier_name, s.supplier_stock_quantity, s.user_id, s.stock_ID "
            "FROM supplier s "
            "INNER JOIN user u ON s.user_id = u.user_id "
            "WHERE u.user_id = ? "
        );
        dbConn.stmt->setString(1, loggedInUserID);

        dbConn.QueryResult();

        if (dbConn.res->next()) {
            cout << "+-------------+----------+---------------------+--------------+----------+-------------+\n";
            cout << "| Supplier ID | UserName | Supplier Name       | Quantity     | User ID  | Stock ID    |\n";
            cout << "+-------------+----------+---------------------+--------------+----------+-------------+\n";

            do {
                string supplierID = dbConn.res->getString("supplier_id");
                string username = dbConn.res->getString("username");
                string supplierName = dbConn.res->getString("supplier_name");
                int quantity = dbConn.res->getInt("supplier_stock_quantity");
                string userID = dbConn.res->getString("user_id");
                string stockID = dbConn.res->getString("stock_ID");

                cout << "| " << setw(12) << left << supplierID
                    << "| " << setw(9) << left << username
                    << "| " << setw(20) << left << supplierName
                    << "| " << setw(13) << left << quantity
                    << "| " << setw(9) << left << userID
                    << "| " << setw(12) << left << stockID
                    << "|\n";

            } while (dbConn.res->next());

            cout << "+-------------+----------+---------------------+--------------+----------+-------------+\n";
        }
        else {
            cout << YELLOW << "\n* No supplier details are currently available in the database. *\n" << RESET;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\n* SQL Error: " << e.what() << " *\n" << RESET;
    }
    _getch();
}

// Display stock_details so supplier can know whether want to restock or not
void Supplier::displayStockDetails() {

    cout << "\n\n+---------------------------------------------------------------------------------------------+\n";
    cout << "|                                                                                             |\n";
    cout << "|                                     DISPLAY TRANSACTION DETAILS                             |\n";
    cout << "|                                                                                             |\n";
    cout << "+---------------------------------------------------------------------------------------------+\n";

    try {
        DBConnection dbConn;
        dbConn.prepareStatement("SELECT transaction_id,stock_ID, supplier_id, quantity_supply, supply_date, quantity_return, return_date, status FROM transaction");
        dbConn.QueryResult();

        if (dbConn.res->next()) {
            cout << "+----------+-----------+-----------+---------------+----------------+---------------+-----------------+---------------+\n";
            cout << "| Trans ID | Stock ID  | Supp ID   | Qty Supplied  | Supply Date    | Qty Returned  | Return Date     | Status        |\n";
            cout << "+----------+-----------+-----------+---------------+----------------+---------------+-----------------+---------------+\n";;

            do {
                int transID = dbConn.res->getInt("transaction_id");
                string stockID = dbConn.res->getString("stock_ID");
                string supplierID = dbConn.res->getString("supplier_id");
                int quantitySupply = dbConn.res->getInt("quantity_supply");
                string supplyDate = dbConn.res->getString("supply_date");
                int quantityReturn = dbConn.res->getInt("quantity_return");
                string returnDate = dbConn.res->getString("return_date");
                string status = dbConn.res->getString("status");

               cout << "| " << setw(9) << left << transID
                    << "| " << setw(10) << left << stockID
                    << "| " << setw(10) << left << supplierID
                    << "| " << setw(14) << left << quantitySupply
                    << "| " << setw(15) << left << supplyDate
                    << "| " << setw(14) << left << quantityReturn
                    << "| " << setw(16) << left << returnDate
                    << "| " << setw(14) << left << status
                    << "|\n";

            } while (dbConn.res->next());

            cout << "+----------+-----------+-----------+---------------+----------------+---------------+-----------------+---------------+\n";
        }
        else {
            cout << YELLOW << "\n* No stock details are currently available in the database. *\n" << RESET;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\n* SQL Error: " << e.what() << " *\n" << RESET;
    }
    _getch();
}

// Check or trace supplier's stock movement
void Supplier::traceStockMovement() {

    cout << "+---------------------------------------------------------------------------------------------+\n";
    cout << "|                                                                                             |\n";
    cout << "|                             TRACE SUPPLIER STOCK MOVEMENT                                   |\n";
    cout << "|                                                                                             |\n";
    cout << "+---------------------------------------------------------------------------------------------+\n";

    string supplierID;
    cout << "\nEnter Supplier ID to trace stock movement: ";
    cin >> supplierID;

    try {
        DBConnection dbConn;

        dbConn.prepareStatement("SELECT stock_ID, supplier_id, quantity_supply, supply_date, quantity_return, return_date, status "
            "FROM transaction WHERE supplier_id = ?");

        dbConn.stmt->setString(1, supplierID);
        dbConn.QueryResult();

        if (dbConn.res->next()) {
            cout << "+------------+--------------+--------------------+--------------+--------------------+--------------+-----------------+\n";
            cout << "|  Stock ID  |  Supp ID     |  Qty Supplied      |  Supply Date |  Qty Returned      |  Return Date |  Status         |\n";
            cout << "+------------+--------------+--------------------+--------------+--------------------+--------------+-----------------+\n";

            do {
                string stockID = dbConn.res->getString("stock_ID");
                string supplierID = dbConn.res->getString("supplier_id");
                int quantitySupply = dbConn.res->getInt("quantity_supply");
                string supplyDate = dbConn.res->getString("supply_date");
                int quantityReturn = dbConn.res->getInt("quantity_return");
                string returnDate = dbConn.res->getString("return_date");
                string status = dbConn.res->getString("status");

                cout << "| " << setw(10) << left << stockID
                    << " | " << setw(12) << left << supplierID
                    << " | " << setw(18) << left << quantitySupply
                    << " | " << setw(12) << left << supplyDate
                    << " | " << setw(18) << left << quantityReturn
                    << " | " << setw(12) << left << returnDate
                    << " | " << setw(15) << left << status
                    << " |\n";

            } while (dbConn.res->next());

            cout << "+------------+--------------+--------------------+--------------+--------------------+--------------+-----------------+\n";
        }
        else {
            cout << YELLOW << "\n* No stock movement found for Supplier ID: " << supplierID << " *\n" << RESET;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\n* SQL Error: " << e.what() << " *\n" << RESET;
    }
    _getch();
}

// Restock stock
void Supplier::reStock() {

    cout << "\n\n*-----------------------------------------------------------------*\n";
    cout << "|                                                                 |\n";
    cout << "|                              RESTOCK                            |\n";
    cout << "|                                                                 |\n";
    cout << "*-----------------------------------------------------------------*\n";

    string supplierID = loggedInSupplierID;
    string stockID, supplyDate;
    int quantitySupply, newTransactionID;

    cout << "\n- Enter stock ID (S001): ";
    cin.ignore();
    getline(cin, stockID);

    try {
        DBConnection dbConn;

        // Check if the stock ID exists and belongs to the supplier
        dbConn.prepareStatement(
            "SELECT stock_ID FROM supplier WHERE stock_ID = ? AND supplier_id = ?"
        );
        dbConn.stmt->setString(1, stockID);
        dbConn.stmt->setString(2, supplierID);
        dbConn.QueryResult();

        if (!dbConn.res->next()) {
            cout << RED << "\nError: The stock ID does not belong to the logged-in supplier or is invalid.\n" << RESET;
            return;
        }

    }
    catch (sql::SQLException& e) {
        cout << RED << "\nSQL error while validating stock ID: " << e.what() << RESET << endl;
        return;
    }

    cout << "\n- Enter quantity to restock: ";
    while (!(cin >> quantitySupply) || quantitySupply <= 0) {
        cout << YELLOW << "Invalid input. Please enter a positive integer: " << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "\n- Enter supply date (YYYY-MM-DD): ";
    cin.ignore();
    getline(cin, supplyDate);

    try {
        DBConnection dbConn;

        // Auto-increment transaction ID
        dbConn.prepareStatement("SELECT MAX(transaction_id) AS maxID FROM transaction");
        dbConn.QueryResult();

        if (dbConn.res->next()) {
            newTransactionID = dbConn.res->getInt("maxID") + 1;
        }
        else {
            newTransactionID = 1;
        }

        // Check supplier stock quantity
        dbConn.prepareStatement("SELECT supplier_stock_quantity FROM supplier WHERE supplier_id = ?");
        dbConn.stmt->setString(1, supplierID);
        dbConn.QueryResult();

        if (dbConn.res->next()) {
            int currentSupplierStock = dbConn.res->getInt("supplier_stock_quantity");

            if (currentSupplierStock < quantitySupply) {
                cout << RED << "\nError: Restock quantity exceeds the available stock of the supplier.\n" << RESET;
            }
            else {
                    // Record does not exist, perform an insert
                    dbConn.prepareStatement(
                        "INSERT INTO transaction (transaction_id,stock_ID, supplier_id, quantity_supply, supply_date, status) "
                        "VALUES ( ?, ?, ?, ?, ?, 'pending')"
                    );
                    dbConn.stmt->setInt(1, newTransactionID);
                    dbConn.stmt->setString(2, stockID);
                    dbConn.stmt->setString(3, supplierID);
                    dbConn.stmt->setInt(4, quantitySupply);
                    dbConn.stmt->setString(5, supplyDate);
                    dbConn.stmt->executeUpdate();

                    // Deduct quantity from supplier stock
                    dbConn.prepareStatement(
                        "UPDATE supplier SET supplier_stock_quantity = supplier_stock_quantity - ? WHERE supplier_id = ?"
                    );
                    dbConn.stmt->setInt(1, quantitySupply);
                    dbConn.stmt->setString(2, supplierID);
                    dbConn.stmt->executeUpdate();

                    cout << GREEN << "\nNew stock record added successfully!" << endl << RESET;
                }
            }

        else {
            cout << RED << "\nError: Supplier ID not found or does not belong to the logged-in user.\n" << RESET;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\nSQL error while processing restock: " << e.what() << RESET << endl;
    }

    cout << "*-----------------------------------------------------------------*\n";
    _getch();
}

// Receive stock from return
void Supplier::receiveStock() {

    cout << "\n\n*------------------------------------------------------*\n";
    cout << "|                                                      |\n";
    cout << "|                  RECEIVE STOCK                       |\n";
    cout << "|                                                      |\n";
    cout << "*------------------------------------------------------*\n";

    string supplierID = loggedInSupplierID;
    string stockID;
    int quantityReceive;

    cout << "\n- Enter stock ID (S001): ";
    cin.ignore();
    getline(cin, stockID);

    try {
        DBConnection dbConn;

        // Check for pending transactions for the given stock ID and supplier ID
        dbConn.prepareStatement(
            "SELECT SUM(quantity_return) AS total_quantity, COUNT(*) AS pending_count, "
            "MAX(status = 'received') AS already_received "
            "FROM transaction "
            "WHERE stock_ID = ? AND supplier_id = ?"
        );
        dbConn.stmt->setString(1, stockID);
        dbConn.stmt->setString(2, supplierID);
        dbConn.QueryResult();

        if (dbConn.res->next()) {
            int totalQuantity = dbConn.res->getInt("total_quantity");
            int pendingCount = dbConn.res->getInt("pending_count");
            bool alreadyReceived = dbConn.res->getInt("already_received");

            // Check if there are no pending restock requests
            if (alreadyReceived) {
                cout << RED << "\nCannot receive this stock because it has already been received for the given Stock ID and Supplier ID." << RESET << endl;
                return;
            }

            if (pendingCount == 0) {
                cout << YELLOW << "\nNo pending restock requests found for the given Stock ID and Supplier ID." << RESET << endl;
                return;
            }

            // Update stock quantity
            dbConn.prepareStatement("UPDATE supplier SET supplier_stock_quantity = supplier_stock_quantity + ? WHERE supplier_id = ?");
            dbConn.stmt->setInt(1, totalQuantity);
            dbConn.stmt->setString(2, supplierID);
            dbConn.stmt->executeUpdate();

            // Mark all pending transactions as received
            dbConn.prepareStatement("UPDATE transaction SET status = 'received' WHERE stock_ID = ? AND supplier_id = ? AND status = 'pending'");
            dbConn.stmt->setString(1, stockID);
            dbConn.stmt->setString(2, supplierID);
            dbConn.stmt->executeUpdate();

            cout << GREEN << "\nStock received successfully! Total quantity added: " << totalQuantity << RESET << endl;
        }
        else {
            cout << YELLOW << "\nNo pending restock requests found for the given Stock ID and Supplier ID." << RESET << endl;
        }
    }
    catch (sql::SQLException& e) {
        cout << RED << "\nSQL error while receiving stock: " << e.what() << RESET << endl;
    }

    cout << "*------------------------------------------------------*\n";
    _getch();
}