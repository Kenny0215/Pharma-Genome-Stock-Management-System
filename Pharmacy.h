#ifndef PHARMACY_H
#define PHARMACY_H

#include <iostream>
#include <string>
#include <mysql/jdbc.h>
#include "DB.h"

using namespace std;

// Pharmacy class to manage the stock operations
class Admin {

private:
    DBConnection db;

public:

    Admin();  // Constructor
    ~Admin() {} // Destructor

    // Operations
    void addItem();
    void removeItem();
    void displayItems();
    void searchItem();

    // Modify In-details
    void modifyItemName();
    void modifyItemQuantity();
    void modifyItemPrice();
    void modifyItemExpiryDate();
    void modifyItemCategoryID();

    // Stock handling
    void receiveStock();
    void returnStock();

    // Stock Report Functions
    void viewStockReport();
    void viewLowStockReport();
    void viewExpiredAndExpiringSoonReport();
    void viewCategoryReport();
};

// Account class to manage register and login operations
class Account {

private:

    string userID, loginUsername, loginPassword;
    string password, supplierID;

    DBConnection db;


public:

    // Constructor
    Account(const string& user, const string& pass) : password(pass) {}

    // Default constructor
    Account() {}

    string getPasswordInput();

    void registerAdmin();
    bool loginAdmin();
    void exitAdmin();

    void registerUser();
    bool loginUser();
    string getUserID();
    void exitUser();
};

// User class to manage stock
class Supplier {

private:

    string loggedInUserID;
    string loggedInSupplierID;
    DBConnection db;

public:

    Supplier();  // Constructor
    ~Supplier() {} // Destructor

    void setSupplierID(string userID);

    // Supplier CRUD functions
    void addSupplier(string userID);
    void removeSupplier();
    void displaySupplier();
    void displayStockDetails();

    // Modify In-details
    void modifySupplierName();
    void modifySupplierStockQuantity();

    // Supplier operations
    void reStock();
    void receiveStock();
    void traceStockMovement();

};

// Category class to manage category items
class Category {

private:
    DBConnection db;

public:
    Category() {}
    ~Category() {}

    void addCategory();
    void removeCategory();
    void modifyCategory();
    void displayCategory();
    void searchCategory();
};
#endif;