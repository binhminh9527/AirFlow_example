import geopandas as gpd

# Load the polygons shapefile (replace with your file path)
polygons = gpd.read_file("/opt/airflow/Parcel_Data/Parcel_data.shp")
#list all point in polygons to verify
for idx, row in polygons.iterrows():        
    print(f"Polygon {idx}: {row['geometry']}")