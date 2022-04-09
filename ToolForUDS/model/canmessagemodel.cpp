#include "canmessagemodel.h"

CanMessageModel::CanMessageModel(QObject *parent)
    : QAbstractTableModel(parent),
      mFreshCount(20)
{
    mTimer.start(500);
    connect(&mTimer, &QTimer::timeout,
            this, [&](){
//        if(mFreshCount != 20){
//            mFreshCount = 20;
//            updateModel();
//        }
        updateModel();
    });
}

void CanMessageModel::insertData(CAN_OBJ data, QString type, QString devID)
{
    QDateTime time = QDateTime::currentDateTime();   //获取当前时间
    data.TimeStamp = time.toTime_t();
    ModelData model;
    model.obj = data;
    model.devID = devID;
    model.type = type;
    mModel.append(model);
//    if(mFreshCount != 0){
//        --mFreshCount;
//        return;
//    }
//    mFreshCount = 20;
//    updateModel();
}

Q_DECLARE_METATYPE(CAN_OBJ)
void CanMessageModel::insertData(const CAN_MESSAGE_PACKAGE &msg)
{
    CAN_OBJ data = msg.canObj.value<CAN_OBJ>();
    QString devID = QString("Device_%1_C%2").arg(msg.devInd).arg(msg.devChan);
    insertData(data, msg.type, devID);
}

void CanMessageModel::updateModel()
{
    beginResetModel();
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
    return 6;
}

QVariant CanMessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if(index.row() < mModel.size()){
            const ModelData& obj = mModel.at(index.row());
            return format(obj, index.column());
        }
    default:
        return QVariant();
    }
}

bool CanMessageModel::saveToFile(QString filepath)
{
    QFile file(filepath);
    if(!file.open(QFile::WriteOnly | QFile::Truncate))
    {
        return false;
    }

    QString datas = QString::fromLocal8Bit("数据流向\t\t时间戳\t\tCAN ID\t\t数据长度\t\tCAN报文\n");
    int base = 16;
    for(int i = 0; i < mModel.size(); i++)
    {
        QString canData = QString("%1 %2 %3 %4 %5 %6 %7 %8")
                .arg(mModel[i].obj.Data[0], 2, base, QLatin1Char('0'))
                .arg(mModel[i].obj.Data[1], 2, base, QLatin1Char('0'))
                .arg(mModel[i].obj.Data[2], 2, base, QLatin1Char('0'))
                .arg(mModel[i].obj.Data[3], 2, base, QLatin1Char('0'))
                .arg(mModel[i].obj.Data[4], 2, base, QLatin1Char('0'))
                .arg(mModel[i].obj.Data[5], 2, base, QLatin1Char('0'))
                .arg(mModel[i].obj.Data[6], 2, base, QLatin1Char('0'))
                .arg(mModel[i].obj.Data[7], 2, base, QLatin1Char('0'));

        QString data = QString("%1\t\t%2\t\t%3\t\t%4\t\t%5\n")
                .arg(mModel[i].type)
                .arg(QDateTime::fromSecsSinceEpoch(mModel[i].obj.TimeStamp).toString("yyyy-MM-dd hh:mm:ss"))
                .arg(mModel[i].obj.ID, 0, base)
                .arg(mModel[i].obj.DataLen)
                .arg(canData);
        datas.append(data);
    }

    file.write(datas.toLocal8Bit());
    file.close();
    return true;
}

QVariant CanMessageModel::headerInfo(int section) const
{
    switch (section) {
    case 0:
        return QStringLiteral("DEV ID");
    case 1:
        return QStringLiteral("数据流向");
    case 2:
        return QStringLiteral("时间戳");
    case 3:
        return QStringLiteral("CAN ID");
    case 4:
        return QStringLiteral("数据长度");
    case 5:
        return QStringLiteral("CAN报文");
    default:
        return QVariant();
    }
}

QVariant CanMessageModel::format(const ModelData &obj, int column) const
{
    int base = mIsHex ? 16 : 10;
    switch (column) {
    case 0:
        return obj.devID;
    case 1:
        return obj.type;
    case 2:
        return QDateTime::fromSecsSinceEpoch(obj.obj.TimeStamp).toString("yyyy-MM-dd hh:mm:ss");
    case 3:
        return QString("%1").arg(obj.obj.ID, 0, base);
    case 4:
        return obj.obj.DataLen;
    case 5:
        return QString("%1 %2 %3 %4 %5 %6 %7 %8")
                .arg(obj.obj.Data[0], 2, base, QLatin1Char('0'))
                .arg(obj.obj.Data[1], 2, base, QLatin1Char('0'))
                .arg(obj.obj.Data[2], 2, base, QLatin1Char('0'))
                .arg(obj.obj.Data[3], 2, base, QLatin1Char('0'))
                .arg(obj.obj.Data[4], 2, base, QLatin1Char('0'))
                .arg(obj.obj.Data[5], 2, base, QLatin1Char('0'))
                .arg(obj.obj.Data[6], 2, base, QLatin1Char('0'))
                .arg(obj.obj.Data[7], 2, base, QLatin1Char('0'));
    case 6:
        if(obj.obj.ExternFlag == 0)
        {
            return QStringLiteral("标准帧");
        }else if(obj.obj.ExternFlag == 1)
        {
            return QStringLiteral("拓展帧");
        }
        return QString("NotDefine");
    case 7:
        if(obj.obj.SendType == 0)
        {
            return QStringLiteral("正常发送");
        }else if(obj.obj.SendType == 1)
        {
            return QStringLiteral("单次发送");
        }else if(obj.obj.SendType == 2)
        {
            return QStringLiteral("自发自收");
        }else if(obj.obj.SendType == 3)
        {
            return QStringLiteral("单次自发自收");
        }
        return QString("NotDefine");
    case 8:
        if(obj.obj.RemoteFlag == 0)
        {
            return QStringLiteral("数据帧");
        }else if(obj.obj.RemoteFlag == 1)
        {
            return QStringLiteral("远程帧");
        }
        return QString("NotDefine");

    default:
        return QVariant();
    }
}
