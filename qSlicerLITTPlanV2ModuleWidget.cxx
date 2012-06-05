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

// Qt includes
#include <QFileDialog>

// SlicerQt includes
#include "qSlicerLITTPlanV2ModuleWidget.h"
#include "ui_qSlicerLITTPlanV2Module.h"
//#include "qSlicerApplication.h"
//#include "qSlicerIOManager.h"

// vtkSlicerLogic includes
#include "vtkSlicerTransformLogic.h"

// MRMLWidgets includes
#include <qMRMLUtils.h>

// MRML includes
#include "vtkMRMLLinearTransformNode.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

//-----------------------------------------------------------------------------
class qSlicerLITTPlanV2ModuleWidgetPrivate: public Ui_qSlicerLITTPlanV2Module
{
  Q_DECLARE_PUBLIC(qSlicerLITTPlanV2ModuleWidget);
protected:
  qSlicerLITTPlanV2ModuleWidget* const q_ptr;
public:
  qSlicerLITTPlanV2ModuleWidgetPrivate(qSlicerLITTPlanV2ModuleWidget& object);
  vtkSlicerTransformLogic*      logic()const;
  QButtonGroup*                 CoordinateReferenceButtonGroup;
  vtkMRMLLinearTransformNode*   MRMLTransformNode;
};

