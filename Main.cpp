#include <iostream>
#include <string>
#include <iomanip>
#include <conio.h>
#include <windows.h> 
#include <limits>
#include <mysql/jdbc.h>
#include "Pharmacy.h"

using namespace std;

// Define color codes
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string CYAN = "\033[36m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";

// Function prototypes
void mainMenu(Admin& admin, Category& category);
void stockManagementMenu(Admin& admin);
void categoryManagementMenu(Category& category);
void reportGenerationMenu(Admin& admin);
void modifyItemMenu(Admin& admin);
void supplierManagementMenu(Supplier& supplier, string userID);
void modifySupplierMenu(Supplier& supplier);

// Function for the login and registration menu
void menu() {

    int choice;
    string username, password;
    Admin admin;
    Account account;
    Category category;
    Supplier supplier;

    do {
        system("cls");

        cout << GREEN << "*-----------------------------------------------*" << endl;
        cout << "|                                               |" << endl;
        cout << "*                                               *" << endl;
        cout << "|                                               |" << endl;
        cout << "*                  LOGIN MENU                   *" << endl;
        cout << "|                                               |" << endl;
        cout << "*                                               *" << endl;
        cout << "|_______________________________________________|" << endl;
        cout << "|                                               |" << RESET << endl;
        cout << YELLOW << "*   1. Admin                                    *" << RESET << endl;
        cout << GREEN << "|                                               |" << RESET << endl;
        cout << CYAN << "*   2. User                                     *" << RESET << endl;
        cout << GREEN << "|                                               |" << RESET << endl;
        cout << RED << "*   3. Exit                                     *" << RESET << endl;
        cout << GREEN << "|                                               |" << RESET << endl;
        cout << GREEN << "*-----------------------------------------------*" << RESET << endl;
        cout << "\nEnter your identity: ";
        int identity;
        cin >> identity;

        // Error Handling (Role Selection System)
        // Validate input
        if (cin.fail()) {
            cin.clear();              
            cin.ignore(INT_MAX, '\n');
            cout << RED << "\n*-----------------------------*\n";
            cout << "|                             |\n";
            cout << "|  Invalid choice. Try again! |\n";
            cout << "|                             |\n";
            cout << "*-----------------------------*\n" << RESET << endl;
            system("pause");
            continue;
        }

        if (identity == 1) {
            // Admin Menu
            do {
                system("cls");

                cout << YELLOW << "*-----------------------------------------------*" << endl;
                cout << "|                                               |" << endl;
                cout << "*                 ADMIN LOGIN                   *" << endl;
                cout << "|                                               |" << endl;
                cout << "*_______________________________________________*" << endl;
                cout << "|                                               |" << endl;
                cout << "*   1. Login                                    *" << endl;
                cout << "|                                               |" << endl;
                cout << "*   2. Register                                 *" << endl;
                cout << "|                                               |" << endl;
                cout << "*   3. Exit                                     *" << endl;
                cout << "|                                               |" << endl;
                cout << "*-----------------------------------------------*" << RESET << endl;
                cout << "\nEnter your choice: ";
                cin >> choice;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    cout << RED << "\nInvalid input! Please enter a number between 1 and 3." << RESET << endl;
                    system("pause");
                    continue;
                }

                switch (choice) {
                case 1:
                    cin.ignore();
                    if (account.loginAdmin()) {
                        string getPasswordInput();
                        cout << GREEN <<"\n*--------------------*\n";
                        cout << "|                    |\n";
                        cout << "|  Login successful! |\n";
                        cout << "|                    |\n";
                        cout << "*--------------------*\n" << RESET;
                        _getch();
                        mainMenu(admin, category);
                    }
                    else {
                        cout << YELLOW << "\n*------------------------------------------------*\n";
                        cout << "|                                                |\n";
                        cout << "|  Login failed. Incorrect username or password. |\n";
                        cout << "|                                                |\n";
                        cout << "*------------------------------------------------*\n" << RESET;
                        _getch();
                    }
                    break;
                case 2:
                    account.registerAdmin();
                    string getPasswordInput();
                    break;
                case 3:
                    break; // Exit to identity menu
                default :

                    cout << RED << "\n*-----------------------------*\n";
                    cout << "|                             |\n";
                    cout << "|  Invalid choice. Try again! |\n";
                    cout << "|                             |\n";
                    cout << "*-----------------------------*\n" << RESET;
                    _getch();

                }
            } while (choice != 3);
        }
        else if (identity == 2) {

            // User Menu
            do {
                system("cls");

                cout << CYAN << "*-----------------------------------------------*" << endl;
                cout << "|                                               |" << endl;
                cout << "*                 USER LOGIN                    *" << endl;
                cout << "|                                               |" << endl;
                cout << "*_______________________________________________*" << endl;
                cout << "|                                               |" << endl;
                cout << "*   1. Login                                    *" << endl;
                cout << "|                                               |" << endl;
                cout << "*   2. Register                                 *" << endl;
                cout << "|                                               |" << endl;
                cout << "*   3. Exit                                     *" << endl;
                cout << "|                                               |" << endl;
                cout << "*-----------------------------------------------*" << RESET << endl;
                cout << "Enter your choice: ";
                cin >> choice;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(INT_MAX, '\n');
                    cout << RED << "\nInvalid input! Please enter a number between 1 and 3." << RESET << endl;
                    system("pause");
                    continue;
                }


                switch (choice) {
                case 1:
                    cin.ignore();
                    if (account.loginUser()) {
                        string userID = account.getUserID();
                        cout << GREEN << "\n*--------------------*\n";
                        cout << "|                    |\n";
                        cout << "|  Login successful! |\n";
                        cout << "|                    |\n";
                        cout << "*--------------------*\n" << RESET;
                        _getch();
                        supplierManagementMenu(supplier, userID); // Direct to supplier menu
                    }
                    else {

                        cout << YELLOW << "\n*------------------------------------------------*\n";
                        cout << "|                                                |\n";
                        cout << "|  Login failed. Incorrect User ID or password.  |\n";
                        cout << "|                                                |\n";
                        cout << "*------------------------------------------------*\n" << RESET;
                        _getch();
                    }
                    break;
                case 2:
                    account.registerUser();
                    string getPasswordInput();
                    break;
                case 3:
                    break; // Exit to identity menu

                default:
                    cout << RED << "\n*-----------------------------*\n";
                    cout << "|                             |\n";
                    cout << "|  Invalid choice. Try again! |\n";
                    cout << "|                             |\n";
                    cout << "*-----------------------------*\n" << RESET;
                    _getch();
                }

            } while (choice != 3);
        }

        else if (identity == 3) {

            cout << GREEN << "\n*---------------------------*\n";
            cout << "|                           |\n";
            cout << "|  Exiting system. Goodbye! |\n";
            cout << "|                           |\n";
            cout << "*---------------------------*" << RESET;
            return;
        }
        else {

            cout << RED << "\n*-----------------------------*\n";
            cout << "|                             |\n";
            cout << "|  Invalid input. Try again!  |\n";
            cout << "|                             |\n";
            cout << "*-----------------------------*\n" << RESET;
            system("pause");
        }
    } while (true);
}

