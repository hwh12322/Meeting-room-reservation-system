#include "DatabaseManager.h"
// 构造函数
DatabaseManager::DatabaseManager(const std::string& db_host, const std::string& db_user,
                                 const std::string& db_password, const std::string& db_name) {
    conn = mysql_init(NULL);
    connect(db_host, db_user, db_password, db_name);
}

// 析构函数
DatabaseManager::~DatabaseManager() {
    if (conn) {
        mysql_close(conn);
    }
}

// 连接数据库
void DatabaseManager::connect(const std::string& db_host, const std::string& db_user,
                              const std::string& db_password, const std::string& db_name) {
     conn = mysql_init(NULL);
    if (!conn) {
        throw std::runtime_error("MySQL 初始化失败");
    }

    if (!mysql_real_connect(conn, db_host.c_str(), db_user.c_str(), 
                            db_password.c_str(), db_name.c_str(), 0, NULL, 0)) {
        std::string error_msg = "连接到数据库出错: " + std::string(mysql_error(conn));
        mysql_close(conn);
        throw std::runtime_error(error_msg);
    }


    if (mysql_set_character_set(conn, "utf8mb3")) {
        std::string error_msg = "设置字符编码失败: " + std::string(mysql_error(conn));
        mysql_close(conn);
        throw std::runtime_error(error_msg);
    }
}


UserType DatabaseManager::stringToUserType(const std::string& userTypeStr) const{
    if (userTypeStr == "Business") {
        return UserType::Business;
    } else if (userTypeStr == "Executive") {
        return UserType::Executive;
    } else if (userTypeStr == "Admin") {
        return UserType::Admin;
    }
    throw std::runtime_error("无效的用户类型");
}

std::string DatabaseManager::userTypeToString(const UserType usertype) const{
    if (usertype == UserType::Business) {
        std::string a = "Business";
        return a;
    } else if (usertype == UserType::Executive) {
        std::string a = "Executive";
        return a;
    } else if (usertype == UserType::Admin) {
        std::string a = "Admin";
        return a;
    }
    throw std::runtime_error("无效的用户类型");
}



bool DatabaseManager::addUser(const std::string& employeeID, const std::string& name, 
                              const std::string& username, const std::string& password, 
                              UserType userType) {
    // 首先检查用户名是否已存在
    std::string checkQuery = "SELECT * FROM Users WHERE Username = '" + username + "'";
    if (mysql_query(conn, checkQuery.c_str()) == 0) {
        MYSQL_RES* result = mysql_store_result(conn);
        if (mysql_num_rows(result) > 0) {  // 用户名已存在
            mysql_free_result(result);
            return false;
        }
        mysql_free_result(result);
    }
    // 插入新用户
    std::string userTypeStr = userTypeToString(userType); // 转换 UserType 枚举为字符串
    std::string query = "INSERT INTO Users (EmployeeID, Name_, Username, Password_, UserType) VALUES ('" 
                        + employeeID + "', '" + name + "', '" + username + "', '" 
                        + password + "', '" + userTypeStr + "')";

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "添加用户失败: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}


void DatabaseManager::viewUsers(UserType userType) {
    std::string userTypeStr = userTypeToString(userType); // 转换 UserType 枚举为字符串
    std::string query = "SELECT EmployeeID, Name_, Username, Password_, UserType FROM Users WHERE UserType = '" 
                        + userTypeStr + "'";

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "查询用户失败: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        std::cerr << "获取查询结果失败: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        std::cout << "EmployeeID: " << row[0] << ", "
                  << "Name: " << row[1] << ", "
                  << "Username: " << row[2] << ", "
                  << "Password: " << row[3] << ", "  
                  << "UserType: " << row[4] << std::endl;
    }

    mysql_free_result(result);
}


