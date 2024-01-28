#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
// unix headers
#include <unistd.h>
#include <dirent.h>
#include <ftw.h>

#include <algorithm>
// some standard vtk headers
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
// headers needed for some examples, clean up if unused later
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleImage.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkSmartVolumeMapper.h>

// needed to easily convert int to std::string
#include <sstream>

// itk-vtk
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageToVTKImageFilter.h>

#include <vtkVersion.h>
#include <vtkImageViewer.h>
#include <vtkImageMapper3D.h>
#include <vtkImageActor.h>
#include <itkRGBPixel.h>

#include <vtkImageMapper.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkCommand.h>

// itk image filters
#include <itkStatisticsImageFilter.h>
#include <itkIntensityWindowingImageFilter.h>
#include <itkAddImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkShiftScaleImageFilter.h>
#include <itkCastImageFilter.h>
#include "itkAdaptiveHistogramEqualizationImageFilter.h"
#include <itkRescaleIntensityImageFilter.h>
#include "itkOtsuThresholdImageFilter.h"
#include <itkMaskImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include "itkGrayscaleErodeImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include <itkCheckerBoardImageFilter.h>
#include <itkSubtractImageFilter.h>
#include <itkAbsoluteValueDifferenceImageFilter.h>
#include <itkNormalizeImageFilter.h>
#include "itkLabelToRGBImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"

// reading/writing DICOM in itk
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkNumericSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <itkImageSeriesWriter.h>

// needed for basic image registration
#include <itkImageRegistrationMethodv4.h>
#include <itkTranslationTransform.h>
#include <itkMeanSquaresImageToImageMetricv4.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkAffineTransform.h>
#include <itkMattesMutualInformationImageToImageMetricv4.h>
#include <itkAffineTransform.h>
#include <itkCompositeTransform.h>

#include <itkCommand.h>

typedef float PixelType;
const unsigned int Dimension = 3;
typedef itk::Image<PixelType, Dimension> ImageType;
typedef itk::Image<PixelType, Dimension-1> OutputImageType;
typedef itk::ImageSeriesReader< ImageType > ReaderType;
typedef itk::ImageSeriesWriter<ImageType, OutputImageType >  WriterType;
typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;
typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames NamesGeneratorType;
typedef itk::NumericSeriesFileNames NumericNamesGeneratorType;
typedef std::vector< std::string > FileNameList;
typedef itk::CheckerBoardImageFilter< ImageType > CheckerBoardFilterType;
typedef itk::SubtractImageFilter <ImageType, ImageType > SubtractImageFilterType;
typedef itk::AbsoluteValueDifferenceImageFilter <ImageType, ImageType, ImageType> AbsoluteValueFilterType;
typedef itk::NormalizeImageFilter <ImageType, ImageType> NormalizeFilterType;

//segmentation/quantification typedefs
typedef itk::RGBPixel<float> RGBPixelType;
typedef itk::Image<RGBPixelType, Dimension> RGBImageType;
typedef itk::CurvatureFlowImageFilter<ImageType, ImageType > CurvatureFlowImageFilterType;
typedef itk::Image<unsigned short, Dimension> CCImageType;
typedef itk::ConnectedComponentImageFilter<ImageType, CCImageType> ConnectedComponentImageFilterType;
typedef itk::LabelToRGBImageFilter<CCImageType, RGBImageType> RGBFilterType;

// registration typedefs
typedef itk::ShrinkImageFilter<ImageType, ImageType> ShrinkFilter;
typedef itk::RegularStepGradientDescentOptimizerv4<double> OptimizerType;
typedef itk::MeanSquaresImageToImageMetricv4<ImageType, ImageType> FirstPassMetricType;
typedef itk::MattesMutualInformationImageToImageMetricv4<ImageType, ImageType> SecondPassMetricType;
typedef itk::TranslationTransform< double, Dimension > TransformType;
typedef itk::AffineTransform< double, Dimension > AffineTransformType;
typedef itk::ImageRegistrationMethodv4<ImageType, ImageType, TransformType> RegistrationType;
typedef itk::CompositeTransform<double, Dimension> CompositeTransformType;
typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;

namespace fire {
    // process
    ImageType::Pointer doPreProcessing(ImageType::Pointer);
    TransformType::ConstPointer doRegistration(ImageType::Pointer, ImageType::Pointer);
    ImageType::Pointer doSegmentation(ImageType::Pointer, int xSeed, int ySeed, int zSeed, int lowerTH, int upperTH);
    
    // basic image filters
    ImageType::Pointer doSmoothing(ImageType::Pointer);
    void doSmoothing(ImageType::Pointer*);
    ImageType::Pointer doSharpening(ImageType::Pointer);
    void doSharpening(ImageType::Pointer*);
    ImageType::Pointer doIntensityWindowing(ImageType::Pointer input);
    void doIntensityWindowing(ImageType::Pointer*);
    ImageType::Pointer doBinaryThresholding(ImageType::Pointer, int);
    void doBinaryThresholding(ImageType::Pointer*, int);
    ImageType::Pointer doMasking(ImageType::Pointer, ImageType::Pointer);
    void doMask(ImageType::Pointer *input, ImageType::Pointer);
    ImageType::Pointer doHistogramEqualization(ImageType::Pointer);
    void doHistogramEqualization(ImageType::Pointer*);
    ImageType::Pointer doIntensityRescaling(ImageType::Pointer);
    void doIntensityRescaling(ImageType::Pointer*);
    ImageType::Pointer doNormalizing(ImageType::Pointer);
    void doNormalizing(ImageType::Pointer*);
    
    ImageType::Pointer doOtsuThresholding(ImageType::Pointer);
    void doOtsuThresholding(ImageType::Pointer*);
    
    // basic image operations
  ImageType::Pointer doDilation(ImageType::Pointer, int radius);
  //void doDilation(ImageType::Pointer *input);
  ImageType::Pointer doErosion(ImageType::Pointer, int radius);
  //void doErosion(ImageType::Pointer *input);
    ImageType::Pointer doClosing(ImageType::Pointer, int radius);
    //void doClosing(ImageType::Pointer *input);
  ImageType::Pointer doOpening(ImageType::Pointer, int radius);
  //void doOpening(ImageType::Pointer *input);
    ImageType::Pointer doShrink(ImageType::Pointer, int);
    void doShrink(ImageType::Pointer *input, int);
    
    ImageType::Pointer loadDICOM(char*);
    int saveDICOM(ImageType::Pointer, char*);
    std::vector<ImageType::Pointer> loadDICOMs(char* folder);
}

class cineView
{
public:
    // option axis parameter slices on x or y axis
    cineView(vtkImageData *data, int axis);
    void SetMapper(vtkImageSliceMapper *mapper);
    vtkSmartPointer<vtkRenderer> getRenderer();
    void addAnimationObserver(vtkSmartPointer<vtkRenderWindowInteractor> interactor);

    class vtkAnimation : public vtkCommand
    {
    public:
        static vtkAnimation *New();
        virtual void Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData));
        virtual void SetMapper(vtkImageSliceMapper *mapper);
        int slice, max, min, delta;
        vtkImageSliceMapper* mapper;
    };
int slice, max, min, delta;

private:
    vtkImageData *data;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkImageSliceMapper> mapper;
    vtkSmartPointer<vtkImageSlice> imageSlice;

};
