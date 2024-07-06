
#include <cppconn/driver.h> 
#include <cppconn/exception.h> 
#include <cppconn/statement.h> 
#include <cppconn/prepared_statement.h>
#include <iostream> 
#include <mysql_connection.h> 
#include <mysql_driver.h> 
#include <fstream>
#include <vector>
#include <ctime>

using namespace std;

#define max_time_to_rent 60;// in seconds

string get_user_type(int input) {
    string userType;
    switch (input) {
    case 1:
        userType = "customer";
        break;
    case 2:
        userType = "employee";
        break;
    case 3:
        userType = "manager";
        break;
    case 4:
        std::cout << "Thank you for using our service" << endl;
        exit(0);
    default:
        std::cout << "Invalid input" << endl;
        exit(1);
    }
    return userType;
}

bool is_valid_user(string ID, string password, string userType, sql::Statement* stmt) {
    sql::ResultSet* res;
    string query;
    if (userType == "customer") {
        query = "SELECT * FROM Customer WHERE customerID = '" + ID + "' AND Password = '" + password + "'";
        res = stmt->executeQuery(query);
        if (res->next()) {
            delete res;
            cout << "Welcome! You are successfully logged in as a customer" << endl;
            return true;
        }
        else {
            delete res;
            return false;
        }
    }
    else if (userType == "employee") {
        query = "SELECT * FROM Employee WHERE employeeID = '" + ID + "' AND Password = '" + password + "'";
        res = stmt->executeQuery(query);
        if (res->next()) {
            delete res;
            cout << "Welcome! You are successfully logged in as an employee" << endl;
            return true;
        }
        else {
            delete res;
            return false;
        }
    }
    else if (userType == "manager") {
        query = "SELECT * FROM Manager WHERE managerID = '" + ID + "' AND Password = '" + password + "'";
        res = stmt->executeQuery(query);
        if (res->next()) {
            delete res;
            cout << "Welcome! You are successfully logged in as a manager" << endl;
            return true;
        }
        else {
            delete res;
            return false;
        }
    }
    else {
        std::cout << "Invalid user type" << endl;
        exit(1);
    }
}

class Car {
private:
    string Model;
    int condition;
    int return_time;// for the purpose of checking i changed date as time;
    int charge;
    int finePerSecond;
    string rent_status;
public:
    void rent_request();
    void show_duedate();
};

bool car_exists(sql::Statement* stmt, string model) {
    string query = "SELECT * FROM Car WHERE model = '" + model + "'";
    sql::ResultSet* res = stmt->executeQuery(query);
    if (res->next()) {
        delete res;
        return true;
    }
    else {
        delete res;
        return false;
    }
}
bool is_rented(sql::Statement* stmt, string model) {
    string query = "SELECT * FROM Car WHERE model = '" + model + "'";
    sql::ResultSet* res = stmt->executeQuery(query);
    if (res->next()) { // Move to the first row
        if (res->getString("rent_status") == "rented") {
            delete res;
            return true;
        }
        else {
            delete res;
            return false;
        }
    }
    else {
        std::cout << "in the is_rented function car does not exits is coming up" << endl;
        delete res;
        return false;
    }
}
void show_all_cars(sql::Statement* stmt) {
    sql::ResultSet* res;
    res = stmt->executeQuery("SELECT * FROM Car");
    std::cout << "list of all the cars:" << endl;
    std::cout << "--------------------------------" << endl;
    while (res->next()) {
        std::cout << " Model: " << res->getString("Model") << endl;
        std::cout << " Condition: " << res->getString("Condn") << endl;
        std::cout << " Return time: " << res->getString("Return_time") << endl;
        std::cout << " Charge: " << res->getString("charge") << endl;
        std::cout << " Fine per second: " << res->getString("finePerSecond") << endl;
        std::cout << " Rent status: " << res->getString("rent_status") << endl;
        std::cout << endl;
    }
    std::cout << "--------------------------------" << endl;
    delete res;
}

