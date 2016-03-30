#include "developerdata.h"

#include <QString>
#include <vector>
#include <functional>
#include <QModelIndexList>
#include <QDomDocument>

#ifndef DEVELOPERDATAMANAGER_H
#define DEVELOPERDATAMANAGER_H

//////////////////////////////////////////////////////////////////////////

class CDeveloperListDataManager : public QAbstractTableModel
{
public:
    CDeveloperListDataManager( QObject * parent = nullptr );
    CDeveloperListDataManager( const CDeveloperListDataManager &src, QObject * parent = nullptr );

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

    void WriteToXML(QDomElement &parentElement);
    void LoadFromXML(QDomElement &parentElement);

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

#endif // DEVELOPERDATAMANAGER_H
