#include "DemonFire.h"

class CommandIterationUpdate : public itk::Command
{
public:
    typedef CommandIterationUpdate Self;
    typedef itk::Command Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::RegularStepGradientDescentOptimizerv4<double> OptimizerType;
    typedef const OptimizerType * OptimizerPointer;
    itkNewMacro( Self );
protected:
    CommandIterationUpdate() {};
public:
    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
        Execute( (const itk::Object *)caller, event);
    }
    
    void Execute(const itk::Object * object, const itk::EventObject & event)
    {
        OptimizerPointer optimizer =
        static_cast< OptimizerPointer >( object );
        if( ! itk::IterationEvent().CheckEvent( &event ) )
        {
            return;
        }
        std::cout << optimizer->GetCurrentIteration() << " = ";
        std::cout << optimizer->GetValue() << " : ";
        std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};

namespace fire {
    // this is a function that takes in fixed and moving images
    // and performs registration on them. Returns a transform for the moving image
    TransformType::ConstPointer doRegistration(ImageType::Pointer fixed, ImageType::Pointer moving){

	FirstPassMetricType::Pointer metric1 = FirstPassMetricType::New();
	SecondPassMetricType::Pointer metric2 = SecondPassMetricType::New();
	OptimizerType::Pointer optimizer = OptimizerType::New();
    TransformType::Pointer      transform     = TransformType::New();
    RegistrationType::Pointer   registration  = RegistrationType::New();
    RegistrationType::Pointer   registration_MI = RegistrationType::New();
	
	registration->SetOptimizer(optimizer);
	registration->SetMetric(metric1);       // mean squares
	registration_MI ->SetOptimizer(optimizer);
	registration_MI ->SetMetric(metric2);    // mutual information
  
	registration->SetFixedImage(fixed);
	registration->SetMovingImage(moving);
	registration_MI->SetFixedImage(fixed);
	registration_MI->SetMovingImage(moving);
	
	// initialize transform
	TransformType::Pointer identityTransform = TransformType::New();
	identityTransform->SetIdentity();
    registration->SetMovingInitialTransform(identityTransform);
	registration->SetFixedInitialTransform(identityTransform);
	
	// optimizer settings
    optimizer->SetLearningRate(4);
	optimizer->SetMinimumStepLength(0.001);
	optimizer->SetRelaxationFactor(0.5);
	optimizer->SetNumberOfIterations(200);
	optimizer->AddObserver( itk::IterationEvent(), CommandIterationUpdate::New() );
	
    RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
    shrinkFactorsPerLevel.SetSize( 2 );
    shrinkFactorsPerLevel[0] = 32;
    shrinkFactorsPerLevel[1] = 16;
    RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
    smoothingSigmasPerLevel.SetSize( 2 );
    smoothingSigmasPerLevel[0] = 0;
    smoothingSigmasPerLevel[1] = 0;
    registration->SetNumberOfLevels ( 2 );
    registration->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );
    registration->SetSmoothingSigmasPerLevel( smoothingSigmasPerLevel );
    
    registration->SetMetricSamplingPercentage(1.0);
    
    // do the actual registration
    registration->Update();

    std::cout << "First pass registration finished with following results:" << std::endl;
    std::cout << optimizer->GetCurrentIteration() << " = ";
    std::cout << optimizer->GetValue() << " : ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
    
    registration_MI->SetMetric(metric2);
    registration_MI->SetMetricSamplingPercentage(.2);    // for performance
    // initialize with results of mean squares registration
    registration_MI->SetMovingInitialTransform(registration->GetTransform());
    
    // set shrink factors for up to six levels
    shrinkFactorsPerLevel.SetSize( 6 );
    shrinkFactorsPerLevel[0] = 32;
    shrinkFactorsPerLevel[1] = 16;
    shrinkFactorsPerLevel[2] = 8;
    shrinkFactorsPerLevel[3] = 4;
    shrinkFactorsPerLevel[4] = 2;
    shrinkFactorsPerLevel[5] = 1;
    
    // for now don't do any smoothing
    smoothingSigmasPerLevel.SetSize( 6 );
    smoothingSigmasPerLevel[0] = 0;
    smoothingSigmasPerLevel[1] = 0;
    smoothingSigmasPerLevel[2] = 0;
    smoothingSigmasPerLevel[3] = 0;
    smoothingSigmasPerLevel[4] = 0;
    smoothingSigmasPerLevel[5] = 0;
    
    // increase number of levels for higher resolution registration
    registration_MI->SetNumberOfLevels ( 2 );
    registration_MI->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );
    registration_MI->SetSmoothingSigmasPerLevel( smoothingSigmasPerLevel );
    
    registration_MI->Update();
    
    std::cout << "Registration finished with following results:" << std::endl;
    std::cout << optimizer->GetCurrentIteration() << " = ";
    std::cout << optimizer->GetValue() << " : ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
	return registration_MI->GetTransform();
    }
}
