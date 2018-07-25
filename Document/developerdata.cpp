#include "developerdata.h"

void CDeveloperData::clear()
{
  m_Name.clear();
  m_WageRate = 1.0;
  m_Calendar.clear();
}

bool CDeveloperData::isValid(QString *errStr) const
{
  if( m_Name.isEmpty() )
  {
    if( errStr )
      *errStr = "Не задано имя разработчика";

    return false;
  }

  if( m_WageRate <= 0.0 )
  {
    if( errStr )
      *errStr = "Неверная ставка, ставка разработчика должна быть больше 0";

    return false;
  }

  return true;
}

void CDeveloperData::ReadFromXML(QDomElement &parentElement)
{
  m_Name = parentElement.attribute( "name" );
  m_WageRate = parentElement.attribute( "wr" ).toDouble();
  
  m_Calendar.ReadFromXML( parentElement );
}

void CDeveloperData::WriteToXML(QDomElement &parentElement)
{
  parentElement.setAttribute( "name", m_Name );
  parentElement.setAttribute( "wr", m_WageRate );
  
  m_Calendar.WriteToXML( parentElement );
}
