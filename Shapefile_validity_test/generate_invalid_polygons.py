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

# 0. Self-intersecting polygon (Bowtie shape)
self_intersecting = [[(0, 0), (2, 2), (0, 2), (2, 0), (0, 0)]]
w.poly(self_intersecting)
w.record("SelfIntersecting")

# 1. Polygon with duplicate points
duplicate_points = [[(0, 0), (1, 0), (1, 1), (1, 1), (0, 1), (0, 0)]]
w.poly(duplicate_points)
w.record("DuplicatePoints")

# 2. Polygon with less than 3 distinct points
less_than_three = [[(0, 0), (0, 0), (0, 0)]]
w.poly(less_than_three)
w.record("LessThanThree")

# 3. Polygon with reversed winding order
reversed_winding = [[(0, 0), (2, 0), (2, 2), (0, 2), (0, 0)]]
w.poly(reversed_winding)
w.record("ReversedWinding")

# 4. Polygon with disconnected rings
# NOTE: To create a polygon with holes in pyshp, the outer ring MUST be clockwise
# and inner rings (holes) MUST be counterclockwise. Pyshp uses winding order to 
# distinguish outer rings from holes.
outer_ring = [(0, 0), (4, 0), (4, 4), (0, 4), (0, 0)]  # Clockwise
inner_ring = [(5, 5), (5, 6), (6, 6), (6, 5), (5, 5)]  # CCW (but disconnected from outer)
disconnected_rings = [outer_ring, inner_ring]
w.poly(disconnected_rings)
w.record("DisconnectedRings")

# 5. Polygon with hole touching outer ring
outer_ring_touch = [(0, 0), (0, 5), (5, 5), (5, 0), (0, 0)]
hole_touch = [(0, 0), (1, 0), (1, 1), (0, 1), (0, 0)]
hole_touch_case = [outer_ring_touch, hole_touch]
w.poly(hole_touch_case)
w.record("HoleTouchOuter")

# 6. Overlapping rings
outer_ring_overlap = [(0, 0), (0, 4), (4, 4), (4, 0), (0, 0)]
overlap_ring = [(2, 2), (6, 2), (6, 6), (2, 6), (2, 2)]
overlapping_rings = [outer_ring_overlap, overlap_ring]
w.poly(overlapping_rings)
w.record("OverlappingRings")

# 7. Two overlapping inner rings (holes)
outer_ring_two_holes = [(0, 0), (0, 10), (10, 10), (10, 0), (0, 0)]
hole1 = [(2, 2), (6, 2), (6, 6), (2, 6), (2, 2)]
hole2 = [(4, 4), (8, 4), (8, 8), (4, 8), (4, 4)]  # Overlaps with hole1
two_overlapping_holes = [outer_ring_two_holes, hole1, hole2]
w.poly(two_overlapping_holes)
w.record("TwoOverlappingHoles")

# 8. Polygon with collinear points only
collinear_points = [[(0, 0), (1, 0), (2, 0), (3, 0), (0, 0)]]
w.poly(collinear_points)
w.record("CollinearPoints")

# 9. Valid polygon (simple rectangle) - correct geometry case
valid_polygon1 = [[(10, 10), (10, 14), (14, 14), (14, 10), (10, 10)]]
w.poly(valid_polygon1)
w.record("ValidPolygon_case1")

# 10. Valid polygon with hole - correct geometry case
outer_ring_valid2 = [(10, 10), (10, 14), (14, 14), (14, 10), (10, 10)]
inner_ring_valid2 = [(11, 11), (13, 11), (13, 13), (11, 13), (11, 11)]
valid_polygon2 = [outer_ring_valid2, inner_ring_valid2]
w.poly(valid_polygon2)
w.record("ValidPolygon_case2")

# Save the shapefile
w.close()

print("Shapefile 'invalid_polygons.shp' created with all invalid polygon cases.")
