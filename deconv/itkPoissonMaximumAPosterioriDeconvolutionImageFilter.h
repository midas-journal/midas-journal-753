/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkPoissonMaximumAPosterioriDeconvolutionImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2009-01-28 18:14:36 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPoissonMaximumAPosterioriDeconvolutionImageFilter_h
#define __itkPoissonMaximumAPosterioriDeconvolutionImageFilter_h

#include "itkIterativeDeconvolutionImageFilter.h"
#include "itkConceptChecking.h"

namespace itk {

namespace Functor {  
  
template< class TReal>
class PoissonMaximumAPosteriori
{
public:
  PoissonMaximumAPosteriori() {};
  ~PoissonMaximumAPosteriori() {};
  bool operator!=( const PoissonMaximumAPosteriori & other ) const
    {
    return !(*this == other);
    }
  bool operator==( const PoissonMaximumAPosteriori & other ) const
    {
    return true;
    }
  // arguments are in an unusual order to allow to run in place
  // of the denominator image
  inline TReal operator()( const TReal & d, const TReal & n )
    {
    if( d < 1e-4 )
      {
      return -1;
      }
    assert( !std::isnan(n / d - 1) );
    return n / d - 1;
    }
};

template< class TReal>
class PoissonMaximumAPosteriori2
{
public:
  PoissonMaximumAPosteriori2() {};
  ~PoissonMaximumAPosteriori2() {};
  bool operator!=( const PoissonMaximumAPosteriori2 & other ) const
    {
    return !(*this == other);
    }
  bool operator==( const PoissonMaximumAPosteriori2 & other ) const
    {
    return true;
    }
  inline TReal operator()( const TReal & a, const TReal & b )
    {
    assert( !std::isnan( b ) );
    assert( !std::isnan( a ) );
    assert( !std::isnan( vcl_exp( a ) ) );
    /*
    if( std::isnan( b * vcl_exp( a ) ) )
      {
      std::cout << "a: " << a
              << "  b: " << b
              << "  vcl_exp( a ): " << vcl_exp( a )
              << "  b * vcl_exp( a ) ): " << b * vcl_exp( a )
              << std::endl;
      }
    */
    // if this assert happen, the check for small values in PoissonMaximumAPosteriori must be changed for
    // a higher value, to avoid producing values of b which can make vcl_exp() produce a greater value
    // than the maximum possible value for the selected real type.
    assert( !std::isnan( b * vcl_exp( a ) ) );
    return b * vcl_exp( a );
    }
};

}

/** \class PoissonMaximumAPosterioriDeconvolutionImageFilter
 * \brief 
 *
 * 
 * \author Gaetan Lehmann
 *
 * \sa FFTShiftImageFilter NormalizeToConstantImageFilter FFTRealToComplexConjugateImageFilter
 */
template<class TInputImage, class TPointSpreadFunction=TInputImage, class TOutputImage=TInputImage, class TInternalPrecision=float>
class ITK_EXPORT PoissonMaximumAPosterioriDeconvolutionImageFilter : 
    public IterativeDeconvolutionImageFilter<TInputImage, TPointSpreadFunction, TOutputImage, TInternalPrecision> 
{
public:
  /** Standard class typedefs. */
  typedef PoissonMaximumAPosterioriDeconvolutionImageFilter Self;

  typedef IterativeDeconvolutionImageFilter<TInputImage, TPointSpreadFunction, TOutputImage, TInternalPrecision>  Superclass;

  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Some convenient typedefs. */
  typedef TInputImage                              InputImageType;
  typedef TPointSpreadFunction                             PointSpreadFunctionType;
  typedef TOutputImage                             OutputImageType;
  typedef TInternalPrecision                            InternalPrecisionType;
  typedef typename InputImageType::Pointer         InputImagePointer;
  typedef typename InputImageType::ConstPointer    InputImageConstPointer;
  typedef typename InputImageType::PixelType       InputImagePixelType;
  typedef typename PointSpreadFunctionType::Pointer        PointSpreadFunctionPointer;
  typedef typename PointSpreadFunctionType::ConstPointer   PointSpreadFunctionConstPointer;
  typedef typename PointSpreadFunctionType::PixelType      PointSpreadFunctionPixelType;
  typedef typename OutputImageType::Pointer        OutputImagePointer;
  typedef typename OutputImageType::ConstPointer   OutputImageConstPointer;
  typedef typename OutputImageType::PixelType      OutputImagePixelType;
  typedef typename InputImageType::RegionType      RegionType;
  typedef typename InputImageType::IndexType       IndexType;
  typedef typename InputImageType::SizeType        SizeType;
  
  
  typedef typename Superclass::FFTFilterType       FFTFilterType;
  typedef typename Superclass::IFFTFilterType      IFFTFilterType;
  typedef typename Superclass::ComplexImageType    ComplexImageType;
  typedef typename ComplexImageType::Pointer       ComplexImagePointerType;
  typedef typename ComplexImageType::PixelType     ComplexType;

  typedef typename Superclass::InternalImageType   InternalImageType;
  typedef typename InternalImageType::Pointer      InternalImagePointerType;
  typedef typename Superclass::InternalFilterType  InternalFilterType;

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(PoissonMaximumAPosterioriDeconvolutionImageFilter, IterativeDeconvolutionImageFilter);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(InputHasPixelTraitsCheck,
    (Concept::HasPixelTraits<InputImagePixelType>));
  itkConceptMacro(InputHasNumericTraitsCheck,
    (Concept::HasNumericTraits<InputImagePixelType>));
  /** End concept checking */
#endif


protected:
  PoissonMaximumAPosterioriDeconvolutionImageFilter() {};
  ~PoissonMaximumAPosterioriDeconvolutionImageFilter() {};

  virtual void Init();
  virtual void SetEstimate( InternalImageType * estimate );
  virtual typename InternalImageType::Pointer NewEstimate();
  virtual void End();

  typename InternalFilterType::Pointer m_Convolution;
  typename InternalFilterType::Pointer m_Division;
  typename InternalFilterType::Pointer m_Convolution2;
  typename InternalFilterType::Pointer m_Multiplication;

private:
  PoissonMaximumAPosterioriDeconvolutionImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPoissonMaximumAPosterioriDeconvolutionImageFilter.txx"
#endif

#endif