class user {
protected:
    string ID;
    string Name;
    string Password;

public:
    user(const std::string& name, const std::string& ID, const std::string& password)
        : Name(name), ID(ID), Password(password) {}
};
bool user_exists(sql::Statement* stmt, string ID, string userType) {
    string query;
    if (userType == "customer") {
        query = "SELECT * FROM Customer WHERE customerID = '" + ID + "'";
    }
    else if (userType == "employee") {
        query = "SELECT * FROM Employee WHERE employeeID = '" + ID + "'";
    }
    else if (userType == "manager") {
        query = "SELECT * FROM Manager WHERE managerID = '" + ID + "'";
    }
    sql::ResultSet* res = stmt->executeQuery(query);
    if (res->next()) {
        delete res;
        return true;
    }
    else {
        delete res;
        return false;
    }
}

class customer : public user {
private:
    string rentedCars;
    int numOfCarRented;
    int customerRecord; // a parameter of 10 to keep track of the behaviour of the customer.

public:
    customer(const std::string& name, const std::string& ID, const std::string& password, string rentedCars, int numOfCarRented, int customerRecord)
        : user(name, ID, password) {}
    void show_customer_options(sql::Statement* stmt, customer customerObj);
    void rent_request(sql::Statement* stmt, customer customerObj, string model);
    void return_request(sql::Statement* stmt, customer customerObj, string model);
    void show_cars(sql::Statement* stmt, customer customerObj);
    void browse_rented_cars(sql::Statement* stmt, customer customerObj);
};

void customer::rent_request(sql::Statement* stmt, customer customerObj, string model) {

    customerObj.rentedCars = customerObj.rentedCars + model + " ";
    customerObj.numOfCarRented = customerObj.numOfCarRented + 1;

    //update customer rentedCars and numOfCarRented
    string query = "UPDATE Customer SET RentedCars = '" + customerObj.rentedCars + "', NumOfRentedCars = " + to_string(customerObj.numOfCarRented) + " WHERE customerID = '" + customerObj.ID + "'";
    stmt->execute(query);

    //update car return time


    //make rent status of rented car as "rented"
    query = "UPDATE Car SET rent_status = 'rented' WHERE model = '" + model + "'";
    stmt->execute(query);

    std::cout << "You have successfully rented car of model " << model << endl;
    customerObj.show_customer_options(stmt, customerObj);
}

void customer::return_request(sql::Statement* stmt, customer customerObj, string model) {
    //remove the car from rented cars
    customerObj.rentedCars = customerObj.rentedCars.erase(customerObj.rentedCars.find(model), model.length());
    customerObj.numOfCarRented = customerObj.numOfCarRented - 1;

    //update customer rentedCars and numOfCarRented
    string query = "UPDATE Customer SET RentedCars = '" + customerObj.rentedCars + "', NumOfRentedCars = " + to_string(customerObj.numOfCarRented) + " WHERE customerID = '" + customerObj.ID + "'";
    stmt->execute(query);

    //update car return time


    //make rent status of rented car as "not rented"
    query = "UPDATE Car SET rent_status = 'not rented' WHERE model = '" + model + "'";
    stmt->execute(query);

    std::cout << "You have successfully returned car of model " << model << endl;
    customerObj.show_customer_options(stmt, customerObj);
}

void customer::show_customer_options(sql::Statement* stmt, customer customerObj) {
    std::cout << "------------------------------------------------" << endl;
    std::cout << "1. Rent a car" << endl;
    std::cout << "2. Return a car" << endl;
    std::cout << "3. Show all cars" << endl;
    std::cout << "4. Browse your rented cars" << endl;
    std::cout << "5. logout and exit" << endl;
    cout << "Enter your choice: 1/2/3/4/5" << endl;
    int input;
    std::cin >> input;
    if (input == 1) {
        show_all_cars(stmt);
        string model;
        std::cout << "Enter the model of the car you want to rent: ";
        std::cin >> model;
        if (!car_exists(stmt, model)) {
            std::cout << "invalid model number, please try again" << endl;
            show_customer_options(stmt, customerObj);
        }
        else if (is_rented(stmt, model)) {
            std::cout << "----------------------------------------------------------" << endl;
            std::cout << "Car is already rented by a user, wait till car is returned" << endl;
            std::cout << "----------------------------------------------------------" << endl;
            show_customer_options(stmt, customerObj);
        }
        else {
            customerObj.rent_request(stmt, customerObj, model);
        }
    }
    else if (input == 2) {
        string model;
        std::cout << "Enter the model of the car you want to return: ";
        std::cin >> model;
        if (car_exists(stmt, model) && is_rented(stmt, model)) {
            return_request(stmt, customerObj, model);
        }
        else {
            std::cout<<"--------------------------------"<<endl;
            std::cout << "Car with the provided model number is not present in our store or is not rented by you" << endl;
            show_customer_options(stmt, customerObj);
        }
    }
    else if (input == 3) {
        customerObj.show_cars(stmt, customerObj);
    }
    else if (input == 4) {
        browse_rented_cars(stmt, customerObj);
    }
    else if (input == 5) {
        std::cout << "------------------------------------------------" << endl;
        std::cout << "Thank you for using our service" << endl;
        exit(0);
    }
    else {
        std::cout << "Invalid input, Please choose a valid option or click 5 to exit " << endl;
        show_customer_options(stmt, customerObj);
    }
}

