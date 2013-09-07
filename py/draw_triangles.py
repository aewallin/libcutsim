import libcutsim
import myvtk
    
def main():
    gl = libcutsim.GLData()  # this class holds lines, triangles, or quads for OpenGL drawing
    iso = libcutsim.MarchingCubes() # this is the algorithm that produces GLData from the stock-model
    octree_size = 10.0 # size of 'world'
    octree_max_depth = 8
    cs = libcutsim.Cutsim(octree_size, octree_max_depth, gl, iso) # this is the cutting simulation
    print cs

    cs.init(3) # initialize by subdividing octree n-times
    print cs

    # create stock material
    vol = libcutsim.SphereVolume() # a volume with which we operate on the stock
    vol.setRadius(4)
    vol.setCenter(0,0,0)
    cs.sum_volume(vol) # sum the volume to the stock, creating new stock material
    
    # resize/position the same sphere for a cut
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
    
    myvtk.drawTriangles(myscreen, gl.get_triangles())
    
    myscreen.render()   
    myscreen.iren.Start()
    
if __name__ == "__main__":  
    main()
