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

std::vector<LandProperty> DatabaseHandler::getLandProperties() {
    std::vector<LandProperty> properties;
    
    if (!isConnected()) {
        std::cerr << "Not connected to database" << std::endl;
        return properties;
    }
    
    const char* query = "SELECT id, owner, polygon FROM parcels_data ORDER BY id";
    PGresult* res = PQexec(conn, query);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "SELECT query failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return properties;
    }
    
    int rows = PQntuples(res);
    std::cout << "Retrieved " << rows << " land properties from database" << std::endl;
    
    for (int i = 0; i < rows; i++) {
        int id = std::atoi(PQgetvalue(res, i, 0));
        std::string owner = PQgetvalue(res, i, 1);
        std::string polygonJson = PQgetvalue(res, i, 2);
        
        // Parse the JSONB polygon data
        // Format: [[x1,y1],[x2,y2],...]
        OGRPolygon coords;
        
        // Remove outer brackets and parse
        size_t start = polygonJson.find('[');
        size_t end = polygonJson.rfind(']');
        
        if (start != std::string::npos && end != std::string::npos) {
            std::string coordsStr = polygonJson.substr(start + 1, end - start - 1);
            
            // Parse each coordinate pair [x,y]
            size_t pos = 0;
            OGRLinearRing t_ring;
            while ((pos = coordsStr.find('[', pos)) != std::string::npos) {
                size_t endBracket = coordsStr.find(']', pos);
                if (endBracket != std::string::npos) {
                    std::string pair = coordsStr.substr(pos + 1, endBracket - pos - 1);
                    size_t comma = pair.find(',');
                    if (comma != std::string::npos) {
                        double x = std::stod(pair.substr(0, comma));
                        double y = std::stod(pair.substr(comma + 1));
                        t_ring.addPoint(x, y);
                    }
                    pos = endBracket + 1;
                } else {
                    break;
                }
            }
            coords.addRing(&t_ring);
        }
        
        // Create a LandProperty and add this property to it
        LandProperty prop;
        prop.addProperty(id, owner, coords);
        
        // DEBUG: Print polygon

        char *wkt = nullptr;
        coords.exportToWkt(&wkt);
        std::cout << "OGRPolygon WKT: " << wkt << std::endl;
        CPLFree(wkt);
        // END DEBUG
        
        properties.push_back(prop);
    }
    
    PQclear(res);
    return properties;
}