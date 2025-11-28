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
};

#endif // POLYGON_VALIDATOR_H