void DatabaseManager::viewMeetingRooms() {
    // 查询所有有预订的会议室
    std::string queryWithBookings = "SELECT MeetingRooms.RoomName, MeetingRooms.Capacity, Bookings.Date_, Bookings.TimeSlot, Bookings.Status_ "
                                    "FROM MeetingRooms "
                                    "JOIN Bookings ON MeetingRooms.RoomID = Bookings.RoomID "
                                    "WHERE Bookings.Status_ = 'Approved' "
                                    "ORDER BY MeetingRooms.RoomName, Bookings.Date_, Bookings.TimeSlot";

    if (mysql_query(conn, queryWithBookings.c_str())) {
        std::cerr << "查询有预订的会议室信息失败: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_RES* resultWithBookings = mysql_store_result(conn);
    if (!resultWithBookings) {
        std::cerr << "存储有预订的会议室查询结果失败: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(resultWithBookings))) {
        std::cout << "会议室名称: " << row[0] << ", "
                  << "容量: " << row[1] << ", "
                  << "预约日期: " << row[2] << ", "
                  << "时段: " << row[3] << ", "
                  << "状态: " << row[4] << std::endl;
    }
    mysql_free_result(resultWithBookings);
    // 查询所有无预订的会议室
    std::string queryWithoutBookings = "SELECT RoomName, Capacity FROM MeetingRooms "
                                       "WHERE RoomID NOT IN (SELECT RoomID FROM Bookings WHERE Status_ = 'Approved')";

    if (mysql_query(conn, queryWithoutBookings.c_str())) {
        std::cerr << "查询无预订的会议室信息失败: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_RES* resultWithoutBookings = mysql_store_result(conn);
    if (!resultWithoutBookings) {
        std::cerr << "存储无预订的会议室查询结果失败: " << mysql_error(conn) << std::endl;
        return;
    }

    while ((row = mysql_fetch_row(resultWithoutBookings))) {
        std::cout << "会议室名称: " << row[0] << ", "
                  << "容量: " << row[1] << ", "
                  << "预约日期: 无预约" << ", "
                  << "时段: 无" << ", "
                  << "状态: 无预约" << std::endl;
    }
    mysql_free_result(resultWithoutBookings);
}

bool DatabaseManager::clearBookings() {
    try {
        beginTransaction(); // 开始事务

        std::string query = "DELETE FROM Bookings";
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "清空预约失败: " << mysql_error(conn) << std::endl;
            rollbackTransaction(); // 回滚事务
            return false;
        }

        commitTransaction(); // 提交事务
        return true;
    } catch (...) {
        rollbackTransaction(); // 确保异常时回滚事务
        throw; // 重新抛出异常
    }
}

bool DatabaseManager::deleteUser(int userID) {
    // 检查账户是否存在且是否为管理员
    std::string checkQuery = "SELECT UserType FROM Users WHERE UserID = " + std::to_string(userID);
    if (mysql_query(conn, checkQuery.c_str())) {
        std::cerr << "查询账户失败: " << mysql_error(conn) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        std::cerr << "无法获取查询结果: " << mysql_error(conn) << std::endl;
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        std::cerr << "账户ID " << userID << " 不存在。\n";
        mysql_free_result(result);
        return false;
    }

    std::string userType = row[0];
    mysql_free_result(result);

    if (userType == "Admin") {
        std::cerr << "不允许删除管理员账户。\n";
        return false;
    }

    // 执行删除操作
    std::string query = "DELETE FROM Users WHERE UserID = " + std::to_string(userID);
    if (mysql_query(conn, query.c_str())) {
        std::cerr << "删除账户失败: " << mysql_error(conn) << std::endl;
        return false;
    }

    return true;
}


bool DatabaseManager::handleAssociatedData(int userID) {
    try {
        beginTransaction();  // 开始数据库事务

        std::string checkQuery = "SELECT UserType FROM Users WHERE UserID = " + std::to_string(userID);
        if (mysql_query(conn, checkQuery.c_str())) {
            std::cerr << "查询账户失败: " << mysql_error(conn) << std::endl;
            rollbackTransaction();
            return false;
        }

        MYSQL_RES* result = mysql_store_result(conn);
        if (!result) {
            std::cerr << "无法获取查询结果: " << mysql_error(conn) << std::endl;
            rollbackTransaction();
            return false;
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        if (!row) {
            std::cerr << "账户ID " << userID << " 不存在。\n";
            rollbackTransaction();
            mysql_free_result(result);
            return false;
        }

        std::string userType = row[0];
        mysql_free_result(result);

        if (userType == "Admin") {
            std::cerr << "不允许删除管理员账户。\n";
            rollbackTransaction();
            return false;
        }

        // 删除与用户相关的预约记录
        std::string query = "DELETE FROM Bookings WHERE UserID = " + std::to_string(userID);
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "删除用户预约记录失败: " << mysql_error(conn) << std::endl;
            rollbackTransaction(); 
            return false;
        }

        commitTransaction();  // 提交事务
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "处理关联数据时发生异常: " << e.what() << std::endl;
        rollbackTransaction();  // 回滚事务
        return false;
    }
}


bool DatabaseManager::bookRoom(int roomID, int userID, const std::string& date,  TimeSlot timeSlot, BookingStatus status) {
    try {
        beginTransaction(); // 开始事务

        // 检查是否已有相同的预订
        std::string timeSlotStr = (timeSlot == TimeSlot::Morning) ? "Morning" : "Afternoon";
        std::string checkQuery = "SELECT COUNT(*) FROM Bookings WHERE RoomID = " + std::to_string(roomID) 
                                 + " AND UserID = " + std::to_string(userID) 
                                 + " AND Date_ = '" + date + "'"
                                 + " AND TimeSlot = '" + timeSlotStr + "'";

        if (mysql_query(conn, checkQuery.c_str())) {
            std::cerr << "检查预约失败: " << mysql_error(conn) << std::endl;
            rollbackTransaction(); // 回滚事务
            return false;
        }

        MYSQL_RES* result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row && std::stoi(row[0]) > 0) {
            std::cerr << "该时间段您已有预约。\n";
            mysql_free_result(result);
            rollbackTransaction();
            return false;
        }
        mysql_free_result(result);

        if (isRoomBooked(roomID, date, timeSlot)) {
            std::cout << "会议室在此时间段已有预约。\n";
            rollbackTransaction(); // 回滚事务
            return false;
        }

        std::string statusStr;
        switch (status) {
            case BookingStatus::Pending:
                statusStr = "Pending";
                break;
            case BookingStatus::Approved:
                statusStr = "Approved";
                break;
            case BookingStatus::Rejected:
                statusStr = "Rejected";
                break;
        }

        std::string query = "INSERT INTO Bookings (RoomID, UserID, Date_, TimeSlot, Status_) VALUES (" 
                            + std::to_string(roomID) + ", " 
                            + std::to_string(userID) + ", '" 
                            + date + "', '" 
                            + timeSlotStr + "', '" 
                            + statusStr + "')";

        if (mysql_query(conn, query.c_str())) {
            std::cerr << "预约会议室失败: " << mysql_error(conn) << std::endl;
            rollbackTransaction(); // 回滚事务
            return false;
        }

        commitTransaction(); // 提交事务
        return true;
    } catch (...) {
        rollbackTransaction(); // 确保异常时回滚事务
        throw; // 重新抛出异常
    }
}



