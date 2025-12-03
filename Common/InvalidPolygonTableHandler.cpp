#include "InvalidPolygonTableHandler.h"
#include <iostream>
#include <sstream>
#include <cstring>

InvalidPolygonTableHandler::InvalidPolygonTableHandler(const std::string& host, 
                                                       const std::string& port,
                                                       const std::string& dbname,
                                                       const std::string& user,
                                                       const std::string& password)
    : conn(nullptr), host(host), port(port), dbname(dbname), user(user), password(password) {
    connect();
    
    // Automatically create table if it doesn't exist
    if (isConnected()) {
        createInvalidWildfireTable();
    }
}

InvalidPolygonTableHandler::~InvalidPolygonTableHandler() {
    disconnect();
}

void InvalidPolygonTableHandler::connect() {
    std::ostringstream conninfo;
    conninfo << "host=" << host 
             << " port=" << port 
             << " dbname=" << dbname 
             << " user=" << user 
             << " password=" << password;
    
    conn = PQconnectdb(conninfo.str().c_str());
    
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        conn = nullptr;
    } else {
        std::cout << "Successfully connected to database: " << dbname << std::endl;
    }
}

void InvalidPolygonTableHandler::disconnect() {
    if (conn) {
        PQfinish(conn);
        conn = nullptr;
        std::cout << "Disconnected from database" << std::endl;
    }
}

bool InvalidPolygonTableHandler::isConnected() const {
    return conn != nullptr && PQstatus(conn) == CONNECTION_OK;
}

bool InvalidPolygonTableHandler::createInvalidWildfireTable() {
    if (!isConnected()) {
        std::cerr << "Not connected to database" << std::endl;
        return false;
    }
    
    const char* createTableQuery = 
        "CREATE TABLE IF NOT EXISTS invalid_wildfire ("
        "    polygon_id INTEGER PRIMARY KEY,"
        "    is_invalid SMALLINT NOT NULL CHECK (is_invalid IN (0, 1))"
        ")";
    
    PGresult* res = PQexec(conn, createTableQuery);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "CREATE TABLE failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }
    
    PQclear(res);
    std::cout << "Table invalid_wildfire created successfully" << std::endl;
    return true;
}

bool InvalidPolygonTableHandler::setWildfireValidity(int polygonId, bool isInvalid) {
    if (!isConnected()) {
        std::cerr << "Not connected to database" << std::endl;
        return false;
    }
    
    std::ostringstream queryStream;
    queryStream << "INSERT INTO invalid_wildfire (polygon_id, is_invalid) "
                << "VALUES (" << polygonId << ", " << (isInvalid ? 1 : 0) << ") "
                << "ON CONFLICT (polygon_id) DO UPDATE SET is_invalid = " << (isInvalid ? 1 : 0);
    
    PGresult* res = PQexec(conn, queryStream.str().c_str());
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "INSERT/UPDATE failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }
    
    PQclear(res);
    return true;
}

bool InvalidPolygonTableHandler::isWildfireInvalid(int polygonId, bool& isInvalid) {
    if (!isConnected()) {
        std::cerr << "Not connected to database" << std::endl;
        return false;
    }
    
    std::ostringstream queryStream;
    queryStream << "SELECT is_invalid FROM invalid_wildfire WHERE polygon_id = " << polygonId;
    
    PGresult* res = PQexec(conn, queryStream.str().c_str());
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SELECT query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }
    
    if (PQntuples(res) == 0) {
        isInvalid = false;
        PQclear(res);
        return true;
    }
    
    int value = std::atoi(PQgetvalue(res, 0, 0));
    isInvalid = (value == 1);
    
    PQclear(res);
    return true;
}

bool InvalidPolygonTableHandler::getWildfireValidity(int polygonId, bool& isInvalid) {
    if (!isConnected()) {
        std::cerr << "Not connected to database" << std::endl;
        return false;
    }
    
    std::ostringstream queryStream;
    queryStream << "SELECT is_invalid FROM invalid_wildfire WHERE polygon_id = " << polygonId;
    
    PGresult* res = PQexec(conn, queryStream.str().c_str());
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SELECT query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }
    
    if (PQntuples(res) == 0) {
        // Polygon not found in table
        PQclear(res);
        return false;
    }
    
    int value = std::atoi(PQgetvalue(res, 0, 0));
    isInvalid = (value == 1);
    
    PQclear(res);
    return true;
}
