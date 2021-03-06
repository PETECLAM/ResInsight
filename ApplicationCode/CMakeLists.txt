cmake_minimum_required (VERSION 2.8)

project (ApplicationCode)

# NB: The generated file is written to Cmake binary folder to avoid source tree pollution
# This folder is added to include_directories
CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/ApplicationCode/Adm/RiaVersionInfo.h.cmake
                ${CMAKE_BINARY_DIR}/Generated/RiaVersionInfo.h
)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Adm
    ${CMAKE_CURRENT_SOURCE_DIR}/Application
    ${CMAKE_CURRENT_SOURCE_DIR}/FileInterface
    ${CMAKE_CURRENT_SOURCE_DIR}/SocketInterface
    ${CMAKE_CURRENT_SOURCE_DIR}/ModelVisualization
    ${CMAKE_CURRENT_SOURCE_DIR}/UserInterface
    ${CMAKE_CURRENT_SOURCE_DIR}/ProjectDataModel
    ${CMAKE_CURRENT_SOURCE_DIR}/ReservoirDataModel
    ${CMAKE_CURRENT_SOURCE_DIR}/WellPathImportSsihub
    ${CMAKE_BINARY_DIR}/Generated
	${CMAKE_CURRENT_BINARY_DIR}
)


# Use all h files in the subdirectories to make them available in the project
file( GLOB_RECURSE HEADER_FILES *.h )

set( APPLICATION_FILES 
    RiaMain.cpp
    RiaStdInclude.cpp
        
    Application/RiaApplication.cpp
    Application/RiaPreferences.cpp
	Application/RiaImageFileCompare.cpp
	Application/RiaImageCompareReporter.cpp
	Application/RiaRegressionTest.cpp
)

set( USER_INTERFACE_FILES
    UserInterface/RiuCursors.cpp
    UserInterface/RiuMainWindow.cpp
    UserInterface/RiuPreferencesDialog.cpp
    UserInterface/RiuResultInfoPanel.cpp
    UserInterface/RiuViewer.cpp
    UserInterface/RiuSimpleHistogramWidget.cpp
	UserInterface/RiuMultiCaseImportDialog.cpp
    UserInterface/RiuProcessMonitor.cpp
)

set( SOCKET_INTERFACE_FILES
    SocketInterface/RiaSocketServer.cpp
    SocketInterface/RiaProjectInfoCommands.cpp
    SocketInterface/RiaCaseInfoCommands.cpp
    SocketInterface/RiaGeometryCommands.cpp
    SocketInterface/RiaPropertyDataCommands.cpp
	SocketInterface/RiaWellDataCommands.cpp
    SocketInterface/RiaSocketTools.cpp
)


list( APPEND CPP_SOURCES
     ${APPLICATION_FILES}
     ${USER_INTERFACE_FILES}
     ${SOCKET_INTERFACE_FILES}
)



list( APPEND REFERENCED_CMAKE_FILES
    ReservoirDataModel/CMakeLists_files.cmake
    FileInterface/CMakeLists_files.cmake
    ProjectDataModel/CMakeLists_files.cmake
    ModelVisualization/CMakeLists_files.cmake
)

# Include source file lists from *.cmake files
foreach (referencedfile ${REFERENCED_CMAKE_FILES})
    include (${referencedfile})
endforeach (referencedfile)

list( APPEND CPP_SOURCES
    ${CODE_SOURCE_FILES}
)

add_subdirectory(WellPathImportSsihub)


# Define files for MOC-ing
set ( QT_MOC_HEADERS
    Application/RiaApplication.h
    
    ProjectDataModel/RimUiTreeModelPdm.h
    ProjectDataModel/RimUiTreeView.h
    ProjectDataModel/RimMimeData.h
    
    UserInterface/RiuMainWindow.h
    UserInterface/RiuPreferencesDialog.h
    UserInterface/RiuResultInfoPanel.h
    UserInterface/RiuViewer.h
    UserInterface/RiuProcessMonitor.h
	SocketInterface/RiaSocketServer.h
	UserInterface/RiuMultiCaseImportDialog.h
)

qt4_wrap_cpp( MOC_FILES_CPP ${QT_MOC_HEADERS} )

# Define files for the uic compiler
set ( QT_UI_FILES
	UserInterface/RiuMultiCaseImportDialog.ui
)

qt4_wrap_ui( FORM_FILES_CPP ${QT_UI_FILES} )

