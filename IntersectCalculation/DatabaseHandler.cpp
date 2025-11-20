#include "DatabaseHandler.h"
#include <iostream>
#include <sstream>
#include <cstring>

DatabaseHandler::DatabaseHandler(const std::string& host, 
                                 const std::string& port,
                                 const std::string& dbname,
                                 const std::string& user,
                                 const std::string& password)
    : conn(nullptr), host(host), port(port), dbname(dbname), user(user), password(password) {
    connect();
}

DatabaseHandler::~DatabaseHandler() {
    disconnect();
}

void DatabaseHandler::connect() {
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
        throw std::runtime_error("Failed to connect to database");
    }
    
    std::cout << "Successfully connected to database: " << dbname << std::endl;
}

void DatabaseHandler::disconnect() {
    if (conn) {
        PQfinish(conn);
        conn = nullptr;
        std::cout << "Disconnected from database" << std::endl;
    }
}

bool DatabaseHandler::isConnected() const {
    return conn != nullptr && PQstatus(conn) == CONNECTION_OK;
}

std::vector<Polygon> DatabaseHandler::getPolygons() {
    std::vector<Polygon> polygons;
    
    if (!isConnected()) {
        std::cerr << "Not connected to database" << std::endl;
        return polygons;
    }
    
    const char* query = "SELECT id, name, owner FROM polygons ORDER BY id";
    PGresult* res = PQexec(conn, query);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SELECT query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return polygons;
    }
    
    int rows = PQntuples(res);
    std::cout << "Retrieved " << rows << " polygon(s) from database" << std::endl;
    
    for (int i = 0; i < rows; i++) {
        Polygon poly;
        poly.id = std::atoi(PQgetvalue(res, i, 0));
        poly.name = PQgetvalue(res, i, 1);
        poly.owner = PQgetvalue(res, i, 2);
        polygons.push_back(poly);
    }
    
    PQclear(res);
    return polygons;
}

std::vector<std::string> DatabaseHandler::getPolygonOwners() {
    std::vector<std::string> owners;
    
    if (!isConnected()) {
        std::cerr << "Not connected to database" << std::endl;
        return owners;
    }
    
    const char* query = "SELECT DISTINCT owner FROM polygons ORDER BY owner";
    PGresult* res = PQexec(conn, query);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SELECT query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return owners;
    }
    
    int rows = PQntuples(res);
    std::cout << "Retrieved " << rows << " unique owner(s) from database" << std::endl;
    
    for (int i = 0; i < rows; i++) {
        owners.push_back(PQgetvalue(res, i, 0));
    }
    
    PQclear(res);
    return owners;
}