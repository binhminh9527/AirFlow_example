#include "PolygonValidator.h"
#include "../Common/ShapefileHandler.h"
#include <iostream>
#include <string>
#include <vector>

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " <shapefile_path>" << std::endl;
    std::cout << "Validates all polygons in the given shapefile." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string shapefilePath = argv[1];
    
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
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Validation Summary:" << std::endl;
    std::cout << "  Total polygons: " << polygons.size() << std::endl;
    std::cout << "  Valid:          " << validCount << std::endl;
    std::cout << "  Invalid:        " << invalidCount << std::endl;
    std::cout << "========================================" << std::endl;
    
    return (invalidCount > 0) ? 1 : 0;
}