void customer::browse_rented_cars(sql::Statement* stmt, customer customerObj) {
    std::string query = "SELECT RentedCars FROM Customer WHERE customerID = '" + customerObj.ID + "'";
    sql::ResultSet* res = stmt->executeQuery(query);

    std::cout << "Your rented cars are: " << endl;
    if(res->next()){
        std::cout << "--------------------------------" << endl;
        std::cout << customerObj.rentedCars << endl;
        std::cout << "--------------------------------" << endl;
    }
    else{
        std::cout << "You have not rented any cars yet" << endl;
    }
    customerObj.show_customer_options(stmt, customerObj);
}

void customer::show_cars(sql::Statement* stmt, customer customerObj) {
    show_all_cars(stmt);
    customerObj.show_customer_options(stmt, customerObj);
}

class employee : public user {
private:
    string rentedCars;
    int fineDue;
    int employeeRecord;
    int numOfCarRented;
public:
    employee(const std::string& name, const std::string& ID, const std::string& password, string rentedCars, int numOfCarRented, int employeeRecord)
        : user(name, ID, password) {}
    void show_employee_options(sql::Statement* stmt, employee employeeObj);
    void rent_request(sql::Statement* stmt, employee employeeObj, string model);
    void return_request(sql::Statement* stmt, employee employeeObj, string model);
    void show_cars(sql::Statement* stmt, employee employeeObj);
    void browse_rented_cars(sql::Statement* stmt, employee employeeObj);
};

void employee::rent_request(sql::Statement* stmt, employee employeeObj, string model) {
    employeeObj.rentedCars = employeeObj.rentedCars + model + " ";
    employeeObj.numOfCarRented = employeeObj.numOfCarRented + 1;

    //update employee rentedCars and numOfCarRented
    string query = "UPDATE Employee SET RentedCars = '" + employeeObj.rentedCars + "', NumOfRentedCars = " + to_string(employeeObj.numOfCarRented) + " WHERE employeeID = '" + employeeObj.ID + "'";
    stmt->execute(query);

    //update car return time

    //make rent status of rented car as "rented"
    query = "UPDATE Car SET rent_status = 'rented' WHERE model = '" + model + "'";
    stmt->execute(query);
    std::cout<<"-------------------------------------------"<<endl;
    std::cout << "You have successfully rented car of model " << model << endl;
    std::cout<<"-------------------------------------------"<<endl;
    employeeObj.show_employee_options(stmt, employeeObj);
}

void employee::return_request(sql::Statement* stmt, employee employeeObj, string model) {
    //remove the car from rented cars
    employeeObj.rentedCars = employeeObj.rentedCars.erase(employeeObj.rentedCars.find(model), model.length());
    employeeObj.numOfCarRented = employeeObj.numOfCarRented - 1;

    //update employee rentedCars and numOfCarRented
    string query = "UPDATE Employee SET RentedCars = '" + employeeObj.rentedCars + "', NumOfRentedCars = " + to_string(employeeObj.numOfCarRented) + " WHERE employeeID = '" + employeeObj.ID + "'";
    stmt->execute(query);

    //update car return time

    //make rent status of rented car as "not rented"
    query = "UPDATE Car SET rent_status = 'not rented' WHERE model = '" + model + "'";
    stmt->execute(query);
    std::cout << "-------------------------------------------" << endl;
    std::cout << "You have successfully returned car of model " << model << endl;
    std::cout << "-------------------------------------------" << endl;
    employeeObj.show_employee_options(stmt, employeeObj);
}

