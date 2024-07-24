#include <iostream>
#include <mysql/mysql.h>
#include <sstream>
#include <iomanip> // For std::setw and std::setfill

using namespace std;

const char* hostname = "127.0.0.1";
const char* user = "root";
const char* pass = "123";
const char* dbname = "library_db";
unsigned int port = 31235;
const char* unixsocket = NULL;
unsigned long clientflag = 0;

MYSQL* connect_db() {
    MYSQL* conn = mysql_init(0);
    if (conn) {
        conn = mysql_real_connect(conn, hostname, user, pass, dbname, port, unixsocket, clientflag);
        if (conn) {
            cout << "Connected to the database successfully." << endl;
        } else {
            cerr << "Connection failed: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "mysql_init failed" << endl;
    }
    return conn;
}

void create_user(const string& username, const string& password, const string& role) {
    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "INSERT INTO users (username, password, role) VALUES ('" 
              << username << "', '" << password << "', '" << role << "')";
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "INSERT failed: " << mysql_error(conn) << endl;
        } else {
            cout << "User successfully registered." << endl;
        }
        mysql_close(conn);
    }
}

int login(const string& username, const string& password) {
    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "SELECT id, role FROM users WHERE username = '" << username << "' AND password = '" << password << "'";
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "SELECT failed: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return -1;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (res == nullptr) {
            cerr << "mysql_store_result failed: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return -1;
        }

        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            int user_id = atoi(row[0]);
            string role = row[1];
            mysql_free_result(res);
            mysql_close(conn);
            return (role == "admin") ? 1 : 2;
        } else {
            cout << "Invalid username or password." << endl;
            mysql_free_result(res);
            mysql_close(conn);
            return -1;
        }
    }
    return -1;
}

void create_book(const string& title, const string& author, const string& genre, int year_published) {
    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "INSERT INTO books (title, author, genre, year_published) VALUES ('" 
              << title << "', '" << author << "', '" << genre << "', " << year_published << ")";
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "INSERT failed: " << mysql_error(conn) << endl;
        } else {
            cout << "Book successfully added." << endl;
        }
        mysql_close(conn);
    }
}

void get_books() {
    MYSQL* conn = connect_db();
    if (conn) {
        if (mysql_query(conn, "SELECT * FROM books")) {
            cerr << "SELECT failed: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (res == nullptr) {
            cerr << "mysql_store_result failed: " << mysql_error(conn) << endl;
            mysql_close(conn);
            return;
        }

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            cout << "ID: " << row[0] << ", Title: " << row[1] << ", Author: " << row[2] << ", Genre: " << row[3] << ", Year Published: " << row[4] << endl;
        }

        mysql_free_result(res);
        mysql_close(conn);
    }
}

void update_book(int book_id, const string& title, const string& author, const string& genre, int year_published) {
    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "UPDATE books SET title = '" << title << "', author = '" << author << "', genre = '" << genre << "', year_published = " << year_published << " WHERE id = " << book_id;
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "UPDATE failed: " << mysql_error(conn) << endl;
        } else {
            cout << "Book successfully updated." << endl;
        }
        mysql_close(conn);
    }
}

void delete_book(int book_id) {
    MYSQL* conn = connect_db();
    if (conn) {
        stringstream query;
        query << "DELETE FROM books WHERE id = " << book_id;
        if (mysql_query(conn, query.str().c_str())) {
            cerr << "DELETE failed: " << mysql_error(conn) << endl;
        } else {
            cout << "Book successfully deleted." << endl;
        }
        mysql_close(conn);
    }
}

void register_user() {
    string username, password, role;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    cout << "Enter role (admin/user): ";
    cin >> role;

    if (role != "admin" && role != "user") {
        cout << "Invalid role. Must be 'admin' or 'user'." << endl;
        return;
    }

    create_user(username, password, role);
}

void login_user() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    int role = login(username, password);
    if (role == -1) {
        cout << "Login failed." << endl;
        return;
    }

    int choice;
    while (true) {
        if (role == 1) {
            cout << "\nAdmin Menu:\n";
            cout << "1. Add Book\n";
            cout << "2. Show All Books\n";
            cout << "3. Update Book\n";
            cout << "4. Delete Book\n";
            cout << "5. Logout\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1) {
                string title, author, genre;
                int year_published;
                cout << "Enter title: ";
                cin.ignore();
                getline(cin, title);
                cout << "Enter author: ";
                getline(cin, author);
                cout << "Enter genre: ";
                getline(cin, genre);
                cout << "Enter year published: ";
                cin >> year_published;
                create_book(title, author, genre, year_published);
            } else if (choice == 2) {
                get_books();
            } else if (choice == 3) {
                int book_id;
                string title, author, genre;
                int year_published;
                cout << "Enter book ID to update: ";
                cin >> book_id;
                cin.ignore();
                cout << "Enter new title: ";
                getline(cin, title);
                cout << "Enter new author: ";
                getline(cin, author);
                cout << "Enter new genre: ";
                getline(cin, genre);
                cout << "Enter new year published: ";
                cin >> year_published;
                update_book(book_id, title, author, genre, year_published);
            } else if (choice == 4) {
                int book_id;
                cout << "Enter book ID to delete: ";
                cin >> book_id;
                delete_book(book_id);
            } else if (choice == 5) {
                break;
            } else {
                cout << "Invalid choice. Please try again." << endl;
            }
        } else if (role == 2) {
            cout << "\nUser Menu:\n";
            cout << "1. Show All Books\n";
            cout << "2. Logout\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1) {
                get_books();
            } else if (choice == 2) {
                break;
            } else {
                cout << "Invalid choice. Please try again." << endl;
            }
        }
    }
}

int main() {
    int choice;
    while (true) {
        cout << "\nMain Menu:\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            register_user();
        } else if (choice == 2) {
            login_user();
        } else if (choice == 3) {
            break;
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
    return 0;
}
