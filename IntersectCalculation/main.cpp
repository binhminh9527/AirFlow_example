#include "DatabaseHandler.h"
#include "IntersectCalculation.h"
#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>

int main() {
    try {
        // Get database connection parameters from environment variables or use defaults

        // Initialize database handler with connection parameters
        DatabaseHandler polygondb_Handler("polygons_db", "5432", "polygons_db", "polygons_user", "polygons_pass");
        if (!polygondb_Handler.isConnected()) {
            std::cerr << "Failed to connect to database. Exiting." << std::endl;
            return 1;
        }
        
        DatabaseHandler resultsdb_Handler("results_db", "5432", "results_db", "results_user", "results_pass");
        if (!resultsdb_Handler.isConnected()) {
            std::cerr << "Failed to connect to results database. Exiting." << std::endl;
            return 1;
        }

        // Fetch polygons from database
        std::vector<Polygon> polygons = polygondb_Handler.getPolygons();
        
        std::cout << "\nPolygons in database:" << std::endl;
        for (const auto& poly : polygons) {
            std::cout << "  ID: " << poly.id 
                      << ", Name: " << poly.name 
                      << ", Owner: " << poly.owner << std::endl;
        }

        // Fetch unique owners
        std::vector<std::string> owners = polygondb_Handler.getPolygonOwners();
        
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