void employee::show_employee_options(sql::Statement* stmt, employee employeeObj) {
    std::cout << "--------------------------------------------------" << endl;
    std::cout << "1. Rent a car" << endl;
    std::cout << "2. Return a car" << endl;
    std::cout << "3. Show all cars" << endl;
    std::cout << "4. Browse your rented cars" << endl;
    std::cout << "5. logout and Exit" << endl;
    cout << "Enter your choice: ";
    int input;
    std::cin >> input;
    if (input == 1) {
        show_all_cars(stmt);
        string model;
        std::cout << "Enter the model of the car you want to rent: ";
        std::cin >> model;
        if (!car_exists(stmt, model)) {
            std::cout << "--------------------------------------------------------------" << endl;
            std::cout << "Car with the provided model number is not present in our store" << endl;
            std::cout << "--------------------------------------------------------------" << endl;
            show_employee_options(stmt, employeeObj);
        }
        else if (is_rented(stmt, model)) {
            std::cout<<"-----------------------------------------------------------"<<endl;
            std::cout << "Car is already rented by a user, wait till car is returned" << endl;
            std::cout<<"-----------------------------------------------------------"<<endl;
            show_employee_options(stmt, employeeObj);
        }
        else {
            employeeObj.rent_request(stmt, employeeObj, model);
        }
    }
    else if (input == 2) {
        string model;
        std::cout << "Enter the model of the car you want to return: ";
        std::cin >> model;
        if (car_exists(stmt, model) && is_rented(stmt, model)) {
            employeeObj.return_request(stmt, employeeObj, model);
        }
        else {
            cout<<"--------------------------------"<<endl;
            std::cout << "Car with the provided model number is not present in our store or is not rented by you" << endl;
            cout<<"--------------------------------"<<endl;
            employeeObj.show_employee_options(stmt, employeeObj);
        }
    }
    else if (input == 3) {
        employeeObj.show_cars(stmt, employeeObj);
    }
    else if(input == 4){
        employeeObj.browse_rented_cars(stmt, employeeObj);
    }
    else if (input == 5) {
        std::cout<<"-------------------------------------------------------"<<endl;
        std::cout << "Thank you for using our service" << endl;
        std::cout<<"-------------------------------------------------------"<<endl;
        exit(0);
    }
    else {
        std::cout << "Invalid input, Please choose a valid option or click 4 to exit " << endl;
        employeeObj.show_employee_options(stmt, employeeObj);
    }
}

void employee::browse_rented_cars(sql::Statement* stmt, employee employeeObj) {
    std::string query = "SELECT RentedCars FROM Employee WHERE employeeID = '" + employeeObj.ID + "' ";
    sql::ResultSet* res = stmt->executeQuery(query);

    std::cout << "Your rented cars are: " << endl;
    if (res->next()) {
        if(res->getInt("NumofRentedCars") == 0){
            std::cout << "You have not rented any cars yet" << endl;
        }
        else{
            std::cout << "--------------------------------" << endl;
            std::cout << employeeObj.rentedCars << endl;
            std::cout << "--------------------------------" << endl;
        }
    }
    else {
        std::cout << "You have not rented any cars yet" << endl;
    }
    employeeObj.show_employee_options(stmt, employeeObj);
}

void employee::show_cars(sql::Statement* stmt, employee employeeObj) {
    show_all_cars(stmt);
    employeeObj.show_employee_options(stmt, employeeObj);
}

class manager : public user {
private:

public:
    manager(const std::string& name, const std::string& ID, const std::string& password)
        : user(name, ID, password) {}

    void add_user(string ID, string Name, string password, string userType, sql::Statement* stmt, manager manager);

    void delete_user(string ID, string userType, sql::Statement* stmt, manager manager);

    void update_user(string ID, string Name, string password, string userType, sql::Statement* stmt, manager manager);

    void show_manager_options(sql::Statement* stmt, manager manager);

