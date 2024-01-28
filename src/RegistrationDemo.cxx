#include "DemonFire.h"
#include "CineView.cxx"
using namespace fire;

int main(int argc, char **argv){
    if(argc<3){
        std::cout << "Usage: " << argv[0];
        std::cout << " FixedFolder MovingFolder" << endl;
        return EXIT_FAILURE;
    }
    

    ImageType::Pointer image[2] = {loadDICOM(argv[1]), loadDICOM(argv[2])};
    
    printf("Performing registration\n");
    TransformType::ConstPointer transform = doRegistration(image[0], image[1]);


    ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetInput(image[0]);
    resampler->SetTransform(transform);

    resampler->SetSize(image[0]->GetLargestPossibleRegion().GetSize());
    resampler->SetOutputOrigin(image[0]->GetOrigin());
    resampler->SetOutputSpacing(image[0]->GetSpacing());
    resampler->SetOutputDirection(image[0]->GetDirection());
    resampler->SetDefaultPixelValue(0);
    resampler->Update();
    //saveDICOM(resampler->GetOutput(), "registration_result");
    
    // visualize registration results with subtraction filter
    AbsoluteValueFilterType::Pointer subtract = AbsoluteValueFilterType::New();
    subtract->SetInput1(image[0]);
    subtract->SetInput2(resampler->GetOutput());
    
    // visualize registration results with checkerboard
    CheckerBoardFilterType::Pointer checkerboard = CheckerBoardFilterType::New();
    checkerboard->SetInput1(image[0]);
    checkerboard->SetInput2(resampler->GetOutput());
    // since we are viewing 2d slices, only checkerboard in 2 dimensions
    itk::FixedArray<double, 3> checker;
    checker[0]=4;
    checker[1]=4;
    checker[2]=1;   // no checkerboard patten on z axis
    checkerboard->SetCheckerPattern(checker);

    ConnectorType::Pointer connector[] = {
                            ConnectorType::New(),
                            ConnectorType::New(),
                           };
    connector[0]->SetInput(subtract->GetOutput());
    connector[1]->SetInput(checkerboard->GetOutput());
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
