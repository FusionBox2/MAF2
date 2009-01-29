#ifndef __lhpHistogram1D_H__
#define __lhpHistogram1D_H__

#include <ostream>






//------------------------------------------------------------------------------
// Simple 1D histogram class for binning data
//------------------------------------------------------------------------------
class lhpHistogram1D
{
public:
  //----------------------------------------------------------------------------
  // Basic methods
  //----------------------------------------------------------------------------

  // Constructors and destructor
  // NB remember that the range is from the min of the first bin to the max of the last,
  // Therefore if the input data is integer type, eg 0-255, the range is actually 0 <= v < 256
  // So minVal = 0, maxVal = 256,
  // or minVal = -0.5, maxVal = 255.5 if you really need to avoid including 256 in the range.
  lhpHistogram1D(int numberOfBins, double minVal, double maxVal) ;
  lhpHistogram1D(double nominalBinSize, double minVal, double maxVal) ;
  ~lhpHistogram1D() ;

  // set all data to zero
  void Initialize() ;

  // Get number of bins
  int GetNumberOfBins() const {return m_numberOfBins ;}

  // Get size of bin
  double GetBinSize() const {return m_binSize ;}

  // Get min and max values in range
  double GetMinVal() const {return m_minVal ;}
  double GetMaxVal() const {return m_maxVal ;}

  // Get total in this bin (returns -1 if out of range)
  int GetBin(int i) const ;

  // Get total of histogram
  int GetTotal() const {return m_total ;}

  // Add value to histogram
  // Values outside the range are ignored
  void AddValue(double val) ;

  // Print self
  void PrintSelf(std::ostream& os, int indent) const ;


  //----------------------------------------------------------------------------
  // Conversion between bin id and value
  //----------------------------------------------------------------------------

  // calculate id of bin from value
  int CalcBinId(double val) const ;

  // get range of values covered by bin i
  // range[0] <= v < range[1]  where i < nbins-1
  // range[0] <= v <= range[1] where i = nbins-1
  void CalculateBinRange(int i, double range[2]) const ;


  //----------------------------------------------------------------------------
  // Statistics
  //----------------------------------------------------------------------------

  // return max bin
  // (returns bin with lowest id in the event of a draw)
  int MaxBin() const ;

  // Return mean bin.
  // If istart > 0, mean is taken from istart to n-1 + istart, assuming histogram is periodic.
  // This is useful if the value is an angle and the mean is close to zero or 2pi
  double MeanBin(int istart = 0) const ;

  // Return Median value.
  // This returns value - not bin.  Use CalcBinId() if bin is required.
  // Note that this does not require nor affect any pre-calculated quantiles.
  double MedianValue() const ;

  // Calculate quantile points - values which mark the start of each quantile range
  // For n quantiles, this allocates n+1 points, with the last one marking the end of the range.
  // NB if the histogram data changes you will need to call this again to refresh the quantile positions.
  void CalculateQuantilePoints(int numberOfQuantiles) ;

  // Get value which is the start of the ith quantile range.
  // Valid arguments are 0 <= i <= n where n is no. of quantiles.
  // If i = 0, returns the min,
  // if i = n, returns the max.
  double GetQuantilePoint(int i) const {return m_quantilePoints[i] ;}

  // Which quantile is value v in.
  // Possible return values are 0 <= i < n where n is no. of quantiles.
  // Returns i = -1 if the value is out of range.
  int WhichQuantile(double v) const ;

private:
  int m_numberOfBins ;
  double m_binSize ;
  double m_minVal, m_maxVal ;
  int *m_bin ;
  int m_total ;

  // variables for quantile calculations
  int m_numberOfQuantiles ;
  double *m_quantilePoints ;
};



#endif