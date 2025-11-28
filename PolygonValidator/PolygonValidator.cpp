#include "PolygonValidator.h"
#include <cmath>

static bool nearlyEqual(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) <= eps;
}

bool PolygonValidator::isValid(const OGRPolygon& poly, std::string* err) {
    if (!hasExteriorRing(poly, err)) return false;
    const OGRLinearRing* ring = poly.getExteriorRing();
    if (!ringHasMinimumPoints(ring, err)) return false;
    if (!ringIsClosed(ring, err)) return false;
    if (!coordsAreFinite(ring, err)) return false;
    if (!geosIsValid(poly, err)) return false;
    return true;
}

bool PolygonValidator::hasExteriorRing(const OGRPolygon& poly, std::string* err) {
    const OGRLinearRing* ring = poly.getExteriorRing();
    if (ring == nullptr) {
        if (err) *err = "Missing exterior ring";
        return false;
    }
    return true;
}

bool PolygonValidator::ringHasMinimumPoints(const OGRLinearRing* ring, std::string* err) {
    // For a valid polygon ring, need at least 4 points (including closing point)
    const int n = ring->getNumPoints();
    if (n < 4) {
        if (err) *err = "Ring has fewer than 4 points";
        return false;
    }
    return true;
}

bool PolygonValidator::ringIsClosed(const OGRLinearRing* ring, std::string* err) {
    const int n = ring->getNumPoints();
    const double x0 = ring->getX(0);
    const double y0 = ring->getY(0);
    const double xN = ring->getX(n - 1);
    const double yN = ring->getY(n - 1);
    if (!nearlyEqual(x0, xN) || !nearlyEqual(y0, yN)) {
        if (err) *err = "Ring is not closed (first != last point)";
        return false;
    }
    return true;
}

bool PolygonValidator::coordsAreFinite(const OGRLinearRing* ring, std::string* err) {
    const int n = ring->getNumPoints();
    for (int i = 0; i < n; ++i) {
        const double x = ring->getX(i);
        const double y = ring->getY(i);
        if (!std::isfinite(x) || !std::isfinite(y)) {
            if (err) *err = "Ring has non-finite coordinates";
            return false;
        }
    }
    return true;
}

bool PolygonValidator::geosIsValid(const OGRPolygon& poly, std::string* err) {
    // If GDAL was built with GEOS, IsValid() will test for self-intersection etc.
    // If GEOS is not available, IsValid() still exists but may be limited.
    if (!poly.IsValid()) {
        if (err) *err = "GEOS validity check failed (self-intersection or other topology issue)";
        return false;
    }
    return true;
}