    void car_menu(sql::Statement* stmt, manager managerObj);
    void show_cars(sql::Statement* stmt, manager managerObj);
};

void manager::show_cars(sql::Statement* stmt, manager managerObj) {
    show_all_cars(stmt);
    managerObj.show_manager_options(stmt, managerObj);
}

void manager::delete_user(string ID, string userType, sql::Statement* stmt, manager manager) {
    string query;
    if (userType == "customer") {
        query = "DELETE FROM Customer WHERE customerID = '" + ID + "'";
    }
    else if (userType == "employee") {
        query = "DELETE FROM Employee WHERE employeeID = '" + ID + "'";
    }
    stmt->execute(query);
    std::cout << userType << "  deleted successfully" << endl;
    std::cout << "--------------------------------" << endl;
    show_manager_options(stmt, manager);
}
void get_details_to_delete(string userType, manager manager, sql::Statement* stmt) {
    string ID;
    std::cout << "Enter the user ID to delete" << endl;
    std::cin >> ID;
    manager.delete_user(ID, userType, stmt, manager);
}

void manager::update_user(string ID, string Name, string password, string userType, sql::Statement* stmt, manager manager) {
    string query;
    int number_of_cars_rented = 0;
    if (userType == "customer") {
        query = "UPDATE Customer SET Name = '" + Name + "', Password = '" + password + "' WHERE customerID = '" + ID + "'";
    }
    else if (userType == "employee") {
        query = "UPDATE Employee SET Name = '" + Name + "', Password = '" + password + "' WHERE employeeID = '" + ID + "'";
    }
    stmt->execute(query);
    cout<<"--------------------------------"<<endl;
    cout << userType << "  updated successfully" << endl;
    show_manager_options(stmt, manager);
}
void get_details_to_update(string userType, manager manager, sql::Statement* stmt) {
    string ID;
    string Name;
    string password;
    std::cout << "Enter the ID of the user you want to update" << endl;
    std::cin >> ID;
    std::cout << "Enter the new/updated user name" << endl;
    std::cin >> Name;
    std::cout << "Enter the new/updated user password" << endl;
    std::cin >> password;
    manager.update_user(ID, Name, password, userType, stmt, manager);
}

void manager::add_user(string ID, string Name, string password, string userType, sql::Statement* stmt, manager managerObj) {
    string query;
    if (user_exists(stmt, ID, userType)) {
        std::cout << "User already exists with the provided ID" << endl;
    }
    else {
        if (userType == "customer") {
            query = "INSERT INTO Customer (Name, customerID, Password, NumOfRentedCars, RentedCars, customerRecord) VALUES ('" + Name + "', '" + ID + "', '" + password + "', 0, '', 10)";
        }
        else if (userType == "employee") {
            query = "INSERT INTO Employee (Name, employeeID, Password, NumOfRentedCars, RentedCars, employeeRecord) VALUES ('" + Name + "', '" + ID + "', '" + password + "', 0, '', 10)";
        }
        else if (userType == "manager") {
            query = "INSERT INTO Manager (Name, managerID, Password) VALUES ('" + Name + "', '" + ID + "', '" + password + "')";
        }
        stmt->execute(query);
        cout << userType << "  added successfully" << endl;
    }
    managerObj.show_manager_options(stmt, managerObj);
}
void get_details_to_add(string userType, manager manager, sql::Statement* stmt) {
    string ID;
    string Name;
    string password;
    std::cout << "Enter the user ID" << endl;
    std::cin >> ID;
    std::cout << "Enter the user name" << endl;
    std::cin >> Name;
    std::cout << "Create a password for the user" << endl;
    std::cin >> password;
    manager.add_user(ID, Name, password, userType, stmt, manager);
}

