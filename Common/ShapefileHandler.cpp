#include "ShapefileHandler.h"
#include <iostream>
#include <ogrsf_frmts.h>

ShapefileHandler::ShapefileHandler(const std::string& path)
    : shapefilePath(path) {
    loadPolygons();
}

ShapefileHandler::~ShapefileHandler() {
    clear();
}

bool ShapefileHandler::loadPolygons() {
    // Register all GDAL drivers
    GDALAllRegister();
    
    // Open the shapefile
    GDALDataset* poDS = (GDALDataset*) GDALOpenEx(shapefilePath.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
    if (poDS == nullptr) {
        std::cerr << "Failed to open shapefile: " << shapefilePath << std::endl;
        return false;
    }
    // Assume the first layer contains the polygons
    OGRLayer* poLayer = poDS->GetLayer(0);
    if (poLayer == nullptr) {
        std::cerr << "Failed to get layer from shapefile: " << shapefilePath << std::endl;
        GDALClose(poDS);
        return false;
    }
    
    // Iterate through all features in the layer
    OGRFeature* poFeature;
    poLayer->ResetReading();
    while ((poFeature = poLayer->GetNextFeature()) != nullptr) {
        OGRGeometry* poGeometry = poFeature->GetGeometryRef();
        if (poGeometry != nullptr) {
            OGRwkbGeometryType geoType = wkbFlatten(poGeometry->getGeometryType());
            
            if (geoType == wkbPolygon) {
                OGRPolygon* poPolygon = poGeometry->toPolygon();
                polygons.push_back(*poPolygon);
            }
            else if (geoType == wkbMultiPolygon) {
                OGRMultiPolygon* poMultiPolygon = poGeometry->toMultiPolygon();
                for (int j = 0; j < poMultiPolygon->getNumGeometries(); j++) {
                    OGRPolygon* poPolygon = (OGRPolygon*)poMultiPolygon->getGeometryRef(j);
                    polygons.push_back(*poPolygon);
                }
            }
        }
        OGRFeature::DestroyFeature(poFeature);
    }
    
    GDALClose(poDS);
    return true;
}

const std::vector<OGRPolygon>& ShapefileHandler::getPolygons() const {
    return polygons;
}

size_t ShapefileHandler::getPolygonCount() const {
    return polygons.size();
}


void ShapefileHandler::printPolygonInfo(size_t index) const {
    if (index >= polygons.size()) {
        std::cout << "Invalid polygon index: " << index << std::endl;
        return;
    }
    
    const OGRPolygon& polygon = polygons[index];
    const OGRLinearRing* poRing = polygon.getExteriorRing();
    
    if (poRing == nullptr) {
        std::cout << "Polygon #" << index << " has no exterior ring" << std::endl;
        return;
    }
    
    std::cout << "Polygon #" << index << std::endl;
    std::cout << "  Number of points: " << poRing->getNumPoints() << std::endl;
    
    if (poRing->getNumPoints() > 0) {
        std::cout << "  First point: (" << poRing->getX(0) << ", " << poRing->getY(0) << ")" << std::endl;
        if (poRing->getNumPoints() > 1) {
            int lastIdx = poRing->getNumPoints() - 1;
            std::cout << "  Last point: (" << poRing->getX(lastIdx) << ", " << poRing->getY(lastIdx) << ")" << std::endl;
        }
    }
}

void ShapefileHandler::clear() {
    polygons.clear();
}