// Main menu for the system
void mainMenu(Admin& admin, Category& category) {
    int choice;

    do {
        system("cls");

        cout << YELLOW << "*----------------------------------------------*\n";
        cout << "|                                              |\n";
        cout << "|             PHARMA_GENOME_SYSTEM             |\n";
        cout << "|                                              |\n";
        cout << "*----------------------------------------------*\n";
        cout << "|                                              |\n" << RESET;
        cout << CYAN << "| 1. Stock Management                          |\n" << RESET;
        cout << YELLOW << "|                                              |\n" << RESET;
        cout << BLUE << "| 2. Category Management                       |\n" << RESET;
        cout << YELLOW << "|                                              |\n" << RESET;
        cout << GREEN << "| 3. Report Generation                         |\n" << RESET;
        cout << YELLOW << "|                                              |\n" << RESET;
        cout << RED << "| 4. Logout                                    |\n" << RESET;
        cout << YELLOW << "|                                              |\n";
        cout << "*----------------------------------------------*\n" << RESET;
        cout << "\nEnter your choice: ";
        cin >> choice;

        // Error handling (Choice Selection)
        if (cin.fail()) {
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            cout << RED << "\nInvalid input! Please enter a number between 1 and 4." << RESET << endl;
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
            stockManagementMenu(admin);
            break;
        case 2:
            categoryManagementMenu(category);
            break;
        case 3:
            reportGenerationMenu(admin);
            break;
        case 4:
            cout << GREEN << "\nLogging out..." << endl << RESET;
            _getch();
            return;

        default:
            cout << RED << "\nInvalid choice. Try again!" << RESET << endl;
            _getch();
        }
    } while (true);
}