bool DatabaseManager::isRoomBooked(int roomID, const std::string& date, TimeSlot timeSlot) {
    std::string timeSlotStr = (timeSlot == TimeSlot::Morning) ? "Morning" : "Afternoon";
    std::string query = "SELECT COUNT(*) FROM Bookings WHERE RoomID = " + std::to_string(roomID)
                        + " AND Date_ = '" + date + "' AND TimeSlot = '" + timeSlotStr + "'"
                        + " AND Status_ = 'Approved'";  

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "查询预约失败: " << mysql_error(conn) << std::endl;
        return true; // 查询失败时，返回 true 表示不允许预约
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        std::cerr << "存储查询结果失败: " << mysql_error(conn) << std::endl;
        return true; // 同上
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int count = std::stoi(row[0]);
    mysql_free_result(result);

    return count > 0;  // 如果有记录，表示该时间段已被预约
}


void DatabaseManager::viewAllBookings() {
    // 在 SQL 查询中包括预约ID
    std::string query = "SELECT Bookings.BookingID, Bookings.Date_, Bookings.TimeSlot, Users.Name_, MeetingRooms.RoomName, MeetingRooms.Capacity, Bookings.Status_ "
                        "FROM Bookings "
                        "JOIN Users ON Bookings.UserID = Users.UserID "
                        "JOIN MeetingRooms ON Bookings.RoomID = MeetingRooms.RoomID";

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "查询预约失败: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        std::cerr << "存储查询结果失败: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        std::string bookingID = row[0];
        std::string date = row[1];
        std::string timeSlot = row[2];
        std::string userName = row[3];
        std::string roomName = row[4];
        std::string capacity = row[5];
        std::string status = row[6];

        std::cout << "日期: " << date << ", "
                  << "时段: " << timeSlot << ", "
                  << "姓名: " << userName << ", "
                  << "会议室名称: " << roomName << ", "
                  << "容量: " << capacity << ", "
                  << "预约状态: " << status << std::endl;
    }

    mysql_free_result(result);
}



