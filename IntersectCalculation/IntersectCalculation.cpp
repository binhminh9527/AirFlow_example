#include "IntersectCalculation.h"
#include <iostream>
#include <algorithm>
#include <limits>

bool IntersectCalculation::isIntersect( 
    const std::vector<std::pair<double, double>>& targetArea, 
    const std::vector<std::vector<std::pair<double, double>>>& disasterArea ) {
    
    // Get bounding box of target area
    if (targetArea.empty()) {
        return false;
    }
    
    double targetMinX = std::numeric_limits<double>::max();
    double targetMaxX = std::numeric_limits<double>::lowest();
    double targetMinY = std::numeric_limits<double>::max();
    double targetMaxY = std::numeric_limits<double>::lowest();
    
    for (const auto& point : targetArea) {
        targetMinX = std::min(targetMinX, point.first);
        targetMaxX = std::max(targetMaxX, point.first);
        targetMinY = std::min(targetMinY, point.second);
        targetMaxY = std::max(targetMaxY, point.second);
    }
    
    // Check intersection with each disaster area polygon
    for (const auto& disasterPolygon : disasterArea) {
        if (disasterPolygon.empty()) {
            continue;
        }
        
        // Get bounding box of disaster polygon
        double disasterMinX = std::numeric_limits<double>::max();
        double disasterMaxX = std::numeric_limits<double>::lowest();
        double disasterMinY = std::numeric_limits<double>::max();
        double disasterMaxY = std::numeric_limits<double>::lowest();
        
        for (const auto& point : disasterPolygon) {
            disasterMinX = std::min(disasterMinX, point.first);
            disasterMaxX = std::max(disasterMaxX, point.first);
            disasterMinY = std::min(disasterMinY, point.second);
            disasterMaxY = std::max(disasterMaxY, point.second);
        }
        
        // Check if bounding boxes intersect
        bool xOverlap = targetMinX <= disasterMaxX && targetMaxX >= disasterMinX;
        bool yOverlap = targetMinY <= disasterMaxY && targetMaxY >= disasterMinY;
        
        if (xOverlap && yOverlap) {
            // Bounding boxes intersect - return true
            return true;
        }
    }
    
    // No intersection found
    return false;
}