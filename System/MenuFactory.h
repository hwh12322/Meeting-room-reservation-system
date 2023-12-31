#ifndef MENUFACTORY_H
#define MENUFACTORY_H

#include"Menu.h"
class MenuFactory {
public:
    virtual ~MenuFactory() {}
    virtual std::unique_ptr<Menu> createMenu(DatabaseManager& dbManager, User currentUser) = 0;
};

class BusinessMenuFactory : public MenuFactory {
public:
    std::unique_ptr<Menu> createMenu(DatabaseManager& dbManager, User currentUser) override {
        return std::make_unique<BusinessMenu>(dbManager, currentUser);
    }
};

class ExecutiveMenuFactory : public MenuFactory {
public:
    std::unique_ptr<Menu> createMenu(DatabaseManager& dbManager, User currentUser) override {
        return std::make_unique<ExecutiveMenu>(dbManager, currentUser);
    }
};

class AdminMenuFactory : public MenuFactory {
public:
    std::unique_ptr<Menu> createMenu(DatabaseManager& dbManager, User currentUser) override {
        return std::make_unique<AdminMenu>(dbManager, currentUser);
    }
};
#endif // MENUFACTORY_H