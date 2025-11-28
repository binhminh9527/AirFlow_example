#!/usr/bin/env python3
"""
Python script to visualize invalid polygon cases from a shapefile.
Uses matplotlib to plot each polygon in a grid layout.
"""

import shapefile
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Patch

# Load the shapefile
sf = shapefile.Reader("invalid_polygons.shp")

# Get shapes and records
shapes = sf.shapes()
records = sf.records()

# Number of polygons
num_polygons = len(shapes)

# Determine grid size for plotting
cols = 3
rows = (num_polygons + cols - 1) // cols

# Create figure
fig, axes = plt.subplots(rows, cols, figsize=(15, 10))
axes = axes.flatten()

def ring_orientation(ring):
    # Shoelace formula: positive area -> counterclockwise, negative -> clockwise
    if len(ring) < 3:
        return 0.0
    x = np.array([p[0] for p in ring])
    y = np.array([p[1] for p in ring])
    return 0.5 * np.sum(x[:-1] * y[1:] - x[1:] * y[:-1])

def detect_orientation(ring):
    # Ensure closed ring for orientation calculation
    if not ring or len(ring) < 3:
        return 'Flat'
    closed = ring if ring[0] == ring[-1] else (ring + [ring[0]])
    area = ring_orientation(closed)
    if area > 0:
        return 'CCW'
    if area < 0:
        return 'CW'
    return 'Flat'

# Create legend for orientation colors
legend_elements = [
    Patch(facecolor='tab:green', label='Clockwise (CW)'),
    Patch(facecolor='tab:red', label='Counterclockwise (CCW)'),
    Patch(facecolor='tab:gray', label='Flat/Degenerate')
]
fig.legend(handles=legend_elements, loc='upper center', ncol=3, frameon=True, fontsize=10)

# Plot each polygon
for i, shape in enumerate(shapes):
    ax = axes[i]
    parts = list(shape.parts) + [len(shape.points)]
    name = records[i][0] if records and len(records[i]) > 0 else f"Feature {i}"
    for j in range(len(shape.parts)):
        start = shape.parts[j]
        end = parts[j+1]
        ring = shape.points[start:end]
        if name == "OpenPolygon" and len(ring) >= 2 and ring[0] == ring[-1]:
            ring = ring[:-1]
        xs, ys = zip(*ring)
        orient_label = detect_orientation(ring)
        color = 'tab:green' if orient_label == 'CW' else ('tab:red' if orient_label == 'CCW' else 'tab:gray')
        ax.plot(xs, ys, marker='o', color=color)
    ax.set_title(name)
    ax.set_aspect('equal')
    ax.grid(True)

# Hide unused subplots
for k in range(num_polygons, len(axes)):
    axes[k].axis('off')

plt.tight_layout(rect=[0, 0, 1, 0.97])  # Leave space for legend at top
plt.savefig("invalid_polygons_visualization.png")
plt.show()