// Stock management menu
void stockManagementMenu(Admin& admin) {
    int choice;

    Supplier supplier;

    do {
        system("cls");

        cout << CYAN<< "*----------------------------------------------*\n";
        cout << "|                                              |\n";
        cout << "|              STOCK MANAGEMENT                |\n";
        cout << "|                                              |\n";
        cout << "*----------------------------------------------*\n";
        cout << "|                                              |\n";
        cout << "| 1. Add Item                                  |\n";
        cout << "|                                              |\n";
        cout << "| 2. Remove Item                               |\n";
        cout << "|                                              |\n";
        cout << "| 3. Modify Item                               |\n";
        cout << "|                                              |\n";
        cout << "| 4. Display Items                             |\n";
        cout << "|                                              |\n";
        cout << "| 5. Search Item                               |\n";
        cout << "|                                              |\n";
        cout << "| 6. Receive stock from Supplier               |\n";
        cout << "|                                              |\n";
        cout << "| 7. Return stock                              |\n";
        cout << "|                                              |\n";
        cout << "| 8. Back to Main Menu                         |\n";
        cout << "|                                              |\n";
        cout << "*----------------------------------------------*\n" << RESET;
        cout << "\nEnter your choice: ";
        cin >> choice;

         // Error handling (Choice Selection)
        if (cin.fail()) {
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            cout << RED << "\nInvalid input! Please enter a number between 1 and 8." << RESET << endl;
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
            system("cls");
            admin.displayItems();
            admin.addItem();
            admin.displayItems();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 2:
            system("cls");
            admin.displayItems();
            admin.removeItem();
            admin.displayItems();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 3:
            modifyItemMenu(admin);
            break;
        case 4:
            system("cls");
            admin.displayItems();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 5:
            system("cls");
            admin.searchItem();
            cout << "\nPress any key to return...";
            break;
        case 6:
            system("cls");
            cout << YELLOW << "** BEFORE OPERATION **" << endl << RESET;
            supplier.displayStockDetails();
            admin.displayItems();
            admin.receiveStock();
            cout << "\n" << GREEN << "** AFTER OPERATION **"  << RESET << endl;
            cout << "\n\n" << CYAN << "~ NOTE 1: Stock from supplier will be received and status of transaction is marked as 'RECEIVED'." << endl;
            cout << "\n\n" << "~ NOTE 2: Remain unchanged if error is detected" << RESET << endl;
            supplier.displayStockDetails();
            cout << "\n" << YELLOW <<"** ADDITION OF STOCK QUANTITY PERFORMED IF SUCCESSFULLY OPERATED" << RESET << endl;
            admin.displayItems();
            cout << GREEN << "Press any key to return..." << endl << RESET;
            _getch();
            break;
        case 7:
            system("cls");
            cout << YELLOW << "** BEFORE OPERATION **" << endl << RESET; 
            supplier.displayStockDetails();
            admin.displayItems();
            admin.returnStock();
            cout << "\n" << GREEN << "** AFTER OPERATION **" << RESET << endl;
            cout << "\n" << CYAN << "~ NOTE 1: Status will be marked as 'PENDING' during this operation." << endl;
            cout << "\n" << "~ NOTE 2: Remain unchanged if error is detected" << RESET << endl;
            supplier.displayStockDetails();
            cout << "\n" << RED << "** DEDUCTION OF STOCK QUANTITY PERFORMED IF SUCCESSFULLY OPERATED" << RESET << endl;
            admin.displayItems();
            cout << GREEN << "Press any key to return..." << endl << RESET;
            _getch();
            break;
        case 8:
            return;
        default:
            cout << RED << "Invalid choice. Try again!" << endl << RESET;
            _getch();
        }
    } while (true);
}

