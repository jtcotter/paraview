IF(NOT CMAKE_CROSSCOMPILING)

  SET(EXECUTABLE_OUTPUT_PATH ${VTKCS_BINARY_DIR}/bin CACHE PATH "Single output path for executable")
  # vtkParse.tab.c has #line markers that break #include with double quotes.
  INCLUDE_DIRECTORIES(${ParaView_SOURCE_DIR}/VTK/Wrapping)
  ADD_EXECUTABLE(vtkWrapClientServer vtkWrapClientServer.c
                ${ParaView_SOURCE_DIR}/VTK/Wrapping/vtkParse.tab.c)
  EXPORT(TARGETS vtkWrapClientServer FILE ${EXPORT_EXECUTABLES_FILE} NAMESPACE "${EXPORT_EXECUTABLES_NAMESPACE}" APPEND)

  IF(NOT PV_INSTALL_NO_DEVELOPMENT)
    INSTALL_TARGETS(/lib/paraview-${PARAVIEW_VERSION} vtkWrapClientServer)
  ENDIF(NOT PV_INSTALL_NO_DEVELOPMENT)

ENDIF(NOT CMAKE_CROSSCOMPILING)
