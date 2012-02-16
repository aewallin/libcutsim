import libcutsim


gl = libcutsim.GLData()
iso = libcutsim.MarchingCubes()

cs = libcutsim.Cutsim(10.0, 5, gl, iso)
print cs

cs.init(3)
print cs

vol = libcutsim.SphereVolume()
vol.setRadius(4)
vol.setCenter(0,0,0)

cs.sum_volume(vol)
cs.updateGL()

print cs
print gl
