#!/usr/bin/env python3
"""
Python script to generate a shapefile with invalid polygon cases using pyshp.
Each polygon demonstrates a different invalid geometry scenario.
"""

import shapefile

# Create a writer for polygon shapefile
w = shapefile.Writer("invalid_polygons.shp", shapeType=shapefile.POLYGON)

# Define an attribute field to label each polygon
w.field("Name", "C")

# 1. Self-intersecting polygon (Bowtie shape)
self_intersecting = [[(0, 0), (2, 2), (0, 2), (2, 0), (0, 0)]]
w.poly(self_intersecting)
w.record("SelfIntersecting")

# 2. Polygon with duplicate points
duplicate_points = [[(0, 0), (1, 0), (1, 1), (1, 1), (0, 1), (0, 0)]]
w.poly(duplicate_points)
w.record("DuplicatePoints")

# 3. Polygon with less than 3 distinct points
less_than_three = [[(0, 0), (0, 0), (0, 0)]]
w.poly(less_than_three)
w.record("LessThanThree")

# 4. Polygon with reversed winding order
reversed_winding = [[(0, 0), (0, 2), (2, 2), (2, 0), (0, 0)]]
w.poly(reversed_winding)
w.record("ReversedWinding")

# 5. Polygon with disconnected rings
outer_ring = [(0, 0), (0, 4), (4, 4), (4, 0), (0, 0)]
inner_ring = [(5, 5), (6, 5), (6, 6), (5, 6), (5, 5)]
disconnected_rings = [outer_ring, inner_ring]
w.poly(disconnected_rings)
w.record("DisconnectedRings")

# 6. Polygon with hole touching outer ring
outer_ring_touch = [(0, 0), (0, 5), (5, 5), (5, 0), (0, 0)]
hole_touch = [(0, 0), (1, 0), (1, 1), (0, 1), (0, 0)]
hole_touch_case = [outer_ring_touch, hole_touch]
w.poly(hole_touch_case)
w.record("HoleTouchOuter")

# 7. Overlapping rings
outer_ring_overlap = [(0, 0), (0, 4), (4, 4), (4, 0), (0, 0)]
overlap_ring = [(2, 2), (6, 2), (6, 6), (2, 6), (2, 2)]
overlapping_rings = [outer_ring_overlap, overlap_ring]
w.poly(overlapping_rings)
w.record("OverlappingRings")

# 8. Open polygon (not closed)
open_polygon = [[(0, 0), (2, 0), (2, 2), (0, 2)]]  # Missing closing point
w.poly(open_polygon)
w.record("OpenPolygon")

# 9. Polygon with collinear points only
collinear_points = [[(0, 0), (1, 0), (2, 0), (3, 0), (0, 0)]]
w.poly(collinear_points)
w.record("CollinearPoints")

# 10. Valid polygon (simple rectangle) - correct geometry case
valid_polygon = [[(10, 10), (14, 10), (14, 14), (10, 14), (10, 10)]]
w.poly(valid_polygon)
w.record("ValidPolygon")

# Save the shapefile
w.close()

print("Shapefile 'invalid_polygons.shp' created with all invalid polygon cases.")
