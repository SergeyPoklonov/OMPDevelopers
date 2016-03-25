#include <QString>
#include <vector>
#include <functional>

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

    CDeveloperData GetDataByInd( size_t ind ) const;
    CDeveloperData GetDataByName( QString name ) const;

    long GetIndByName( QString name ) const;

    bool SetDataByInd( CDeveloperData newData, size_t ind );

    void SortData( std::function<bool(const CDeveloperData &f, const CDeveloperData &s)> sortPredicate );

private:
    std::vector<CDeveloperData> m_DevelopersList;
};

//////////////////////////////////////////////////////////////////////////

#endif // DEVELOPERDATA_H
