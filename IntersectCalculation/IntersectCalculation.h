#ifndef INTERSECT_CALCULATION_H
#define INTERSECT_CALCULATION_H

#include <vector>
#include <string>
#include <utility>

class IntersectCalculation {
public:
    bool isIntersect( 
        const std::vector<std::pair<double, double>>& targetArea, 
        const std::vector<std::vector<std::pair<double, double>>>& disasterArea );
};
#endif // INTERSECT_CALCULATION_H