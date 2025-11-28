#!/usr/bin/env python3
"""
Shapefile Validation Test Script

Tests polygon validity in shapefiles using GeoPandas and Shapely.
Provides detailed validation reports for each polygon.
"""

import sys
import geopandas as gpd
from shapely.validation import explain_validity
from shapely.geometry import Polygon, MultiPolygon
import argparse


def validate_shapefile(shapefile_path, verbose=False):
    """
    Validate all polygons in a shapefile.
    
    Args:
        shapefile_path: Path to the shapefile
        verbose: If True, print details for each polygon
    
    Returns:
        Dictionary with validation results
    """
    print(f"Loading shapefile: {shapefile_path}")
    
    try:
        gdf = gpd.read_file(shapefile_path)
    except Exception as e:
        print(f"Error loading shapefile: {e}")
        return None
    
    print(f"Found {len(gdf)} features\n")
    
    results = {
        'total': len(gdf),
        'valid': 0,
        'invalid': 0,
        'invalid_details': []
    }
    
    for idx, row in gdf.iterrows():
        geom = row.geometry
        
        # Get OBJECTID or other ID field if available
        obj_id = None
        for id_field in ['OBJECTID', 'ObjectID', 'FID', 'ID', 'id']:
            if id_field in row:
                obj_id = row[id_field]
                break
        
        id_display = f"[ObjectID: {obj_id}]" if obj_id is not None else f"[Index: {idx}]"
        
        if geom is None:
            print(f"Feature {id_display}: NULL geometry")
            results['invalid'] += 1
            results['invalid_details'].append({
                'index': idx,
                'objectid': obj_id,
                'reason': 'NULL geometry'
            })
            continue
        
        # Check validity
        is_valid = geom.is_valid
        
        if is_valid:
            results['valid'] += 1
            if verbose:
                print(f"✓ Feature {id_display}: VALID")
                print(f"  Type: {geom.geom_type}")
                print(f"  Bounds: {geom.bounds}")
                print()
        else:
            results['invalid'] += 1
            reason = explain_validity(geom)
            
            print(f"✗ Feature {id_display}: INVALID")
            print(f"  Type: {geom.geom_type}")
            print(f"  Reason: {reason}")
            print(f"  Bounds: {geom.bounds}")
            
            # Additional checks
            checks = []
            if not geom.is_simple:
                checks.append("Not simple (self-intersecting or self-tangent)")
            if geom.is_empty:
                checks.append("Empty geometry")
            if hasattr(geom, 'exterior'):
                if not geom.exterior.is_ring:
                    checks.append("Exterior ring not closed")
                if len(geom.exterior.coords) < 4:
                    checks.append(f"Too few points ({len(geom.exterior.coords)})")
            
            if checks:
                print(f"  Additional issues: {', '.join(checks)}")
            
            print()
            
            results['invalid_details'].append({
                'index': idx,
                'objectid': obj_id,
                'reason': reason,
                'additional': checks
            })
    
    return results


def print_summary(results):
    """Print validation summary."""
    if results is None:
        return
    
    print("=" * 60)
    print("VALIDATION SUMMARY")
    print("=" * 60)
    print(f"Total features:    {results['total']}")
    print(f"Valid features:    {results['valid']}")
    print(f"Invalid features:  {results['invalid']}")
    print(f"Success rate:      {results['valid']/results['total']*100:.1f}%")
    print("=" * 60)
    
    if results['invalid'] > 0:
        print("\nINVALID FEATURES:")
        for detail in results['invalid_details']:
            print(f"  Feature {detail['index']}: {detail['reason']}")


def main():
    parser = argparse.ArgumentParser(
        description='Validate polygons in a shapefile',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s /path/to/shapefile.shp
  %(prog)s /path/to/shapefile.shp --verbose
  %(prog)s /opt/airflow/Dataset_Cali_Wildfire/Wildfires.shp
        """
    )
    
    parser.add_argument('shapefile', help='Path to the shapefile')
    parser.add_argument('-v', '--verbose', action='store_true',
                       help='Print details for valid features too')
    
    args = parser.parse_args()
    
    results = validate_shapefile(args.shapefile, verbose=args.verbose)
    
    if results:
        print_summary(results)
        
        # Exit with error code if any features are invalid
        sys.exit(0 if results['invalid'] == 0 else 1)
    else:
        sys.exit(1)


if __name__ == '__main__':
    main()
