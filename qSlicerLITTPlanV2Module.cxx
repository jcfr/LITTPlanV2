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
#include <QtPlugin>

// SlicerQt includes
#include "qSlicerApplication.h"
#include "qSlicerCoreIOManager.h"
#include "vtkSlicerTransformLogic.h"

// LITTPlanV2 includes
#include "qSlicerLITTPlanV2IO.h"
#include "qSlicerLITTPlanV2Module.h"
#include "qSlicerLITTPlanV2ModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerLITTPlanV2Module, qSlicerLITTPlanV2Module);


//-----------------------------------------------------------------------------
class qSlicerLITTPlanV2ModulePrivate
{
public:
};

//-----------------------------------------------------------------------------
qSlicerLITTPlanV2Module::qSlicerLITTPlanV2Module(QObject* _parentObject)
  : Superclass(_parentObject)
  , d_ptr(new qSlicerLITTPlanV2ModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerLITTPlanV2Module::~qSlicerLITTPlanV2Module()
{
}

//-----------------------------------------------------------------------------
QIcon qSlicerLITTPlanV2Module::icon()const
{
  return QIcon(":/Icons/LITTPlanV2.png");
}


//-----------------------------------------------------------------------------
QStringList qSlicerLITTPlanV2Module::categories() const
{
  return QStringList() << "" << "Registration";
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerLITTPlanV2Module::createWidgetRepresentation()
{
  return new qSlicerLITTPlanV2ModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerLITTPlanV2Module::createLogic()
{
  return vtkSlicerTransformLogic::New();
}

//-----------------------------------------------------------------------------
QString qSlicerLITTPlanV2Module::helpText()const
{
  QString help =
    "The LITTPlanV2 Module creates and edits transforms.<br>"
    "<a href=%1/Documentation/%2.%3/Modules/LITTPlanV2>%1/Documentation/%2.%3/Modules/LITTPlanV2</a>";
  return help.arg(this->slicerWikiUrl()).arg(Slicer_VERSION_MAJOR).arg(Slicer_VERSION_MINOR);
}

//-----------------------------------------------------------------------------
QString qSlicerLITTPlanV2Module::acknowledgementText()const
{
  QString acknowledgement =
    "This work was supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer "
    "Community.<br>"
    "See <a href=\"http://www.slicer.org\">www.slicer.org</a> for details.<br>"
    "The LITTPlanV2 module was contributed by Alex Yarmarkovich, Isomics Inc. "
    "with help from others at SPL, BWH (Ron Kikinis) and Kitware Inc.";
  return acknowledgement;
}

//-----------------------------------------------------------------------------
QStringList qSlicerLITTPlanV2Module::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Alex Yarmarkovich (Isomics)");
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  moduleContributors << QString("Julien Finet (Kitware)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
void qSlicerLITTPlanV2Module::setup()
{
  qSlicerApplication * app = qSlicerApplication::application();
  if (!app)
    {
    return;
    }
  vtkSlicerTransformLogic* transformLogic =
    vtkSlicerTransformLogic::SafeDownCast(this->logic());
  app->coreIOManager()->registerIO(new qSlicerLITTPlanV2IO(transformLogic, this));
}
