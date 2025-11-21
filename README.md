# Airflow Geospatial ETL Pipeline

## Overview
This repository contains a complete geospatial data processing pipeline using Apache Airflow, PostgreSQL, and C++ with GDAL for high-performance polygon intersection calculations. The workflow loads parcel shapefile data into PostgreSQL, then uses a C++ binary to calculate intersections with wildfire areas.

## Architecture

### Services (docker-compose.yaml)
- **postgres** (port 5432): Airflow metadata database
- **webserver** (port 8080): Airflow web UI and API
- **scheduler**: Airflow task scheduler
- **polygons_db** (port 5433): PostgreSQL database storing land parcel geometries
- **results_db** (port 5434): PostgreSQL database for intersection results

### Data Flow
1. **Python ETL** (`LoadParceltoPostgre.py`): Reads shapefile with geopandas → Converts polygon coordinates to JSONB → Inserts into `polygons_db.parcels_data`
2. **C++ Intersection Calculation** (`IntersectCalculation_bin`): Reads land properties from PostgreSQL → Loads wildfire shapefile with GDAL → Calculates polygon intersections → Outputs affected properties

### DAG: Flood_Customers_DAG
```
hello_task >> Download_task >> IntersectCalculation >> VerifyDB
```
- **hello_task**: Initialization and logging
- **Download_task**: Loads parcel shapefile into PostgreSQL (Python)
- **IntersectCalculation**: Runs C++ binary to find intersecting properties (GDAL)
- **VerifyDB**: Verification step

## Project Structure
```
.
├── docker-compose.yaml          # Main services orchestration
├── docker-compose.build.yaml    # C++ build environment
├── Dockerfile.airflow           # Custom Airflow image with GDAL & build tools
├── dags/
│   ├── Customers_affect_flood.py               # Main Airflow DAG
│   ├── LoadParceltoPostgre/
│   │   └── LoadParceltoPostgre.py             # Python ETL script
│   └── bin/
│       └── IntersectCalculation_bin           # Compiled C++ binary
├── IntersectCalculation/                       # C++ source code
│   ├── main.cpp
│   ├── DatabaseHandler.{h,cpp}                # PostgreSQL connectivity
│   ├── LandProperty.{h,cpp}                   # Land parcel data model
│   ├── ShapefileHandler.{h,cpp}               # GDAL shapefile reader
│   ├── IntersectCalculation.{h,cpp}           # Geometry intersection logic
│   └── Makefile
├── Dataset_Cali_Wildfire/                     # Wildfire shapefile data
└── Parcel_Data/                               # Land parcel shapefile data
```

## Prerequisites
- Docker & Docker Compose
- Git (for repository management)

Verify installation:
```bash
docker --version
docker-compose --version
```

## Quick Start

### 1. Initial Setup
```bash
# Clone the repository
git clone <repository-url>
cd AirFlow_example

# Build custom Airflow image with GDAL
docker-compose build

# Start all services
docker-compose up -d

# Initialize Airflow database (first time only)
docker-compose run --rm webserver airflow db init

# Create admin user
docker-compose run --rm webserver airflow users create \
  --username admin \
  --firstname Admin \
  --lastname User \
  --role Admin \
  --email admin@example.com \
  --password admin
```

### 2. Build C++ Binary
```bash
# Build using docker-compose.build.yaml
docker-compose -f docker-compose.build.yaml build
docker-compose -f docker-compose.build.yaml up

# Or rebuild manually inside container
docker-compose -f docker-compose.build.yaml run --rm cpp_builder bash
# Inside container:
cd /workspace/IntersectCalculation
make clean && make
exit
```

### 3. Access Airflow UI
- Open browser: `http://localhost:8080`
- Login: admin / admin
- Navigate to DAGs and enable `Flood_Customers_DAG`

## Database Management

### PostgreSQL Connection Details

#### Airflow Metadata DB
```bash
Host: postgres (or localhost:5432 from host)
User: airflow
Password: airflow
Database: airflow
```

#### Polygons Database (Land Parcels)
```bash
Host: polygons_db (or localhost:5433 from host)
User: polygons_user
Password: polygons_pass
Database: polygons_db
```

