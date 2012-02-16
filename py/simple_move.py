import libcutsim
import myvtk
import math

# draw triangles from GLData
def drawTriangles(myscreen, gl):
    trianglelist = gl.get_triangles()
    # list of triangles
    # [ [p1,p2,p3] ,
    #   [p4,p5,p6] ,
    #   ...
    # ]
    print "drawing ",len(trianglelist)," triangles"
    
    # this draws all triangles with the same color
    triactor = myvtk.STLSurf(triangleList=trianglelist, color=myvtk.cyan)
    #triactor.SetWireframe()
    myscreen.addActor(triactor)
    
def main():
    
    gl = libcutsim.GLData()  # this class holds lines, triangles, or quads for OpenGL drawing
    iso = libcutsim.MarchingCubes() # this is the algorithm that produces GLData from the stock-model
    cs = libcutsim.Cutsim(20.0, 9, gl, iso) # this is the cutting simulation
    print cs

    cs.init(3) # initialize by subdividing octree n-times
    print cs

    vol = libcutsim.CubeVolume() # a volume with which we operate on the stock
    vol.setSide(10)
    vol.setCenter(0,0,-5)

    cs.sum_volume(vol) # sum the volume to the stock, creating new material
    
    # the volume with which we cut
    
    cutter = libcutsim.SphereVolume()
    cutter.setRadius(float(0.7))
    # move around the cutter and subtract at each point
    for n in range(100):
        x = 3*math.cos(0.1*n)
        y = -3 + 0.08*n
        #print x,y
        cutter.setCenter(x,y,0.1)
        cs.diff_volume(cutter) # subtract the volume from the stock
        
    cs.updateGL()
    # this updates the GLData so we can draw the stock
    
    print cs
    print gl
    
    # create a VTK view for drawing
    w=1024
    h=1024
    myscreen = myvtk.VTKScreen(width=w, height=h) 
    
    drawTriangles(myscreen, gl)
    
    myscreen.render()   
    myscreen.iren.Start()
    
if __name__ == "__main__":  
    main()
