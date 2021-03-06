# Microsoft Developer Studio Project File - Name="PaintsNow" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=PaintsNow - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PaintsNow.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PaintsNow.mak" CFG="PaintsNow - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PaintsNow - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "PaintsNow - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PaintsNow - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /YX /FD /Zm600 /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "PaintsNow - Win32 Debug"

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
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /Zm700 /c
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

# Name "PaintsNow - Win32 Release"
# Name "PaintsNow - Win32 Debug"
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZBatchRanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZBatchRanger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZDynamicObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZDynamicObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZMemoryStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZMemoryStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZMeta.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZMeta.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZPacket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZPacket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZPipeline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZPipeline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZRemoteProxy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZRemoteProxy.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZRenderFlow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZRenderFlow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZScriptReflect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZScriptReflect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZShaderWalker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZShaderWalker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZTaskQueue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZTaskQueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZThreadPool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZThreadPool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Common\ZType.h
# End Source File
# End Group
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IArchive.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IArchive.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IAsset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IAsset.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IAudio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IDatabase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IDebugger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IDebugger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IDeferredObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IDeferredObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IDevice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IFilterBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IFilterBase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IFontBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IFontBase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\ILayout.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\ILayout.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IMemory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IMemory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\INetwork.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\INetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\Interfaces.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\Interfaces.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IRandom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IRandom.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IReflect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IReflect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IRender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IRender.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IScript.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IScript.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IShader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IShader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IStreamBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IStreamBase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\ITask.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\ITask.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\IThread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\ITimer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\ITimer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\ITunnel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Interface\ITunnel.h
# End Source File
# End Group
# Begin Group "Template"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TAlgorithm.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TAtomic.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TEvent.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TFactory.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TMap.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TMaskType.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TMatrix.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TObject.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TProxy.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TShaderGenerator.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TShaderMacro.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TShaderVariable.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Template\TVector.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Object\Graph.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Object\Graph.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Object\Tiny.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Core\Object\Tiny.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Source\Core\PaintsNow.h
# End Source File
# End Group
# Begin Group "Driver"

# PROP Default_Filter ""
# Begin Group "Archive"

# PROP Default_Filter ""
# Begin Group "7Z"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Archive\7Z\ZArchive7Z.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Archive\7Z\ZArchive7Z.h
# End Source File
# End Group
# Begin Group "Dirent"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Archive\Dirent\ZArchiveDirent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Archive\Dirent\ZArchiveDirent.h
# End Source File
# End Group
# End Group
# Begin Group "Asset"

# PROP Default_Filter ""
# Begin Group "Cursor"

# PROP Default_Filter ""
# End Group
# Begin Group "Map"

# PROP Default_Filter ""
# End Group
# Begin Group "Model"

# PROP Default_Filter ""
# Begin Group "MS3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Asset\Model\MS3D\ZAssetMS3D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Asset\Model\MS3D\ZAssetMS3D.h
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Audio"

# PROP Default_Filter ""
# Begin Group "OpenAL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Audio\OpenAL\ZAudioOpenAL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Audio\OpenAL\ZAudioOpenAL.h
# End Source File
# End Group
# Begin Group "Decoder"

# PROP Default_Filter ""
# Begin Group "LAME"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Audio\Decoder\LAME\ZDecoderLAME.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Audio\Decoder\LAME\ZDecoderLAME.h
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Filter"

# PROP Default_Filter ""
# Begin Group "LZMA"

