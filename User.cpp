#include "User.h"

User::User(int userID, const std::string& employeeID, const std::string& name, 
           const std::string& username, const std::string& password, UserType userType) 
    : userID(userID), employeeID(employeeID), name(name), 
      username(username), password(password), userType(userType) {}

int User::getUserID() const {
    return userID;
}

std::string User::getEmployeeID() const {
    return employeeID;
}

std::string User::getName() const {
    return name;
}

std::string User::getUsername() const {
    return username;
}

std::string User::getPassword() const {
    return password;
}

UserType User::getUserType() const {
    return userType;
}



std::ostream& operator<<(std::ostream& os, const UserType userType) {
    switch (userType) {
        case UserType::Business: os << "Business"; break;
        case UserType::Executive: os << "Executive"; break;
        case UserType::Admin: os << "Admin"; break;
    }
    return os;
}