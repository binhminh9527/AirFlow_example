#include "DatabaseHandler.h"
#include "IntersectCalculation.h"
#include "LandProperty.h"
#include "ShapefileHandler.h"
#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <ogrsf_frmts.h>
#include "InvalidPolygonTableHandler.h"

int main() {

    // Initialize database handler with connection parameters
    DatabaseHandler LandPropertyDB_Handler("polygons_db", "5432", "polygons_db", "polygons_user", "polygons_pass");
    if (!LandPropertyDB_Handler.isConnected()) {
        std::cerr << "Failed to connect to database. Exiting." << std::endl;
        return 1;
    }
    
    // Retrieve land properties from the database
    std::vector<LandProperty> landProperties = LandPropertyDB_Handler.getLandProperties();
    if (landProperties.empty()) {
        std::cerr << "No land properties retrieved. Exiting." << std::endl;
        return 1;
    }

    // California wildfire area bounding box (example)
    ShapefileHandler wildfireAreaHandler("/opt/airflow/Dataset_Cali_Wildfire/Wildfires.shp");
    auto wildfirePolygons = wildfireAreaHandler.getPolygons();

    // Initialize invalid wildfire lookup (invalid_wildfire table)
    InvalidPolygonTableHandler invalidHandler("polygons_db", "5432", "polygons_db", "polygons_user", "polygons_pass");
    bool invalidTableAvailable = invalidHandler.isConnected();

    // Check intersections
    std::vector<bool> isaffected(landProperties.size(), false);
    for (size_t i = 0; i < landProperties.size(); i++) {
        for (size_t j = 0; j < wildfirePolygons.size(); j++) {
            // Skip wildfire polygon if marked invalid in DB (1=invalid)
            if (invalidTableAvailable) {
                bool wfInvalid = false;
                if (invalidHandler.isWildfireInvalid(static_cast<int>(j), wfInvalid) && wfInvalid) {
                    continue;
                }
            }

            OGRGeometry* intersection = landProperties[i].getPolygon().Intersection(&wildfirePolygons[j]);

            if (intersection != nullptr && !intersection->IsEmpty()) {
                std::cout << "Land Property ID " << landProperties[i].getId() 
                          << " owned by " << landProperties[i].getOwner() 
                          << " intersects with wildfire area." << std::endl;
                OGRGeometryFactory::destroyGeometry(intersection);
                isaffected[i] = true;
                break;
            } else {
                OGRGeometryFactory::destroyGeometry(intersection);
            }
        }
    }
    
    return 0;
}