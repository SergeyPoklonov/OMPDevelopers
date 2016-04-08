#ifndef DEVELOPERDATA_H
#define DEVELOPERDATA_H

#include <QDomDocument>
#include <QString>



//////////////////////////////////////////////////////////////////////////

class CDeveloperData
{
public:
    CDeveloperData() { clear(); }
    CDeveloperData(QString name, double wr) { clear(); setName(name); setWageRate(wr); }

    void clear();

    QString getName() const { return m_Name; }
    double  getWageRate() const { return m_WageRate > 0 ? m_WageRate : 1.0; }


    void setName( QString name ) { m_Name = name; }
    void setWageRate( double wr ) { m_WageRate = wr; }

    bool isValid(QString *errStr = nullptr) const;

    void WriteToXML(QDomElement &parentElement);
    void ReadFromXML(QDomElement &parentElement);

private:
    QString m_Name;
    double m_WageRate;
};

#endif // DEVELOPERDATA_H