// Item Modify menu
void modifyItemMenu(Admin& admin) {

    int choice;

    do {
        system("cls");

        cout << GREEN << "*----------------------------------------------*\n";
        cout << "|                                              |\n";
        cout << "|              MODIFY ITEM DETAILS             |\n";
        cout << "|                                              |\n";
        cout << "*----------------------------------------------*\n";
        cout << "|                                              |\n";
        cout << "| 1. Modify Item Name                          |\n";
        cout << "|                                              |\n";
        cout << "| 2. Modify Item Quantity                      |\n";
        cout << "|                                              |\n";
        cout << "| 3. Modify Item Price                         |\n";
        cout << "|                                              |\n";
        cout << "| 4. Modify Item Expiry Date                   |\n";
        cout << "|                                              |\n";
        cout << "| 5. Modify Item Category ID                   |\n";
        cout << "|                                              |\n";
        cout << "| 6. Back to Stock Management Menu             |\n";
        cout << "|                                              |\n";
        cout << "*----------------------------------------------*\n" << RESET;
        cout << "\nEnter your choice: ";
        cin >> choice;

        // Error handling (Choice Selection)
        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            cout << RED << "\nInvalid input! Please enter a number between 1 and 6." << RESET << endl;
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
            admin.displayItems();
            admin.modifyItemName();
            admin.displayItems();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 2:
            admin.displayItems();
            admin.modifyItemQuantity();
            admin.displayItems();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 3:
            admin.displayItems();
            admin.modifyItemPrice();
            admin.displayItems();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 4:
            admin.displayItems();
            admin.modifyItemExpiryDate();
            admin.displayItems();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 5:
            admin.displayItems();
            admin.modifyItemCategoryID();
            admin.displayItems();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 6:
            return;
        default:
            cout << RED << "Invalid choice. Try again!" << endl << RESET;
            _getch();
        }
    } while (true);
}

// Category management menu
void categoryManagementMenu(Category& category) {
    int choice;

    do {
        system("cls");

        cout << BLUE << "*----------------------------------------------*\n";
        cout << "|                                              |\n";
        cout << "|             CATEGORY MANAGEMENT              |\n";
        cout << "|                                              |\n";
        cout << "*----------------------------------------------*\n";
        cout << "|                                              |\n";
        cout << "| 1. Add Category                              |\n";
        cout << "|                                              |\n";
        cout << "| 2. Remove Category                           |\n";
        cout << "|                                              |\n";
        cout << "| 3. Modify Category                           |\n";
        cout << "|                                              |\n";
        cout << "| 4. Display Category                          |\n";
        cout << "|                                              |\n";
        cout << "| 5. Search Category                           |\n";
        cout << "|                                              |\n";
        cout << "| 6. Back to Main Menu                         |\n";
        cout << "|                                              |\n";
        cout << "*----------------------------------------------*\n" << RESET;
        cout << "\nEnter your choice: ";
        cin >> choice;

        // Error handling (Choice Selection)
        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            cout << RED << "\nInvalid input! Please enter a number between 1 and 6." << RESET << endl;
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
            system("cls");
            category.displayCategory();
            category.addCategory();
            category.displayCategory();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 2:
            system("cls");
            category.displayCategory();
            category.removeCategory();
            category.displayCategory();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 3:
            system("cls");
            category.displayCategory();
            category.modifyCategory();
            category.displayCategory();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 4:
            system("cls");
            category.displayCategory();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 5:
            system("cls");
            category.searchCategory();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 6:
            return;
        default:
            cout << RED << "\nInvalid choice. Try again!" << endl << RESET;
            _getch();
        }
    } while (true);
}

