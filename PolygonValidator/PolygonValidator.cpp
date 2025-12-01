#include "PolygonValidator.h"
#include <iostream>
#include <cmath>
#include "ogrsf_frmts.h"


static bool nearlyEqual(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) <= eps;
}

bool PolygonValidator::isValid(const OGRPolygon& poly, std::string* err) {
    if (!hasExteriorRing(poly, err)) return false;
    const OGRLinearRing* ring = poly.getExteriorRing();
    if (!ringHasMinimumPoints(ring, err)) return false;
    if (!ringIsClosed(ring, err)) return false;
    if (!coordsAreFinite(ring, err)) return false;
    if (!hasMinimumDistinctPoints(ring, err)) return false;
    if (!hasNoDuplicateConsecutivePoints(ring, err)) return false;
    if (!isNotCollinear(ring, err)) return false;
    if (!hasCorrectWindingOrder(ring, true, err)) return false;
    
    // Check interior rings (holes)
    for (int i = 0; i < poly.getNumInteriorRings(); ++i) {
        const OGRLinearRing* hole = poly.getInteriorRing(i);
        if (!ringHasMinimumPoints(hole, err)) return false;
        if (!ringIsClosed(hole, err)) return false;
        if (!coordsAreFinite(hole, err)) return false;
        if (!hasCorrectWindingOrder(hole, false, err)) return false;
        if (!isNotCollinear(hole, err)) return false;
    }
    if(!hasOverlappingHoles(poly, err)) return false;
    
    if (!holesAreContainedInOuter(poly, err)) return false;
    if (!holesDoNotTouchOuter(poly, err)) return false;
    if (!ringsDoNotOverlap(poly, err)) return false;
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

bool PolygonValidator::hasNoDuplicateConsecutivePoints(const OGRLinearRing* ring, std::string* err) {
    const int n = ring->getNumPoints();
    for (int i = 1; i < n; ++i) {
        if (pointsAreEqual(ring->getX(i-1), ring->getY(i-1), ring->getX(i), ring->getY(i))) {
            if (err) *err = "Ring has duplicate consecutive points";
            return false;
        }
    }
    return true;
}

bool PolygonValidator::hasMinimumDistinctPoints(const OGRLinearRing* ring, std::string* err) {
    const int n = ring->getNumPoints();
    if (n < 4) return true; // Already checked by ringHasMinimumPoints
    
    // Count distinct points (excluding the closing point)
    int distinctCount = 1;
    for (int i = 1; i < n - 1; ++i) {
        bool isDuplicate = false;
        for (int j = 0; j < i; ++j) {
            if (pointsAreEqual(ring->getX(i), ring->getY(i), ring->getX(j), ring->getY(j))) {
                isDuplicate = true;
                break;
            }
        }
        if (!isDuplicate) distinctCount++;
    }
    
    if (distinctCount < 3) {
        if (err) *err = "Ring has fewer than 3 distinct points";
        return false;
    }
    return true;
}

bool PolygonValidator::isNotSelfIntersecting(const OGRPolygon& poly, std::string* err) {
    // This is primarily handled by GEOS IsValid(), but we keep it as a separate check
    // for clarity. GEOS will catch self-intersections.
    return true;
}

bool PolygonValidator::hasCorrectWindingOrder(const OGRLinearRing* ring, bool isOuter, std::string* err) {
    double area = computeSignedArea(ring);
    
    // Outer rings should be clockwise (negative area in standard coordinate system)
    // Inner rings (holes) should be counterclockwise (positive area)
    if (isOuter && area > 0) {
        if (err) *err = "Outer ring has counterclockwise winding order (should be clockwise)";
        return false;
    }
    if (!isOuter && area < 0) {
        if (err) *err = "Inner ring (hole) has clockwise winding order (should be counterclockwise)";
        return false;
    }
    
    return true;
}

bool PolygonValidator::isNotCollinear(const OGRLinearRing* ring, std::string* err) {
    const int n = ring->getNumPoints();
    if (n < 4) return true;
    
    // Check if all points (excluding closing point) are collinear
    bool allCollinear = true;
    for (int i = 2; i < n - 1; ++i) {
        if (!areCollinear(ring->getX(0), ring->getY(0),
                         ring->getX(1), ring->getY(1),
                         ring->getX(i), ring->getY(i))) {
            allCollinear = false;
            break;
        }
    }
    
    if (allCollinear) {
        if (err) *err = "Ring has only collinear points (no area)";
        return false;
    }
    return true;
}

bool PolygonValidator::holesAreContainedInOuter(const OGRPolygon& poly, std::string* err) {
    
    OGRPolygon outerPoly;
    outerPoly.addRing(const_cast<OGRLinearRing*>(poly.getExteriorRing()));
    int n = poly.getNumInteriorRings();
    for (int i = 0; i < n; ++i) {
        OGRPolygon holePoly;
        holePoly.addRing(const_cast<OGRLinearRing*>(poly.getInteriorRing(i)));
        
        if(!outerPoly.Contains(&holePoly)) {
            if (err) *err = "Interior ring (hole) is not contained within outer ring";
            return false;
        }

    }
    return true;
}



bool PolygonValidator::holesDoNotTouchOuter(const OGRPolygon& poly, std::string* err) {
    const OGRLinearRing* outer = poly.getExteriorRing();
    if (!outer) return true;

    double eps = 1e-9; // tolerance for collinearity

    for (int i = 0; i < poly.getNumInteriorRings(); ++i) {
        const OGRLinearRing* hole = poly.getInteriorRing(i);

        // Compare each segment of hole with each segment of outer ring
        for (int j = 0; j < hole->getNumPoints() - 1; ++j) {
            double hx1 = hole->getX(j), hy1 = hole->getY(j);
            double hx2 = hole->getX(j + 1), hy2 = hole->getY(j + 1);

            for (int k = 0; k < outer->getNumPoints() - 1; ++k) {

                double ox = outer->getX(k), oy = outer->getY(k);

                if (areCollinear(hx1, hy1, hx2, hy2, ox, oy, eps)) {
                    if (err) *err = "Interior ring touches outer ring (collinear point)";
                    return false;
                }

            }
        }
    }
    return true;
}



bool PolygonValidator::ringsDoNotOverlap(const OGRPolygon& poly, std::string* err) {
    // Check if interior rings overlap with each other
    for (int i = 0; i < poly.getNumInteriorRings(); ++i) {
        const OGRLinearRing* ring1 = poly.getInteriorRing(i);
        OGRPolygon poly1;
        poly1.addRing(const_cast<OGRLinearRing*>(ring1));
        
        for (int j = i + 1; j < poly.getNumInteriorRings(); ++j) {
            const OGRLinearRing* ring2 = poly.getInteriorRing(j);
            OGRPolygon poly2;
            poly2.addRing(const_cast<OGRLinearRing*>(ring2));
            
            OGRGeometry* intersection = poly1.Intersection(&poly2);
            if (intersection && !intersection->IsEmpty()) {
                delete intersection;
                if (err) *err = "Interior rings overlap";
                return false;
            }
            if (intersection) delete intersection;
        }
    }
    return true;
}

bool PolygonValidator::hasOverlappingHoles(const OGRPolygon& poly, std::string* err) {
    // Check if any two interior rings (holes) overlap with each other
    int numHoles = poly.getNumInteriorRings();
    
    for (int i = 0; i < numHoles; ++i) {
        const OGRLinearRing* hole1 = poly.getInteriorRing(i);
        OGRPolygon poly1;
        poly1.addRing(const_cast<OGRLinearRing*>(hole1));
        
        for (int j = i + 1; j < numHoles; ++j) {
            const OGRLinearRing* hole2 = poly.getInteriorRing(j);
            OGRPolygon poly2;
            poly2.addRing(const_cast<OGRLinearRing*>(hole2));
            
            // Check if the two holes intersect
            OGRGeometry* intersection = poly1.Intersection(&poly2);
            if (intersection && !intersection->IsEmpty()) {
                // Check if the intersection is more than just a point or line (i.e., an area overlap)
                if (intersection->getGeometryType() == wkbPolygon || 
                    intersection->getGeometryType() == wkbMultiPolygon) {
                    delete intersection;
                    if (err) *err = "Two or more interior rings (holes) overlap";
                    return false;
                }
            }
            if (intersection) delete intersection;
        }
    }
    return true;
}

double PolygonValidator::computeSignedArea(const OGRLinearRing* ring) {
    const int n = ring->getNumPoints();
    double area = 0.0;
    for (int i = 0; i < n - 1; ++i) {
        area += ring->getX(i) * ring->getY(i + 1) - ring->getX(i + 1) * ring->getY(i);
    }
    return area * 0.5;
}

bool PolygonValidator::pointsAreEqual(double x1, double y1, double x2, double y2, double eps) {
    return std::fabs(x1 - x2) <= eps && std::fabs(y1 - y2) <= eps;
}

bool PolygonValidator::areCollinear(double x1, double y1, double x2, double y2, double x3, double y3, double eps) {
    // Cross product of vectors (p2-p1) and (p3-p1)
    double cross = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
    return std::fabs(cross) <= eps;
}
