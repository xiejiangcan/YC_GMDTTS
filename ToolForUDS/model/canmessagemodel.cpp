#include "canmessagemodel.h"

CanMessageModel::CanMessageModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void CanMessageModel::insertData(CAN_OBJ data)
{
    beginResetModel();
    UINT time = QDateTime::currentDateTime().currentDateTime().toString("mmsszzz").toUInt();
    data.TimeStamp = time;
    mModel.append(data);
    endResetModel();
}

void CanMessageModel::setIsHex(bool enable)
{
    mIsHex = enable;
}

QVariant CanMessageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return QVariant();
    switch (role) {
    case Qt::DisplayRole:
        return headerInfo(section);
    default:
        return QVariant();
    }
}

int CanMessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return mModel.size();
}

int CanMessageModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4;
}

QVariant CanMessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if(index.row() < mModel.size()){
            const CAN_OBJ& obj = mModel.at(index.row());
            return format(obj, index.column());
        }
    default:
        return QVariant();
    }
}

QVariant CanMessageModel::headerInfo(int section) const
{
    switch (section) {
    case 0:
        return QStringLiteral("时间戳");
    case 1:
        return QStringLiteral("CAN ID");
    case 2:
        return QStringLiteral("数据长度");
    case 3:
        return QStringLiteral("CAN报文");
    default:
        return QVariant();
    }
}

QVariant CanMessageModel::format(const CAN_OBJ &obj, int column) const
{
    int base = mIsHex ? 16 : 10;
    switch (column) {
    case 0:
        return obj.TimeStamp;
    case 1:
        return QString("%1").arg(obj.ID, 0, base);
    case 2:
        return obj.DataLen;
    case 3:
        return QString("%1 %2 %3 %4 %5 %6 %7 %8")
                .arg(obj.Data[0], 2, base, QLatin1Char('0'))
                .arg(obj.Data[1], 2, base, QLatin1Char('0'))
                .arg(obj.Data[2], 2, base, QLatin1Char('0'))
                .arg(obj.Data[3], 2, base, QLatin1Char('0'))
                .arg(obj.Data[4], 2, base, QLatin1Char('0'))
                .arg(obj.Data[5], 2, base, QLatin1Char('0'))
                .arg(obj.Data[6], 2, base, QLatin1Char('0'))
                .arg(obj.Data[7], 2, base, QLatin1Char('0'));
    case 4:
        if(obj.ExternFlag == 0)
        {
            return QStringLiteral("标准帧");
        }else if(obj.ExternFlag == 1)
        {
            return QStringLiteral("拓展帧");
        }
        return QString("NotDefine");
    case 5:
        if(obj.SendType == 0)
        {
            return QStringLiteral("正常发送");
        }else if(obj.SendType == 1)
        {
            return QStringLiteral("单次发送");
        }else if(obj.SendType == 2)
        {
            return QStringLiteral("自发自收");
        }else if(obj.SendType == 3)
        {
            return QStringLiteral("单次自发自收");
        }
        return QString("NotDefine");
    case 6:
        if(obj.RemoteFlag == 0)
        {
            return QStringLiteral("数据帧");
        }else if(obj.RemoteFlag == 1)
        {
            return QStringLiteral("远程帧");
        }
        return QString("NotDefine");

    default:
        return QVariant();
    }
}
