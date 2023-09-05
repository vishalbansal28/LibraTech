#include <iostream>
#include<bits/stdc++.h>
#include<windows.h>
#include <mysql.h>

using namespace std;

bool exitProgram = false;


void connectToDatabase(MYSQL *connection)
{
    if (!mysql_real_connect(connection, "localhost", "root", "", "book_management_store", 0, NULL, 0))
    {
        std::cerr << "Error: Failed to connect to database: " << mysql_error(connection) << std::endl;
        exit(1);
    }
}
void dropBooksTable(MYSQL *connection)
{
    // Disable foreign key checks
    std::string disableFKChecksQuery = "SET FOREIGN_KEY_CHECKS = 0";
    if (mysql_query(connection, disableFKChecksQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to disable foreign key checks: " << mysql_error(connection) << std::endl;
        exit(1);
    }

    // Drop the table
    std::string dropTableQuery = "DROP TABLE IF EXISTS Books";
    if (mysql_query(connection, dropTableQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to drop table: " << mysql_error(connection) << std::endl;
        exit(1);
    }

    // Enable foreign key checks
    std::string enableFKChecksQuery = "SET FOREIGN_KEY_CHECKS = 1";
    if (mysql_query(connection, enableFKChecksQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to enable foreign key checks: " << mysql_error(connection) << std::endl;
        exit(1);
    }
}


void insertBook(MYSQL *connection)
{
    std::string title, author, genre, isbn;
    double price;
    int quantity;

    std::cout << "Enter book title: ";
    std::getline(std::cin, title);

    std::cout << "Enter book author: ";
    std::getline(std::cin, author);

    std::cout << "Enter book genre: ";
    std::getline(std::cin, genre);

    std::cout << "Enter book ISBN: ";
    std::getline(std::cin, isbn);

    std::cout << "Enter book price: ";
    std::cin >> price;

    std::cout << "Enter book quantity: ";
    std::cin >> quantity;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string insertBookQuery = "INSERT INTO Books (Title, Author, Genre, ISBN, Price, Quantity) "
                                  "VALUES ('" + title + "', '" + author + "', '" + genre + "', '" + isbn + "', " + std::to_string(price) + ", " + std::to_string(quantity) + ")";
    if (mysql_query(connection, insertBookQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to insert book: " << mysql_error(connection) << std::endl;
        exit(1);
    }
}

void selectBooks(MYSQL *connection)
{
    std::string selectBooksQuery = "SELECT * FROM Books";
    if (mysql_query(connection, selectBooksQuery.c_str()) == 0)
    {
        MYSQL_RES *result = mysql_store_result(connection);
        if (result != NULL)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result)))
            {
                for (int i = 0; i < mysql_num_fields(result); i++)
                {
                    std::cout << row[i] << " ";
                }
                std::cout << std::endl;
            }
            mysql_free_result(result);
        }
    }
    else
    {
        std::cerr << "Error: Failed to retrieve books: " << mysql_error(connection) << std::endl;
        exit(1);
    }
}

