/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: TestSliderWidget2D.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// This example tests the vtkSliderWidget with a 2D representation.

// First include the required header files for the VTK classes we are using.
#include "vtkSliderWidget.h"
#include "vtkSliderRepresentation2D.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkAppendPolyData.h"
#include "vtkClipPolyData.h"
#include "vtkConeSource.h"
#include "vtkGlyph3D.h"
#include "vtkActor.h"
#include "vtkLODActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCommand.h"
#include "vtkSphere.h"
#include "vtkInteractorEventRecorder.h"
#include "vtkRegressionTestImage.h"
#include "vtkDebugLeaks.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkWidgetEvent.h"

// This does the actual work: updates the probe.
// Callback for the interaction
class vtkSlider2DCallback : public vtkCommand
{
public:
  static vtkSlider2DCallback *New() 
    { return new vtkSlider2DCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      vtkSliderWidget *sliderWidget = 
        reinterpret_cast<vtkSliderWidget*>(caller);
      this->Glyph->SetScaleFactor(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
    }
  vtkSlider2DCallback():Glyph(0) {}
  vtkGlyph3D *Glyph;
};

int TestSliderWidget2D( int argc, char *argv[] )
{
  // Create a mace out of filters.
  //
  vtkSphereSource *sphereSource = vtkSphereSource::New();
  vtkConeSource *cone = vtkConeSource::New();
  vtkGlyph3D *glyph = vtkGlyph3D::New();
  glyph->SetInput(sphereSource->GetOutput());
  glyph->SetSource(cone->GetOutput());
  glyph->SetVectorModeToUseNormal();
  glyph->SetScaleModeToScaleByVector();
  glyph->SetScaleFactor(0.25);

  // The sphere and spikes are appended into a single polydata. 
  // This just makes things simpler to manage.
  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(glyph->GetOutput());
  apd->AddInput(sphereSource->GetOutput());

  vtkPolyDataMapper *maceMapper = vtkPolyDataMapper::New();
  maceMapper->SetInput(apd->GetOutput());

  vtkLODActor *maceActor = vtkLODActor::New();
  maceActor->SetMapper(maceMapper);
  maceActor->VisibilityOn();
  maceActor->SetPosition(1,1,1);

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkRenderer *ren1 = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(ren1);

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);

  // VTK widgets consist of two parts: the widget part that handles event processing;
  // and the widget representation that defines how the widget appears in the scene 
  // (i.e., matters pertaining to geometry).
  vtkSliderRepresentation2D *sliderRep = vtkSliderRepresentation2D::New();
  sliderRep->SetValue(0.25);
  sliderRep->SetTitleText("Spike Size");
  sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  sliderRep->GetPoint1Coordinate()->SetValue(0.2,0.1);
  sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  sliderRep->GetPoint2Coordinate()->SetValue(0.8,0.1);
  sliderRep->SetSliderLength(0.02);
  sliderRep->SetSliderWidth(0.03);
  sliderRep->SetEndCapLength(0.01);
  sliderRep->SetEndCapWidth(0.03);
  sliderRep->SetTubeWidth(0.005);

  vtkSliderWidget *sliderWidget = vtkSliderWidget::New();
  sliderWidget->SetInteractor(iren);
  sliderWidget->SetRepresentation(sliderRep);
  sliderWidget->SetAnimationModeToAnimate();

  vtkSlider2DCallback *callback = vtkSlider2DCallback::New();
  callback->Glyph = glyph;
  sliderWidget->AddObserver(vtkCommand::InteractionEvent,callback);

  ren1->AddActor(maceActor);

  // Add the actors to the renderer, set the background and size
  //
  ren1->SetBackground(0.1, 0.2, 0.4);
  renWin->SetSize(300, 300);

  // record events
  vtkInteractorEventRecorder *recorder = vtkInteractorEventRecorder::New();
  recorder->SetInteractor(iren);
  recorder->SetFileName("c:/record.log");
//  recorder->Record();
//  recorder->ReadFromInputStringOn();
//  recorder->SetInputString(eventLog);

  // render the image
  //
  iren->Initialize();
  renWin->Render();
//  recorder->Play();

  // Remove the observers so we can go interactive. Without this the "-I"
  // testing option fails.
  recorder->Off();

  int retVal = vtkRegressionTestImage( renWin );
  if ( retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    iren->Start();
    }

  sphereSource->Delete();
  cone->Delete();
  glyph->Delete();
  apd->Delete();
  maceMapper->Delete();
  maceActor->Delete();
  sliderWidget->RemoveObserver(callback);
  sliderWidget->Off();
  sliderWidget->Delete();
  sliderRep->Delete();
  callback->Delete();
  iren->Delete();
  renWin->Delete();
  ren1->Delete();
  recorder->Delete();

  return !retVal;

}


