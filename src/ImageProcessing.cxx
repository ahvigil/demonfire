#include "DemonFire.h"
namespace fire {
    ImageType::Pointer doPreProcessing(ImageType::Pointer input){
        return input;
    }

    // each processing function has two versions:
    // 'doFilter(image)' applies the requested filter and returns the result
    // 'doFilter(&image)' applies the requested filter and replaces the input with the result
    ImageType::Pointer doSmoothing(ImageType::Pointer input){
        typedef itk::DiscreteGaussianImageFilter<
        ImageType, ImageType >  filterType;
        
        filterType::Pointer gaussianFilter = filterType::New();
        gaussianFilter->SetInput( input );
        gaussianFilter->SetVariance(2);
        gaussianFilter->SetMaximumKernelWidth(9);
        gaussianFilter->Update();
        
        return gaussianFilter->GetOutput();
    }

    void doSmoothing(ImageType::Pointer *input){
        *input = doSmoothing(*input);
        return;
    }

    ImageType::Pointer doSharpen(ImageType::Pointer input){
        return NULL;
    }

    void doSharpen(ImageType::Pointer *input){
        *input = doSharpen(*input);
        return;
    }

    ImageType::Pointer doBinaryThresholding(ImageType::Pointer input, int threshold){
        typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
        BinaryThresholdImageFilterType;
        
        BinaryThresholdImageFilterType::Pointer filter
            = BinaryThresholdImageFilterType::New();
        filter->SetInput(input);
        
        return filter->GetOutput();
    }

    void doBinaryThresholding(ImageType::Pointer *input, int threshold){
        *input = doBinaryThresholding(*input, threshold);
        return;
    }

    ImageType::Pointer doMasking(ImageType::Pointer input, ImageType::Pointer mask){
        typedef itk::MaskImageFilter< ImageType, ImageType > MaskFilterType;
        MaskFilterType::Pointer maskFilter = MaskFilterType::New();
        maskFilter->SetInput(input);
        maskFilter->SetMaskImage(mask);
        
        maskFilter->Update();
        return maskFilter->GetOutput();
    }

    void doMasking(ImageType::Pointer *input, ImageType::Pointer mask){
        *input = doMasking(*input, mask);
        return;
    }

  //Erosion
  ImageType::Pointer doErosion(ImageType::Pointer input, int radius){
    
    typedef itk::BinaryBallStructuringElement<PixelType, Dimension>
    StructuringElementType;
    
    StructuringElementType structuringElement;
    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();
    
    typedef itk::GrayscaleErodeImageFilter <ImageType, ImageType, StructuringElementType>
    GrayscaleErodeImageFilterType;
    
    GrayscaleErodeImageFilterType::Pointer erodeFilter
    = GrayscaleErodeImageFilterType::New();
    erodeFilter->SetInput(input);
    erodeFilter->SetKernel(structuringElement);
    erodeFilter->Update();
    return erodeFilter->GetOutput();
  }
  
  /*
  void doErosion(ImageType::Pointer *input){
    *input = doErosion(*input);
    return;
  }
  */
   
  //Dilation
  ImageType::Pointer doDilation(ImageType::Pointer input, int radius){
    
    typedef itk::BinaryBallStructuringElement<PixelType, Dimension>
    StructuringElementType;
    
    StructuringElementType structuringElement;
    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();
    
    typedef itk::GrayscaleDilateImageFilter <ImageType, ImageType, StructuringElementType>
    GrayscaleDilateImageFilterType;
    
    GrayscaleDilateImageFilterType::Pointer dilateFilter
    = GrayscaleDilateImageFilterType::New();
    dilateFilter->SetInput(input);
    dilateFilter->SetKernel(structuringElement);
    dilateFilter->Update();
    return dilateFilter->GetOutput();
  }
  
  /*
  void doDilation(ImageType::Pointer *input){
    *input = doDilation(*input);
    return;
  }
  */

  //Closing(f) = Erosion(Dilation(f))
  ImageType::Pointer doClosing(ImageType::Pointer input, int radius){
    return doErosion(doDilation(input, radius), radius);
  }
  
  /*
    void doClosing(ImageType::Pointer *input){
        *input = doClosing(*input);
        return;
    }
   */
  
  //Opening(f) = Dilation(Erosion(f))
  ImageType::Pointer doOpening(ImageType::Pointer input, int radius){
    return doDilation(doErosion(input, radius), radius);
  }
  
  /*
  void doOpening(ImageTypep::Oointer *input){
    *input = doOpening(*input);
    return
  }
   */
    
    ImageType::Pointer doOtsuThresholding(ImageType::Pointer input){
        typedef  itk::OtsuThresholdImageFilter< ImageType , ImageType> Filter;
        Filter::Pointer filter = Filter::New();
        
        filter->SetInput(input);
        filter->Update();
        
        printf("Otsu threshold is at %f\n", filter->GetThreshold());
        
        return filter->GetOutput();
    }
    
    void doOtsuThresholding(ImageType::Pointer *input){
        *input = doOtsuThresholding(*input);
        return;
    };

    ImageType::Pointer doIntensityWindowing(ImageType::Pointer input){
        typedef itk::IntensityWindowingImageFilter <ImageType, ImageType> FilterType;
        typedef itk::StatisticsImageFilter<ImageType> StatisticsFilterType;
        StatisticsFilterType::Pointer statisticsFilter = StatisticsFilterType::New ();
        statisticsFilter->SetInput(input);
        statisticsFilter->Update();
        
        FilterType::Pointer filter = FilterType::New();
        filter->SetInput(input);
        filter->SetWindowMinimum(statisticsFilter->GetMean()-statisticsFilter->GetSigma()*2.1);
        filter->SetWindowMaximum(statisticsFilter->GetMaximum());
        filter->SetOutputMinimum(statisticsFilter->GetMinimum());
        filter->SetOutputMaximum(statisticsFilter->GetMaximum());
        filter->Update();
        return filter->GetOutput();
    }

    void doIntensityWindowing(ImageType::Pointer *input){
        *input = doIntensityWindowing(*input);
        return;
    }
    
    ImageType::Pointer doHistogramEqualization(ImageType::Pointer input){
        typedef  itk::AdaptiveHistogramEqualizationImageFilter< ImageType > EqualizationImageFilterType;
        EqualizationImageFilterType::Pointer equalizationImageFilter = EqualizationImageFilterType::New();
        
        equalizationImageFilter->SetInput(input);
        equalizationImageFilter->SetAlpha(1);
        equalizationImageFilter->SetBeta(1);
        equalizationImageFilter->Update();
        
        return equalizationImageFilter->GetOutput();
    }
    
    void doHistogramEqualization(ImageType::Pointer *input){
        *input = doHistogramEqualization(*input);
        return;
    }
    
    ImageType::Pointer doIntensityRescaling(ImageType::Pointer input){
        typedef  itk::RescaleIntensityImageFilter< ImageType > Filter;
        Filter::Pointer filter = Filter::New();
        
        filter->SetInput(input);
        filter->SetOutputMinimum(-2048);
        filter->SetOutputMaximum(2048);
        filter->Update();
        
        return filter->GetOutput();
    }
    
    void doIntensityRescaling(ImageType::Pointer *input){
        *input = doIntensityRescaling(*input);
        return;
    }
}