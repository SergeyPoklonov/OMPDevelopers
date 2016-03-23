#include <QString>
#include <vector>

#ifndef DEVELOPERDATA_H
#define DEVELOPERDATA_H

//////////////////////////////////////////////////////////////////////////

class CDeveloperData
{
public:
    CDeveloperData() { clear(); }

    void clear();

    QString getName() const { return m_Name; }
    double  getWageRate() const { return m_WageRate; }


    void setName( QString name ) { m_Name = name; }
    void setWageRate( double wr ) { m_WageRate = wr; }

private:
    QString m_Name;
    double m_WageRate;
};

//////////////////////////////////////////////////////////////////////////

class CDeveloperListDataManager
{
public:
    CDeveloperListDataManager() { clear(); }

    void clear();

    size_t Count() const { return m_DevelopersList.size(); }

    CDeveloperData GetByInd( size_t ind ) const;
    CDeveloperData GetByName( QString name ) const;

private:
    std::vector<CDeveloperData> m_DevelopersList;
};

//////////////////////////////////////////////////////////////////////////

#endif // DEVELOPERDATA_H
