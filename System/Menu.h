// menu.h
#ifndef MENU_H
#define MENU_H

#include "DatabaseManager.h"
#include <limits>
#include <chrono>
#include <sstream>
#include <ctime>


class Menu {
public:
    virtual void display() = 0;
};

class BusinessMenu : public Menu {
private:
    DatabaseManager& dbManager;
    User currentUser;
public:
    BusinessMenu(DatabaseManager& dbManager, User currentUser) : dbManager(dbManager), currentUser(currentUser) {}
    void display() override;
    void bookRoom(DatabaseManager& dbManager);
    void viewAllBookings(DatabaseManager& dbManager);
    void viewOwnBookings(DatabaseManager& dbManager);
    void cancelBooking(DatabaseManager& dbManager);
    void changePassword(DatabaseManager& dbManager);
};

class ExecutiveMenu : public Menu {
private:
    DatabaseManager& dbManager;
    User currentUser;
public:
    ExecutiveMenu(DatabaseManager& dbManager, User currentUser) : dbManager(dbManager), currentUser(currentUser) {}
    void display() override;
    void viewAllBookings(DatabaseManager& dbManager);
    void approveBooking(DatabaseManager& dbManager);
};

class AdminMenu : public Menu {
private:
    DatabaseManager& dbManager;
    User currentUser;
public:
    AdminMenu(DatabaseManager& dbManager, User currentUser) : dbManager(dbManager), currentUser(currentUser) {}
    void display() override;
    void addUser(DatabaseManager& dbManager);
    void viewUsers(DatabaseManager& dbManager);
    void viewMeetingRooms(DatabaseManager& dbManager);
    void clearBookings(DatabaseManager& dbManager);
    void deleteUser(DatabaseManager& dbManager);
};




#endif
