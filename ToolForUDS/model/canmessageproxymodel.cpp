#include "canmessageproxymodel.h"

CanMessageProxyModel::CanMessageProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_timeL = 0x0;
    m_timeH = (~0x0);
    m_canIDL = 0x0;
    m_canIDH = (~0x0);
    m_devInfo = QString("");
}

void CanMessageProxyModel::setIsHex(bool enable)
{
    mIsHex = enable;
}

void CanMessageProxyModel::setDevInfo(QString devInfo)
{
    m_devInfo = devInfo;
}

void CanMessageProxyModel::setTimeRange(const QString &low, const QString &high)
{
    if(low.isEmpty()){
        m_timeL = 0x0;
    }else{
        m_timeL = low.toUInt(nullptr, mIsHex ? 16 : 10);
    }

    if(high.isEmpty()){
        m_timeH = (~0x0);
    }else{
        m_timeH = high.toUInt(nullptr, mIsHex ? 16 : 10);
    }

}

void CanMessageProxyModel::setCanIDRange(const QString &low, const QString &high)
{
    if(low.isEmpty()){
        m_canIDL = 0x0;
    }else{
        m_canIDL = low.toUInt(nullptr, mIsHex ? 16 : 10);
    }

    if(high.isEmpty()){
        m_canIDH = (~0x0);
    }else{
        m_canIDH = high.toUInt(nullptr, mIsHex ? 16 : 10);
    }
}

bool CanMessageProxyModel::filterAcceptsRow(int source_row,
                                            const QModelIndex &source_parent) const
{
    QModelIndex index0 = sourceModel()->index(source_row, 0, source_parent);
    QModelIndex index1 = sourceModel()->index(source_row, 1, source_parent);
    QModelIndex index2 = sourceModel()->index(source_row, 2, source_parent);

    QString dev = sourceModel()->data(index0).toString();
    uint time = sourceModel()->data(index1).toString().toUInt(nullptr, mIsHex ? 16 : 10);
    uint canID = sourceModel()->data(index2).toString().toUInt(nullptr, mIsHex ? 16 : 10);

    if(m_devInfo.isEmpty()){
        return time <= m_timeH && time >= m_timeL
                && canID <= m_canIDH && canID >= m_canIDL;
    }

    return dev == m_devInfo
            && time <= m_timeH && time >= m_timeL
            && canID <= m_canIDH && canID >= m_canIDL;
}
