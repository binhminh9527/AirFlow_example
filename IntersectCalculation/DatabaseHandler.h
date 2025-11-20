#ifndef DATABASE_HANDLER_H
#define DATABASE_HANDLER_H

#include <string>
#include <vector>
#include <memory>
#include <libpq-fe.h>

struct Polygon {
    std::string owner;
    std::string name;
    int id;
};

class DatabaseHandler {
private:
    PGconn* conn;
    std::string host;
    std::string port;
    std::string dbname;
    std::string user;
    std::string password;
    
    void connect();
    void disconnect();

public:
    DatabaseHandler(const std::string& host = "polygons_db", 
                   const std::string& port = "5432",
                   const std::string& dbname = "polygons_db",
                   const std::string& user = "polygons_user",
                   const std::string& password = "polygons_pass");
    
    ~DatabaseHandler();

    std::vector<Polygon> getPolygons();
    std::vector<std::string> getPolygonOwners();
    bool isConnected() const;
};

#endif // DATABASE_HANDLER_H