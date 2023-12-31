#ifndef USER_H
#define USER_H


#include <string>
#include <iostream>

enum class UserType {
    Business,   // 业务人员
    Executive,  // 行政人员
    Admin       // 系统管理员
};

std::ostream& operator<<(std::ostream& os, UserType userType);
class User {
public:
    User() : userID(0), userType(UserType::Business) { }
    User(int userID, const std::string& employeeID, const std::string& name, 
         const std::string& username, const std::string& password, UserType userType);

    // 获取方法
    int getUserID() const;
    std::string getEmployeeID() const;
    std::string getName() const;
    std::string getUsername() const;
    std::string getPassword() const;
    UserType getUserType() const;
    void setPassword(const std::string& newPassword) {
        password = newPassword;
    }

private:
    int userID;
    std::string employeeID;
    std::string name;
    std::string username;
    std::string password;
    UserType userType;
};




#endif