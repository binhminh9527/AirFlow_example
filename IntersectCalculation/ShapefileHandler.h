#ifndef SHAPEFILE_HANDLER_H
#define SHAPEFILE_HANDLER_H

#include <vector>
#include <string>
#include <utility>
#include <ogrsf_frmts.h>
class ShapefileHandler {
private:
    std::vector<OGRPolygon> polygons;
    std::string shapefilePath;

public:
    ShapefileHandler(const std::string& path);
    ~ShapefileHandler();
    
    // Load all polygons from shapefile
    bool loadPolygons();
    
    // Get all loaded polygons
    const std::vector<OGRPolygon>& getPolygons() const;
    
    // Get polygon count
    size_t getPolygonCount() const;
    
    // Print polygon information
    void printPolygonInfo(size_t index) const;
    
    // Clear all loaded polygons
    void clear();
};

#endif // SHAPEFILE_HANDLER_H
