# Invalid Wildfire Table

## Overview
The `invalid_wildfire` table stores polygon validity status with O(1) indexed access using PostgreSQL PRIMARY KEY.

## Table Schema
```sql
CREATE TABLE invalid_wildfire (
    polygon_id INTEGER PRIMARY KEY,  -- O(1) lookup via B-tree index
    is_invalid SMALLINT NOT NULL CHECK (is_invalid IN (0, 1))
);
```

## Workflow

### 1. Validate and Populate Table
Run the PolygonValidator on wildfire shapefile to validate all polygons and store results in database:

```bash
cd PolygonValidator
make
../dags/bin/PolygonValidator_bin /opt/airflow/Dataset_Cali_Wildfire/Wildfires.shp
```

This will:
- Validate each polygon in the shapefile
- Store validity status in `invalid_wildfire` table (1 = invalid, 0 = valid)
- Use PRIMARY KEY index on `polygon_id` for O(1) lookup

### 2. Use in IntersectCalculation
The IntersectCalculation binary automatically checks validity before processing:

```cpp
bool wildfireIsInvalid = false;
if (db.isWildfireInvalid(polygonId, wildfireIsInvalid)) {
    if (wildfireIsInvalid) {
        continue; // Skip invalid polygons
    }
}
```

This lookup is O(1) because PostgreSQL uses a B-tree index on the PRIMARY KEY.

## API Methods

### DatabaseHandler::createInvalidWildfireTable()
Creates the table if it doesn't exist.

### DatabaseHandler::setWildfireValidity(int polygonId, bool isInvalid)
Stores validity status (uses UPSERT for updates).

### DatabaseHandler::isWildfireInvalid(int polygonId, bool& isInvalid)
O(1) lookup to check if a polygon is invalid.
Returns true on success, false on error.
Sets `isInvalid` to false if polygon_id not found (assumes valid).

## Performance
- **Index Type**: B-tree on PRIMARY KEY
- **Lookup Complexity**: O(log n) worst case, effectively O(1) for cached queries
- **Storage**: ~8 bytes per row (4 bytes int + 2 bytes smallint + overhead)

## Example Output
```
Database connected successfully
Table invalid_wildfire created successfully
Loading shapefile: /opt/airflow/Dataset_Cali_Wildfire/Wildfires.shp
Found 1000 polygons.

Validating polygons...

✓ Polygon 0: VALID
✗ Polygon 1: INVALID - GEOS validity check failed (self-intersection 0.000250% > tolerance 0.000100%)
✓ Polygon 2: VALID
...

Validation Summary:
  Total polygons: 1000
  Valid:          950
  Invalid:        50
```