#### Results Database
```bash
Host: results_db (or localhost:5434 from host)
User: results_user
Password: results_pass
Database: results_db
```

### Database Commands

#### Connect to Databases
```bash
# Connect to polygons_db from host
psql -h localhost -p 5433 -U polygons_user -d polygons_db
# Password: polygons_pass

# Connect to results_db from host
psql -h localhost -p 5434 -U results_user -d results_db
# Password: results_pass

# Connect from inside Airflow container
docker exec -it airflow_example-webserver-1 bash
psql -h polygons_db -U polygons_user -d polygons_db
```

#### Query Parcel Data
```sql
-- View table structure
\d parcels_data

-- Count total parcels
SELECT COUNT(*) FROM parcels_data;

-- View first 5 parcels
SELECT id, owner, polygon FROM parcels_data LIMIT 5;

-- View specific parcel geometry
SELECT id, owner, 
       jsonb_array_length(polygon) as num_points 
FROM parcels_data 
WHERE id = 1;

-- Extract coordinates for specific parcel
SELECT id, owner, 
       jsonb_array_elements(polygon) as coordinate 
FROM parcels_data 
WHERE id = 1;
```

#### Manage Tables
```sql
-- Drop and recreate table
DROP TABLE IF EXISTS parcels_data;

CREATE TABLE parcels_data (
    id SERIAL PRIMARY KEY,
    owner VARCHAR(255),
    polygon JSONB
);

-- Clear table data (keep structure)
DELETE FROM parcels_data;

-- Check table size
SELECT 
    pg_size_pretty(pg_total_relation_size('parcels_data')) as total_size,
    pg_size_pretty(pg_relation_size('parcels_data')) as table_size;
```

## Container Management

### Docker Compose Commands

#### Service Control
```bash
# Start all services
docker-compose up -d

# Stop all services
docker-compose down

# Restart specific service
docker-compose restart webserver
docker-compose restart scheduler

# View service status
docker-compose ps

# Follow logs for all services
docker-compose logs -f

# Follow logs for specific service
docker-compose logs -f webserver
docker-compose logs -f scheduler
docker-compose logs -f polygons_db
```

#### Container Interaction
```bash
# Execute bash in webserver container
docker exec -it airflow_example-webserver-1 bash

# Execute bash in scheduler container
docker exec -it airflow_example-scheduler-1 bash

# Execute bash in polygons_db container
docker exec -it airflow_example-polygons_db-1 bash

# Run one-off command in container
docker exec airflow_example-webserver-1 ls -la /opt/airflow/dags

# Test C++ binary in container
docker exec airflow_example-webserver-1 /opt/airflow/dags/bin/IntersectCalculation_bin
```

#### Build and Rebuild
```bash
# Rebuild Airflow image (after Dockerfile changes)
docker-compose build webserver scheduler

# Force rebuild without cache
docker-compose build --no-cache

# Rebuild and restart
docker-compose up -d --build

# Build C++ binary
docker-compose -f docker-compose.build.yaml up
```

#### Cleanup
```bash
# Stop and remove containers (keeps volumes)
docker-compose down

# Remove containers and volumes (deletes all data)
docker-compose down -v

# Remove dangling images
docker image prune

# Remove all unused Docker resources
docker system prune -a
```

## Airflow CLI Commands

### DAG Management
```bash
# List all DAGs
docker exec airflow_example-webserver-1 airflow dags list

# Trigger DAG manually
docker exec airflow_example-webserver-1 airflow dags trigger Flood_Customers_DAG

# Pause/Unpause DAG
docker exec airflow_example-webserver-1 airflow dags pause Flood_Customers_DAG
docker exec airflow_example-webserver-1 airflow dags unpause Flood_Customers_DAG

# Test specific task
docker exec airflow_example-webserver-1 airflow tasks test Flood_Customers_DAG Download_task 2023-01-01

# View DAG dependencies
docker exec airflow_example-webserver-1 airflow dags show Flood_Customers_DAG
```

