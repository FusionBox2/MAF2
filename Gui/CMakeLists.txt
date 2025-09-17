cmake_minimum_required(VERSION 3.27)
#
# Copyright and description to be placed here
#

PROJECT(mafGui)

CONFIGURE_FILE("${FTK_SOURCE_DIR}/Gui/manifest.in" "${FTK_BINARY_DIR}/Gui/manifest.in" COPYONLY)

SET(PROJECT_SRCS
  mafEvent.cpp
  mafEvent.h
  mafGUIDialogFindVme.cpp
  mafGUIDialogFindVme.h
  
  mafGUIDialogTransferFunction2D.cpp
  mafGUIDialogTransferFunction2D.h
  mafTransferFunction2DInteractors.cpp
  
  mafGUISettings.cpp
  mafGUISettings.h
  mafGUISettingsStorage.cpp
  mafGUISettingsStorage.h
  mafGUISettingsTimeBar.cpp
  mafGUISettingsTimeBar.h
  
  mafPics.cpp
  mafPics.h
  mafWXLog.cpp
  mafWXLog.h
  mafGUIApplicationLayoutSettings.cpp
  mafGUIApplicationLayoutSettings.h
  mafGUIApplicationSettings.cpp
  mafGUIApplicationSettings.h
  mafGUIButton.cpp
  mafGUIButton.h
  mafGUICheckListBox.cpp
  mafGUICheckListBox.h
  ../ftk/Gui/wx/CheckTree.cpp
  ../ftk/Gui/wx/CheckTree.h
  mafGUIColorSwatch.cpp
  mafGUIColorSwatch.h
  mafGUIColorWidget.cpp
  mafGUIColorWidget.h
  mafGUIDialog.cpp
  mafGUIDialog.h
  mafGUIDockSettings.cpp
  mafGUIDockSettings.h
  mafGUIFloatSlider.cpp
  mafGUIFloatSlider.h
  mafGUI.cpp
  mafGUI.h
  mafGUIHolder.cpp
  mafGUIHolder.h
  mafGUILab.cpp
  mafGUILab.h
  mafGUIListBox.cpp
  mafGUIListBox.h
  mafGUIListCtrl.cpp
  mafGUIListCtrl.h
  mafGUILocaleSettings.cpp
  mafGUILocaleSettings.h
  mafGUIMeasureUnitSettings.cpp
  mafGUIMeasureUnitSettings.h
  mafGUIMovieCtrl.cpp
  mafGUIMovieCtrl.h
  ../ftk/Gui/wx/MainFrame.h
  ../ftk/Gui/wx/ViewFrame.h
  mafGUINamedPanel.cpp
  mafGUINamedPanel.h
  ../ftk/Gui/wx/Panel.h
  mafGUIPanelStack.cpp
  mafGUIPanelStack.h
  mafGUIPicButton.cpp
  mafGUIPicButton.h
  mafGUIRangeSlider.cpp
  mafGUIRangeSlider.h
  mafGUIRollOut.cpp
  mafGUIRollOut.h
  mafGUIScrolledPanel.cpp
  mafGUIScrolledPanel.h
  mafGUISettingsDialog.cpp
  mafGUISettingsDialog.h
  mafGUISplittedPanel.cpp
  mafGUISplittedPanel.h
  mafGUITimeBar.cpp
  mafGUITimeBar.h
  ../ftk/Gui/wx/Tree.cpp
  ../ftk/Gui/wx/Tree.h
  mafGUIValidator.cpp
  mafGUIValidator.h
  mafGUIViewWin.cpp
  mafGUIViewWin.h
  mafGUICrossIncremental.cpp
  mafGUICrossIncremental.h
  mafGUIComboBox.cpp
  mafGUIComboBox.h
  mafGUISettingsHelp.h
  mafGUISettingsHelp.cpp
  mafObjectWithGUI.cpp
  mafObjectWithGUI.h



  medGUILandmark.cpp
  medGUILandmark.h
  medGUIWizard.cpp
  medGUIWizard.h
  medGUIWizardPage.cpp
  medGUIWizardPage.h
  medGUIWizardPageNew.cpp
  medGUIWizardPageNew.h
  medGUIDicomSettings.cpp
  medGUIDicomSettings.h
  # medGUIContextualMenu.cpp
  # medGUIContextualMenu.h
  medGUISettingsAdvanced.cpp
  medGUISettingsAdvanced.h
  medGUIDynamicVP.cpp
  medGUIDynamicVP.h
  medGUILutHistogramSwatch.cpp
  medGUILutHistogramSwatch.h
  medGUILutHistogramEditor.cpp
  medGUILutHistogramEditor.h
)

IF(MAF_USE_CURL)
  SET(PROJECT_SRCS ${PROJECT_SRCS}
	mafGUIDialogRemoteFile.cpp
	mafGUIDialogRemoteFile.h
  )
ENDIF(MAF_USE_CURL)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafLightKit.cpp
  mafLightKit.h

  mafGUIDialogPreview.cpp
  mafGUIDialogPreview.h

  mafGUIDictionaryWidget.cpp
  mafGUIDictionaryWidget.h

  mafAnimate.cpp
  mafAnimate.h
  mafAttachCamera.cpp
  mafAttachCamera.h

  mafSideBar.cpp
  mafSideBar.h
  mafTextKit.cpp
  mafTextKit.h
  mafVTKLog.cpp
  mafVTKLog.h

  # depends on mafAssembly -> wxVTKRenderWindowInteractor
  mafView.cpp
  mafView.h

  # these depends on vtkLookupTable
  mafColor.cpp
  mafColor.h
  mafGUILutEditor.cpp
  mafGUILutEditor.h
  mafGUILutPreset.cpp
  mafGUILutPreset.h
  mafGUILutSlider.cpp
  mafGUILutSlider.h
  mafGUILutSwatch.cpp
  mafGUILutSwatch.h
  mafGUILutWidget.cpp
  mafGUILutWidget.h

  mafGUIHistogramWidget.cpp
  mafGUIHistogramWidget.h
  )
ENDIF (MAF_USE_VTK)

IF (WITH_OPENSSL)


ENDIF(WITH_OPENSSL)



# List the kits from VTK that are needed by this project.
SET(PROJECT_LIBS mafBase IO vtkMAF)

IF (MAF_USE_CURL)
  SET(PROJECT_LIBS ${PROJECT_LIBS} CURL::libcurl)
ENDIF (MAF_USE_CURL)

# Create the library.
add_library(${PROJECT_NAME} ${PROJECT_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_SOURCE_DIR}/Common)
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_SOURCE_DIR}/VME)
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})
