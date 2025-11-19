#include "DatabaseHandler.h"
#include "IntersectCalculation.h"
#include <iostream>
#include <vector>
#include <utility>

int main() {
    // Initialize database handler
    DatabaseHandler dbHandler("polygons_db");

    // Fetch polygon owners (mocked for now)
    std::vector<std::string> owners = dbHandler.getPolygonOwners();

    // Define some mock polygons
    std::vector<std::vector<std::pair<double, double>>> polygons = {
        {{0, 0}, {0, 1}, {1, 1}, {1, 0}},
        {{2, 2}, {2, 3}, {3, 3}, {3, 2}}
    };

    // Initialize intersection calculation
    IntersectCalculation intersectCalc;
    std::vector<std::string> resultOwners = intersectCalc.calculateIntersections(polygons);

    // Print results
    std::cout << "Intersected Owners:" << std::endl;
    for (const auto& owner : resultOwners) {
        std::cout << owner << std::endl;
    }

    return 0;
}