### Task Management
```bash
# List tasks in DAG
docker exec airflow_example-webserver-1 airflow tasks list Flood_Customers_DAG

# Clear task instances (for rerun)
docker exec airflow_example-webserver-1 airflow tasks clear Flood_Customers_DAG -s 2023-01-01 -e 2023-01-02

# View task state
docker exec airflow_example-webserver-1 airflow tasks state Flood_Customers_DAG Download_task 2023-01-01
```

### Connection Management
```bash
# List connections
docker exec airflow_example-webserver-1 airflow connections list

# Add PostgreSQL connection
docker exec airflow_example-webserver-1 airflow connections add 'polygons_db_conn' \
  --conn-type 'postgres' \
  --conn-host 'polygons_db' \
  --conn-port '5432' \
  --conn-login 'polygons_user' \
  --conn-password 'polygons_pass' \
  --conn-schema 'polygons_db'

# Delete connection
docker exec airflow_example-webserver-1 airflow connections delete 'polygons_db_conn'

# Export connections to JSON
docker exec airflow_example-webserver-1 airflow connections export connections.json
```

### User Management
```bash
# List users
docker exec airflow_example-webserver-1 airflow users list

# Create new user
docker exec airflow_example-webserver-1 airflow users create \
  --username developer \
  --firstname Dev \
  --lastname User \
  --role User \
  --email dev@example.com \
  --password devpass

# Delete user
docker exec airflow_example-webserver-1 airflow users delete developer
```

## Python ETL Script

### Run Standalone (Outside Airflow)
```bash
# Enter webserver container
docker exec -it airflow_example-webserver-1 bash

# Run Python script directly
cd /opt/airflow/dags/LoadParceltoPostgre
python LoadParceltoPostgre.py
```

### Script Functions
- `create_table()`: Creates `parcels_data` table with SERIAL id, VARCHAR owner, JSONB polygon
- `check_and_print_table()`: Verifies table exists and prints first 5 rows
- `clear_table()`: Deletes all rows (prevents duplicates on re-run)
- `load_parcel_to_postgres_array()`: Main ETL function - reads shapefile, converts coordinates to JSON, inserts into database

## C++ Binary Development

### Building
```bash
# Build using docker-compose (recommended)
docker-compose -f docker-compose.build.yaml up

# Manual build inside container
docker-compose -f docker-compose.build.yaml run --rm cpp_builder bash
cd /workspace/IntersectCalculation
make clean && make
```

### Running and Testing
```bash
# Run inside Airflow container (has network access to DBs)
docker exec airflow_example-webserver-1 /opt/airflow/dags/bin/IntersectCalculation_bin

# Interactive debugging
docker exec -it airflow_example-webserver-1 bash
/opt/airflow/dags/bin/IntersectCalculation_bin
```

### Development Workflow
```bash
# Edit C++ files locally in IntersectCalculation/
vim IntersectCalculation/main.cpp

# Rebuild
docker-compose -f docker-compose.build.yaml up

# Test
docker exec airflow_example-webserver-1 /opt/airflow/dags/bin/IntersectCalculation_bin

# If compilation fails, clean build cache
cd IntersectCalculation && make clean
```

### C++ Components
- **DatabaseHandler**: PostgreSQL connection with libpq, parses JSONB polygon data
- **LandProperty**: Stores OGRPolygon objects with id and owner
- **ShapefileHandler**: Reads shapefiles with GDAL, handles both Polygon and MultiPolygon geometries
- **IntersectCalculation**: Uses GDAL's Intersection() method for precise polygon-polygon intersection

## Troubleshooting

### Airflow UI Not Accessible
```bash
# Check webserver is running
docker-compose ps

# View webserver logs
docker-compose logs -f webserver

# Restart webserver
docker-compose restart webserver

# For WSL2, get IP address
hostname -I
# Access via http://<WSL_IP>:8080
```

### DAGs Not Appearing
```bash
# Check DAG files have correct Python syntax
docker exec airflow_example-webserver-1 python /opt/airflow/dags/Customers_affect_flood.py

# View scheduler logs for errors
docker-compose logs -f scheduler

# List DAGs from CLI
docker exec airflow_example-webserver-1 airflow dags list

# Refresh DAGs (trigger scheduler)
docker-compose restart scheduler
```

