#include "DatabaseHandler.h"
#include "IntersectCalculation.h"
#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>

int main() {
    try {
        // Get database connection parameters from environment variables or use defaults
        std::string host = std::getenv("PGHOST") ? std::getenv("PGHOST") : "polygons_db";
        std::string port = std::getenv("PGPORT") ? std::getenv("PGPORT") : "5432";
        std::string dbname = std::getenv("PGDATABASE") ? std::getenv("PGDATABASE") : "polygons_db";
        std::string user = std::getenv("PGUSER") ? std::getenv("PGUSER") : "polygons_user";
        std::string password = std::getenv("PGPASSWORD") ? std::getenv("PGPASSWORD") : "polygons_pass";

        // Initialize database handler with connection parameters
        DatabaseHandler dbHandler(host, port, dbname, user, password);

        if (!dbHandler.isConnected()) {
            std::cerr << "Failed to connect to database. Exiting." << std::endl;
            return 1;
        }

        // Fetch polygons from database
        std::vector<Polygon> polygons = dbHandler.getPolygons();
        
        std::cout << "\nPolygons in database:" << std::endl;
        for (const auto& poly : polygons) {
            std::cout << "  ID: " << poly.id 
                      << ", Name: " << poly.name 
                      << ", Owner: " << poly.owner << std::endl;
        }

        // Fetch unique owners
        std::vector<std::string> owners = dbHandler.getPolygonOwners();
        
        std::cout << "\nUnique owners:" << std::endl;
        for (const auto& owner : owners) {
            std::cout << "  - " << owner << std::endl;
        }

        // Define some mock polygons for intersection calculation
        std::vector<std::vector<std::pair<double, double>>> mockPolygons = {
            {{0, 0}, {0, 1}, {1, 1}, {1, 0}},
            {{2, 2}, {2, 3}, {3, 3}, {3, 2}}
        };

        // Initialize intersection calculation
        IntersectCalculation intersectCalc;
        std::vector<std::string> resultOwners = intersectCalc.calculateIntersections(mockPolygons);

        // Print results
        std::cout << "\nIntersection calculation results:" << std::endl;
        for (const auto& owner : resultOwners) {
            std::cout << "  - " << owner << std::endl;
        }

        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}