# NOTE! Resources in subfolders must append to QRC_FILES using the following statement
#   set( QRC_FILES
#       ${QRC_FILES}
#       ${CMAKE_CURRENT_SOURCE_DIR}/Resources/myLibrary.qrc
#       PARENT_SCOPE
#   )

set( QRC_FILES
    ${QRC_FILES} 
    Resources/ResInsight.qrc
)

# Runs RCC on specified files
qt4_add_resources( QRC_FILES_CPP ${QRC_FILES} )

# Adding resource (RC) files for Windows
if ( MSVC )
    set( WIN_RESOURCE Resources/ResInsight.rc )
endif()

#############################################################################
# creating PCH's for MSVC and GCC on Linux
#############################################################################

set( RAW_SOURCES ${CPP_SOURCES} )

list( REMOVE_ITEM RAW_SOURCES 
	RiaStdInclude.cpp

	${CODE_SOURCE_FILES}

	ModelVisualization/RivCellEdgeEffectGenerator.cpp
	ModelVisualization/RivPipeGeometryGenerator.cpp
	ModelVisualization/RivWellPipesPartMgr.cpp
	ModelVisualization/RivWellHeadPartMgr.cpp
	
	Application/RiaImageFileCompare.cpp
	Application/RiaImageCompareReporter.cpp
	Application/RiaRegressionTest.cpp
	    
    FileInterface/RifEclipseInputFileTools.cpp
    FileInterface/RifEclipseOutputFileTools.cpp
    FileInterface/RifEclipseRestartFilesetAccess.cpp
    FileInterface/RifEclipseRestartDataAccess.cpp
    FileInterface/RifEclipseUnifiedRestartFileAccess.cpp
    FileInterface/RifReaderEclipseInput.cpp
    FileInterface/RifReaderEclipseOutput.cpp

    UserInterface/RiuSimpleHistogramWidget.cpp
    UserInterface/RiuMultiCaseImportDialog.cpp
)

include( CustomPCH.cmake )
set( ALL_INCLUDES 
    ${LibCore_SOURCE_DIR}
    ${LibGeometry_SOURCE_DIR}
    ${LibGuiQt_SOURCE_DIR}
    ${LibRender_SOURCE_DIR}
    ${LibViewing_SOURCE_DIR}
    ${QT_INCLUDES}
)

set( PCH_NAME RiaStdInclude )
set( GCC_PCH_TARGET gccPCH )
set( PCH_COMPILER_DEFINE EMPTY )
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set( PCH_COMPILER_DEFINE CVF_LINUX)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

precompiled_header( RAW_SOURCES ALL_INCLUDES ${GCC_PCH_TARGET} ${PCH_NAME} ${PCH_COMPILER_DEFINE} )


################################################################################
# Create source groups - see also included CMakeLists_files.cmake
################################################################################
source_group( "Application"         FILES ${APPLICATION_FILES} )
source_group( "ModelVisualization"  FILES ${MODEL_VISUALIZATION_FILES} )
source_group( "UserInterface"       FILES ${USER_INTERFACE_FILES} )
source_group( "SocketInterface"     FILES ${SOCKET_INTERFACE_FILES} )


# Default behaviour for a Qt application is a console application, resulting in a console window always being launced at startup
# The following statement is used to control this behaviour
#   set_target_properties( MY_TARGET PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
# This setting i not honored by Cmake - http://public.kitware.com/Bug/view.php?id=14326 
# We use the following workaround described in the bug report
#   ADD_EXECUTABLE(${TARGET_NAME} WIN32 ${SRC})
# See CMake symbol WIN32_EXECUTABLE for details

if (MSVC)
    set( EXE_FILES WIN32)
endif()
set( EXE_FILES
    ${EXE_FILES}
    ${CPP_SOURCES}
    ${MOC_FILES_CPP}
	${FORM_FILES_CPP}
    ${QRC_FILES_CPP}
    ${WIN_RESOURCE}
	${HEADER_FILES}
	${REFERENCED_CMAKE_FILES}
    ../ResInsightVersion.cmake
)

add_executable( ResInsight ${EXE_FILES} )


