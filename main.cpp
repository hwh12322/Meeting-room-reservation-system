#include "DatabaseManager.h"
#include "Menu.h"
#include "MenuFactory.h"
#include <vector>
#include <stdexcept>
#include <limits>
#include <memory>
#include <future>
#include <atomic>
#include <random>

#define _CRT_SECURE_NO_WARNINGS



std::mutex logMutex;

void safeLogOutput(const std::string& message) {
    std::lock_guard<std::mutex> guard(logMutex);
    std::cout << message << std::endl;
}

void simulateUser(int userID, std::atomic<bool>& stopFlag) {
    DatabaseManager dbManager("127.0.0.1", "root", "hwh272106", "Kingsoft");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> roomDis(1, 6); // 假设有 6 个会议室
    std::uniform_int_distribution<> timeSlotDis(0, 1); // 0 代表上午，1 代表下午
    std::uniform_int_distribution<int> dateDis(1, 30); // 假设一个月内的任何一天

    while (!stopFlag) {
        int roomID = roomDis(gen);
        TimeSlot timeSlot = timeSlotDis(gen) == 0 ? TimeSlot::Morning : TimeSlot::Afternoon;

        // 生成随机日期字符串
        auto days = dateDis(gen);
        auto random_date = std::chrono::system_clock::now() + std::chrono::hours(24 * days);
        time_t tt = std::chrono::system_clock::to_time_t(random_date);
        std::tm localTime = {};  // 创建 std::tm 结构体实例
        localtime_s(&localTime, &tt);  // 使用 localtime_s 函数

        char buffer[11];
        if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &localTime)) {
            std::string date(buffer);

            // 模拟预约会议室
            dbManager.bookRoom(roomID, userID, date, timeSlot, BookingStatus::Pending);
        }
    }
}


void simulateAdminApproving( int adminID,std::atomic<bool>& stopFlag) {
    DatabaseManager dbManager("127.0.0.1", "root", "hwh272106", "Kingsoft");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1); // 用于随机决定通过或不通过

    while (!stopFlag) {
        int bookingID = dbManager.getPendingBookingID();
        if (bookingID != -1) {
            // 模拟审批决策
            BookingStatus newStatus = dis(gen) == 0 ? BookingStatus::Approved : BookingStatus::Rejected;
            // 尝试更新预订状态
            bool res = dbManager.updateBookingStatus(bookingID, newStatus);
            if ( res == true ){
                safeLogOutput("Admin " + std::to_string(adminID) + " processed booking " + std::to_string(bookingID) + " with status " + (newStatus == BookingStatus::Approved ? "Approved" : "Rejected") ); 
            }
        } 
        else {
            // 没有找到待审批的预订，等待一段时间后再次尝试
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}


int main() {
    /*
    const int userCount = 5000; // 模拟的用户数量
    std::atomic<bool> stopFlag(false); 

    // 创建并启动模拟用户
    std::vector<std::future<void>> futures;
    std::vector<std::future<void>> futures1;
    for (int i = 1; i <= 0; ++i) {
        futures.emplace_back(std::async(std::launch::async, simulateUser, i, std::ref(stopFlag)));
    }

    for (int i = 1; i <= 3000; ++i) { // 假设有 3 个行政人员
        futures1.emplace_back(std::async(std::launch::async, simulateAdminApproving, i, std::ref(stopFlag)));
    }

    // 运行模拟一段时间后停止
    std::this_thread::sleep_for(std::chrono::seconds(10)); // 模拟运行
    stopFlag = true;   

    // 等待所有模拟操作完成
    for (auto& future : futures) {
        future.get();
    }
    for (auto& future : futures1) {
        future.get();
    }
    return 0;
    */

    // 初始化数据库连接
    const std::string server = "127.0.0.1";
    const std::string dbUser = "root";  
    const std::string dbPassword = "hwh272106";  
    const std::string database = "Kingsoft";  
    DatabaseManager dbManager(server, dbUser, dbPassword, database);
    int choice;
    try {
        while (true) {
            std::cout << "欢迎使用金山办公会议室预约系统，请选择登录身份:\n";
            std::cout << "1. 业务人员\n";
            std::cout << "2. 行政人员\n";
            std::cout << "3. 系统管理员\n";
            std::cout << "4. 退出\n";

            std::cin >> choice;

            if (std::cin.fail() || choice < 1 || choice > 4) {
                std::cin.clear();
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');  
                std::cout << "无效选择，请重试。\n";
                continue;
            }

            if (choice == 4) break; // 用户选择退出

            std::string username, password;
            std::cout << "请输入账号：";
            std::cin >> username;
            std::cout << "请输入密码：";
            std::cin >> password;

            User currentUser;
            try {
                currentUser = dbManager.getUserByUsernameAndPassword(username, password);
            } catch (const std::runtime_error& e) {
                std::cerr << "登录失败: " << e.what() << std::endl;
                continue;
            }

            // 根据用户身份显示相应的菜单
            std::unique_ptr<MenuFactory> menuFactory;

            switch (currentUser.getUserType()) {
            case UserType::Business:
                menuFactory = std::make_unique<BusinessMenuFactory>();
                break;
            case UserType::Executive:
                menuFactory = std::make_unique<ExecutiveMenuFactory>();
                break;
            case UserType::Admin:
                menuFactory = std::make_unique<AdminMenuFactory>();
                break;
            }

            if (menuFactory) {
                auto menu = menuFactory->createMenu(dbManager, currentUser);
                menu->display();
            }
            else {
                std::cout << "菜单初始化失败！" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "发生错误: " << e.what() << std::endl;
        return 1;
    }
 
    return 0;

}