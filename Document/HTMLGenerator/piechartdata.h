#ifndef PIECHARTDATA_H
#define PIECHARTDATA_H

#include <QString>
#include <vector>


struct HTMLPieChartSlice
{
  QString caption;
  double weight;
  QString colorString; // valid html color string, can be empty
  
  HTMLPieChartSlice() : weight(0.0) {}
};

class HTMLPieChartData
{
public:
  HTMLPieChartData();
  
  // set
  void addSlice(QString caption, double weight, QString colorStr = "");
  
  void setDimensions(unsigned width, unsigned height);
  
  void setSlicesDescription(QString sliceTypeName, QString measName);
  
  void setResidueSliceParams( QString label = "Others", QString color = "Silver" );
  
  void setSliceVisibilityThresholdPrc(double prc); // from 0 to 100
  
  // get
  std::vector<HTMLPieChartSlice> getSlices() const { return m_Slices; }
  
  QString getSliceTypeName() const { return m_SliceTypeName; }
  QString getSliceMeasName() const { return m_SliceMeasName; }
  
  unsigned getWidth() const { return m_Width; }
  unsigned getHeight() const { return m_Height; }
  
  QString getResidueSliceColor() const { return m_ResidueSliceColor; }
  QString getResidueSliceLabel() const { return m_ResidueSliceLabel; }
  
  double getSliceVisibilityThreshold() const;
  
  bool slicesAreAdvanced() const;
  
private:
  QString m_SliceTypeName;
  QString m_SliceMeasName;
  
  QString m_ResidueSliceColor;
  QString m_ResidueSliceLabel;
  double  m_SliceVisibilityThresholdPrc;
  
  unsigned m_Width;
  unsigned m_Height;
  
  std::vector<HTMLPieChartSlice> m_Slices;
};

#endif // PIECHARTDATA_H
