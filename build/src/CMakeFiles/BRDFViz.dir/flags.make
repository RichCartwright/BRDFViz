# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# compile CXX with /usr/bin/c++
CXX_FLAGS =     -std=c++11 -Wall -Werror -O3 -std=c++11

CXX_DEFINES = -DvtkDomainsChemistry_AUTOINIT="1(vtkDomainsChemistryOpenGL2)" -DvtkIOExport_AUTOINIT="1(vtkIOExportOpenGL2)" -DvtkRenderingContext2D_AUTOINIT="1(vtkRenderingContextOpenGL2)" -DvtkRenderingCore_AUTOINIT="3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL2)" -DvtkRenderingOpenGL2_AUTOINIT="1(vtkRenderingGL2PSOpenGL2)" -DvtkRenderingVolume_AUTOINIT="1(vtkRenderingVolumeOpenGL2)"

CXX_INCLUDES = -I/opt/VTK8.1/Utilities/KWIML -I/home/rich/Downloads/VTK-8.1.1/Utilities/KWIML -I/opt/VTK8.1/Utilities/KWSys -I/home/rich/Downloads/VTK-8.1.1/Utilities/KWSys -I/opt/VTK8.1/Common/Core -I/home/rich/Downloads/VTK-8.1.1/Common/Core -I/opt/VTK8.1/Common/Math -I/home/rich/Downloads/VTK-8.1.1/Common/Math -I/opt/VTK8.1/Common/Misc -I/home/rich/Downloads/VTK-8.1.1/Common/Misc -I/opt/VTK8.1/Common/System -I/home/rich/Downloads/VTK-8.1.1/Common/System -I/opt/VTK8.1/Common/Transforms -I/home/rich/Downloads/VTK-8.1.1/Common/Transforms -I/opt/VTK8.1/Common/DataModel -I/home/rich/Downloads/VTK-8.1.1/Common/DataModel -I/opt/VTK8.1/Common/Color -I/home/rich/Downloads/VTK-8.1.1/Common/Color -I/opt/VTK8.1/Common/ExecutionModel -I/home/rich/Downloads/VTK-8.1.1/Common/ExecutionModel -I/opt/VTK8.1/Common/ComputationalGeometry -I/home/rich/Downloads/VTK-8.1.1/Common/ComputationalGeometry -I/opt/VTK8.1/Filters/Core -I/home/rich/Downloads/VTK-8.1.1/Filters/Core -I/opt/VTK8.1/Filters/General -I/home/rich/Downloads/VTK-8.1.1/Filters/General -I/opt/VTK8.1/Imaging/Core -I/home/rich/Downloads/VTK-8.1.1/Imaging/Core -I/opt/VTK8.1/Imaging/Fourier -I/home/rich/Downloads/VTK-8.1.1/Imaging/Fourier -I/opt/VTK8.1/ThirdParty/alglib -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/alglib -I/opt/VTK8.1/Filters/Statistics -I/home/rich/Downloads/VTK-8.1.1/Filters/Statistics -I/opt/VTK8.1/Filters/Extraction -I/home/rich/Downloads/VTK-8.1.1/Filters/Extraction -I/opt/VTK8.1/Infovis/Core -I/home/rich/Downloads/VTK-8.1.1/Infovis/Core -I/opt/VTK8.1/Filters/Geometry -I/home/rich/Downloads/VTK-8.1.1/Filters/Geometry -I/opt/VTK8.1/Filters/Sources -I/home/rich/Downloads/VTK-8.1.1/Filters/Sources -I/opt/VTK8.1/Rendering/Core -I/home/rich/Downloads/VTK-8.1.1/Rendering/Core -I/opt/VTK8.1/ThirdParty/zlib -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/zlib -I/opt/VTK8.1/ThirdParty/freetype -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/freetype -I/opt/VTK8.1/Rendering/FreeType -I/home/rich/Downloads/VTK-8.1.1/Rendering/FreeType -I/opt/VTK8.1/Rendering/Context2D -I/home/rich/Downloads/VTK-8.1.1/Rendering/Context2D -I/opt/VTK8.1/Charts/Core -I/home/rich/Downloads/VTK-8.1.1/Charts/Core -I/opt/VTK8.1/Utilities/DICOMParser -I/home/rich/Downloads/VTK-8.1.1/Utilities/DICOMParser -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/lz4/vtklz4/lib -I/opt/VTK8.1/ThirdParty/lz4/vtklz4 -I/opt/VTK8.1/ThirdParty/lz4 -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/lz4 -I/opt/VTK8.1/IO/Core -I/home/rich/Downloads/VTK-8.1.1/IO/Core -I/opt/VTK8.1/IO/Legacy -I/home/rich/Downloads/VTK-8.1.1/IO/Legacy -I/opt/VTK8.1/ThirdParty/expat -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/expat -I/opt/VTK8.1/IO/XMLParser -I/home/rich/Downloads/VTK-8.1.1/IO/XMLParser -I/opt/VTK8.1/Domains/Chemistry -I/home/rich/Downloads/VTK-8.1.1/Domains/Chemistry -I/opt/VTK8.1/Utilities/EncodeString -I/home/rich/Downloads/VTK-8.1.1/Utilities/EncodeString -I/opt/VTK8.1/ThirdParty/glew -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/glew -I/opt/VTK8.1/Rendering/OpenGL2 -I/home/rich/Downloads/VTK-8.1.1/Rendering/OpenGL2 -I/opt/VTK8.1/Domains/ChemistryOpenGL2 -I/home/rich/Downloads/VTK-8.1.1/Domains/ChemistryOpenGL2 -I/opt/VTK8.1/IO/XML -I/home/rich/Downloads/VTK-8.1.1/IO/XML -I/opt/VTK8.1/Utilities/HashSource -I/home/rich/Downloads/VTK-8.1.1/Utilities/HashSource -I/opt/VTK8.1/Parallel/Core -I/home/rich/Downloads/VTK-8.1.1/Parallel/Core -I/opt/VTK8.1/Filters/AMR -I/home/rich/Downloads/VTK-8.1.1/Filters/AMR -I/opt/VTK8.1/Filters/FlowPaths -I/home/rich/Downloads/VTK-8.1.1/Filters/FlowPaths -I/opt/VTK8.1/Filters/Generic -I/home/rich/Downloads/VTK-8.1.1/Filters/Generic -I/opt/VTK8.1/Imaging/Sources -I/home/rich/Downloads/VTK-8.1.1/Imaging/Sources -I/opt/VTK8.1/Filters/Hybrid -I/home/rich/Downloads/VTK-8.1.1/Filters/Hybrid -I/opt/VTK8.1/Filters/HyperTree -I/home/rich/Downloads/VTK-8.1.1/Filters/HyperTree -I/opt/VTK8.1/Imaging/General -I/home/rich/Downloads/VTK-8.1.1/Imaging/General -I/opt/VTK8.1/Filters/Imaging -I/home/rich/Downloads/VTK-8.1.1/Filters/Imaging -I/opt/VTK8.1/Filters/Modeling -I/home/rich/Downloads/VTK-8.1.1/Filters/Modeling -I/opt/VTK8.1/Filters/Parallel -I/home/rich/Downloads/VTK-8.1.1/Filters/Parallel -I/opt/VTK8.1/Filters/ParallelImaging -I/home/rich/Downloads/VTK-8.1.1/Filters/ParallelImaging -I/opt/VTK8.1/Filters/Points -I/home/rich/Downloads/VTK-8.1.1/Filters/Points -I/opt/VTK8.1/Filters/Programmable -I/home/rich/Downloads/VTK-8.1.1/Filters/Programmable -I/opt/VTK8.1/Filters/SMP -I/home/rich/Downloads/VTK-8.1.1/Filters/SMP -I/opt/VTK8.1/Filters/Selection -I/home/rich/Downloads/VTK-8.1.1/Filters/Selection -I/opt/VTK8.1/Filters/Texture -I/home/rich/Downloads/VTK-8.1.1/Filters/Texture -I/opt/VTK8.1/Filters/Topology -I/home/rich/Downloads/VTK-8.1.1/Filters/Topology -I/opt/VTK8.1/ThirdParty/verdict -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/verdict -I/opt/VTK8.1/Filters/Verdict -I/home/rich/Downloads/VTK-8.1.1/Filters/Verdict -I/opt/VTK8.1/Interaction/Style -I/home/rich/Downloads/VTK-8.1.1/Interaction/Style -I/opt/VTK8.1/GUISupport/Qt -I/home/rich/Downloads/VTK-8.1.1/GUISupport/Qt -I/opt/VTK8.1/ThirdParty/sqlite -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/sqlite -I/opt/VTK8.1/IO/SQL -I/home/rich/Downloads/VTK-8.1.1/IO/SQL -I/opt/VTK8.1/GUISupport/QtSQL -I/home/rich/Downloads/VTK-8.1.1/GUISupport/QtSQL -I/opt/VTK8.1/Utilities/MetaIO/vtkmetaio -I/opt/VTK8.1/Utilities/MetaIO -I/home/rich/Downloads/VTK-8.1.1/Utilities/MetaIO -I/opt/VTK8.1/ThirdParty/jpeg -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/jpeg -I/opt/VTK8.1/ThirdParty/png -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/png -I/opt/VTK8.1/ThirdParty/tiff/vtktiff/libtiff -I/opt/VTK8.1/ThirdParty/tiff -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/tiff -I/opt/VTK8.1/IO/Image -I/home/rich/Downloads/VTK-8.1.1/IO/Image -I/opt/VTK8.1/Imaging/Hybrid -I/home/rich/Downloads/VTK-8.1.1/Imaging/Hybrid -I/opt/VTK8.1/Infovis/Layout -I/home/rich/Downloads/VTK-8.1.1/Infovis/Layout -I/opt/VTK8.1/Imaging/Color -I/home/rich/Downloads/VTK-8.1.1/Imaging/Color -I/opt/VTK8.1/Rendering/Annotation -I/home/rich/Downloads/VTK-8.1.1/Rendering/Annotation -I/opt/VTK8.1/Rendering/Volume -I/home/rich/Downloads/VTK-8.1.1/Rendering/Volume -I/opt/VTK8.1/Interaction/Widgets -I/home/rich/Downloads/VTK-8.1.1/Interaction/Widgets -I/opt/VTK8.1/Views/Core -I/home/rich/Downloads/VTK-8.1.1/Views/Core -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/libproj4/vtklibproj4 -I/opt/VTK8.1/ThirdParty/libproj4/vtklibproj4 -I/opt/VTK8.1/ThirdParty/libproj4 -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/libproj4 -I/opt/VTK8.1/Geovis/Core -I/home/rich/Downloads/VTK-8.1.1/Geovis/Core -I/opt/VTK8.1/ThirdParty/hdf5/vtkhdf5 -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/hdf5/vtkhdf5/hl/src -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/hdf5/vtkhdf5/src -I/opt/VTK8.1/ThirdParty/hdf5 -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/hdf5 -I/opt/VTK8.1/IO/AMR -I/home/rich/Downloads/VTK-8.1.1/IO/AMR -I/opt/VTK8.1/IO/EnSight -I/home/rich/Downloads/VTK-8.1.1/IO/EnSight -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/netcdf/vtknetcdf/include -I/opt/VTK8.1/ThirdParty/netcdf/vtknetcdf -I/opt/VTK8.1/ThirdParty/netcdf -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/netcdf -I/opt/VTK8.1/ThirdParty/exodusII -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/exodusII -I/opt/VTK8.1/IO/Exodus -I/home/rich/Downloads/VTK-8.1.1/IO/Exodus -I/opt/VTK8.1/ThirdParty/gl2ps -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/gl2ps -I/opt/VTK8.1/Rendering/GL2PSOpenGL2 -I/home/rich/Downloads/VTK-8.1.1/Rendering/GL2PSOpenGL2 -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/libharu/vtklibharu/include -I/opt/VTK8.1/ThirdParty/libharu/vtklibharu/include -I/opt/VTK8.1/ThirdParty/libharu -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/libharu -I/opt/VTK8.1/IO/Export -I/home/rich/Downloads/VTK-8.1.1/IO/Export -I/opt/VTK8.1/IO/ExportOpenGL2 -I/home/rich/Downloads/VTK-8.1.1/IO/ExportOpenGL2 -I/opt/VTK8.1/IO/Geometry -I/home/rich/Downloads/VTK-8.1.1/IO/Geometry -I/opt/VTK8.1/IO/Import -I/home/rich/Downloads/VTK-8.1.1/IO/Import -I/opt/VTK8.1/ThirdParty/libxml2/vtklibxml2 -I/opt/VTK8.1/ThirdParty/libxml2 -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/libxml2 -I/opt/VTK8.1/IO/Infovis -I/home/rich/Downloads/VTK-8.1.1/IO/Infovis -I/opt/VTK8.1/IO/LSDyna -I/home/rich/Downloads/VTK-8.1.1/IO/LSDyna -I/opt/VTK8.1/IO/MINC -I/home/rich/Downloads/VTK-8.1.1/IO/MINC -I/opt/VTK8.1/ThirdParty/oggtheora -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/oggtheora -I/opt/VTK8.1/IO/Movie -I/home/rich/Downloads/VTK-8.1.1/IO/Movie -I/opt/VTK8.1/ThirdParty/netcdfcpp -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/netcdfcpp -I/opt/VTK8.1/IO/NetCDF -I/home/rich/Downloads/VTK-8.1.1/IO/NetCDF -I/opt/VTK8.1/IO/PLY -I/home/rich/Downloads/VTK-8.1.1/IO/PLY -I/opt/VTK8.1/ThirdParty/jsoncpp -I/home/rich/Downloads/VTK-8.1.1/ThirdParty/jsoncpp -I/opt/VTK8.1/IO/Parallel -I/home/rich/Downloads/VTK-8.1.1/IO/Parallel -I/opt/VTK8.1/IO/ParallelXML -I/home/rich/Downloads/VTK-8.1.1/IO/ParallelXML -I/opt/VTK8.1/IO/TecplotTable -I/home/rich/Downloads/VTK-8.1.1/IO/TecplotTable -I/opt/VTK8.1/IO/Video -I/home/rich/Downloads/VTK-8.1.1/IO/Video -I/opt/VTK8.1/Imaging/Math -I/home/rich/Downloads/VTK-8.1.1/Imaging/Math -I/opt/VTK8.1/Imaging/Morphological -I/home/rich/Downloads/VTK-8.1.1/Imaging/Morphological -I/opt/VTK8.1/Imaging/Statistics -I/home/rich/Downloads/VTK-8.1.1/Imaging/Statistics -I/opt/VTK8.1/Imaging/Stencil -I/home/rich/Downloads/VTK-8.1.1/Imaging/Stencil -I/opt/VTK8.1/Interaction/Image -I/home/rich/Downloads/VTK-8.1.1/Interaction/Image -I/opt/VTK8.1/Rendering/ContextOpenGL2 -I/home/rich/Downloads/VTK-8.1.1/Rendering/ContextOpenGL2 -I/opt/VTK8.1/Rendering/Image -I/home/rich/Downloads/VTK-8.1.1/Rendering/Image -I/opt/VTK8.1/Rendering/LOD -I/home/rich/Downloads/VTK-8.1.1/Rendering/LOD -I/opt/VTK8.1/Rendering/Label -I/home/rich/Downloads/VTK-8.1.1/Rendering/Label -I/opt/VTK8.1/Rendering/Qt -I/home/rich/Downloads/VTK-8.1.1/Rendering/Qt -I/opt/VTK8.1/Rendering/VolumeOpenGL2 -I/home/rich/Downloads/VTK-8.1.1/Rendering/VolumeOpenGL2 -I/opt/VTK8.1/Views/Context2D -I/home/rich/Downloads/VTK-8.1.1/Views/Context2D -I/opt/VTK8.1/Views/Infovis -I/home/rich/Downloads/VTK-8.1.1/Views/Infovis -I/opt/VTK8.1/Views/Qt -I/home/rich/Downloads/VTK-8.1.1/Views/Qt -I/usr/include/OpenEXR -isystem /home/rich/Documents/Code/BRDFViz/src/../external 