set( LINK_LIBRARIES
    WellPathImportSsihub

    cafPdmCvf
    cafUserInterface
    cafProjectDataModel
    cafViewer
    cafAnimControl
    CommonCode
    LibGuiQt
    LibViewing
    LibRender
    LibGeometry
    LibCore

    ecl
    ert_util
    ert_geometry
    ecl_well

    ${OPENGL_LIBRARIES}
    ${QT_LIBRARIES}
)
set( EXTERNAL_LINK_LIBRARIES ${ERT_LIBRARY_LIST} )

# According to ivarun this is needed on OpenSuse, and Fedora. See: https://github.com/OPM/ResInsight/pull/7
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set ( EXTERNAL_LINK_LIBRARIES
		${EXTERNAL_LINK_LIBRARIES}
		rt
    )
endif()

target_link_libraries( ResInsight ${LINK_LIBRARIES} ${EXTERNAL_LINK_LIBRARIES})

# Copy Dlls
if (MSVC)

    # Qt DLLs
    set (QTLIBLIST QtCore QtCored QtGui QtGuid QtOpenGl QtOpenGld QtNetwork QtNetworkd QtScript QtScriptd QtScriptTools QtScriptToolsd)
    foreach (qtlib ${QTLIBLIST})
        add_custom_command(TARGET ResInsight POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_BINARY_DIR}/${qtlib}4.dll"
                ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>)
    endforeach( qtlib )

endif(MSVC)

#############################################################################
# Install 
#############################################################################

set (RESINSIGHT_LICENSE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/Adm/LicenseInformation.txt
  ${CMAKE_CURRENT_SOURCE_DIR}/Adm/gplLicense.txt
  )

# bundle libraries together with private installation
if (RESINSIGHT_PRIVATE_INSTALL)

	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	  # tell binary to first attempt to load libraries from its own directory
	  set_target_properties (ResInsight PROPERTIES INSTALL_RPATH "\$ORIGIN")

		# Find Qt libraries and sym links
		file (GLOB RESINSIGHT_FILES
				${QT_LIBRARY_DIR}/libQtCore.so*
				${QT_LIBRARY_DIR}/libQtGui.so*
				${QT_LIBRARY_DIR}/libQtOpenGL.so*
				${QT_LIBRARY_DIR}/libQtNetwork.so*
				${QT_LIBRARY_DIR}/libQtScript.so*
				${QT_LIBRARY_DIR}/libQtScriptTools.so*
			)

	endif()

	if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	  # put a .exe.local file in the target directory to pick up DLLs from there
	  install (CODE "exec_program (${CMAKE_COMMAND} ARGS -E touch \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}${RESINSIGHT_FINAL_NAME}/ResInsight${CMAKE_EXECUTABLE_SUFFIX}.local)")

		set (RESINSIGHT_FILES
				${QT_BINARY_DIR}/QtCore4.dll
				${QT_BINARY_DIR}/QtGui4.dll
				${QT_BINARY_DIR}/QtOpenGL4.dll
				${QT_BINARY_DIR}/QtNetwork4.dll
				${QT_BINARY_DIR}/QtScript4.dll
				${QT_BINARY_DIR}/QtScriptTools4.dll
			)
	endif()

	set (RESINSIGHT_FILES ${RESINSIGHT_FILES} ${RESINSIGHT_LICENSE_FILES})
        

	install(TARGETS ResInsight DESTINATION ${RESINSIGHT_FINAL_NAME})

	install(FILES ${RESINSIGHT_FILES} DESTINATION ${RESINSIGHT_FINAL_NAME} )

	if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resinsight DESTINATION ${RESINSIGHT_FINAL_NAME} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
	endif()

else (RESINSIGHT_PRIVATE_INSTALL)
  # binaries go in /usr/bin
  install (TARGETS ResInsight
	DESTINATION bin
	)
  # license go in /usr/share/doc
  install (FILES ${RESINSIGHT_LICENSE_FILES}
	DESTINATION share/doc/ResInsight
	)
  # no bundled libraries for system install
  # application icon
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resources/AppLogo48x48.png
	DESTINATION share/icons/hicolor/48x48/apps
	RENAME ResInsight.png
	)
  # desktop environment icon; remember to call `update-desktop-database`
  # in package post-install scripts
  configure_file (
	${CMAKE_CURRENT_SOURCE_DIR}/resinsight.desktop.in
	${CMAKE_CURRENT_BINARY_DIR}/resinsight.desktop
	@ONLY
	)
  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/resinsight.desktop
	DESTINATION share/applications
	)
endif (RESINSIGHT_PRIVATE_INSTALL)
