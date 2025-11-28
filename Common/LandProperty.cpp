#include "LandProperty.h"
#include <iostream>

LandProperty::LandProperty() : id(0), owner("") {
}

int LandProperty::getId() const {
    return id;
}

std::string LandProperty::getOwner() const {
    return owner;
}

const OGRPolygon& LandProperty::getPolygon() const {
    return polygon;
}

void LandProperty::printPolygonInfo() const {
    std::cout << "  ID: " << id << std::endl;
    std::cout << "  Owner: " << owner << std::endl;
    
    const OGRLinearRing* ring = polygon.getExteriorRing();
    if (ring != nullptr) {
        std::cout << "  Polygon points: " << ring->getNumPoints() << std::endl;
        if (ring->getNumPoints() > 0) {
            std::cout << "  First coordinate: (" 
                      << ring->getX(0) << ", " 
                      << ring->getY(0) << ")" << std::endl;
        }
    }
}

void LandProperty::addProperty(int propId, 
                                const std::string& propOwner, 
                                const OGRPolygon& coords) {
    this->id = propId;
    this->owner = propOwner;
    this->polygon = coords;
}
