#use/env python3
import geopandas as gpd
from airflow.providers.postgres.hooks.postgres import PostgresHook
import psycopg2
import json
def check_and_print_table(table_name="parcels_data"):
    """Check if table exists and print its contents for verification"""
    conn = psycopg2.connect(
        host="polygons_db",
        port=5432,
        database="polygons_db",
        user="polygons_user",
        password="polygons_pass"
    )
    cur = conn.cursor()
    
    # Check if table exists
    cur.execute("""
        SELECT EXISTS (
            SELECT FROM information_schema.tables 
            WHERE table_schema = 'public' 
            AND table_name = %s
        )
    """, (table_name,))
    
    table_exists = cur.fetchone()[0]
    
    if table_exists:
        # Get row count
        cur.execute(f"SELECT COUNT(*) FROM {table_name}")
        count = cur.fetchone()[0]
        print(f"✓ Table '{table_name}' exists with {count} rows")
        
        # Print first 5 rows
        cur.execute(f"SELECT id, owner, polygon FROM {table_name} LIMIT 5")
        rows = cur.fetchall()
        print("\nFirst 5 rows:")
        for row in rows:
            print(f"  ID: {row[0]}, Owner: {row[1]}, Polygon: {row[2][:50]}...")
    else:
        print(f"✗ Table '{table_name}' does not exist")
    
    cur.close()
    conn.close()
    return table_exists

def create_table(table_name="parcels_data"):
    """Create the parcels_data table if it doesn't exist"""
    conn = psycopg2.connect(
        host="polygons_db",
        port=5432,
        database="polygons_db",
        user="polygons_user",
        password="polygons_pass"
    )
    cur = conn.cursor()
    
    # Create table
    cur.execute(f"""
        CREATE TABLE IF NOT EXISTS {table_name} (
            id SERIAL PRIMARY KEY,
            owner VARCHAR(255),
            polygon JSONB
        )
    """)
    conn.commit()
    print(f"✓ Table '{table_name}' created successfully")
    
    cur.close()
    conn.close()

def clear_table(table_name="parcels_data"):
    """Clear all data from the table to avoid duplicates"""
    conn = psycopg2.connect(
        host="polygons_db",
        port=5432,
        database="polygons_db",
        user="polygons_user",
        password="polygons_pass"
    )
    cur = conn.cursor()
    
    # Delete all rows from the table
    cur.execute(f"DELETE FROM {table_name}")
    conn.commit()
    
    deleted_count = cur.rowcount
    print(f"✓ Cleared {deleted_count} rows from table '{table_name}'")
    
    cur.close()
    conn.close()

def load_parcel_to_postgres_array():
    shapefile_path = "/opt/airflow/Parcel_Data/Parcel_data.shp"
    table_name = "parcels_data"

    if not check_and_print_table(table_name):
        create_table(table_name)

    gdf = gpd.read_file(shapefile_path)
    clear_table(table_name)
    # Connect directly to polygons_db for data insertion
    conn = psycopg2.connect(
        host="polygons_db",
        port=5432,
        database="polygons_db",
        user="polygons_user",
        password="polygons_pass"
    )
    cur = conn.cursor()

    for _, row in gdf.iterrows():
        owner = row.get("Owner", None)
        geom = row.geometry
        if geom.is_empty:
            print("Empty geometry found, skipping...")
            continue
        # Convert polygon to list of coordinate pairs
        coords = [list(c) for c in geom.exterior.coords]  # [[x,y], [x,y], ...]

        # Insert into table - convert coords to JSON string for JSONB column
        
        cur.execute(
            f"INSERT INTO {table_name} (owner, polygon) VALUES (%s, %s::jsonb)",
            (owner, json.dumps(coords))
        )

    conn.commit()
    cur.close()
    conn.close()


if __name__ == "__main__":
    import sys
    import os
    

    check_and_print_table("parcels_data")
    load_parcel_to_postgres_array()

