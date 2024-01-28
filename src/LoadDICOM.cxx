//
// This example demonstrates how to read a series of dicom images
// and how to scroll with the mousewheel or the up/down keys
// through all slices
//

#include <algorithm>
// some standard vtk headers
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
// headers needed for this example
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleImage.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
// needed to easily convert int to std::string
#include <sstream>

// itk-vtk
#include <itkImage.h>
#include <itkImageFileReader.h>

#include <itkImageToVTKImageFilter.h>

#include "vtkVersion.h"
#include "vtkImageViewer.h"
#include "vtkImageMapper3D.h"
#include "vtkImageActor.h"
#include "itkRGBPixel.h"

#include <vtkImageMapper.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkCommand.h>

// reading DICOM in itk
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"


// helper class to format slice status message
class StatusMessage {
public:
   static std::string Format(int slice, int maxSlice) {
      std::stringstream tmp;
      tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
      return tmp.str();
   }
};

class vtkAnimation : public vtkCommand
{
public:
    static vtkAnimation *New()
    {
        vtkAnimation *cb = new vtkAnimation;
        cb->slice = 0;
        return cb;
    }
    
    virtual void Execute(vtkObject *caller, unsigned long eventId,
                         void * vtkNotUsed(callData))
    {
        if (vtkCommand::TimerEvent == eventId)
        {
            ++this->slice;
        }
        
        if(this->slice > this->max){
            this->slice = min;
        }
        mapper->SetSliceNumber(this->slice);
        vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
        iren->GetRenderWindow()->Render();
    }
    
    virtual void SetMapper(vtkImageSliceMapper *mapper){
        this->mapper = mapper;
        this->min = mapper->GetSliceNumberMinValue();
        this->max = mapper->GetSliceNumberMaxValue();
        this->slice = this->min;
    }
    
private:
    int slice, max, min;
public:
    vtkImageSliceMapper* mapper;
};

int main(int argc, char* argv[])
{
    // Constants and type definitions
    typedef signed short PixelType;
    const unsigned int Dimension = 3;
    typedef itk::Image<PixelType, Dimension> ImageType;
    typedef itk::ImageSeriesReader< ImageType > ReaderType;
    typedef itk::ImageToVTKImageFilter<ImageType>       ConnectorType;
    typedef itk::GDCMImageIO ImageIOType;
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    typedef std::vector< std::string > FileNameList;
    
   // Verify input arguments
   if ( argc != 2 )
   {
      std::cout << "Usage: " << argv[0]
      << " FolderName" << std::endl;
      return EXIT_FAILURE;
   }

    // pull path from command line args for now
   std::string folder = argv[1];
    
    ReaderType::Pointer reader = ReaderType::New();
    ConnectorType::Pointer connector = ConnectorType::New();
    
    // read data in as DICOM image using GDCMImageIO class
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    reader->SetImageIO( dicomIO );
    
    // generate file names for DICOM image
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetDirectory(folder);
    nameGenerator->SetUseSeriesDetails( true );
    //nameGenerator->AddSeriesRestriction("0008|0021" );    // filter by additional requirements
    const FileNameList & seriesUID = nameGenerator->GetSeriesUIDs();
    
    // print out series information to help with debugging
    FileNameList::const_iterator seriesItr = seriesUID.begin();
    FileNameList::const_iterator seriesEnd = seriesUID.end();
    while( seriesItr != seriesEnd )
    {
        std::cout << "Found series " << seriesItr->c_str() << " containing "
            << nameGenerator->GetFileNames( seriesItr->c_str() ).size() << " files" << std::endl;
        ++seriesItr;
    }
    
    // for now, visualize the first series found in the input folder
    std::string seriesIdentifier = seriesUID.begin()->c_str();
    FileNameList fileNames = nameGenerator->GetFileNames( seriesIdentifier );
    
    // read image from list of filenames as an itk Image object
    // convert itk image to a vtkImageData object with connector
    reader->SetFileNames( fileNames );
    try
    {
        reader->Update();
        connector->SetInput(reader->GetOutput());
        connector->Update();
    }
    catch (itk::ExceptionObject &ex)
    {
        std::cout << ex << std::endl;
        return EXIT_FAILURE;
    }
    
    cout << "We've got a " << connector->GetOutput()->GetDataDimension() << "d vtk image" << endl;

   // Visualize using VTK
    // slice of 3d image
    
    vtkSmartPointer<vtkImageSliceMapper> imageSliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
    imageSliceMapper->SetInputData(connector->GetOutput());
    imageSliceMapper->SetSliceNumber(0);
    vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
    imageSlice->SetMapper(imageSliceMapper);
    
    // Setup renderer
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddViewProp(imageSlice);
    renderer->ResetCamera();
    
    // Setup render window containing vtk visualization (no other GUI elements for now)
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(600, 600);
    renderWindow->AddRenderer(renderer);
    
    // Setup render window interactor (defines how user interacts with visualization)
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkInteractorStyleImage> interactorStyle =
    vtkSmartPointer<vtkInteractorStyleImage>::New();
    renderWindowInteractor->SetInteractorStyle(interactorStyle);
    
    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    renderWindowInteractor->Initialize();
    
    // Set up scrolling animation
    vtkSmartPointer<vtkAnimation> cb =
    vtkSmartPointer<vtkAnimation>::New();
    cb->SetMapper(imageSliceMapper);
    renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cb);
    renderWindowInteractor->CreateRepeatingTimer(50);
    
    renderWindowInteractor->Start();

   return EXIT_SUCCESS;
}
