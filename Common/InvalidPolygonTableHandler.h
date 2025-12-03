#ifndef INVALID_POLYGON_TABLE_HANDLER_H
#define INVALID_POLYGON_TABLE_HANDLER_H

#include <string>
#include <libpq-fe.h>

class InvalidPolygonTableHandler {
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
    InvalidPolygonTableHandler(const std::string& host = "polygons_db", 
                               const std::string& port = "5432",
                               const std::string& dbname = "polygons_db",
                               const std::string& user = "polygons_user",
                               const std::string& password = "polygons_pass");
    
    ~InvalidPolygonTableHandler();

    bool isConnected() const;
    bool createInvalidWildfireTable();
    bool setWildfireValidity(int polygonId, bool isInvalid);
    bool isWildfireInvalid(int polygonId, bool& isInvalid);
    bool getWildfireValidity(int polygonId, bool& isInvalid);
};

#endif // INVALID_POLYGON_TABLE_HANDLER_H
