#ifndef CANMESSAGEPROXYMODEL_H
#define CANMESSAGEPROXYMODEL_H

#include <QSortFilterProxyModel>

class CanMessageProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit CanMessageProxyModel(QObject *parent = nullptr);

    // setting
    void setIsHex(bool enable);
    void setDevInfo(QString devInfo);
    void setTimeRange(const QString& low, const QString& high);
    void setCanIDRange(const QString& low, const QString& high);

protected:
    bool filterAcceptsRow(int source_row,
                          const QModelIndex &source_parent) const override;

private:
    QString         m_devInfo;
    uint            m_timeL;
    uint            m_timeH;
    uint            m_canIDL;
    uint            m_canIDH;
    bool            mIsHex = true;
};

#endif // CANMESSAGEPROXYMODEL_H
