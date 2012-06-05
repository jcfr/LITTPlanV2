/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerLITTPlanV2ModuleWidget_h
#define __qSlicerLITTPlanV2ModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

// LITTPlanV2 includes
#include "qSlicerLITTPlanV2ModuleExport.h"

class vtkMatrix4x4;
class vtkMRMLNode;
class qSlicerLITTPlanV2ModuleWidgetPrivate;

class Q_SLICER_QTMODULES_TRANSFORMS_EXPORT qSlicerLITTPlanV2ModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerLITTPlanV2ModuleWidget(QWidget *parent=0);
  virtual ~qSlicerLITTPlanV2ModuleWidget();

  /// Reimplemented for internal reasons
  void setMRMLScene(vtkMRMLScene* scene);

public slots:
  /// Set the matrix to identity, the sliders are reset to the position 0
  void identity();

  /// Invert the matrix. The sliders are reset to the position 0.
  void invert();

protected:
  virtual void setup();

protected slots:
  void onCoordinateReferenceButtonPressed(int id);
  void onNodeSelected(vtkMRMLNode* node);
  void onTranslationRangeChanged(double newMin, double newMax);

  void transformSelectedNodes();
  void untransformSelectedNodes();
  /// 
  /// Triggered upon MRML transform node updates
  void onMRMLTransformNodeModified(vtkObject* caller);

protected:
  /// 
  /// Fill the 'minmax' array with the min/max translation value of the matrix.
  /// Parameter expand allows to specify (using a value between 0 and 1)
  /// which percentage of the found min/max value should be substracted/added
  /// to the min/max value found.
  void extractMinMaxTranslationValue(vtkMatrix4x4 * mat, double& min, double& max);

  /// 
  /// Convenient method to return the coordinate system currently selected
  int coordinateReference()const;

protected:
  QScopedPointer<qSlicerLITTPlanV2ModuleWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerLITTPlanV2ModuleWidget);
  Q_DISABLE_COPY(qSlicerLITTPlanV2ModuleWidget);
};

#endif
