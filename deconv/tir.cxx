#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkTotalIntensityRatioCalculator.h"
#include "itkTestingMacros.h"

int main(int argc, char * argv[])
{

  if( argc < 3 )
    {
    std::cerr << "usage: " << argv[0] << " intput new" << std::endl;
    std::cerr << " input: the input image" << std::endl;
    std::cerr << " output: the output image" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );
  reader2->Update();

  typedef itk::TotalIntensityRatioCalculator< IType > CalculatorType;
  CalculatorType::Pointer tir = CalculatorType::New();
  tir->SetImage( reader->GetOutput() );
  tir->SetNewImage( reader2->GetOutput() );
  tir->Compute();
  std::cout << "Total Intensity Ratio: " << tir->GetOutput() << std::endl;  

  return 0;
}