void manager::show_manager_options(sql::Statement* stmt, manager managerObj) {
    std::cout << "------------------------------------------------" << endl;
    std::cout << "1. Add user" << endl;
    std::cout << "2. Delete user" << endl;
    std::cout << "3. Update user" << endl;
    std::cout << "4. Show Car Menu" << endl;
    std::cout << "5. logout and exit" << endl;
    cout << "Enter your choice: ";
    int input;
    std::cin >> input;
    if (input == 1) {
        std::cout << "choose the user type you want to add:" << endl;
        std::cout << "1. customer" << endl;
        std::cout << "2. employee" << endl;
        std::cout << "3. manager" << endl;
        std::cout << "4. go back to main menu" << endl;
        std::cout << "Enter your choice: ";
        int input;
        std::cin >> input;
        if (input == 1) {
            string userType = get_user_type(input);
            get_details_to_add(userType, managerObj, stmt);
        }
        else if (input == 2) {
            string userType = get_user_type(input);
            get_details_to_add(userType, managerObj, stmt);
        }
        else if (input == 3) {
            string userType = get_user_type(input);
            get_details_to_add(userType, managerObj, stmt);
        }
        else if (input == 4) {
            show_manager_options(stmt, managerObj);
        }
        else {
            std::cout << "Invalid user type choosen, Choose a valid user type or press 4 to exit" << endl;
            show_manager_options(stmt, managerObj);
        }
    }
    else if (input == 2) {
        std::cout << "Enter the user type you want to delete:" << endl;
        std::cout << "1. customer" << endl;
        std::cout << "2. employee" << endl;
        std::cout << "3. manager" << endl;
        std::cout << "4. exit" << endl;
        std::cout << "Enter your choice: ";
        int input;
        std::cin >> input;
        if (input == 1) {
            string userType = get_user_type(input);
            get_details_to_delete(userType, managerObj, stmt);
        }
        else if (input == 2) {
            string userType = get_user_type(input);
            get_details_to_delete(userType, managerObj, stmt);
        }
        else if (input == 3) {
            string userType = get_user_type(input);
            get_details_to_delete(userType, managerObj, stmt);
        }
        else if (input == 4) {
            show_manager_options(stmt, managerObj);
        }
        else {
            std::cout << "Invalid user type choosen, Choose a valid user type or press 4 to exit" << endl;
            show_manager_options(stmt, managerObj);
        }
    }
    else if (input == 3) {
        std::cout << "Enter the user type you want to update:" << endl;
        std::cout << "1. customer" << endl;
        std::cout << "2. employee" << endl;
        std::cout << "3. manager" << endl;
        std::cout << "4. exit" << endl;
        std::cout << "Enter your choice: 1/2/3" << endl;
        int input;
        std::cin >> input;
        if (input == 1) {
            string userType = get_user_type(input);
            get_details_to_update(userType, managerObj, stmt);
        }
        else if (input == 2) {
            string userType = get_user_type(input);
            get_details_to_update(userType, managerObj, stmt);
        }
        else if (input == 3) {
            string userType = get_user_type(input);
            get_details_to_update(userType, managerObj, stmt);
        }
        else if (input == 4) {
            show_manager_options(stmt, managerObj);
        }
        else {
            std::cout << "Invalid user type choosen, Choose a valid user type or press 4 to exit" << endl;
            show_manager_options(stmt, managerObj);
        }
    }
    else if (input == 4) {
        managerObj.car_menu(stmt, managerObj);
    }
    else if (input == 5) {
        std::cout << "Thank you for using our service" << endl;
        exit(0);
    }
    else {
        std::cout << "Invalid input, Please choose a valid option or click 5 to exit " << endl;
        show_manager_options(stmt, managerObj);
    }
}

