#include "piechartdata.h"


HTMLPieChartData::HTMLPieChartData()
  : m_SliceTypeName("Task")
  , m_SliceMeasName("Hours")
  , m_Width(550)
  , m_Height(400)
  , m_SliceVisibilityThresholdPrc(0.0)
{
  
}

void HTMLPieChartData::addSlice(QString caption, double weight, QString colorStr)
{
  Q_ASSERT( weight > 0 );
  
  HTMLPieChartSlice sliceData;
  sliceData.caption = caption;
  sliceData.weight = weight;
  sliceData.colorString = colorStr;
  
  m_Slices.push_back( sliceData );
}

void HTMLPieChartData::setDimensions(unsigned width, unsigned height)
{
  m_Width = width;
  m_Height = height;
}

void HTMLPieChartData::setSlicesDescription(QString sliceTypeName, QString measName)
{
  m_SliceTypeName = sliceTypeName;
  m_SliceMeasName = measName;
}

void HTMLPieChartData::setResidueSliceParams( QString label, QString color)
{
  m_ResidueSliceLabel = label;
  m_ResidueSliceColor = color;
}

double HTMLPieChartData::getSliceVisibilityThreshold() const 
{ 
  if( m_SliceVisibilityThresholdPrc <= 0 || m_SliceVisibilityThresholdPrc >= 100 )
    return 0;
  
  return m_SliceVisibilityThresholdPrc / 100.0; 
}

void HTMLPieChartData::setSliceVisibilityThresholdPrc(double prc)
{
  Q_ASSERT( prc > 0.0 && prc < 100.0 );
  
  m_SliceVisibilityThresholdPrc = prc;
}

bool HTMLPieChartData::slicesAreAdvanced() const
{
  for( const HTMLPieChartSlice &sliceData : m_Slices )
  {
    if( sliceData.colorString.size() )
      return true;
  }
  
  return false;
}
