#include "DemonFire.h"


namespace fire {
  ImageType::Pointer doSegmentation (ImageType::Pointer input, int xSeed, int ySeed, int zSeed, int lowerTH, int upperTH){
    //the region growing filter type
    typedef itk::ConnectedThresholdImageFilter<ImageType, ImageType > ConnectedFilterType;
    
    //the region growing filter
    ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
    CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New(); //works
    
    int radius = 3;
    ImageType::Pointer image;
    
    
    smoothing->SetInput(input); //new
    //connectedThreshold->SetInput(input); //works
    connectedThreshold->SetInput(smoothing->GetOutput()); //works
    
    //the uppper and lower thresholds and the seeds are hardcoded for now
    const PixelType lowerThreshold = lowerTH; //5
    const PixelType upperThreshold = upperTH; //80
    connectedThreshold->SetLower(lowerThreshold);
    connectedThreshold->SetUpper(upperThreshold);
    
    ImageType::IndexType index; //seed
    index[0] = xSeed;
    index[1] = ySeed;
    index[2] = zSeed;
    connectedThreshold->SetSeed(index);
    connectedThreshold->SetReplaceValue(255);
    
    try {
      connectedThreshold->Update();
      image = (doOpening(connectedThreshold->GetOutput(), radius));
      std::cout << "Result of doSegmentation: ";
      //connectedThreshold->Print(std::cout, 0);
      std::cout << std::endl;
    } catch (itk::ExceptionObject & e ) {
      std::cerr << "Exception caught in doSegmentation()!" << std::endl;
      std::cerr << e << std::endl;
    }
    
    //Labeling each distinct object in the binary image with a unique label
    ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New();
    connected->SetInput(image);
    connected->Update();
    
    std::cout << "Number of objects: " << connected->GetObjectCount() << std::endl;
    
    //Applying a colormap to the labeled image
    //RGBFilterType::Pointer rgbFilter = RGBFilterType::New();
    //rgbFilter->SetInput(connected->GetOutput());
    
    //TODO: do stuff with labeled objects
    
    return image; //connectedThreshold->GetOutput();
  }
}