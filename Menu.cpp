#include "Menu.h"

// BusinessMenu 类的实现
void BusinessMenu::display() {
    while (true) {
        std::cout << "业务人员菜单:\n";
        std::cout << "1. 申请预约\n";
        std::cout << "2. 查看所有预约\n";
        std::cout << "3. 查看自己的预约\n";
        std::cout << "4. 取消预约\n";
        std::cout << "5. 修改密码\n";
        std::cout << "0. 退出\n";
        

        int choice;
        std::cout << "请输入您的选择：";
        std::cin >> choice;

        // 检查输入有效性
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            std::cout << "无效的输入，请重试。\n";
            continue;
        }

        switch (choice) {
            case 1:
                bookRoom(dbManager); 
                break;
            case 2:
                viewAllBookings(dbManager);
                break;
            case 3:
                viewOwnBookings(dbManager);
                break;
            case 4:
                cancelBooking(dbManager);
                break;
            case 5:
                changePassword(dbManager);
                break;
            case 0:
                return; // 返回主菜单或退出
            default:
                std::cout << "无效选择，请重试。\n";
        }
    }
}

void BusinessMenu::bookRoom(DatabaseManager& dbManager) {
    int roomID, timeSlotStr;
    std::string date;
    TimeSlot timeSlot;
    BookingStatus initialStatus = BookingStatus::Pending; // 初始状态通常是 Pending
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = {};  // 创建 std::tm 结构体实例
    localtime_s(&localTime, &timeT);  // 使用 localtime_s 函数

    std::cout << "◦ 1号，紫阳湖会议室——最大容纳14人" << std::endl 
    << "◦ 2号，后官湖会议室——最大容纳8人"  << std::endl 
    << "◦ 3号，清潭湖会议室——最大容纳10人" << std::endl 
    << "◦ 4号，黄家湖会议室——最大容纳6人" << std::endl 
    << "◦ 5号，杨春湖会议室——最大容纳18人" << std::endl 
    << "◦ 6号，中山湖会议室——最大容纳150人" << std::endl ;
    std::cout << "请输入会议室编号: ";
    std::cin >> roomID;
    std::cout << "请输入日期 (YYYY-MM-DD): ";
    std::cout << "Today is: " << std::put_time(&localTime, "%Y-%m-%d, %A") << "，您可以预约未来一周内工作日的会议室。" << std::endl;
    std::cin >> date;
    std::cout << "请输入时段 (上午:1/下午:2): ";
    std::cin >> timeSlotStr;

    // 将字符串转换为 TimeSlot 枚举
    if (timeSlotStr == 1) {
        timeSlot = TimeSlot::Morning;
    } else if (timeSlotStr == 2) {
        timeSlot = TimeSlot::Afternoon;
    } else {
        std::cout << "无效的时段输入。请重新输入。\n";
        return;  // 或者可以选择重试
    }

     // 日期合法性检查
    std::tm bookedDate = {};
    std::istringstream dateStream(date);
    dateStream >> std::get_time(&bookedDate, "%Y-%m-%d");
    if (dateStream.fail()) {
        std::cout << "无效的日期格式。请重新输入。\n";
        return;
    }

    // 检查日期是否在周一至周五
    std::time_t t_booked = std::mktime(&bookedDate);
    if (t_booked <= timeT || t_booked > timeT + 7 * 24 * 60 * 60 || bookedDate.tm_wday == 0 || bookedDate.tm_wday == 6) {
        std::cout << "只能预约未来一周内的工作日会议室。请重新输入。\n";
        return;
    }

    int userID = currentUser.getUserID(); // 获取当前用户ID

    if (dbManager.bookRoom(roomID, userID, date, timeSlot, initialStatus)) {
        std::cout << "预约成功。\n";
    } else {
        std::cout << "预约失败。\n";
    }
}

void BusinessMenu::viewAllBookings(DatabaseManager& dbManager) {
    dbManager.viewAllBookings();
}

void BusinessMenu::viewOwnBookings(DatabaseManager& dbManager) {
    int userID = currentUser.getUserID();
    dbManager.viewBookingsByUser(userID);
}

void BusinessMenu::cancelBooking(DatabaseManager& dbManager) {
    int bookingID;
    std::cout << "请输入您要取消的预约ID: ";
    std::cin >> bookingID;

    if (dbManager.cancelBooking(bookingID)) {
        std::cout << "预约已取消。\n";
    } else {
        std::cout << "取消预约失败。\n";
    }
}

void BusinessMenu::changePassword(DatabaseManager& dbManager) {
    std::string oldPassword, newPassword;
    std::cout << "请输入当前密码：";
    std::cin >> oldPassword;

    // 验证旧密码
    if (currentUser.getPassword() != oldPassword) {
        std::cout << "当前密码错误。\n";
        return;
    }

    std::cout << "请输入新密码：";
    std::cin >> newPassword;

    // 更新密码
    if (dbManager.updatePassword(currentUser.getUserID(), newPassword)) {
        std::cout << "密码已更新。\n";
        currentUser.setPassword(newPassword); // 更新内存中的用户密码
    }
    else {
        std::cout << "密码更新失败。\n";
    }
}

