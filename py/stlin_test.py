import os
import libcutsim
import myvtk
import math
import time

# Test importing / loading of STL file
    
def main():

    gl = libcutsim.GLData()  # this class holds lines, triangles, or quads for OpenGL drawing
    iso = libcutsim.MarchingCubes() # this is the algorithm that produces GLData from the stock-model
    cs = libcutsim.Cutsim(200.0, 9, gl, iso) # this is the cutting simulation
    cs.init(3) # initialize by subdividing octree n-times

    # define path to the stl file
    dir = os.path.dirname(__file__)
    file_name = "libcutsim_import.stl"
    file_path = dir + os.sep + file_name

    print("Importing: ", file_path)

    volume = libcutsim.MeshVolume() # create the MeshVolume to hold the STL data
    fileLoaded = volume.loadStl(file_path)       # load the stl file into the mesh volume

    if not fileLoaded:
        print("Error loading stl file")
        return

    cs.sum_volume(volume)

    cutter = libcutsim.SphereVolume()
    cutter.setRadius(float(3))
    # move around the cutter and subtract at each point
    t_before = time.time()
    Nmax = 1000
    x = 50
    y = -1
    for n in range(Nmax):
        y = 0.1*n
        print("position:", x, y)
        cutter.setCenter(x,y,10.5)
        cs.diff_volume(cutter) # subtract the volume from the stock
        #cs.updateGL()
    t_after = time.time()
    print(Nmax, " diff() calls took ", t_after-t_before," seconds")
    cs.updateGL()
    #cs.updateGL()


    # create a VTK view for drawing
    w=1024
    h=1024
    myscreen = myvtk.VTKScreen(width=w, height=h) 
    myvtk.drawTriangles(myscreen, gl.get_triangles())
    myscreen.render()   
    myscreen.iren.Start()


if __name__ == "__main__":  
    main()
