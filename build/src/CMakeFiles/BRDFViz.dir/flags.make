# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# compile CXX with /usr/bin/c++
CXX_FLAGS =     -std=c++11 -Wall -Werror -O3 -fPIC -std=c++11

CXX_DEFINES = -DQT_CORE_LIB -DQT_GUI_LIB -DQT_NO_DEBUG -DQT_SQL_LIB -DQT_WIDGETS_LIB -DvtkDomainsChemistry_AUTOINIT="1(vtkDomainsChemistryOpenGL2)" -DvtkFiltersCore_AUTOINIT="1(vtkFiltersParallelDIY2)" -DvtkIOExport_AUTOINIT="1(vtkIOExportOpenGL2)" -DvtkIOGeometry_AUTOINIT="1(vtkIOMPIParallel)" -DvtkIOImage_AUTOINIT="1(vtkIOMPIImage)" -DvtkIOParallel_AUTOINIT="1(vtkIOMPIParallel)" -DvtkRenderingContext2D_AUTOINIT="1(vtkRenderingContextOpenGL2)" -DvtkRenderingCore_AUTOINIT="3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL2)" -DvtkRenderingOpenGL2_AUTOINIT="1(vtkRenderingGL2PSOpenGL2)" -DvtkRenderingVolume_AUTOINIT="1(vtkRenderingVolumeOpenGL2)"

