#include "PolygonValidator.h"
#include "../Common/ShapefileHandler.h"
#include "InvalidPolygonTableHandler.h"
#include <iostream>
#include <string>
#include <vector>

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " <shapefile_path>" << std::endl;
    std::cout << "Validates all polygons in the given shapefile." << std::endl;
    std::cout << "If shapefile is wildfire data, stores validity in database." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string shapefilePath = argv[1];
    
    // Connect to database and auto-create table if needed
    InvalidPolygonTableHandler db("polygons_db", "5432", "polygons_db", "polygons_user", "polygons_pass");
    bool dbConnected = db.isConnected();
    
    if (!dbConnected) {
        std::cout << "Warning: Database not connected. Validation results won't be stored." << std::endl;
    }
    
    std::cout << "Loading shapefile: " << shapefilePath << std::endl;
    ShapefileHandler handler(shapefilePath);
    
    const auto& polygons = handler.getPolygons();
    
    if (polygons.empty()) {
        std::cerr << "No polygons found in shapefile." << std::endl;
        return 1;
    }
    
    std::cout << "Found " << polygons.size() << " polygons." << std::endl;
    std::cout << "\nValidating polygons...\n" << std::endl;
    
    int validCount = 0;
    int invalidCount = 0;
    
    for (size_t i = 0; i < polygons.size(); ++i) {
        std::string err;
        bool isValid = PolygonValidator::isValid(polygons[i], &err);
        
        if (isValid) {
            std::cout << "✓ Polygon " << i << ": VALID" << std::endl;
            validCount++;
        } else {
            std::cout << "✗ Polygon " << i << ": INVALID - " << err << std::endl;
            invalidCount++;
        }
        
        // Store result in database (1 = invalid, 0 = valid)
        if (dbConnected) {
            bool isInvalid = !isValid;
            if (!db.setWildfireValidity(static_cast<int>(i), isInvalid)) {
                std::cerr << "Warning: Failed to store validity for polygon " << i << std::endl;
            }
        }
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Validation Summary:" << std::endl;
    std::cout << "  Total polygons: " << polygons.size() << std::endl;
    std::cout << "  Valid:          " << validCount << std::endl;
    std::cout << "  Invalid:        " << invalidCount << std::endl;
    std::cout << "========================================" << std::endl;
    
    return (invalidCount > 100) ? 1 : 0;
}
