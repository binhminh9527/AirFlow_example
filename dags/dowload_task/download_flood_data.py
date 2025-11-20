#!/usr/bin/env python3
"""
Connect to polygons_db, try to read one row first. If none exists and the connected user
has write privileges, create the table and insert a test row. Then download flood data
to the provided output path.

Usage:
  - Set env vars PGHOST, PGPORT, PGUSER, PGPASSWORD, PGDATABASE as needed (defaults assume compose service names).
  - Call download_flood_data(url, output_path) or run this file as a script.
"""
import os
import sys
import uuid
from datetime import datetime

import requests

try:
    import psycopg2
    from psycopg2.extras import RealDictCursor
    from psycopg2 import sql, errors
except Exception:
    print("psycopg2 not installed. Install with: pip install psycopg2-binary", file=sys.stderr)
    raise

POLYGON_HOST = os.getenv("PGHOST", "polygons_db")
POLYGON_PORT = int(os.getenv("PGPORT", "5432"))
PGUSER = os.getenv("PGUSER", "polygons_user")
PGPASSWORD = os.getenv("PGPASSWORD", "polygons_pass")
PGDATABASE = os.getenv("PGDATABASE", "polygons_db")

TABLE_NAME = "test_table_for_scripts"

DSN = f"host={POLYGON_HOST} port={POLYGON_PORT} dbname={PGDATABASE} user={PGUSER} password={PGPASSWORD}"

def connect():
    try:
        return psycopg2.connect(DSN)
    except Exception as e:
        print("Failed to connect to polygons_db:", e, file=sys.stderr)
        raise

def fetch_one_row(conn):
    with conn.cursor(cursor_factory=RealDictCursor) as cur:
        try:
            cur.execute(sql.SQL("SELECT * FROM {} LIMIT 1").format(sql.Identifier(TABLE_NAME)))
            return cur.fetchone()
        except errors.UndefinedTable:
            conn.rollback()
            return None
        except Exception:
            conn.rollback()
            raise

def ensure_table(conn):
    with conn.cursor() as cur:
        cur.execute(sql.SQL("""
            CREATE TABLE IF NOT EXISTS {} (
                id UUID PRIMARY KEY,
                name TEXT NOT NULL,
                created_at TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now()
            )
        """).format(sql.Identifier(TABLE_NAME)))
        conn.commit()

def insert_test_row(conn, name="inserted-from-container"):
    new_id = str(uuid.uuid4())
    now = datetime.now()
    with conn.cursor() as cur:
        cur.execute(
            sql.SQL("INSERT INTO {} (id, name, created_at) VALUES (%s, %s, %s)")
            .format(sql.Identifier(TABLE_NAME)),
            (new_id, name, now)
        )
        conn.commit()
    return new_id

def fetch_by_id(conn, row_id):
    with conn.cursor(cursor_factory=RealDictCursor) as cur:
        cur.execute(sql.SQL("SELECT * FROM {} WHERE id = %s").format(sql.Identifier(TABLE_NAME)), (row_id,))
        return cur.fetchone()

def download_file(url: str, output_path: str, timeout: int = 30):
    print(f"Downloading {url} -> {output_path}")
    resp = requests.get(url, stream=True, timeout=timeout)
    resp.raise_for_status()
    os.makedirs(os.path.dirname(output_path) or ".", exist_ok=True)
    with open(output_path, "wb") as f:
        for chunk in resp.iter_content(chunk_size=8192):
            if chunk:
                f.write(chunk)
    print("Download completed.")

def download_flood_data(url: str, output_path: str):
    """
    Connects to polygons_db, tries read-first; if no data and writable, inserts a test row.
    Then downloads the flood data to output_path.
    Raises exceptions on critical failures.
    """
    conn = None
    try:
        conn = connect()
        try:
            row = fetch_one_row(conn)
        except errors.InsufficientPrivilege as e:
            print("Read failed due to insufficient privileges:", e, file=sys.stderr)
            row = None
        except Exception as e:
            print("Unexpected error during read:", e, file=sys.stderr)
            raise

        if row:
            inserted_id = insert_test_row(conn)
            row = fetch_by_id(conn, inserted_id)
            print("Read-first: found an existing row:")
            print(row)
        else:
            # Table missing or empty - try to create/insert if allowed
            try:
                ensure_table(conn)
                inserted_id = insert_test_row(conn)
                row = fetch_by_id(conn, inserted_id)
                print("No existing data found. Created table/inserted test row:")
                print(row)
            except errors.InsufficientPrivilege:
                print("Connected user does not have write privileges. Cannot create/insert.", file=sys.stderr)
            except Exception as e:
                print("Error while creating/inserting/fetching:", e, file=sys.stderr)
                raise

        # Regardless of DB outcome, attempt to download the flood data file
        try:
            #download_file(url, output_path)
            print("Failed to download flood data: Downloading is currently disabled.", file=sys.stderr)
        except Exception as e:
            print("Failed to download flood data:", e, file=sys.stderr)
            raise

    finally:
        if conn:
            conn.close()

if __name__ == "__main__":
    # Example defaults - change as needed or pass via env in container
    example_url = os.getenv("FLOOD_DATA_URL", "https://example.com/flood_data.csv")
    example_output = os.getenv("FLOOD_OUTPUT_PATH", "data/flood_data.csv")
    try:
        download_flood_data(example_url, example_output)
    except Exception as e:
        print("Error:", e, file=sys.stderr)
        sys.exit(1)