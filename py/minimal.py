import libcutsim # https://github.com/aewallin/libcutsim


gl = libcutsim.GLData()         # holds GL-data for drawing
iso = libcutsim.MarchingCubes() # isosurface algorithm
world_size = 10.0
max_tree_depth = 5
cs = libcutsim.Cutsim(world_size, max_tree_depth, gl, iso) # cutting simulation
print(cs)

cs.init(3) # initial subdivision of octree
print(cs)

vol = libcutsim.SphereVolume() # a volume for adding/subtracting 
vol.setRadius(4)
vol.setCenter(0,0,0)

cs.sum_volume(vol) # add volume to octree
cs.updateGL()

print(cs)
print(gl)
