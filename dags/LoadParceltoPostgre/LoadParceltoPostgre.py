import geopandas as gpd
from airflow.providers.postgres.hooks.postgres import PostgresHook

def load_parcel_to_postgres_array(shapefile_path: str, table_name: str):
    gdf = gpd.read_file(shapefile_path)

    pg_hook = PostgresHook(postgres_conn_id="polygons_db")  # Airflow connection
    conn = pg_hook.get_conn()
    cur = conn.cursor()

    for _, row in gdf.iterrows():
        owner = row.get("owner", None)
        geom = row.geometry
        if geom.is_empty:
            continue

        # Convert polygon to list of coordinate pairs
        coords = [list(c) for c in geom.exterior.coords]  # [[x,y], [x,y], ...]

        # Insert into table
        cur.execute(
            f"INSERT INTO {table_name} (owner, polygon) VALUES (%s, %s)",
            (owner, coords)
        )

    conn.commit()
    cur.close()
    conn.close()


if __name__ == "__main__":
    import sys
    import os
    
    # Example usage for testing
    if len(sys.argv) > 1:
        shapefile_path = sys.argv[1]
    else:
        # Default test path (adjust as needed)
        shapefile_path = "/opt/airflow/Dataset_Cali_Wildfire/your_shapefile.shp"
    
    table_name = "polygons"
    
    print(f"Loading shapefile: {shapefile_path}")
    print(f"Target table: {table_name}")
    
    try:
        load_parcel_to_postgres_array(shapefile_path, table_name)
        print("✓ Successfully loaded shapefile to database")
    except Exception as e:
        print(f"✗ Error: {e}")
        sys.exit(1)