void manager::car_menu(sql::Statement* stmt, manager managerObj) {
    std::cout << "------------------------------------------------" << endl;
    std::cout << "1. Add a car" << endl;
    std::cout << "2. Delete a car" << endl;
    std::cout << "3. Update a car" << endl;
    std::cout << "4. Show all cars" << endl;
    std::cout << "5. go back to main menu" << endl;
    cout << "Enter your choice: ";
    int input;
    std::cin >> input;
    if (input == 1) {
        string model, condition;
        string return_time = "0000";
        string rent_status = "not rented";
        int charge, finePerDay;
        std::cout << "Enter the model of the car you want to add: ";
        std::cin >> model;
        std::cout << "Enter the condition of the car you want to add: ";
        std::cin >> condition;
        std::cout << "Enter the charge of the car you want to add: ";
        std::cin >> charge;
        std::cout << "Enter the fine per second of the car you want to add: ";
        std::cin >> finePerDay;
        if (car_exists(stmt, model)) {
            std::cout << "Car already exists with the provided model number" << endl;
        }
        else {
            string query = "INSERT INTO Car (Model, Condn, Return_time, charge, finePerSecond, rent_status) VALUES ('" + model + "', '" + condition + "', '" + return_time + "', " + to_string(charge) + ", " + to_string(finePerDay) + ", '" + rent_status + "')";

            stmt->execute(query);

            std::cout << "Car added successfully" << endl;
        }
        managerObj.car_menu(stmt, managerObj);
    }
    else if (input == 2) {
        string model;
        managerObj.show_cars(stmt, managerObj);
        std::cout << "Enter the model of the car you want to delete: " << endl;
        std::cin >> model;

        //check if car exists or not
        if (!car_exists(stmt, model)) {
            std::cout << "error 404: no car exists with provided model number" << endl;
        }
        else {
            //check if car is rented or not
            if (is_rented(stmt, model)) {
                std::cout << "Car is rented by a user, wait till car is returned" << endl;
            }
            else {
                string query = "DELETE FROM Car WHERE model = '" + model + "'";
                stmt->execute(query);
                std::cout << "Car deleted successfully" << endl;
            }
        }
        managerObj.car_menu(stmt, managerObj);
    }
    else if (input == 3) {
        string model, condition;
        int charge, finePerDay;
        managerObj.show_cars(stmt, managerObj);
        std::cout << "Enter the model of the car you want to update: ";
        std::cin >> model;
        if (!car_exists(stmt, model)) {
            std::cout << "Car doesn't exists with the provided model number" << endl;
        }
        else {
            std::cout << "enter the new condition of the car: ";
            std::cin >> condition;
            std::cout << "Enter the new charge per day of the car: ";
            std::cin >> charge;
            std::cout << "Enter the new fine per day of the car: ";
            std::cin >> finePerDay;
            string query = "UPDATE Car SET Condn = '" + condition + "', charge = " + to_string(charge) + ", finePerSecond = " + to_string(finePerDay) + " WHERE model = '" + model + "'";
            stmt->execute(query);
            std::cout << "Car updated successfully" << endl;
        }
        managerObj.car_menu(stmt, managerObj);
    }
    else if (input == 4) {
        managerObj.show_cars(stmt, managerObj);
        managerObj.car_menu(stmt, managerObj);
    }
    else if (input == 5) {
        managerObj.show_manager_options(stmt, managerObj);
    }
    else {
        std::cout << "Invalid input, Please choose a valid option or click 5 to exit " << endl;
        managerObj.car_menu(stmt, managerObj);
    }
}


manager create_manager_object(sql::Statement* stmt, sql::Connection* con, string managerID) {
    sql::PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM Manager WHERE managerID = ?");
    pstmt->setString(1, managerID); // replace with the ID of the manager you want to retrieve
    sql::ResultSet* res = pstmt->executeQuery();
    if (res->next()) {
        manager manager(res->getString("Name"), res->getString("managerID"), res->getString("Password"));
        return manager;
    }
    else {
        std::cout << "No manager found" << endl;
        exit(1);
    }
}

customer create_customer_object(sql::Statement* stmt, sql::Connection* con, string customerID) {
    sql::PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM Customer WHERE customerID = ?");
    pstmt->setString(1, customerID); // replace with the ID of the customer you want to retrieve
    sql::ResultSet* res = pstmt->executeQuery();
    if (res->next()) {
        customer customerObj(res->getString("Name"), res->getString("customerID"), res->getString("Password"), res->getString("RentedCars"), res->getInt("NumOfRentedCars"), res->getInt("customerRecord"));
        return customerObj;
    }
    else {
        std::cout << "No customer found" << endl;
        exit(1);
    }
}

employee create_employee_object(sql::Statement* stmt, sql::Connection* con, string employeeID) {
    sql::PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM Employee WHERE employeeID = ?");
    pstmt->setString(1, employeeID); // replace with the ID of the employee you want to retrieve
    sql::ResultSet* res = pstmt->executeQuery();
    if (res->next()) {
        employee employeeObj(res->getString("Name"), res->getString("employeeID"), res->getString("Password"), res->getString("RentedCars"), res->getInt("NumOfRentedCars"), res->getInt("employeeRecord"));
        return employeeObj;
    }
    else {
        std::cout << "No employee found" << endl;
        exit(1);
    }
}


