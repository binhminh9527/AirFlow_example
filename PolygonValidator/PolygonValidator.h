#ifndef POLYGON_VALIDATOR_H
#define POLYGON_VALIDATOR_H

#include <string>
#include <ogrsf_frmts.h>

class PolygonValidator {
public:
    // Returns true if polygon passes basic and GEOS validity checks.
    // Optional err will contain a concise reason when invalid.
    static bool isValid(const OGRPolygon& poly, std::string* err = nullptr);

private:
    static bool hasExteriorRing(const OGRPolygon& poly, std::string* err);
    static bool ringHasMinimumPoints(const OGRLinearRing* ring, std::string* err);
    static bool ringIsClosed(const OGRLinearRing* ring, std::string* err);
    static bool coordsAreFinite(const OGRLinearRing* ring, std::string* err);
    static bool geosIsValid(const OGRPolygon& poly, std::string* err);
    
    // Additional validation checks for specific invalid cases
    static bool hasNoDuplicateConsecutivePoints(const OGRLinearRing* ring, std::string* err);
    static bool hasMinimumDistinctPoints(const OGRLinearRing* ring, std::string* err);
    static bool isNotSelfIntersecting(const OGRPolygon& poly, std::string* err);
    static bool hasCorrectWindingOrder(const OGRLinearRing* ring, bool isOuter, std::string* err);
    static bool isNotCollinear(const OGRLinearRing* ring, std::string* err);
    static bool holesAreContainedInOuter(const OGRPolygon& poly, std::string* err);
    static bool holesDoNotTouchOuter(const OGRPolygon& poly, std::string* err);
    static bool ringsDoNotOverlap(const OGRPolygon& poly, std::string* err);
    
    // Helper functions
    static double computeSignedArea(const OGRLinearRing* ring);
    static bool pointsAreEqual(double x1, double y1, double x2, double y2, double eps = 1e-9);
    static bool areCollinear(double x1, double y1, double x2, double y2, double x3, double y3, double eps = 1e-9);
};

#endif // POLYGON_VALIDATOR_H