### Database Connection Errors
```bash
# Test PostgreSQL connectivity from host
psql -h localhost -p 5433 -U polygons_user -d polygons_db -c "SELECT 1;"

# Test from container
docker exec airflow_example-webserver-1 psql -h polygons_db -U polygons_user -d polygons_db -c "SELECT 1;"

# Check database container is running
docker-compose ps polygons_db

# View database logs
docker-compose logs polygons_db
```

### C++ Binary Issues
```bash
# Check binary exists
docker exec airflow_example-webserver-1 ls -la /opt/airflow/dags/bin/IntersectCalculation_bin

# Test binary execution
docker exec airflow_example-webserver-1 /opt/airflow/dags/bin/IntersectCalculation_bin

# Check shared library dependencies
docker exec airflow_example-webserver-1 ldd /opt/airflow/dags/bin/IntersectCalculation_bin

# Rebuild if needed
docker-compose -f docker-compose.build.yaml up
```

### Permission Issues
```bash
# Fix dags/ directory permissions
chown -R $USER:$USER ./dags ./logs ./plugins

# Fix binary permissions
chmod +x ./dags/bin/IntersectCalculation_bin
```

### Clear and Reset
```bash
# Reset Airflow metadata database
docker-compose down -v
docker-compose up -d
docker-compose run --rm webserver airflow db init

# Clear task history for specific DAG
docker exec airflow_example-webserver-1 airflow tasks clear Flood_Customers_DAG

# Reset PostgreSQL data volumes
docker-compose down -v
docker volume rm airflow_example_polygons_data
docker volume rm airflow_example_results_data
docker-compose up -d
```

## Production Considerations

### Security
- Change default passwords in `docker-compose.yaml`
- Use Docker secrets for sensitive credentials
- Enable SSL/TLS for PostgreSQL connections
- Restrict network exposure (remove unnecessary port mappings)

### Performance
- Increase PostgreSQL shared_buffers and work_mem
- Add indexes on frequently queried columns
- Use connection pooling (PgBouncer)
- Monitor resource usage with Docker stats

### Monitoring
```bash
# Monitor container resource usage
docker stats

# Check disk usage
docker system df

# View Airflow metrics
docker exec airflow_example-webserver-1 airflow metrics

# PostgreSQL query statistics
psql -h localhost -p 5433 -U polygons_user -d polygons_db -c "SELECT * FROM pg_stat_user_tables;"
```

## Development Tips

### Fast Iteration Cycle
```bash
# Edit Python DAG → Airflow auto-reloads (30-60s delay)
vim dags/Customers_affect_flood.py

# Edit C++ → Rebuild → Test
vim IntersectCalculation/main.cpp
docker-compose -f docker-compose.build.yaml up
docker exec airflow_example-webserver-1 /opt/airflow/dags/bin/IntersectCalculation_bin

# Edit Python ETL script → Test directly
vim dags/LoadParceltoPostgre/LoadParceltoPostgre.py
docker exec -it airflow_example-webserver-1 python /opt/airflow/dags/LoadParceltoPostgre/LoadParceltoPostgre.py
```

### Debugging Python Tasks
```python
# Add breakpoints with pdb (interactive debugging in logs)
import pdb; pdb.set_trace()

# Add verbose logging
import logging
logging.info(f"Processing {len(data)} records")
```

### Debugging C++ Binary
```bash
# Add debug prints
std::cout << "Debug: variable = " << variable << std::endl;

# Check GDAL driver availability
docker exec airflow_example-webserver-1 bash -c "gdalinfo --formats | grep -i 'ESRI Shapefile'"

# Validate shapefile
docker exec airflow_example-webserver-1 ogrinfo /opt/airflow/Dataset_Cali_Wildfire/Wildfires.shp
```

## Contributing
1. Create feature branch: `git checkout -b feature/your-feature`
2. Make changes and test locally
3. Commit: `git commit -am 'Add feature'`
4. Push: `git push origin feature/your-feature`
5. Create Pull Request

## License
[Specify your license]

## Contact
[Your contact information]