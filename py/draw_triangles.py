import libcutsim
import myvtk

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
    myscreen.addActor(triactor)
    
def main():
    
    gl = libcutsim.GLData()  # this class holds lines, triangles, or quads for OpenGL drawing
    iso = libcutsim.MarchingCubes() # this is the algorithm that produces GLData from the stock-model
    cs = libcutsim.Cutsim(10.0, 8, gl, iso) # this is the cutting simulation
    print cs

    cs.init(3) # initialize by subdividing octree n-times
    print cs

    vol = libcutsim.SphereVolume() # a volume with which we operate on the stock
    vol.setRadius(4)
    vol.setCenter(0,0,0)

    cs.sum_volume(vol) # sum the volume to the stock, creating new material
    
    # resize and position the sphere for a cut
    vol.setRadius(1)
    vol.setCenter(0,4,0)
    cs.diff_volume(vol) # subtract the volume from the stock
    
    cs.updateGL() # this updates the GLData so we can draw the stock
    
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
