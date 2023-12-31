#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include "Bookings.h"
#include "User.h"
#include <mysql.h>
#include <string>
#include <iostream>
#include <mutex>
#include <iomanip>

extern std::mutex logMutex;

class DatabaseManager {
public:
    DatabaseManager(const std::string& db_host, const std::string& db_user,
                    const std::string& db_password, const std::string& db_name);
    ~DatabaseManager();
    UserType stringToUserType(const std::string& userTypeStr) const;
    std::string userTypeToString(const UserType usertype) const;
    bool addUser(const std::string& employeeID, const std::string& name, 
                              const std::string& username, const std::string& password, 
                              UserType userType);
    void viewUsers(UserType userType);
    void viewMeetingRooms();
    bool clearBookings();
    bool deleteUser(int userID);
    bool handleAssociatedData(int userID);
    void viewAllBookings();
    void viewBookingsByUser(int userID);
    bool cancelBooking(int bookingID);
    int getPendingBookingID()const;
    bool bookRoom(int roomID, int userID, const std::string& date, 
                               TimeSlot timeSlot, BookingStatus status);
    bool isRoomBooked(int roomID, const std::string& date, TimeSlot timeSlot);
    bool updateBookingStatus(int bookingID, BookingStatus newStatus);
    User getUserByUsernameAndPassword(const std::string& username, const std::string& password)const;
    bool updatePassword(int userID, const std::string& newPassword);

private:
    MYSQL* conn;
    void connect(const std::string& db_host, const std::string& db_user,
                 const std::string& db_password, const std::string& db_name);
    void beginTransaction() {
        if (mysql_query(conn, "SET TRANSACTION ISOLATION LEVEL SERIALIZABLE")) {
            std::cerr << "无法设置事务隔离级别: " << mysql_error(conn) << std::endl;
            throw std::runtime_error("无法设置事务隔离级别");
        }
        if (mysql_query(conn, "START TRANSACTION")) {
            std::cerr << "无法开始事务: " << mysql_error(conn) << std::endl;
            throw std::runtime_error("无法开始事务");
        }
    }
    void commitTransaction() {
        if (mysql_query(conn, "COMMIT")) {
            std::cerr << "无法提交事务: " << mysql_error(conn) << std::endl;
            throw std::runtime_error("无法提交事务");
        }
    }
    void rollbackTransaction() {
    if (mysql_query(conn, "ROLLBACK")) {
        std::cerr << "无法回滚事务: " << mysql_error(conn) << std::endl;
        // 这里不抛出异常，因为回滚本身可能是在异常处理中调用的
        }
    }
                 
};




#endif // DATABASEMANAGER_H