CXX_INCLUDES = -I/home/u1776267/Documents/Code/BRDFViz/build/src -I/home/u1776267/Documents/Code/BRDFViz/src -I/opt/VTK-8.1.1/Utilities/KWIML -isystem /opt/VTK-8.1.1/Utilities/KWSys -I/opt/VTK-8.1.1/Common/Core -I/opt/VTK-8.1.1/Utilities/Python -I/usr/include/python2.7 -I/opt/VTK-8.1.1/ThirdParty/SixPython -I/opt/VTK-8.1.1/ThirdParty/ZopeInterface -I/opt/VTK-8.1.1/ThirdParty/constantly -I/opt/VTK-8.1.1/ThirdParty/hyperlink -I/opt/VTK-8.1.1/ThirdParty/incremental -I/opt/VTK-8.1.1/ThirdParty/Twisted -I/opt/VTK-8.1.1/ThirdParty/txaio -I/opt/VTK-8.1.1/ThirdParty/AutobahnPython -I/opt/VTK-8.1.1/Common/Math -I/opt/VTK-8.1.1/Common/Misc -I/opt/VTK-8.1.1/Common/System -I/opt/VTK-8.1.1/Common/Transforms -I/opt/VTK-8.1.1/Common/DataModel -I/opt/VTK-8.1.1/Common/Color -I/opt/VTK-8.1.1/Common/ExecutionModel -I/opt/VTK-8.1.1/Common/ComputationalGeometry -I/opt/VTK-8.1.1/Filters/Core -I/opt/VTK-8.1.1/Filters/General -I/opt/VTK-8.1.1/Imaging/Core -I/opt/VTK-8.1.1/Imaging/Fourier -I/opt/VTK-8.1.1/ThirdParty/alglib -I/opt/VTK-8.1.1/Filters/Statistics -I/opt/VTK-8.1.1/Filters/Extraction -I/opt/VTK-8.1.1/Infovis/Core -I/opt/VTK-8.1.1/Filters/Geometry -I/opt/VTK-8.1.1/Filters/Sources -I/opt/VTK-8.1.1/Rendering/Core -I/opt/VTK-8.1.1/ThirdParty/zlib -I/opt/VTK-8.1.1/ThirdParty/freetype -I/opt/VTK-8.1.1/Rendering/FreeType -I/opt/VTK-8.1.1/Rendering/Context2D -I/opt/VTK-8.1.1/Charts/Core -I/opt/VTK-8.1.1/Utilities/DICOMParser -I/opt/VTK-8.1.1/ThirdParty/lz4/vtklz4/lib -I/opt/VTK-8.1.1/ThirdParty/lz4/vtklz4 -I/opt/VTK-8.1.1/ThirdParty/lz4 -I/opt/VTK-8.1.1/IO/Core -I/opt/VTK-8.1.1/IO/Legacy -I/opt/VTK-8.1.1/ThirdParty/expat -I/opt/VTK-8.1.1/IO/XMLParser -I/opt/VTK-8.1.1/Domains/Chemistry -I/opt/VTK-8.1.1/Utilities/EncodeString -I/opt/VTK-8.1.1/ThirdParty/glew -I/opt/VTK-8.1.1/Rendering/OpenGL2 -I/opt/VTK-8.1.1/Domains/ChemistryOpenGL2 -I/opt/VTK-8.1.1/IO/XML -I/opt/VTK-8.1.1/Utilities/HashSource -I/opt/VTK-8.1.1/Parallel/Core -I/opt/VTK-8.1.1/Filters/AMR -I/opt/VTK-8.1.1/Filters/FlowPaths -I/opt/VTK-8.1.1/Filters/Generic -I/opt/VTK-8.1.1/Imaging/Sources -I/opt/VTK-8.1.1/Filters/Hybrid -I/opt/VTK-8.1.1/Filters/HyperTree -I/opt/VTK-8.1.1/Imaging/General -I/opt/VTK-8.1.1/Filters/Imaging -I/opt/VTK-8.1.1/Filters/Modeling -I/opt/VTK-8.1.1/Filters/Parallel -I/opt/VTK-8.1.1/Parallel/MPI -I/usr/lib/openmpi/include/openmpi/opal/mca/event/libevent2021/libevent -I/usr/lib/openmpi/include/openmpi/opal/mca/event/libevent2021/libevent/include -I/usr/lib/openmpi/include -I/usr/lib/openmpi/include/openmpi -I/opt/VTK-8.1.1/ThirdParty/diy2/vtkdiy2/include -I/opt/VTK-8.1.1/ThirdParty/diy2 -I/opt/VTK-8.1.1/Filters/ParallelDIY2 -I/opt/VTK-8.1.1/Filters/ParallelGeometry -I/opt/VTK-8.1.1/Filters/ParallelImaging -I/opt/VTK-8.1.1/Filters/ParallelMPI -I/opt/VTK-8.1.1/ThirdParty/verdict -I/opt/VTK-8.1.1/Filters/Verdict -I/opt/VTK-8.1.1/Filters/ParallelVerdict -I/opt/VTK-8.1.1/Filters/Points -I/opt/VTK-8.1.1/Filters/Programmable -I/opt/VTK-8.1.1/Wrapping/Tools -I/opt/VTK-8.1.1/Wrapping/PythonCore -I/opt/VTK-8.1.1/Filters/Python -I/opt/VTK-8.1.1/Filters/SMP -I/opt/VTK-8.1.1/Filters/Selection -I/opt/VTK-8.1.1/Filters/Texture -I/opt/VTK-8.1.1/Filters/Topology -I/opt/VTK-8.1.1/Interaction/Style -I/opt/VTK-8.1.1/GUISupport/Qt -I/opt/VTK-8.1.1/ThirdParty/sqlite -I/opt/VTK-8.1.1/IO/SQL -I/opt/VTK-8.1.1/GUISupport/QtSQL -I/opt/VTK-8.1.1/Utilities/MetaIO/vtkmetaio -I/opt/VTK-8.1.1/Utilities/MetaIO -I/opt/VTK-8.1.1/ThirdParty/jpeg -I/opt/VTK-8.1.1/ThirdParty/png -I/opt/VTK-8.1.1/ThirdParty/tiff/vtktiff/libtiff -I/opt/VTK-8.1.1/ThirdParty/tiff -I/opt/VTK-8.1.1/IO/Image -I/opt/VTK-8.1.1/Imaging/Hybrid -I/opt/VTK-8.1.1/Infovis/Layout -I/opt/VTK-8.1.1/Imaging/Color -I/opt/VTK-8.1.1/Rendering/Annotation -I/opt/VTK-8.1.1/Rendering/Volume -I/opt/VTK-8.1.1/Interaction/Widgets -I/opt/VTK-8.1.1/Views/Core -I/opt/VTK-8.1.1/ThirdParty/libproj4/vtklibproj4 -I/opt/VTK-8.1.1/ThirdParty/libproj4 -I/opt/VTK-8.1.1/Geovis/Core -I/opt/VTK-8.1.1/ThirdParty/hdf5/vtkhdf5 -isystem /opt/VTK-8.1.1/ThirdParty/hdf5/vtkhdf5/hl/src -isystem /opt/VTK-8.1.1/ThirdParty/hdf5/vtkhdf5/src -I/opt/VTK-8.1.1/ThirdParty/hdf5 -I/opt/VTK-8.1.1/IO/AMR -I/opt/VTK-8.1.1/IO/EnSight -I/opt/VTK-8.1.1/ThirdParty/netcdf/vtknetcdf/include -I/opt/VTK-8.1.1/ThirdParty/netcdf/vtknetcdf -I/opt/VTK-8.1.1/ThirdParty/netcdf -I/opt/VTK-8.1.1/ThirdParty/exodusII -I/opt/VTK-8.1.1/IO/Exodus -I/opt/VTK-8.1.1/ThirdParty/gl2ps -I/opt/VTK-8.1.1/Rendering/GL2PSOpenGL2 -I/opt/VTK-8.1.1/ThirdParty/libharu/vtklibharu/include -I/opt/VTK-8.1.1/ThirdParty/libharu -I/opt/VTK-8.1.1/IO/Export -I/opt/VTK-8.1.1/IO/ExportOpenGL2 -I/opt/VTK-8.1.1/IO/Geometry -I/opt/VTK-8.1.1/IO/Import -I/opt/VTK-8.1.1/ThirdParty/libxml2/vtklibxml2 -I/opt/VTK-8.1.1/ThirdParty/libxml2 -I/opt/VTK-8.1.1/IO/Infovis -I/opt/VTK-8.1.1/IO/LSDyna -I/opt/VTK-8.1.1/IO/MINC -I/opt/VTK-8.1.1/IO/MPIImage -I/opt/VTK-8.1.1/ThirdParty/netcdfcpp -I/opt/VTK-8.1.1/IO/NetCDF -I/opt/VTK-8.1.1/ThirdParty/jsoncpp -I/opt/VTK-8.1.1/IO/Parallel -I/opt/VTK-8.1.1/IO/MPIParallel -I/opt/VTK-8.1.1/ThirdParty/oggtheora -I/opt/VTK-8.1.1/IO/Movie -I/opt/VTK-8.1.1/IO/PLY -I/opt/VTK-8.1.1/IO/ParallelNetCDF -I/opt/VTK-8.1.1/IO/ParallelXML -I/opt/VTK-8.1.1/IO/TecplotTable -I/opt/VTK-8.1.1/IO/Video -I/opt/VTK-8.1.1/Imaging/Math -I/opt/VTK-8.1.1/Imaging/Morphological -I/opt/VTK-8.1.1/Imaging/Statistics -I/opt/VTK-8.1.1/Imaging/Stencil -I/opt/VTK-8.1.1/Interaction/Image -I/opt/VTK-8.1.1/ThirdParty/mpi4py -I/opt/VTK-8.1.1/Parallel/MPI4Py -I/opt/VTK-8.1.1/Rendering/ContextOpenGL2 -I/opt/VTK-8.1.1/Rendering/Image -I/opt/VTK-8.1.1/Rendering/LOD -I/opt/VTK-8.1.1/Rendering/Label -I/opt/VTK-8.1.1/Rendering/Qt -I/opt/VTK-8.1.1/Rendering/VolumeOpenGL2 -I/opt/VTK-8.1.1/Views/Context2D -I/opt/VTK-8.1.1/Views/Infovis -I/opt/VTK-8.1.1/Views/Qt -I/opt/VTK-8.1.1/Web/WebGLExporter -I/opt/VTK-8.1.1/Web/Core -I/opt/VTK-8.1.1/ThirdParty/wslink -I/opt/VTK-8.1.1/Web/Python -I/usr/include/OpenEXR -isystem /home/u1776267/Documents/Code/BRDFViz/src/../external -isystem /usr/include/x86_64-linux-gnu/qt5 -isystem /usr/include/x86_64-linux-gnu/qt5/QtWidgets -isystem /usr/include/x86_64-linux-gnu/qt5/QtGui -isystem /usr/include/x86_64-linux-gnu/qt5/QtCore -isystem /usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++-64 -isystem /usr/include/x86_64-linux-gnu/qt5/QtSql -isystem /opt/VTK-8.1.1/ThirdParty/netcdfcpp/vtknetcdfcpp 

