#include <QString>
#include <vector>
#include <functional>
#include <QModelIndexList>

#ifndef DEVELOPERDATA_H
#define DEVELOPERDATA_H

//////////////////////////////////////////////////////////////////////////

class CDeveloperData
{
public:
    CDeveloperData() { clear(); }
    CDeveloperData(QString name, double wr) { clear(); setName(name); setWageRate(wr); }

    void clear();

    QString getName() const { return m_Name; }
    double  getWageRate() const { return m_WageRate; }


    void setName( QString name ) { m_Name = name; }
    void setWageRate( double wr ) { m_WageRate = wr; }

    bool isValid(QString *errStr = nullptr) const;

private:
    QString m_Name;
    double m_WageRate;
};

//////////////////////////////////////////////////////////////////////////

class CDeveloperListDataManager : public QAbstractTableModel
{
public:
    CDeveloperListDataManager( QObject * parent = nullptr );
    CDeveloperListDataManager( const CDeveloperListDataManager &src );

    void clear();

    size_t Count() const { return m_DevelopersList.size(); }

    CDeveloperData GetDataByInd( size_t ind ) const;
    CDeveloperData GetDataByName( QString name ) const;

    long GetIndByName( QString name ) const;

    bool SetDataByInd( CDeveloperData newData, size_t ind, QString *errStr = nullptr );
    bool InsertData( CDeveloperData newData, size_t ind, QString *errStr = nullptr );
    bool AddData( CDeveloperData newData, QString *errStr = nullptr );

    bool DeleteData(size_t ind, QString *errStr = nullptr);

    void SortData( std::function<bool(const CDeveloperData &f, const CDeveloperData &s)> sortPredicate );

// QAbstractTableModel
public:
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    bool CheckIfNameAlreadyExists(QString name, size_t ignoreInd = ULONG_MAX);

private:
    std::vector<CDeveloperData> m_DevelopersList;

    static const int COLQTY     = 2;
    static const int COLNUM_NAME = 0;
    static const int COLNUM_WR   = 1;
};

//////////////////////////////////////////////////////////////////////////

#endif // DEVELOPERDATA_H