void DatabaseManager::viewBookingsByUser(int userID) {
    // 执行 JOIN 操作以获取所需信息
    std::string query = "SELECT Bookings.BookingID, Bookings.Date_, Bookings.TimeSlot, MeetingRooms.RoomName, MeetingRooms.Capacity, Bookings.Status_ "
                        "FROM Bookings "
                        "JOIN MeetingRooms ON Bookings.RoomID = MeetingRooms.RoomID "
                        "WHERE Bookings.UserID = " + std::to_string(userID);

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "查询预约失败: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        std::cerr << "存储查询结果失败: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        std::string bookingID = row[0];
        std::string date = row[1];
        std::string timeSlot = row[2];
        std::string roomName = row[3];
        std::string capacity = row[4];
        std::string status = row[5];

        std::cout << "预约ID: " << bookingID << ", "
                  << "日期: " << date << ", "
                  << "时段: " << timeSlot << ", "
                  << "会议室名称: " << roomName << ", "
                  << "容量: " << capacity << ", "
                  << "预约状态: " << status << std::endl;
    }

    mysql_free_result(result);
}



bool DatabaseManager::cancelBooking(int bookingID) {
    try {
        beginTransaction(); // 开始事务

        // 首先检查 bookingID 是否存在
        std::string checkQuery = "SELECT COUNT(*) FROM Bookings WHERE BookingID = " + std::to_string(bookingID);
        if (mysql_query(conn, checkQuery.c_str())) {
            std::cerr << "检查预约存在性失败: " << mysql_error(conn) << std::endl;
            rollbackTransaction();
            return false;
        }

        MYSQL_RES* result = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(result);
        if (!row || std::stoi(row[0]) == 0) {
            std::cerr << "预约不存在。\n" ;
            mysql_free_result(result);
            rollbackTransaction();
            return false;
        }
        mysql_free_result(result);

        // 更新预约状态
        std::string query = "UPDATE Bookings SET Status_ = 'Cancelled' WHERE BookingID = " + std::to_string(bookingID);
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "更新预约状态为取消失败: " << mysql_error(conn) << std::endl;
            rollbackTransaction(); // 回滚事务
            return false;
        }

        commitTransaction(); // 提交事务
        return true;
    }
    catch (...) {
        rollbackTransaction(); // 确保异常时回滚事务
        throw; // 重新抛出异常
    }
}