// Supplier management menu
void supplierManagementMenu(Supplier& supplier, string userID) {
    int choice;
    Admin admin;

    supplier.setSupplierID(userID);

    do {
        system("cls");

        cout << CYAN << "*-----------------------------------------------------*\n";
        cout << "|                                                     |\n";
        cout << "|                    SUPPLIER MENU                    |\n";
        cout << "|                                                     |\n";
        cout << "*-----------------------------------------------------*\n";
        cout << "|                                                     |\n";
        cout << "| 1. Add Supplier Details                             |\n";
        cout << "|                                                     |\n";
        cout << "| 2. Remove Supplier Details                          |\n";
        cout << "|                                                     |\n";
        cout << "| 3. Modify Supplier Details                          |\n";
        cout << "|                                                     |\n";
        cout << "| 4. Display Supplier Details                         |\n";
        cout << "|                                                     |\n";
        cout << "| 5. Display Transaction Details                      |\n";
        cout << "|                                                     |\n";
        cout << "| 6. Restock                                          |\n";
        cout << "|                                                     |\n";
        cout << "| 7. Receive stock from Admin                         |\n";
        cout << "|                                                     |\n";
        cout << "| 8. Trace stock movement                             |\n";
        cout << "|                                                     |\n";
        cout << "| 9. Exit system                                      |\n";
        cout << "|                                                     |\n";
        cout << "*-----------------------------------------------------*\n" << RESET;
        cout << "\nEnter your choice: ";
        cin >> choice;

        // Error handling (Choice Selection)
        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            cout << RED << "\nInvalid input! Please enter a number between 1 and 9." << RESET << endl;
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
            system("cls");
            supplier.displaySupplier();
            supplier.addSupplier(userID);
            supplier.displaySupplier();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 2:
            system("cls");
            supplier.displaySupplier();
            supplier.removeSupplier();
            supplier.displaySupplier();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 3:
            modifySupplierMenu(supplier);
            break;
        case 4:
            system("cls");
            supplier.displaySupplier();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 5:
            system("cls");
            supplier.displayStockDetails();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 6:
            system("cls");
            cout << YELLOW <<"** BEFORE OPERATION **" << endl << RESET;
            supplier.displayStockDetails();
            supplier.displaySupplier();
            admin.displayItems();
            supplier.reStock();
            cout << GREEN << "\n** AFTER OPERATION **" << RESET;
            cout << CYAN << "\n\n" << "~ NOTE 1: Status will be marked as 'PENDING' during this operation." << endl;
            cout <<  "\n" << "~ NOTE 2: Remain unchanged if error is detected." << RESET << endl;
            supplier.displayStockDetails();
            cout << RED << "\n** DEDUCTION OF SUPPLIER STOCK QUANTITY PERFORMED IF SUCCESSFULLY OPERATED" << endl << RESET;
            supplier.displaySupplier();
            cout << GREEN << "Press any key to return..." <<  RESET << endl;
            _getch();
            break;
        case 7:
            system("cls");
            cout << YELLOW << "** BEFORE OPERATION **" << endl << RESET;
            supplier.displayStockDetails();
            supplier.displaySupplier();
            supplier.receiveStock();
            cout << GREEN << "\n** AFTER OPERATION **" << RESET << endl;
            cout << "\n\n" << CYAN << "~ NOTE 1: Stock from admin will be received and status of transaction is marked as 'RECEIVED'" << endl;
            cout << "\n" << "~ NOTE 2: Remain unchanged if error is detected." << RESET << endl;
            supplier.displayStockDetails();
            cout << YELLOW << "\n** ADDITION OF SUPPLIER STOCK QUANTITY PERFORMED IF SUCCESSFULLY OPERATED" << endl << RESET;
            supplier.displaySupplier();
            cout << GREEN << "Press any key to return..." << RESET << endl;
            _getch();
            break;
        case 8:
            system("cls");
            supplier.traceStockMovement();
            cout << "Press any key to return..." << endl;
            _getch();
            break;
        case 9:
            return;

        default:
            cout << RED << "\nInvalid choice. Try again!" << endl << RESET;
            _getch();
        }
    } while (true);
}

