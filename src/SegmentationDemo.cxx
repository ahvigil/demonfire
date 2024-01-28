#include "DemonFire.h"
#include "CineView.cxx"
using namespace fire;

int main(int argc, char **argv){
    if(argc<2){
        std::cout << "Usage: " << argv[0];
        std::cout << " Folder" << endl;
        return EXIT_FAILURE;
    }
    

    ImageType::Pointer image[3] = {loadDICOM(argv[1]), NULL, NULL};
    //image[1] = doSmoothing(image[0]);
    image[2] = image[0];

    //doSegmentation works sort of with SegmentationImageSeries (in segmentaion file in drive)
    //Because the seed is currently hardcoded, it only works with specific series
    int xSeed = 217;
    int ySeed = 224;
    int zSeed = 16;
    int lowerThreshold = 5;
    int upperThreshold = 58;
    image[1] = doSegmentation(image[0], xSeed, ySeed, zSeed, lowerThreshold, upperThreshold);
    //doRegistration(image, image);

    ConnectorType::Pointer connector[] = {
                            ConnectorType::New(),
                            ConnectorType::New(),							
                           };
	
    connector[0]->SetInput(image[0]);
    connector[1]->SetInput(image[1]);

    try {
        connector[0]->Update();
        connector[1]->Update();
    }
    catch (itk::ExceptionObject &ex)
    {
        std::cout << ex << std::endl;
        return EXIT_FAILURE;
    }

    // Setup render window containing vtk visualization (no other GUI elements for now)
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
    double xmins[] = {0,.55};
    double xmaxs[] = {0.45,1.};
    for(int i = 0; i < 2; i++)
    {
        cineView *cr = new cineView(connector[i]->GetOutput());
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