# PROP Default_Filter ""
# Begin Group "LZMACore"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7z.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zAlloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zAlloc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zArcIn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zBuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zBuf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zBuf2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zCrc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zCrc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zCrcOpt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zDec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zFile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zFile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zStream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\7zVersion.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Aes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Aes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\AesOpt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Alloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Alloc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Bcj2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Bcj2.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Bra.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Bra.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Bra86.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\BraIA64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Compiler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\CpuArch.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\CpuArch.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Delta.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Delta.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzFind.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzFind.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzFindMt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzFindMt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzHash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Lzma2Dec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Lzma2Dec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Lzma2Enc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Lzma2Enc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Lzma86.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Lzma86Dec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Lzma86Enc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzmaDec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzmaDec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzmaEnc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzmaEnc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzmaLib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\LzmaLib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\MtCoder.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\MtCoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Ppmd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Ppmd7.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Ppmd7.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Ppmd7Dec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Ppmd7Enc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Precomp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\RotateDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Sha256.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Sha256.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Sort.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Sort.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Threads.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Threads.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Xz.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\Xz.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\XzCrc64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\XzCrc64.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\XzCrc64Opt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\XzDec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\XzEnc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\XzEnc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\Core\XzIn.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\ZFilterLZMA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZMA\ZFilterLZMA.h
# End Source File
# End Group
# Begin Group "Pod"

# PROP Default_Filter ""
# Begin Group "PodCore"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Pod\Core\Pod.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Pod\Core\Pod.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Pod\ZFilterPod.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Pod\ZFilterPod.h
# End Source File
# End Group
# Begin Group "Json"

# PROP Default_Filter ""
# Begin Group "JsonCore"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\assertions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\autolink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\features.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\forwards.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\json.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\json_reader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\json_tool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\json_value.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\json_valueiterator.inl
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\json_writer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\reader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\value.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\version.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\Core\writer.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\ZFilterJson.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\Json\ZFilterJson.h
# End Source File
# End Group
# Begin Group "LZW"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZW\ZFilterLZW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Filter\LZW\ZFilterLZW.h
# End Source File
# End Group
# End Group
# Begin Group "Font"

# PROP Default_Filter ""
# Begin Group "Freetype"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Font\Freetype\ZFontFreetype.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Font\Freetype\ZFontFreetype.h
# End Source File
# End Group
# End Group
# Begin Group "Frame"

# PROP Default_Filter ""
# Begin Group "Freeglut"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Frame\Freeglut\ZFrameFreeglut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Frame\Freeglut\ZFrameFreeglut.h
# End Source File
# End Group
# End Group
# Begin Group "Image"

# PROP Default_Filter ""
# Begin Group "FreeImage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Image\FreeImage\ZImageFreeImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Image\FreeImage\ZImageFreeImage.h
# End Source File
# End Group
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Group "LibEvent"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Network\LibEvent\ZNetworkLibEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Network\LibEvent\ZNetworkLibEvent.h
# End Source File
# End Group
# End Group
# Begin Group "Render"

# PROP Default_Filter ""
# Begin Group "OpenGL"

# PROP Default_Filter ""
# Begin Group "GL3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Render\OpenGL\3\ZRenderOpenGL3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Render\OpenGL\3\ZRenderOpenGL3.h
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Script"

# PROP Default_Filter ""
# Begin Group "Lua"

# PROP Default_Filter ""
# Begin Group "LuaCore"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lapi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lapi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lauxlib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lauxlib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lbaselib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lbitlib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lcode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lcode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lcorolib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lctype.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lctype.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ldblib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ldebug.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ldebug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ldo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ldo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ldump.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lfunc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lfunc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lgc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lgc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\linit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\liolib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\llex.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\llex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\llimits.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lmathlib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lmem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lmem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\loadlib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lobject.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lobject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lopcodes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lopcodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\loslib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lparser.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lparser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lpcap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lpcap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lpcode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lpcode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lpprint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lpprint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lprefix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lptree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lptree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lptypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lpvm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lpvm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lstate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lstate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lstring.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lstring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lstrlib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ltable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ltable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ltablib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ltm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\ltm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lua.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\luaconf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lualib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lundump.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lundump.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lutf8lib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lvm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lvm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lzio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\Core\lzio.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\ZScriptLua.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\Lua\ZScriptLua.h
# End Source File
# End Group
# Begin Group "TCC"

