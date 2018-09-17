# BRDF Vis
*BRDFViz* is a visualisation tool for path tracing. Allowing the user to see the path traced scene built in real-time in point-cloud. Each point in the cloud is selectable, showing the full path contrubuting to the final image.

![](https://i.imgur.com/ACnZ8aS.png)

**Coming Soon** - The main purpose of this project is to visualise the Bidirectional Reflectance Distrubution Function (BRDF) of the specific material the path has bounced off. This is not yet implemented, and will hopefully be implemented soon.

**Building**
This is a qVTK project with the RGK path tracer (https://github.com/rafalcieslak/RGK) crow-barred in to drive the path tracing and should be built using CMake.

**Required Libraries**
- **VTK 7**
- **QT5**
- **GLM** - (*0.9.9~a2* is not working, currently investigating why) should be working with *0.9.8.3-2*
- **PNG++**
- **libjpeg**
- **OpenEXR**
- **assimp**

**Bugs**
- Too many to list. 
-- It is slow
-- it will happy eat as much memory as the scene allows (keep the samples low for now)
-- VTK camera controls are horrible, going to write a WASD style class for the library
-- Random crashes