// Supplier Modify Menu
void modifySupplierMenu(Supplier& supplier) {
    int choice;

    do {
        system("cls");

        cout << GREEN << "*---------------------------------------------------------*\n";
        cout << "|                                                         |\n";
        cout << "|                  MODIFY SUPPLIER DETAILS                |\n";
        cout << "|                                                         |\n";
        cout << "*---------------------------------------------------------*\n";
        cout << "|                                                         |\n";
        cout << "| 1. Modify Supplier Name                                 |\n";
        cout << "|                                                         |\n";
        cout << "| 2. Modify Supplier Stock Quantity                       |\n";
        cout << "|                                                         |\n";
        cout << "| 3. Back to menu                                         |\n";
        cout << "|                                                         |\n";
        cout << "*---------------------------------------------------------*\n" << RESET;
        cout << "\nEnter your choice: ";
        cin >> choice;

        // Error handling (Choice Selection)
        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            cout << RED << "\nInvalid input! Please enter a number between 1 and 3." << RESET << endl;
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
            system("cls");
            supplier.displaySupplier();
            supplier.modifySupplierName();
            supplier.displaySupplier();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 2:
            system("cls");
            supplier.displaySupplier();
            supplier.modifySupplierStockQuantity();
            supplier.displaySupplier();
            cout << "\nPress any key to return...";
            _getch();
            break;
        case 3:
            return;

        default:
            cout << RED << "\nInvalid choice. Try again!" << endl << RESET;
            _getch();
        }
    } while (true);
}

// Report generation menu
void reportGenerationMenu(Admin& admin) {
    int choice;

    do {
        system("cls");

        cout << GREEN << "*----------------------------------------------*\n";
        cout << "|                                              |\n";
        cout << "|               REPORT GENERATION              |\n";
        cout << "|                                              |\n";
        cout << "*----------------------------------------------*\n";
        cout << "|                                              |\n";
        cout << "| 1. View Stock Report                         |\n";
        cout << "|                                              |\n";
        cout << "| 2. View Low Stock Report                     |\n";
        cout << "|                                              |\n";
        cout << "| 3. View Expired & Expiring Soon Report       |\n";
        cout << "|                                              |\n";
        cout << "| 4. View Most Favourable Category             |\n";
        cout << "|                                              |\n";
        cout << "| 5. Back to Main Menu                         |\n";
        cout << "*----------------------------------------------*\n" << RESET;
        cout << "\nEnter your choice: ";
        cin >> choice;

        // Error handling (Choice Selection)
        // Validate input
        if (cin.fail()) {
            cin.clear();
            cin.ignore(INT_MAX, '\n');
            cout << RED << "\nInvalid input! Please enter a number between 1 and 3." << RESET << endl;
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
            admin.viewStockReport();
            break;
        case 2: {
            admin.viewLowStockReport();
            break;
        }
        case 3: {
            admin.viewExpiredAndExpiringSoonReport();
            break;
        }
        case 4:
            admin.viewCategoryReport();
            break;
        case 5:
            return;
        default:
            cout << RED << "\nInvalid choice. Try again!" << endl << RESET;
            _getch();
        }
    } while (true);
}

int main() {
    menu();
}