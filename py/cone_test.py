import libcutsim
import myvtk
import math
import time
    
def main():
    gl = libcutsim.GLData()  # this class holds lines, triangles, or quads for OpenGL drawing
    iso = libcutsim.MarchingCubes() # this is the algorithm that produces GLData from the stock-model
    cs = libcutsim.Cutsim(20.0, 10, gl, iso) # this is the cutting simulation
    print(cs)

    cs.init(3) # initialize by subdividing octree n-times
    print(cs)

    vol = libcutsim.CubeVolume() # a volume for creating stock
    vol.setSide(10)
    vol.setCenter(0,0,-5)
    cs.sum_volume(vol) # sum the volume to the stock, creating new material
        
    cutter = libcutsim.ConeVolume()
    #cutter.setRadius(float(0.7))
    # move around the cutter and subtract at each point
    t_before = time.time()
    Nmax = 100
    for n in range(Nmax):
        x = 3*math.cos(0.1*n)
        y = -3 + 0.08*n
        #print x,y
        cutter.setCenter(x,y,-0.4)
        cs.diff_volume(cutter) # subtract the volume from the stock
        #cs.updateGL()
    t_after = time.time()
    print(Nmax, " diff() calls took ", t_after-t_before," seconds")

    cs.updateGL() # this updates the GLData so we can draw the stock
    
    print(cs)
    print(gl)
    
    # create a VTK view for drawing
    w=1024
    h=1024
    myscreen = myvtk.VTKScreen(width=w, height=h) 
    myvtk.drawTriangles(myscreen, gl.get_triangles())
    myscreen.render()   
    myscreen.iren.Start()
    
if __name__ == "__main__":  
    main()