# PROP Default_Filter ""
# Begin Group "TCCCore"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\Source\Driver\Script\TCC\Core\arm-gen.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Source\Driver\Script\TCC\Core\c67-gen.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\coff.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\elf.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Source\Driver\Script\TCC\Core\i386-asm.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Source\Driver\Script\TCC\Core\i386-asm.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Source\Driver\Script\TCC\Core\i386-gen.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Source\Driver\Script\TCC\Core\i386-tok.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Source\Driver\Script\TCC\Core\il-gen.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Source\Driver\Script\TCC\Core\il-opcodes.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\libtcc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\libtcc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\stab.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\tcc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\tccasm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\tcccoff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\tccelf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\tccgen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\tccpe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\tccpp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\tccrun.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\Core\tcctok.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Source\Driver\Script\TCC\Core\x86_64-asm.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Source\Driver\Script\TCC\Core\x86_64-gen.c"
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\ZScriptTCC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Script\TCC\ZScriptTCC.h
# End Source File
# End Group
# End Group
# Begin Group "Thread"

# PROP Default_Filter ""
# Begin Group "Pthread"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Thread\Pthread\ZThreadPthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Thread\Pthread\ZThreadPthread.h
# End Source File
# End Group
# End Group
# Begin Group "Timer"

# PROP Default_Filter ""
# Begin Group "FreeglutTimer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Timer\Freeglut\ZTimerFreeglut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Timer\Freeglut\ZTimerFreeglut.h
# End Source File
# End Group
# Begin Group "WinTimerQueue"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Timer\WinTimerQueue\ZWinTimerQueue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Timer\WinTimerQueue\ZWinTimerQueue.h
# End Source File
# End Group
# Begin Group "PosixTimer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Timer\PosixTimer\ZPosixTimer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Timer\PosixTimer\ZPosixTimer.h
# End Source File
# End Group
# End Group
# Begin Group "Database"

# PROP Default_Filter ""
# Begin Group "Sqlite"

# PROP Default_Filter ""
# Begin Group "SqliteCore"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Database\Sqlite\Core\sqlite3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Database\Sqlite\Core\sqlite3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Database\Sqlite\Core\sqlite3ext.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Database\Sqlite\ZDatabaseSqlite.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Database\Sqlite\ZDatabaseSqlite.h
# End Source File
# End Group
# End Group
# Begin Group "Random"

# PROP Default_Filter ""
# Begin Group "Libnoise"

# PROP Default_Filter ""
# Begin Group "Libnoise Core"

# PROP Default_Filter ""
# Begin Group "modules"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\abs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\abs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\add.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\add.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\billow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\billow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\blend.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\blend.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\cache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\cache.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\checkerboard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\checkerboard.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\clamp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\clamp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\const.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\const.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\curve.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\curve.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\cylinders.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\cylinders.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\displace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\displace.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\exponent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\exponent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\invert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\invert.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\max.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\max.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\min.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\min.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\module.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\modulebase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\modulebase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\multiply.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\multiply.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\perlin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\perlin.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\power.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\power.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\ridgedmulti.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\ridgedmulti.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\rotatepoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\rotatepoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\scalebias.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\scalebias.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\scalepoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\scalepoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\select.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\select.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\spheres.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\spheres.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\terrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\terrace.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\translatepoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\translatepoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\turbulence.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\turbulence.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\voronoi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\module\voronoi.h
# End Source File
# End Group
# Begin Group "models"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\model\cylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\model\cylinder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\model\line.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\model\line.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\model\model.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\model\plane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\model\plane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\model\sphere.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\model\sphere.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\basictypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\exception.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\interp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\latlon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\latlon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\mathconsts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\noise.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\noisegen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\noisegen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\Core\vectortable.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\ZRandomLibnoisePerlin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Random\Libnoise\ZRandomLibnoisePerlin.h
# End Source File
# End Group
# End Group
# Begin Group "Debugger"

# PROP Default_Filter ""
# Begin Group "MiniDump"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Debugger\MiniDump\ZDebuggerWin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Debugger\MiniDump\ZDebuggerWin.h
# End Source File
# End Group
# End Group
# Begin Group "Tunnel"

# PROP Default_Filter ""
# Begin Group "WinPipe"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Tunnel\WinPipe\ZWinPipe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Source\Driver\Tunnel\WinPipe\ZWinPipe.h
# End Source File
# End Group
# End Group
# End Group
# End Target
# End Project