int welcome_menu() {
    std::cout << "------------------------------------------------" << endl;
    std::cout << "Welcome to the Car Rental System" << endl;
    std::cout << "Please select your user type" << endl;
    std::cout << "1. Customer" << endl;
    std::cout << "2. Employee" << endl;
    std::cout << "3. Manager" << endl;
    std::cout << "4. Exit" << endl;
    std::cout << "Enter your choice(1/2/3/4): " ;
    int input;
    std::cin >> input;
    if (input == 4) {
        std::cout << "Thank you for using our service" << endl;
        exit(0);
    }
    else if (input < 1 || input > 4) {
        std::cout << "Invalid input, Please choose a valid option or click 4 to exit " << endl;
        welcome_menu();
    }
    return input;
}

void intialize_tables(sql::Statement* stmt) {
    string createTableSQL;
    createTableSQL = "CREATE TABLE IF NOT EXISTS Car ("
        "`Model` VARCHAR(255), "
        "`Condn` VARCHAR(255), "
        "`Return_time` INT, "
        "`charge` INT,"
        "`finePerSecond` INT, "
        "`rent_status` VARCHAR(255),"
        "PRIMARY KEY (`model`))";

    stmt->execute(createTableSQL);

    createTableSQL = "CREATE TABLE IF NOT EXISTS Customer ("
        "`Name` VARCHAR(255), "
        "`customerID` VARCHAR(255), "
        "`Password` VARCHAR(255), "
        "`RentedCars` VARCHAR(255), "
        "`NumOfRentedCars` INT,"
        "`customerRecord` INT,"
        "PRIMARY KEY (`customerID`));";

    stmt->execute(createTableSQL);

    createTableSQL = "CREATE TABLE IF NOT EXISTS Employee ("
        "`Name` VARCHAR(255), "
        "`employeeID` VARCHAR(255), "
        "`Password` VARCHAR(255), "
        "`RentedCars` VARCHAR(255),"
        "`NumOfRentedCars` INT,"
        "`employeeRecord` INT,"
        "PRIMARY KEY (`employeeID`));";

    stmt->execute(createTableSQL);

    createTableSQL = "CREATE TABLE IF NOT EXISTS Manager ("
        "`Name` VARCHAR(255), "
        "`managerID` VARCHAR(255), "
        "`Password` VARCHAR(255), "
        "PRIMARY KEY (`managerID`));";

    stmt->execute(createTableSQL);
}


int main() {
    try {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;

        //replace with your username and password
        string sqlpassword = "Alpana@224";

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("localhost:3306", "root", sqlpassword); //replace with your username and password

        string dbname = "CS253Assignment1_DB";

        con->setSchema(dbname);
        sql::Statement* stmt = con->createStatement();

        stmt->execute("CREATE DATABASE IF NOT EXISTS " + dbname);

        //initializing 4 tables
        intialize_tables(stmt);


        //welcome menu
        int input = welcome_menu();
        //get user type
        string userType = get_user_type(input);
        std::cout<<"--------------------------------"<<endl;
        std::cout << "welcome " << userType << endl;

        // check if the user is valid
        string ID, password;
        std::cout << "Please enter your ID" << endl;
        std::cin >> ID;
        std::cout << "Please enter your password" << endl;
        std::cin >> password;
        if (is_valid_user(ID, password, userType, stmt)) {
            if (userType == "customer") {
                customer customerObj = create_customer_object(stmt, con, ID);
                customerObj.show_customer_options(stmt, customerObj);
            }
            else if (userType == "employee") {
                employee employeeObj = create_employee_object(stmt, con, ID);
                employeeObj.show_employee_options(stmt, employeeObj);
            }
            else if (userType == "manager") {
                manager managerObj = create_manager_object(stmt, con, ID);
                managerObj.show_manager_options(stmt, managerObj);
            }
        }
        else {
            cout << "Invalid ID or password" << endl;
            exit(1);
        }

        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }

    return 0;
}

