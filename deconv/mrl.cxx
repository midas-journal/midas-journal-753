#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

// #include "itkFFTWRealToComplexConjugateImageFilter.h"
// #include "itkFFTWComplexConjugateToRealImageFilter.h"
#include "itkRichardsonLucyDeconvolutionImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkTestingMacros.h"
// #include "itkCommand.h"
// 
// void disableReg(itk::Object* object, const itk::EventObject &, void*)
// {
//   // the same typedefs than in the main function - should be done in a nicer way
//   const int                 Dimension = 2;
//   typedef unsigned char     PixelType;
// 
//   typedef itk::Image< PixelType, Dimension >                   ImageType;
//   typedef itk::RichardsonLucyDeconvolutionImageFilter< ImageType, ImageType > FilterType;
//   typedef itk::MedianImageFilter< FilterType::InternalImageType,
//     FilterType::InternalImageType >                      MedianType;
//   
//   // real stuff begins here
//   // get the LR filter and the median filter
//   FilterType * filter = dynamic_cast< FilterType * >( object );
//   MedianType * median = dynamic_cast< MedianType * >( filter->GetSmoothingFilter() );
// 
//   // set half of the slice number as radius
//   MedianType::InputSizeType radius;
//   if( filter->GetIteration() % 10 == 0 )
//     {
//     radius.Fill( 1 );
//     }
//   else
//     {
//     radius.Fill( 0 );
//     }  
//   median->SetRadius( radius );
// }

int main(int argc, char * argv[])
{

  if( argc < 4 )
    {
    std::cerr << "usage: " << argv[0] << " intput kernel output [nbIt [th [period [method [gpf]]]]]" << std::endl;
    std::cerr << " input: the input image" << std::endl;
    std::cerr << " output: the output image" << std::endl;
    std::cerr << " method: padding method - 1: zero flux, 2: zero, 3: mirror, 4: wrap" << std::endl;
    std::cerr << " gpf: greatest prime factor of the size of the padded image" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  // itk::SimpleFilterWatcher watcher_reader(reader, "reader");

  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );
  // itk::SimpleFilterWatcher watcher_reader2(reader2, "reader2");

  typedef itk::MedianImageFilter< IType, IType > MedianType;
  MedianType::Pointer median = MedianType::New();
  median->SetInput( reader->GetOutput() );

  typedef itk::RichardsonLucyDeconvolutionImageFilter< IType > FFTConvolutionType;
  FFTConvolutionType::Pointer conv = FFTConvolutionType::New();
  conv->SetInput( median->GetOutput() );
//  conv->SetInput( reader->GetOutput() );
  conv->SetPointSpreadFunction( reader2->GetOutput() );
  // test default value
  TEST_SET_GET_VALUE( 13, conv->GetGreatestPrimeFactor() );
  TEST_SET_GET_VALUE( FFTConvolutionType::ZERO_FLUX_NEUMANN, conv->GetPadMethod() );
  TEST_SET_GET_VALUE( 10, conv->GetNumberOfIterations() );
  TEST_SET_GET_VALUE( 0.0, conv->GetRelativeChangeThreshold() );
  TEST_SET_GET_VALUE( NULL, conv->GetSmoothingFilter() );
  TEST_SET_GET_VALUE( 1, conv->GetSmoothingPeriod() );
  
  typedef itk::MedianImageFilter< FFTConvolutionType::InternalImageType, FFTConvolutionType::InternalImageType > InternalMedianType;
  InternalMedianType::Pointer imedian = InternalMedianType::New();
  conv->SetSmoothingFilter( imedian );

//  itk::CStyleCommand::Pointer command = itk::CStyleCommand::New();
//  command->SetCallback( *disableReg );
//  conv->AddObserver( itk::IterationEvent(), command );

  if( argc >= 5 )
    {
    conv->SetNumberOfIterations( atoi(argv[4]) );
    TEST_SET_GET_VALUE( atoi(argv[4]), conv->GetNumberOfIterations() );
    }

  if( argc >= 6 )
    {
    conv->SetRelativeChangeThreshold( atof(argv[5]) );
    TEST_SET_GET_VALUE( atof(argv[5]), conv->GetRelativeChangeThreshold() );
    }

  if( argc >= 7 )
    {
    conv->SetSmoothingPeriod( atoi(argv[6]) );
    TEST_SET_GET_VALUE( atoi(argv[6]), conv->GetSmoothingPeriod() );
    }

  if( argc >= 8 )
    {
    conv->SetPadMethod( atoi(argv[7]) );
    TEST_SET_GET_VALUE( atoi(argv[7]), conv->GetPadMethod() );
    }

  if( argc >= 9 )
    {
    conv->SetGreatestPrimeFactor( atoi(argv[8]) );
    TEST_SET_GET_VALUE( atoi(argv[8]), conv->GetGreatestPrimeFactor() );
    }
  itk::SimpleFilterWatcher watcher_norm(conv, "conv");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( conv->GetOutput() );
  writer->SetFileName( argv[3] );
  // itk::SimpleFilterWatcher watcher_writer(writer, "writer");
  
  writer->Update();

  return 0;
}