// ExecutiveMenu 类的实现
void ExecutiveMenu::display() {
    while (true) {
        std::cout << "行政人员菜单:\n";
        std::cout << "1. 查看所有预约\n";
        std::cout << "2. 审批预约\n";
        std::cout << "3. 返回主菜单\n";

        int choice;
        std::cout << "请输入您的选择：";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            std::cout << "无效的输入，请重试。\n";
            continue;
        }

        switch (choice) {
            case 1:
                viewAllBookings(dbManager);
                break;
            case 2:
                approveBooking(dbManager);
                break;
            case 3:
                return; // 返回主菜单
            default:
                std::cout << "无效选择，请重试。\n";
        }
    }
}

void ExecutiveMenu::viewAllBookings(DatabaseManager& dbManager) {
    dbManager.viewAllBookings();
}

void ExecutiveMenu::approveBooking(DatabaseManager& dbManager) {
    int bookingID,newStatusStr;
    BookingStatus newStatus;

    std::cout << "请输入要审批的预约ID: ";
    std::cin >> bookingID;
    std::cout << "请输入新状态 (通过:1/不通过:2): ";
    std::cin >> newStatusStr;

    if (newStatusStr == 1) {
        newStatus = BookingStatus::Approved;
    } else if (newStatusStr == 2) {
        newStatus = BookingStatus::Rejected;
    } else {
        std::cout << "无效的状态输入，请输入 '通过' 或 '不通过'。\n";
        return;
    }

    if (dbManager.updateBookingStatus(bookingID, newStatus)) {
        std::cout << "预约状态已更新。\n";
    } else {
        std::cout << "更新预约状态失败。\n";
    }
}



// AdminMenu 类的实现
void AdminMenu::display() {
    while (true) {
        std::cout << "管理员菜单:\n";
        std::cout << "1. 添加账户\n";
        std::cout << "2. 查看账户\n";
        std::cout << "3. 查看会议室信息\n";
        std::cout << "4. 清空预约\n";
        std::cout << "5. 删除用户\n";  
        std::cout << "0. 返回主菜单\n";

        int choice;
        std::cout << "请输入您的选择：";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            std::cout << "无效的输入，请重试。\n";
            continue;
        }

        switch (choice) {
            case 1:
                addUser(dbManager);
                break;
            case 2:
                viewUsers(dbManager);
                break;
            case 3:
                viewMeetingRooms(dbManager);
                break;
            case 4:
                clearBookings(dbManager);
                break;
            case 5:  // 对应删除用户功能
                deleteUser(dbManager);
                break;
            case 0:
                return; // 返回主菜单
            default:
                std::cout << "无效选择，请重试。\n";
        }
    }
}

void AdminMenu::addUser(DatabaseManager& dbManager) {
    std::string employeeID, name, username, password;
    int userTypeInput;
    std::cout << "请输入新账户的工号: ";
    std::cin >> employeeID;
    std::cout << "请输入姓名: ";
    std::cin >> name;
    std::cout << "请输入用户名: ";
    std::cin >> username;
    std::cout << "请输入密码: ";
    std::cin >> password;
    std::cout << "请输入用户类型 (1-业务人员, 2-行政人员): ";
    std::cin >> userTypeInput;

    UserType userType = static_cast<UserType>(userTypeInput - 1);
    if (dbManager.addUser(employeeID, name, username, password, userType)) {
        std::cout << "账户添加成功。\n";
    } else {
        std::cout << "账户添加失败。\n";
    }
}


void AdminMenu::viewUsers(DatabaseManager& dbManager) {
    std::cout << "查看账户:\n";
    std::cout << "1. 业务人员账户\n";
    std::cout << "2. 行政人员账户\n";

    int choice;
    std::cin >> choice;

    UserType userType = (choice == 1) ? UserType::Business : UserType::Executive;
    dbManager.viewUsers(userType);
}


void AdminMenu::viewMeetingRooms(DatabaseManager& dbManager) {
    dbManager.viewMeetingRooms();
}


void AdminMenu::clearBookings(DatabaseManager& dbManager) {
    if (dbManager.clearBookings()) {
        std::cout << "所有预约已清空。\n";
    } else {
        std::cout << "清空预约失败。\n";
    }
}

void AdminMenu::deleteUser(DatabaseManager& dbManager) {
    int userID;
    std::cout << "请输入要删除的用户ID：";
    std::cin >> userID;

    // 确保当前用户是管理员
    if (currentUser.getUserType() != UserType::Admin) {
        std::cout << "只有管理员可以执行此操作。\n";
        return;
    }

    // 二次确认
    std::string confirmation;
    std::cout << "确认要删除用户ID为 " << userID << " 的用户吗？(yes/no): ";
    std::cin >> confirmation;
    if (confirmation != "yes") {
        std::cout << "操作已取消。\n";
        return;
    }

    if (dbManager.handleAssociatedData(userID)) {
        if (dbManager.deleteUser(userID)) {
            std::cout << "用户及其相关数据已成功删除。\n";
        }
        else {
            std::cout << "删除用户失败。\n";
        }
    }
    else {
        std::cout << "处理关联数据失败，未删除用户。\n";
    }
}

