# C++ Components Architecture

## Project Structure

```
.
├── Common/                          # Shared libraries
│   ├── DatabaseHandler.{h,cpp}      # PostgreSQL connectivity & data loading
│   ├── LandProperty.{h,cpp}         # Land parcel data model (OGRPolygon)
│   └── ShapefileHandler.{h,cpp}     # GDAL shapefile reader
│
├── IntersectCalculation/            # Intersection calculation binary
│   ├── main.cpp                     # Loads DB + shapefile, validates, computes intersections
│   ├── IntersectCalculation.{h,cpp} # Intersection algorithms
│   └── Makefile                     # Builds: ../dags/bin/IntersectCalculation_bin
│
└── PolygonValidator/                # Polygon validation binary
    ├── main.cpp                     # CLI tool to validate shapefile polygons
    ├── PolygonValidator.{h,cpp}     # Validation logic (ring closure, finite coords, GEOS)
    └── Makefile                     # Builds: ../dags/bin/PolygonValidator_bin

```

## Dependencies

### Common Components
- **DatabaseHandler**: Reads land properties from PostgreSQL `parcels_data` table, parses JSONB polygon coordinates
- **LandProperty**: Stores `OGRPolygon` objects with id and owner attributes
- **ShapefileHandler**: Uses GDAL/OGR to read Polygon and MultiPolygon geometries from shapefiles

### IntersectCalculation Binary
**Purpose**: Load land parcels from database, load wildfire shapefile, validate all polygons, calculate intersections

**Dependencies**:
- Common: DatabaseHandler, LandProperty, ShapefileHandler
- PolygonValidator: Validation logic
- Libraries: libpq (PostgreSQL), libgdal (GDAL/OGR)

**Output**: Prints validated parcels and wildfire polygons, then lists intersecting properties

### PolygonValidator Binary
**Purpose**: Standalone CLI tool to validate any shapefile's polygon geometry

**Dependencies**:
- Common: ShapefileHandler
- Local: PolygonValidator validation logic
- Libraries: libgdal (GDAL/OGR)

**Usage**:
```bash
./dags/bin/PolygonValidator_bin /path/to/shapefile.shp
```

**Validation Checks**:
1. Has exterior ring
2. Ring has ≥4 points (including closing point)
3. Ring is closed (first point == last point)
4. All coordinates are finite (no NaN/Inf)
5. GEOS validity check (no self-intersection, proper topology)

## Building

### Build Both Binaries
```bash
docker compose -f docker-compose.build.yaml up
```

### Build Individually
```bash
# IntersectCalculation
cd IntersectCalculation
make clean && make

# PolygonValidator
cd PolygonValidator
make clean && make
```

## Testing

### IntersectCalculation
```bash
docker exec airflow_example-webserver-1 /opt/airflow/dags/bin/IntersectCalculation_bin
```

### PolygonValidator
```bash
# Validate wildfire shapefile
docker exec airflow_example-webserver-1 /opt/airflow/dags/bin/PolygonValidator_bin /opt/airflow/Dataset_Cali_Wildfire/Wildfires.shp

# Validate parcel shapefile
docker exec airflow_example-webserver-1 /opt/airflow/dags/bin/PolygonValidator_bin /opt/airflow/Parcel_Data/Parcel_data.shp
```

## Integration with Airflow

Both binaries are compiled and placed in `dags/bin/` which is mounted into the Airflow containers. They can be called from Airflow DAGs using `BashOperator`:

```python
validate_task = BashOperator(
    task_id="validate_polygons",
    bash_command="/opt/airflow/dags/bin/PolygonValidator_bin /opt/airflow/Dataset_Cali_Wildfire/Wildfires.shp"
)

intersect_task = BashOperator(
    task_id="calculate_intersections",
    bash_command="/opt/airflow/dags/bin/IntersectCalculation_bin"
)
```

## Design Principles

1. **Separation of Concerns**: Common libraries are shared, business logic is isolated
2. **Reusability**: ShapefileHandler and PolygonValidator can be used by multiple binaries
3. **Testability**: PolygonValidator can be tested independently on any shapefile
4. **Modularity**: Each binary has a focused purpose with clear inputs/outputs