//-----------------------------------------------------------------------------
qSlicerLITTPlanV2ModuleWidgetPrivate::qSlicerLITTPlanV2ModuleWidgetPrivate(qSlicerLITTPlanV2ModuleWidget& object)
  : q_ptr(&object)
{
  this->CoordinateReferenceButtonGroup = 0;
  this->MRMLTransformNode = 0;
}
//-----------------------------------------------------------------------------
vtkSlicerTransformLogic* qSlicerLITTPlanV2ModuleWidgetPrivate::logic()const
{
  Q_Q(const qSlicerLITTPlanV2ModuleWidget);
  return vtkSlicerTransformLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
qSlicerLITTPlanV2ModuleWidget::qSlicerLITTPlanV2ModuleWidget(QWidget* _parentWidget)
  : Superclass(_parentWidget)
  , d_ptr(new qSlicerLITTPlanV2ModuleWidgetPrivate(*this))
{
}

//-----------------------------------------------------------------------------
qSlicerLITTPlanV2ModuleWidget::~qSlicerLITTPlanV2ModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::setup()
{
  Q_D(qSlicerLITTPlanV2ModuleWidget);
  d->setupUi(this);

  // Add coordinate reference button to a button group
  d->CoordinateReferenceButtonGroup =
    new QButtonGroup(d->CoordinateReferenceGroupBox);
  d->CoordinateReferenceButtonGroup->addButton(
    d->GlobalRadioButton, qMRMLTransformSliders::GLOBAL);
  d->CoordinateReferenceButtonGroup->addButton(
    d->LocalRadioButton, qMRMLTransformSliders::LOCAL);

  // Connect button group
  this->connect(d->CoordinateReferenceButtonGroup,
                SIGNAL(buttonPressed(int)),
                SLOT(onCoordinateReferenceButtonPressed(int)));

  // Connect identity button
  this->connect(d->IdentityPushButton,
                SIGNAL(clicked()),
                SLOT(identity()));

  // Connect revert button
  this->connect(d->InvertPushButton,
                SIGNAL(clicked()),
                SLOT(invert()));

  // Connect node selector with module itself
  this->connect(d->TransformNodeSelector,
                SIGNAL(currentNodeChanged(vtkMRMLNode*)),
                SLOT(onNodeSelected(vtkMRMLNode*)));

  // Connect minimum and maximum from the translation sliders to the matrix
  this->connect(d->TranslationSliders,
               SIGNAL(rangeChanged(double,double)),
               SLOT(onTranslationRangeChanged(double,double)));

  // Notify the matrix of the current translation min/max values
  this->onTranslationRangeChanged(d->TranslationSliders->minimum(),
                                  d->TranslationSliders->maximum());

  // Transform nodes connection
  this->connect(d->TransformToolButton, SIGNAL(clicked()),
                SLOT(transformSelectedNodes()));
  this->connect(d->UntransformToolButton, SIGNAL(clicked()),
                SLOT(untransformSelectedNodes()));

  // Icons
  QIcon rightIcon =
    QApplication::style()->standardIcon(QStyle::SP_ArrowRight);
  d->TransformToolButton->setIcon(rightIcon);

  QIcon leftIcon =
    QApplication::style()->standardIcon(QStyle::SP_ArrowLeft);
  d->UntransformToolButton->setIcon(leftIcon);

  this->onNodeSelected(0);
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::onCoordinateReferenceButtonPressed(int id)
{
  Q_D(qSlicerLITTPlanV2ModuleWidget);
  
  qMRMLTransformSliders::CoordinateReferenceType ref =
    (id == qMRMLTransformSliders::GLOBAL) ? qMRMLTransformSliders::GLOBAL : qMRMLTransformSliders::LOCAL;
  d->TranslationSliders->setCoordinateReference(ref);
  d->RotationSliders->setCoordinateReference(ref);
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::onNodeSelected(vtkMRMLNode* node)
{
  Q_D(qSlicerLITTPlanV2ModuleWidget);
  
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(node);

  // Enable/Disable CoordinateReference, identity buttons, MatrixViewGroupBox,
  // Min/Max translation inputs
  d->CoordinateReferenceGroupBox->setEnabled(transformNode != 0);
  d->IdentityPushButton->setEnabled(transformNode != 0);
  d->InvertPushButton->setEnabled(transformNode != 0);
  d->MatrixViewGroupBox->setEnabled(transformNode != 0);

  // Listen for Transform node changes
  this->qvtkReconnect(d->MRMLTransformNode, transformNode,
    vtkMRMLTransformableNode::TransformModifiedEvent,
    this, SLOT(onMRMLTransformNodeModified(vtkObject*)));

  QStringList nodeTypes;
  // If no transform node, it would show the entire scene, lets shown none
  // instead.
  if (transformNode == 0)
    {
    nodeTypes << QString("vtkMRMLNotANode");
    }
  d->TransformedTreeView->setNodeTypes(nodeTypes);

  // Filter the current node in the transformed tree view
  d->TransformedTreeView->setRootNode(transformNode);

  // Hide the current node in the transformable tree view
  QStringList hiddenNodeIDs;
  if (transformNode)
    {
    hiddenNodeIDs << QString(transformNode->GetID());
    }
  d->TransformableTreeView->sortFilterProxyModel()
    ->setHiddenNodeIDs(hiddenNodeIDs);
  d->MRMLTransformNode = transformNode;
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::identity()
{
  Q_D(qSlicerLITTPlanV2ModuleWidget);

  if (!d->MRMLTransformNode)
    {
    return;
    }

  d->RotationSliders->resetUnactiveSliders();
  d->MRMLTransformNode->GetMatrixTransformToParent()->Identity();
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::invert()
{
  Q_D(qSlicerLITTPlanV2ModuleWidget);
  
  if (!d->MRMLTransformNode) { return; }

  d->RotationSliders->resetUnactiveSliders();
  d->MRMLTransformNode->GetMatrixTransformToParent()->Invert();
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::onMRMLTransformNodeModified(vtkObject* caller)
{
  Q_D(qSlicerLITTPlanV2ModuleWidget);
  
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast(caller);
  if (!transformNode) { return; }

  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  qMRMLUtils::getTransformInCoordinateSystem(d->MRMLTransformNode,
    this->coordinateReference() == qMRMLTransformSliders::GLOBAL, transform);

  // The matrix can be changed externally. The min/max values shall be updated 
  //accordingly to the new matrix if needed.
  vtkMatrix4x4 * mat = transform->GetMatrix();
  double min = 0.;
  double max = 0.;
  this->extractMinMaxTranslationValue(mat, min, max);
  if (min < d->TranslationSliders->minimum())
    {
    min = min - 0.3 * fabs(min);
    d->TranslationSliders->setMinimum(min);
    }
  if (max > d->TranslationSliders->maximum())
    {
    max = max + 0.3 * fabs(max);
    d->TranslationSliders->setMaximum(max);
    }
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::extractMinMaxTranslationValue(
  vtkMatrix4x4 * mat, double& min, double& max)
{
  if (!mat)
    {
    Q_ASSERT(mat);
    return;
    }
  for (int i=0; i <3; i++)
    {
    min = qMin(min, mat->GetElement(i,3));
    max = qMax(max, mat->GetElement(i,3));
    }
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::onTranslationRangeChanged(double newMin,
                                                              double newMax)
{
  Q_D(qSlicerLITTPlanV2ModuleWidget);
  d->MatrixWidget->setRange(newMin, newMax);
}

//-----------------------------------------------------------------------------
int qSlicerLITTPlanV2ModuleWidget::coordinateReference()const
{
  Q_D(const qSlicerLITTPlanV2ModuleWidget);
  return d->CoordinateReferenceButtonGroup->checkedId();
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerLITTPlanV2ModuleWidget);
  this->Superclass::setMRMLScene(scene);
  // If the root index is set before the scene, it will show the scene as
  // top-level item. Setting the root index to be the scene makes the nodes
  // top-level, and this can only be done once the scene is set.
  d->TransformableTreeView->setRootIndex(
    d->TransformableTreeView->sortFilterProxyModel()->mrmlSceneIndex());
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::transformSelectedNodes()
{
  Q_D(qSlicerLITTPlanV2ModuleWidget);
  QModelIndexList selectedIndexes =
    d->TransformableTreeView->selectionModel()->selectedRows();
  selectedIndexes = qMRMLTreeView::removeChildren(selectedIndexes);
  foreach(QModelIndex selectedIndex, selectedIndexes)
    {
    vtkMRMLTransformableNode* node = vtkMRMLTransformableNode::SafeDownCast(
    d->TransformableTreeView->sortFilterProxyModel()->
      mrmlNodeFromIndex( selectedIndex ));
    Q_ASSERT(node);
    node->SetAndObserveTransformNodeID(d->MRMLTransformNode->GetID());
    }
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2ModuleWidget::untransformSelectedNodes()
{
  Q_D(qSlicerLITTPlanV2ModuleWidget);
  QModelIndexList selectedIndexes =
    d->TransformedTreeView->selectionModel()->selectedRows();
  selectedIndexes = qMRMLTreeView::removeChildren(selectedIndexes);
  foreach(QModelIndex selectedIndex, selectedIndexes)
    {
    vtkMRMLTransformableNode* node = vtkMRMLTransformableNode::SafeDownCast(
    d->TransformedTreeView->sortFilterProxyModel()->
      mrmlNodeFromIndex( selectedIndex ));
    Q_ASSERT(node);
    node->SetAndObserveTransformNodeID(0);
    }
}
