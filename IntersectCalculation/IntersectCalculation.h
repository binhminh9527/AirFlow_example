#ifndef INTERSECT_CALCULATION_H
#define INTERSECT_CALCULATION_H

#include <vector>
#include <string>
#include <utility>

class IntersectCalculation {
public:
    std::vector<std::string> calculateIntersections(const std::vector<std::vector<std::pair<double, double>>>& polygons);
};

#endif // INTERSECT_CALCULATION_H