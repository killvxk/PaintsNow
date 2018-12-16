# Microsoft Developer Studio Project File - Name="SnowyStream" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SnowyStream - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SnowyStream.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SnowyStream.mak" CFG="SnowyStream - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SnowyStream - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SnowyStream - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SnowyStream - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /YX /FD /Zm700 /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SnowyStream - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /Zm800 /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "SnowyStream - Win32 Release"
# Name "SnowyStream - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\AnalyticCurveResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\AudioResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Cache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\DeferredCompact.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\DeferredLightingPipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\File.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\FontResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\GraphicResourceBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\HashTracePipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\MaterialResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\MeshResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\ModelResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\MultiHashTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\ParticlePipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\ParticleResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\RenderTargetResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\ResourceBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\ResourceManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\ScreenSpaceTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\ShaderResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\ShadowMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\SineWave.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\SkeletonResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\SnowyStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\StandardLighting.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\StandardParameter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\StandardPipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\StandardSkinning.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\StandardTransform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\TapeResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\TerrainPipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\TerrainResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\TextResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\TextureResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\VolumePipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\VolumeResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\WaterPipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\WidgetPipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\WidgetShading.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\WidgetTransform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Zipper.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\AnalyticCurveResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\AudioResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Cache.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\DeferredCompact.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\DeferredLightingPipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\File.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\FontResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\GraphicResourceBase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\HashTracePipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\MaterialResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\MeshResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\ModelResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\MultiHashTrace.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\ParticlePipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\ParticleResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\RenderTargetResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\ResourceBase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\ResourceManager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\ScreenSpaceTrace.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\ShaderResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\ShadowMap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\SineWave.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\SkeletonResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\SnowyStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\StandardLighting.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\StandardParameter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\StandardPipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\StandardSkinning.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\StandardTransform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\TapeResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\TerrainPipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\TerrainResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\TextResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\TextureResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\VolumePipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\VolumeResource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\WaterPipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Pipelines\WidgetPipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\WidgetShading.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Resource\Shaders\WidgetTransform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Utility\SnowyStream\Zipper.h
# End Source File
# End Group
# End Target
# End Project
