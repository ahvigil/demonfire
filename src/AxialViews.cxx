
#include "DemonFire.h"
#include "CineView.cxx"

using namespace fire;


// helper class to format slice status message
class StatusMessage {
public:
   static std::string Format(int slice, int maxSlice) {
      std::stringstream tmp;
      tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
      return tmp.str();
   }
};

int main(int argc, char* argv[])
{
    
   // Verify input arguments
   if ( argc != 2 )
   {
      std::cout << "Usage: " << argv[0]
      << " FolderName" << std::endl;
      return EXIT_FAILURE;
   }    
    ConnectorType::Pointer connector = ConnectorType::New();

    ImageType::Pointer image = loadDICOM(argv[1]);
    try
    {
        connector->SetInput(image);
        connector->Update();
    }
    catch (itk::ExceptionObject &ex)
    {
        std::cout << ex << std::endl;
        return EXIT_FAILURE;
    }
    
    cout << "We've got a " << connector->GetOutput()->GetDataDimension() << "d vtk image" << endl;

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(900, 600);
    
    // Setup render window interactor (defines how user interacts with visualization)
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkInteractorStyleImage> interactorStyle =
    vtkSmartPointer<vtkInteractorStyleImage>::New();
    renderWindowInteractor->SetInteractorStyle(interactorStyle);
    
    renderWindowInteractor->SetRenderWindow(renderWindow);
    
    
    // Define viewport ranges
    double xmins[3] = {0,.35,.7};
    double xmaxs[3] = {0.3,.65,1.};
    for(int i = 0; i < 3; i++)
    {
        cineView *cr = new cineView(connector->GetOutput(), i);
        
        renderWindow->AddRenderer(cr->getRenderer());
        cr->getRenderer()->SetViewport(xmins[i],0.0,xmaxs[i],1.0);
        cr->addAnimationObserver(renderWindowInteractor);
    }
    
    // Render and start interaction
    renderWindow->Render();
    renderWindow->SetWindowName("Axial Views");
    renderWindowInteractor->Start();

   return EXIT_SUCCESS;
}
