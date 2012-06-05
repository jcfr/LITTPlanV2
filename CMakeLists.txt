
#-----------------------------------------------------------------------------
set(MODULE_NAME "LITTPlanV2") # Do not use 'project()'
set(MODULE_TITLE "${MODULE_NAME}")

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_SLICER_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

set(MODULE_INCLUDE_DIRECTORIES
  )

set(MODULE_SRCS
  qSlicerLITTPlanV2IO.cxx
  qSlicerLITTPlanV2IO.h
  qSlicerLITTPlanV2Module.cxx
  qSlicerLITTPlanV2Module.h
  qSlicerLITTPlanV2ModuleWidget.cxx
  qSlicerLITTPlanV2ModuleWidget.h
  )

set(MODULE_MOC_SRCS
  qSlicerLITTPlanV2IO.h
  qSlicerLITTPlanV2Module.h
  qSlicerLITTPlanV2ModuleWidget.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSlicerLITTPlanV2Module.ui
  )

set(MODULE_TARGET_LIBRARIES
  )

set(MODULE_RESOURCES
  Resources/qSlicerLITTPlanV2Module.qrc
  )

#-----------------------------------------------------------------------------
slicerMacroBuildQtModule(
  NAME ${MODULE_NAME}
  TITLE ${MODULE_TITLE}
  EXPORT_DIRECTIVE ${MODULE_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  SRCS ${MODULE_SRCS}
  MOC_SRCS ${MODULE_MOC_SRCS}
  UI_SRCS ${MODULE_UI_SRCS}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  RESOURCES ${MODULE_RESOURCES}
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

