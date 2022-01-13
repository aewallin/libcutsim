import libcutsim
import myvtk
import time
import os
import math
import sys


gl = libcutsim.GLData()         # holds GL-data for drawing
iso = libcutsim.MarchingCubes() # isosurface algorithm
world_size = 100.0
max_tree_depth = 10
cs = libcutsim.Cutsim(world_size, max_tree_depth, gl, iso) # cutting simulation
# print("Initial Data", cs)

cs.init(3) # initial subdivision of octree
# print("Init Data", cs)

vol = libcutsim.MeshVolume() # a volume for adding/subtracting
facets = [[(-1.0, 0.0, 0.0), (-66.24879455566406, -39.76404571533203, -1.0), (-66.24879455566406, -39.76404571533203, 11.0), (-66.24879455566406, 44.41893768310547, -1.0)], [(-1.0, 0.0, 0.0), (-66.24879455566406, 44.41893768310547, -1.0), (-66.24879455566406, -39.76404571533203, 11.0), (-66.24879455566406, 44.41893768310547, 11.0)], [(1.0, 0.0, 0.0), (54.04975509643555, -39.76404571533203, 11.0), (54.04975509643555, -39.76404571533203, -1.0), (54.04975509643555, 44.41893768310547, -1.0)], [(1.0, -0.0, 0.0), (54.04975509643555, -39.76404571533203, 11.0), (54.04975509643555, 44.41893768310547, -1.0), (54.04975509643555, 44.41893768310547, 11.0)], [(0.0, -1.0, 0.0), (54.04975509643555, -39.76404571533203, -1.0), (54.04975509643555, -39.76404571533203, 11.0), (-66.24879455566406, -39.76404571533203, -1.0)], [(0.0, -1.0, 0.0), (-66.24879455566406, -39.76404571533203, -1.0), (54.04975509643555, -39.76404571533203, 11.0), (-66.24879455566406, -39.76404571533203, 11.0)], [(0.0, 1.0, 0.0), (54.04975509643555, 44.41893768310547, 11.0), (54.04975509643555, 44.41893768310547, -1.0), (-66.24879455566406, 44.41893768310547, -1.0)], [(0.0, 1.0, 0.0), (54.04975509643555, 44.41893768310547, 11.0), (-66.24879455566406, 44.41893768310547, -1.0), (-66.24879455566406, 44.41893768310547, 11.0)], [(0.0, 0.0, -1.0), (-66.24879455566406, 44.41893768310547, -1.0), (54.04975509643555, 44.41893768310547, -1.0), (-66.24879455566406, -39.76404571533203, -1.0)], [(0.0, 0.0, -1.0), (-66.24879455566406, -39.76404571533203, -1.0), (54.04975509643555, 44.41893768310547, -1.0), (54.04975509643555, -39.76404571533203, -1.0)], [(0.0, 0.0, 1.0), (54.04975509643555, 44.41893768310547, 11.0), (-66.24879455566406, 44.41893768310547, 11.0), (-66.24879455566406, -39.76404571533203, 11.0)], [(0.0, 0.0, 1.0), (54.04975509643555, 44.41893768310547, 11.0), (-66.24879455566406, -39.76404571533203, 11.0), (54.04975509643555, -39.76404571533203, 11.0)]]
meshLoaded = vol.loadMesh(facets)  # load mesh data

if not meshLoaded:
    print("Error loading mesh data")
    sys.exit()

cs.sum_volume(vol)  # add volume to octree

if (True):
    cutter = libcutsim.SphereVolume()
    cutter.setRadius(float(5.0))
    # move around the cutter and subtract at each point
    t_before = time.time()

    stepSize = 0.5
    x = 0
    y = 0
    ymax = 100

    while y < ymax:
        print("y value:", y)
        y = y + stepSize
        cutter.setCenter(x,y,12.0)
        cs.diff_volume(cutter) # subtract the volume from the stock
    t_after = time.time()
    
    print(ymax / stepSize, " diff() calls took ", t_after-t_before," seconds")
    cs.updateGL()

'''
# create a VTK view for drawing
w=1024
h=1024
myscreen = myvtk.VTKScreen(width=w, height=h) 

myvtk.drawTriangles(myscreen, gl.get_triangles())

myscreen.render()   
myscreen.iren.Start()
'''

# generate an stl file
dir = os.path.dirname(__file__)
file_name = "libcutsim.stl"
file_path = dir + os.sep + file_name
stl_file_path = gl.get_stl(file_path, True)  # second parameter True for binary stl, False for assci
print("stl file written:", stl_file_path)