#ifndef DATABASE_HANDLER_H
#define DATABASE_HANDLER_H

#include <string>
#include <vector>

class DatabaseHandler {
public:
    DatabaseHandler(const std::string& db_name);

    std::vector<std::string> getPolygonOwners();
};

#endif // DATABASE_HANDLER_H