// some standard vtk headers
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleImage.h>
#include <sstream>

#include "vtkVersion.h"

#include <vtkImageMapper.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkCommand.h>
#include <vtkCamera.h>

#include <vtkTextActor.h>
#include <vtkTextProperty.h>

// option axis parameter slices on x or y axis
cineView::cineView(vtkImageData *data, int axis = 2)
{
this->data = data;

this->mapper = vtkSmartPointer<vtkImageSliceMapper>::New();
this->mapper->SetInputData(data);
this->mapper->SetSliceNumber(this->mapper->GetSliceNumberMinValue());
this->mapper->SetOrientation(axis);

this->imageSlice = vtkSmartPointer<vtkImageSlice>::New();
this->imageSlice->SetMapper(this->mapper);
this->renderer = vtkSmartPointer<vtkRenderer>::New();
this->renderer->Clear();    // clear to background image
this->renderer->AddViewProp(this->imageSlice);

vtkCamera *camera = this->renderer->GetActiveCamera();
double p[3] = {0.,0.,0.};
p[axis]=1;
camera->SetPosition(p);

// need a slight camera roll to maintain image orientation for axis 0
if(axis==0){
    camera->Roll(-90);
}
this->renderer->ResetCamera();
}

vtkSmartPointer<vtkRenderer> cineView::getRenderer(){
return this->renderer;
}

void cineView::addAnimationObserver(vtkSmartPointer<vtkRenderWindowInteractor> interactor){
interactor->Initialize();
// Set up scrolling animation
vtkSmartPointer<cineView::vtkAnimation> cb =
vtkSmartPointer<cineView::vtkAnimation>::New();
cb->SetMapper(this->mapper);
interactor->AddObserver(vtkCommand::TimerEvent, cb);
interactor->CreateRepeatingTimer(50);
}

cineView::vtkAnimation* cineView::vtkAnimation::New()
{
    cineView::vtkAnimation *cb = new cineView::vtkAnimation;
    cb->slice = 0;
    cb->delta=1;
    return cb;
}

void cineView::vtkAnimation::Execute(vtkObject *caller, unsigned long eventId,
                     void * vtkNotUsed(callData))
{
    if (vtkCommand::TimerEvent == eventId)
    {
        this->slice += delta;
    }
    
    if(this->slice==this->min || this->slice==this->max) delta=-delta;
    
    mapper->SetSliceNumber(this->slice);
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
    iren->GetRenderWindow()->Render();
}

void cineView::vtkAnimation::SetMapper(vtkImageSliceMapper *mapper){
    this->mapper = mapper;
    this->min = mapper->GetSliceNumberMinValue();
    this->max = mapper->GetSliceNumberMaxValue();
    this->slice = this->min;
}
