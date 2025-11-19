#include "DatabaseHandler.h"
#include <iostream>

DatabaseHandler::DatabaseHandler(const std::string& db_name) {
    // Initialize database connection
    std::cout << "Connecting to database: " << db_name << std::endl;
}

std::vector<std::string> DatabaseHandler::getPolygonOwners() {
    // Mock implementation: Replace with actual database query
    return {"Alice", "Bob", "Charlie"};
}