int DatabaseManager::getPendingBookingID() const{
    std::string query = "SELECT BookingID FROM Bookings WHERE Status_ = 'Pending' LIMIT 1";
    if (mysql_query(conn, query.c_str())) {
        std::cerr << "查询待审批预约失败: " << mysql_error(conn) << std::endl;
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) {
        std::cerr << "存储查询结果失败: " << mysql_error(conn) << std::endl;
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int bookingID = -1;
    if (row) {
        bookingID = std::stoi(row[0]);
    }

    mysql_free_result(result);
    return bookingID;
}



bool DatabaseManager::updateBookingStatus(int bookingID, BookingStatus newStatus) {
    try {
        beginTransaction(); // 开始事务

        // 首先检查预约当前的状态
        std::string checkQuery = "SELECT Status_ FROM Bookings WHERE BookingID = " + std::to_string(bookingID) + " FOR UPDATE";
        if (mysql_query(conn, checkQuery.c_str())) {
            std::cerr << "检查预约状态失败: " << mysql_error(conn) << std::endl;
            rollbackTransaction(); // 回滚事务
            return false;
        }

        MYSQL_RES* result = mysql_store_result(conn);
        if (!result) {
            std::cerr << "获取预约状态失败: " << mysql_error(conn) << std::endl;
            rollbackTransaction();
            return false;
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        if (!row || std::string(row[0]) == "Approved" || std::string(row[0]) == "Rejected") {
            mysql_free_result(result);
            std::lock_guard<std::mutex> guard(logMutex);
            std::cerr << "该预约已被其他行政人员审批"  << std::endl;
            rollbackTransaction();
            return false; // 如果预约已被处理或无法获取状态，则不进行更新
        }
        mysql_free_result(result);

        // 更新预约状态
        std::string statusStr = newStatus == BookingStatus::Approved ? "Approved" : "Rejected";
        std::string query = "UPDATE Bookings SET Status_ = '" + statusStr + "' WHERE BookingID = " + std::to_string(bookingID);

        if (mysql_query(conn, query.c_str())) {
            std::cerr << "更新预约状态失败: " << mysql_error(conn) << std::endl;
            rollbackTransaction(); // 回滚事务
            return false;
        }

        commitTransaction(); // 提交事务
        return true;
    } catch (...) {
        rollbackTransaction(); // 确保异常时回滚事务
        throw; // 重新抛出异常
    }
}

User DatabaseManager::getUserByUsernameAndPassword(const std::string& username, const std::string& password) const{

    std::string query = "SELECT UserID, EmployeeID, Name_, UserType FROM users WHERE Username = '" 
                        + username + "' AND Password_ = '" + password + "'";
    if (mysql_query(conn, query.c_str())) {
        std::cerr << "数据库查询失败: " << mysql_error(conn) << std::endl;
        throw std::runtime_error("数据库查询失败");
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == NULL) {
        std::cerr << "获取查询结果失败: " << mysql_error(conn) << std::endl;
        throw std::runtime_error("获取查询结果失败");
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        throw std::runtime_error("未找到用户或密码错误");
    }

    int userID = std::stoi(row[0]);
    std::string employeeID = row[1];
    std::string name = row[2];
    UserType userType;
    if (row[3] != NULL) {
        userType = DatabaseManager::stringToUserType(row[3]);
        std::cout << userType ;
    } else {
        throw std::runtime_error("用户类型字段为空");
    }
    mysql_free_result(result);
    return User(userID, employeeID, name, username, password, userType);
}


bool DatabaseManager::updatePassword(int userID, const std::string& newPassword) {
    // 首先检查 userID 是否存在
    std::string checkQuery = "SELECT COUNT(*) FROM Users WHERE UserID = " + std::to_string(userID);
    if (mysql_query(conn, checkQuery.c_str())) {
        std::cerr << "检查用户存在性失败: " << mysql_error(conn) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row || std::stoi(row[0]) == 0) {
        std::cerr << "用户ID " << userID << " 不存在。\n";
        mysql_free_result(result);
        return false;
    }
    mysql_free_result(result);

    // 更新密码
    std::string query = "UPDATE Users SET Password_ = '" + newPassword + "' WHERE UserID = " + std::to_string(userID);
    if (mysql_query(conn, query.c_str())) {
        std::cerr << "更新密码失败: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

