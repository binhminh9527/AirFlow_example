#ifndef LAND_PROPERTY_H
#define LAND_PROPERTY_H

#include <vector>
#include <string>
#include <utility>
#include <ogrsf_frmts.h>
class LandProperty {
private:
    int id;
    std::string owner;
    OGRPolygon polygon;

public:
    LandProperty();
    
    // Get polygon data by index
    int getId() const;
    std::string getOwner() const;
    const OGRPolygon& getPolygon() const;
    void printPolygonInfo() const;
    
    // Add a single land property
    void addProperty(int propId, 
                    const std::string& propOwner, 
                    const OGRPolygon& coords);
};

#endif // LAND_PROPERTY_H