void updateBook(MYSQL *connection)
{
    std::string title;
    double price;

    std::cout << "Enter book title to update: ";
    std::getline(std::cin, title);

    std::cout << "Enter new price: ";
    std::cin >> price;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string updateBookQuery = "UPDATE Books SET Price = " + std::to_string(price) + " WHERE Title = '" + title + "'";
    if (mysql_query(connection, updateBookQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to update book: " << mysql_error(connection) << std::endl;
        exit(1);
    }
}


//data integrity
void deleteBook(MYSQL* connection)
{
    int quantity;

    std::cout << "Enter quantity limit for deletion: ";
    std::cin >> quantity;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Delete records from the Inventory table first
    std::string deleteInventoryQuery = "DELETE FROM Inventory WHERE BookID IN (SELECT Id FROM Books WHERE Quantity <= " + std::to_string(quantity) + ")";
    if (mysql_query(connection, deleteInventoryQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to delete inventory records: " << mysql_error(connection) << std::endl;
        exit(1);
    }

    // Delete the book from the Books table
    std::string deleteBookQuery = "DELETE FROM Books WHERE Quantity <= " + std::to_string(quantity);
    if (mysql_query(connection, deleteBookQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to delete book: " << mysql_error(connection) << std::endl;
        exit(1);
    }
}



// Step 4: Implement relationships between entities
void createTables(MYSQL* connection)
{
    //Book Table
    std::string createTableQuery = "CREATE TABLE Books ("
                                   "Id INT AUTO_INCREMENT PRIMARY KEY ON DELETE CASCADE,"
                                   "Title VARCHAR(255) NOT NULL,"
                                   "Author VARCHAR(255) NOT NULL,"
                                   "Genre VARCHAR(255) NOT NULL,"
                                   "ISBN VARCHAR(20) NOT NULL,"
                                   "Price DECIMAL(10, 2) NOT NULL,"
                                   "Quantity INT NOT NULL"
                                   ")";
    if (mysql_query(connection, createTableQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to create table: " << mysql_error(connection) << std::endl;
        exit(1);
    }



    //Customer Table
    std::string createCustomersTableQuery = "CREATE TABLE Customers ("
                                            "Id INT AUTO_INCREMENT PRIMARY KEY,"
                                            "Name VARCHAR(255) NOT NULL,"
                                            "Email VARCHAR(255) NOT NULL"
                                            ")";
    if (mysql_query(connection, createCustomersTableQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to create customers table: " << mysql_error(connection) << std::endl;
        return;
    }
//Reviews Table

   std::string createReviewsTableQuery = "CREATE TABLE Reviews ("
                                          "ReviewID INT PRIMARY KEY AUTO_INCREMENT, "
                                          "BookID INT, "
                                          "CustomerID INT, "
                                          "Rating INT, "
                                          "Comment VARCHAR(255), "
                                          "FOREIGN KEY (BookID) REFERENCES Books(Id) ON DELETE CASCADE, "
                                          "FOREIGN KEY (CustomerID) REFERENCES Customers(Id) ON DELETE CASCADE"
                                          ")";
    if (mysql_query(connection, createReviewsTableQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to create Reviews table: " << mysql_error(connection) << std::endl;
        return;
    }
    std::cout << "Reviews table created successfully." << std::endl;

      //Orders Table
std::string createOrdersTableQuery = "CREATE TABLE Orders ("
                                     "Id INT AUTO_INCREMENT PRIMARY KEY,"
                                     "CustomerId INT NOT NULL,"
                                     "BookId INT NOT NULL,"
                                     "Quantity INT NOT NULL,"
                                     "OrderDate DATE NOT NULL,"
                                     "FOREIGN KEY (CustomerId) REFERENCES Customers(Id) ON DELETE CASCADE,"
                                     "FOREIGN KEY (BookId) REFERENCES Books(Id) ON DELETE CASCADE"
                                     ")";

    if (mysql_query(connection, createOrdersTableQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to create orders table: " << mysql_error(connection) << std::endl;
        return;
    }

        //inventory table
        std::string createInventoryTableQuery = "CREATE TABLE Inventory ("
                                            "InventoryID INT PRIMARY KEY AUTO_INCREMENT, "
                                            "BookID INT, "
                                            "Quantity INT, "
                                            "FOREIGN KEY (BookID) REFERENCES Books(Id) ON DELETE CASCADE"
                                            ")";
    if (mysql_query(connection, createInventoryTableQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to create Inventory table: " << mysql_error(connection) << std::endl;
        return;
    }

    std::cout << "Tables created successfully." << std::endl;
}

//step 7
void viewOrderDetails(MYSQL* connection)
{
    int customerId;
    cout << "Enter Customer ID: ";
    cin >> customerId;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string selectOrderDetailsQuery = "SELECT O.Id, B.Title, O.Quantity, O.OrderDate "
                                     "FROM Orders O "
                                     "JOIN Books B ON O.BookId = B.Id "
                                     "WHERE O.CustomerId = " + to_string(customerId);

    if (mysql_query(connection, selectOrderDetailsQuery.c_str()) == 0)
    {
        MYSQL_RES* result = mysql_store_result(connection);
        if (result != NULL)
        {
            cout << "Order Details:" << endl;
            cout << "================" << endl;
            cout << "Order ID\tTitle\t\tQuantity\tOrder Date" << endl;

            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result)))
            {
                cout << row[0] << "\t\t" << row[1] << "\t" << row[2] << "\t\t" << row[3] << endl;
            }
            mysql_free_result(result);
        }
    }
    else
    {
        cerr << "Error: Failed to retrieve order details: " << mysql_error(connection) << endl;
    }
}

void viewBookReviews(MYSQL* connection)
{
    int bookId;
    cout << "Enter Book ID: ";
    cin >> bookId;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string selectReviewsQuery = "SELECT R.Rating, R.Comment, C.Name "
                                "FROM Reviews R "
                                "JOIN Customers C ON R.CustomerID = C.Id "
                                "WHERE R.BookID = " + to_string(bookId);

    if (mysql_query(connection, selectReviewsQuery.c_str()) == 0)
    {
        MYSQL_RES* result = mysql_store_result(connection);
        if (result != NULL)
        {
            cout << "Book Reviews:" << endl;
            cout << "================" << endl;
            cout << "Rating\t\tComment\t\t\tCustomer Name" << endl;

            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result)))
            {
                cout << row[0] << "\t\t" << row[1] << "\t\t" << row[2] << endl;
            }
            mysql_free_result(result);
        }
    }
    else
    {
        cerr << "Error: Failed to retrieve book reviews: " << mysql_error(connection) << endl;
    }
}

void addBookReview(MYSQL* connection)
{
    int bookId, customerId, rating;
    string comment;

    cout << "Enter Book ID: ";
    cin >> bookId;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter Customer ID: ";
    cin >> customerId;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter Rating (1-5): ";
    cin >> rating;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter Comment: ";
    getline(cin, comment);

    string insertReviewQuery = "INSERT INTO Reviews (BookID, CustomerID, Rating, Comment) "
                               "VALUES (" + to_string(bookId) + ", " + to_string(customerId) + ", "
                               + to_string(rating) + ", '" + comment + "')";

    if (mysql_query(connection, insertReviewQuery.c_str()) == 0)
    {
        cout << "Review added successfully." << endl;
    }
    else
    {
        cerr << "Error: Failed to add review: " << mysql_error(connection) << endl;
    }
}
void updateCustomerInformation(MYSQL* connection)
{
    int customerId;
    string newName, newEmail;

    cout << "Enter Customer ID: ";
    cin >> customerId;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter New Name: ";
    getline(cin, newName);

    cout << "Enter New Email: ";
    getline(cin, newEmail);

    string updateCustomerQuery = "UPDATE Customers SET Name = '" + newName + "', Email = '" + newEmail + "' "
                                 "WHERE Id = " + to_string(customerId);

    if (mysql_query(connection, updateCustomerQuery.c_str()) == 0)
    {
        cout << "Customer information updated successfully." << endl;
    }
    else
    {
        cerr << "Error: Failed to update customer information: " << mysql_error(connection) << endl;
    }
}


 // Step 10: Implement Transaction Management
void implementTransaction(MYSQL* connection)
{
    // Read customer ID
    int customerId;
    std::cout << "Enter Customer ID: ";
    std::cin >> customerId;

    // Read book ID
    int bookId;
    std::cout << "Enter Book ID: ";
    std::cin >> bookId;

    // Read quantity
    int quantity;
    std::cout << "Enter Quantity: ";
    std::cin >> quantity;

    // Start the transaction
    if (mysql_query(connection, "START TRANSACTION") != 0)
    {
        std::cerr << "Error: Failed to start transaction: " << mysql_error(connection) << std::endl;
        return;
    }

    // Retrieve current quantity from Books table
    std::string selectQuantityQuery = "SELECT Quantity FROM Books WHERE Id = " + std::to_string(bookId);
    if (mysql_query(connection, selectQuantityQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to retrieve current quantity: " << mysql_error(connection) << std::endl;
        mysql_query(connection, "ROLLBACK");
        return;
    }

    MYSQL_RES* result = mysql_store_result(connection);
    if (result == nullptr)
    {
        std::cerr << "Error: Failed to get result set: " << mysql_error(connection) << std::endl;
        mysql_query(connection, "ROLLBACK");
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int currentQuantity = std::stoi(row[0]);
    mysql_free_result(result);

    // Check if sufficient quantity is available
    if (quantity > currentQuantity)
    {
        std::cerr << "Error: Insufficient quantity available." << std::endl;
        mysql_query(connection, "ROLLBACK");
        return;
    }

    // Update the inventory quantity
    std::string updateInventoryQuery = "UPDATE Books SET Quantity = Quantity - " + std::to_string(quantity) + " WHERE Id = " + std::to_string(bookId);
    if (mysql_query(connection, updateInventoryQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to update inventory quantity: " << mysql_error(connection) << std::endl;
        mysql_query(connection, "ROLLBACK");
        return;
    }

    // Insert the order
    std::string insertOrderQuery = "INSERT INTO Orders (CustomerId, BookId, OrderDate) VALUES (" + std::to_string(customerId) + ", " + std::to_string(bookId) + ", CURDATE())";
    if (mysql_query(connection, insertOrderQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to insert order: " << mysql_error(connection) << std::endl;
        mysql_query(connection, "ROLLBACK");
        return;
    }

    // Commit the transaction
    if (mysql_query(connection, "COMMIT") != 0)
    {
        std::cerr << "Error: Failed to commit transaction: " << mysql_error(connection) << std::endl;
        mysql_query(connection, "ROLLBACK");
        return;
    }

    // Update the current quantity after the transaction
    currentQuantity -= quantity;
    std::string selectTitleQuery = "SELECT Title FROM Books WHERE Id = " + std::to_string(bookId);
    if (mysql_query(connection, selectTitleQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to retrieve book title: " << mysql_error(connection) << std::endl;
        return;
    }

    result = mysql_store_result(connection);
    if (result == nullptr)
    {
        std::cerr << "Error: Failed to get result set: " << mysql_error(connection) << std::endl;
        return;
    }

    row = mysql_fetch_row(result);
    std::string bookTitle = row[0];
    mysql_free_result(result);

    std::cout << "You have successfully bought " << quantity << " of the " << bookTitle << " book." << std::endl;

    std::cout << "Transaction completed successfully. Updated quantity: " << currentQuantity << std::endl;

}


 // Step 11: Implement Indexing
void implementIndexing(MYSQL* connection)
{
    // Create an index on the Title column of the Books table
    std::string createIndexQuery = "CREATE INDEX idx_title ON Books (Title)";
    if (mysql_query(connection, createIndexQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to create index: " << mysql_error(connection) << std::endl;
        return;
    }

    std::cout << "Index created successfully on the 'Title' column of the 'Books' table." << std::endl;
}
void displayMenu()
{
    cout << "===========================" << endl;
    cout << "Book Management System Menu" << endl;
    cout << "===========================" << endl;
    cout << "1. Insert a Book" << endl;
    cout << "2. Display all Books" << endl;
    cout << "3. Update a Book's Price" << endl;
    cout << "4. Delete Books with Quantity Limit" << endl;
    cout << "5. Implement Transaction" << endl;
    cout << "6. Implement Indexing" << endl;
    cout << "7. View Order Details" << endl;
    cout << "8. View Book Reviews" << endl;
    cout << "9. Add Book Review" << endl;
    cout << "10. Update Customer Information" << endl;
    cout << "11. Exit" << endl;
    cout << "===========================" << endl;
    cout << "Enter your choice: ";
}

int getUserChoice()
{
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return choice;
}
void handleUserChoice(int choice, MYSQL* connection)
{
    switch (choice)
    {
        case 1:
            insertBook(connection);
            break;
        case 2:
            selectBooks(connection);
            break;
        case 3:
            updateBook(connection);
            break;
        case 4:
            deleteBook(connection);
            break;
        case 5:
            implementTransaction(connection);
            break;
        case 6:
            implementIndexing(connection);
            break;
        case 7:
            viewOrderDetails(connection);
            break;
        case 8:
            viewBookReviews(connection);
            break;
        case 9:
            addBookReview(connection);
            break;
        case 10:
            updateCustomerInformation(connection);
            break;
        case 11:
            exitProgram = true;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
    }
}

bool isExistingUser(MYSQL* connection, const std::string& name, const std::string& email)
{
    std::string checkUserQuery = "SELECT * FROM Customers WHERE Name = '" + name + "' AND Email = '" + email + "'";
    if (mysql_query(connection, checkUserQuery.c_str()) == 0)
    {
        MYSQL_RES* result = mysql_store_result(connection);
        if (result != NULL)
        {
            bool exists = (mysql_num_rows(result) > 0);
            mysql_free_result(result);
            return exists;
        }
    }
    return false;
}

void addUser(MYSQL* connection, const std::string& name, const std::string& email)
{
    std::string addUserQuery = "INSERT INTO Customers (Name, Email) VALUES ('" + name + "', '" + email + "')";
    if (mysql_query(connection, addUserQuery.c_str()) != 0)
    {
        std::cerr << "Error: Failed to add user: " << mysql_error(connection) << std::endl;
        exit(1);
    }
}

void getUserInfo(MYSQL* connection)
{
    std::cout << "Are you an existing user? (Y/N): ";
    std::string answer;
    std::cin >> answer;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (answer == "Y" || answer == "y")
    {
        std::string name, email;
        std::cout << "Enter your name: ";
        std::getline(std::cin, name);

        std::cout << "Enter your email: ";
        std::getline(std::cin, email);

        if (isExistingUser(connection, name, email))
        {
            std::cout << "Welcome back, " << name << "!" << std::endl;

            std::string getCustomerIdQuery = "SELECT Id FROM Customers WHERE Name = '" + name + "' AND Email = '" + email + "'";
            if (mysql_query(connection, getCustomerIdQuery.c_str()) == 0)
            {
                MYSQL_RES* result = mysql_store_result(connection);
                if (result != NULL)
                {
                    MYSQL_ROW row = mysql_fetch_row(result);
                    std::cout << "Your Customer ID: " << row[0] << std::endl;
                    mysql_free_result(result);
                }
            }
        }
        else
        {
            std::cout << "Sorry, the provided name and email do not match our records. Please try again or create a new account." << std::endl;
            std::string choice;
            std::cout << "Enter 'A' to create a new account or any other key to exit: ";
            std::getline(std::cin, choice);

            if (choice == "A" || choice == "a")
            {
                std::cout << "Enter your name: ";
                std::getline(std::cin, name);

                std::cout << "Enter your email: ";
                std::getline(std::cin, email);

                addUser(connection, name, email);
                std::cout << "Welcome, " << name << "!" << std::endl;

                std::string getCustomerIdQuery = "SELECT LAST_INSERT_ID()";
                if (mysql_query(connection, getCustomerIdQuery.c_str()) == 0)
                {
                    MYSQL_RES* result = mysql_store_result(connection);
                    if (result != NULL)
                    {
                        MYSQL_ROW row = mysql_fetch_row(result);
                        std::cout << "Your Customer ID: " << row[0] << std::endl;
                        mysql_free_result(result);
                    }
                }
            }
        }
    }
    else if (answer == "N" || answer == "n")
    {
        std::string name, email;
        std::cout << "Enter your name: ";
        std::getline(std::cin, name);

        std::cout << "Enter your email: ";
        std::getline(std::cin, email);

        addUser(connection, name, email);
        std::cout << "Welcome, " << name << "!" << std::endl;

        std::string getCustomerIdQuery = "SELECT LAST_INSERT_ID()";
        if (mysql_query(connection, getCustomerIdQuery.c_str()) == 0)
        {
            MYSQL_RES* result = mysql_store_result(connection);
            if (result != NULL)
            {
                MYSQL_ROW row = mysql_fetch_row(result);
                std::cout << "Your Customer ID: " << row[0] << std::endl;
                mysql_free_result(result);
            }
        }
    }
    else
    {
        std::cout << "Invalid choice. Please try again." << std::endl;
        exitProgram=true;
    }
}



int main()
{


    MYSQL* connection = mysql_init(NULL);

    connectToDatabase(connection);
   // createTables(connection);
    //dropBooksTable(connection);
 cout << "===========================" << endl;
    cout << "Book Management System Menu" << endl;
    cout << "===========================" << endl;
    getUserInfo(connection);

    int choice;



 while (!exitProgram)
    {
        displayMenu();
        int choice = getUserChoice();
        handleUserChoice(choice, connection);
        cout << endl;
    }



    // Step 12: Implement NoSQL, Clustering, Partitioning & Sharding, CAP, and CRUD
    // NoSQL: Consider incorporating NoSQL databases like MongoDB or Cassandra for storing and managing unstructured or semi-structured data.
    // Clustering: Implement clustering techniques to group similar books based on genre, author, or other attributes.
    // Partitioning & Sharding: Implement partitioning and sharding strategies to distribute the database across multiple servers.
    // CAP Theorem: Make design decisions based on the system's requirements and trade-offs between consistency, availability, and partition tolerance.
    // CRUD Operations: Implement Create, Read, Update, and Delete (CRUD) operations for managing books, customers, orders, and inventory.

    // Close the connection

    mysql_close(connection);

    return 0;
}
