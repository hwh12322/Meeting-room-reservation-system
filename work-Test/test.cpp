#include "pch.h"

class DatabaseManagerTest : public ::testing::Test {
protected:
    // 假设参数
    std::string employeeID = "1";
    std::string name = "1";
    std::string username = "1";
    std::string password = "1";
    UserType userType = UserType::Business;
    DatabaseManager dbManager = DatabaseManager("127.0.0.1", "root", "hwh272106", "Kingsofttest");
    void SetUp() {};
    void TearDown() {};

};

TEST_F(DatabaseManagerTest, AddUser) {
    // 测试 addUser 方法
    bool result = dbManager.addUser(employeeID, name, username, password, userType);
    EXPECT_TRUE(result);  // 预期 addUser 应该返回 true
}

TEST_F(DatabaseManagerTest, AddExistingUser) {
    bool firstAttempt = dbManager.addUser("existing_admin", "Existing User", "existinguser", "password", UserType::Admin);
    bool secondAttempt = dbManager.addUser("existing_admin", "Existing User", "existinguser", "password", UserType::Admin);
    EXPECT_TRUE(firstAttempt);
    EXPECT_FALSE(secondAttempt); // 预期第二次添加相同的用户应该返回 false
}

TEST_F(DatabaseManagerTest, BookRoomTest) {
    bool result = dbManager.bookRoom(1, 1, "2023-12-04", TimeSlot::Afternoon, BookingStatus::Pending);
    bool result1 = dbManager.bookRoom(1, 1, "2023-12-04", TimeSlot::Afternoon, BookingStatus::Pending);
    dbManager.bookRoom(1, 1, "2023-12-05", TimeSlot::Afternoon, BookingStatus::Pending);
    EXPECT_TRUE(result);
    EXPECT_FALSE(result1);
}

TEST_F(DatabaseManagerTest, ViewUsers) {
    dbManager.viewUsers(UserType::Business);
}

TEST_F(DatabaseManagerTest, ViewMeetingrooms) {
    dbManager.viewMeetingRooms();
}

TEST_F(DatabaseManagerTest, ViewAllBookings) {
    dbManager.viewAllBookings();
}

TEST_F(DatabaseManagerTest, viewBookingsByUser) {
    dbManager.viewBookingsByUser(1);
}

TEST_F(DatabaseManagerTest, CancelBooking) {
    bool result = dbManager.cancelBooking(2);
    EXPECT_TRUE(result);  // 预期 cancelBooking 应该返回 true

}

TEST_F(DatabaseManagerTest, getPendingBookingID) {
   int result = dbManager.getPendingBookingID();
    EXPECT_EQ(result,1); 

}

TEST_F(DatabaseManagerTest, updateBookingStatus) {
    // 测试 updateBookingStatus 方法
    bool result = dbManager.updateBookingStatus(1, BookingStatus::Approved);
    EXPECT_TRUE(result);  // 预期 updateBookingStatus 应该返回 true
}


TEST_F(DatabaseManagerTest, UpdatePasswordTest) {
    bool result = dbManager.updatePassword(1, "123456");
    EXPECT_TRUE(result);

}


TEST_F(DatabaseManagerTest, GetUserInfo) {
    // 测试 getUserInfo 方法
    User userInfo = dbManager.getUserByUsernameAndPassword("1","123456");
    EXPECT_EQ(userInfo.getName(), "1"); // 验证获取的用户信息是否正确
    EXPECT_EQ(userInfo.getEmployeeID(), "1"); 
    EXPECT_EQ(userInfo.getPassword(), "123456");
    EXPECT_EQ(userInfo.getUserType(), userType);
    EXPECT_EQ(userInfo.getUserID(), 1);
    EXPECT_EQ(userInfo.getUsername(), "1");
}


TEST_F(DatabaseManagerTest, handleAssociatedData) {
    // 测试 handleAssociatedData 方法
    bool result = dbManager.handleAssociatedData(1);
    bool result1 = dbManager.handleAssociatedData(9999);
    bool result2 = dbManager.handleAssociatedData(2);
    EXPECT_TRUE(result);  // 预期 handleAssociatedData 应该返回 true
    EXPECT_FALSE(result1);
    EXPECT_FALSE(result2);
}
TEST_F(DatabaseManagerTest, DeleteUser) {
    // 测试 deleteUser 方法
    bool result = dbManager.deleteUser(1);
    bool result1 = dbManager.deleteUser(2);
    EXPECT_TRUE(result);  // 预期 deleteUser 应该返回 true
    EXPECT_FALSE(result1); //不允许删除管理员账户
}


TEST_F(DatabaseManagerTest, ClearBookings) {
    // 测试 clearBookings 方法
    bool result = dbManager.clearBookings();
    EXPECT_TRUE(result);  // 预期 clearBookings 应该返回 true

}

TEST_F(DatabaseManagerTest, UserTypeConversion) {
    EXPECT_EQ(dbManager.stringToUserType("Business"), UserType::Business);
    EXPECT_EQ(dbManager.stringToUserType("Executive"), UserType::Executive);
    EXPECT_EQ(dbManager.stringToUserType("Admin"), UserType::Admin);

    EXPECT_EQ(dbManager.userTypeToString(UserType::Business), "Business");
    EXPECT_EQ(dbManager.userTypeToString(UserType::Executive), "Executive");
    EXPECT_EQ(dbManager.userTypeToString(UserType::Admin), "Admin");
}

TEST_F(DatabaseManagerTest, DeleteNonExistingUser) {
    bool result = dbManager.deleteUser(99999); // 假设 99999 是一个不存在的用户ID
    EXPECT_FALSE(result); // 预期删除不存在的用户应该失败
}


TEST_F(DatabaseManagerTest, CancelNonExistingBooking) {
    bool result = dbManager.cancelBooking(99999); // 假设 99999 是一个不存在的预约ID
    EXPECT_FALSE(result); // 预期取消不存在的预约应该失败
}

TEST_F(DatabaseManagerTest, UpdatePasswordNonExistingUser) {
    bool result = dbManager.updatePassword(99999, "newPassword"); // 假设 99999 是一个不存在的用户ID
    EXPECT_FALSE(result); // 预期更新不存在用户的密码应该失败
}



TEST_F(DatabaseManagerTest, UpdateStatusOfProcessedBooking) {
    // 先添加一个预约，并将其状态设置为已审批
    dbManager.bookRoom(3, 4, "2023-12-07", TimeSlot::Afternoon, BookingStatus::Pending);
    int bookingID = dbManager.getPendingBookingID();
    dbManager.updateBookingStatus(bookingID, BookingStatus::Approved);

    // 尝试再次更新同一个预约的状态
    bool result = dbManager.updateBookingStatus(bookingID, BookingStatus::Rejected);
    EXPECT_FALSE(result); // 预期无法更新已处理的预约状态
}

