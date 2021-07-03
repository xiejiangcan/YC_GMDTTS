#include "udscanidmapmodel.h"

UdsCanIdMapModel::UdsCanIdMapModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

void UdsCanIdMapModel::initData(const QMap<QString, QVariant> &map)
{
    beginResetModel();
    int index = 0;
    for(auto iter = map.begin(); iter != map.end(); ++iter, ++index){
        QPair<QString,QString> value;
        value.first = iter.key();
        value.second = iter.value().toString();
        mModel.insert(index, value);
    }
    endResetModel();
}

QMap<QString, QVariant> UdsCanIdMapModel::getModel() const
{
    QMap<QString, QVariant> model;
    for(auto iter = mModel.begin(); iter != mModel.end(); ++iter){
        QString key = iter.value().first;
        QString value = iter.value().second;
        if(key.isEmpty() || value.isEmpty())
            continue;
        if(model.contains(key)){
            if(model[key].toString().isEmpty())
                continue;
            model[key] = iter.value().second;
        }else{
            model.insert(key, value);
        }
    }
    return model;
}

void UdsCanIdMapModel::insertNewData(const QString &key, const QString &value)
{
    beginResetModel();
    int index = mModel.size();
    while(mModel.contains(index)){
        ++index;
    }
    QPair<QString,QString> tempPair;;
    tempPair.first = key;
    tempPair.second = value;
    mModel.insert(index, tempPair);
    endResetModel();
}

QVariant UdsCanIdMapModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int UdsCanIdMapModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return mModel.size();
}

int UdsCanIdMapModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

Qt::ItemFlags UdsCanIdMapModel::flags(const QModelIndex &index) const
{
    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable|Qt::ItemIsEditable;
}

QVariant UdsCanIdMapModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if(mModel.contains(index.row())){
            if(index.column() == 0){
                return QVariant::fromValue(mModel[index.row()].first);
            }else{
                return QVariant::fromValue(mModel[index.row()].second);
            }
        }
        break;
    case Qt::EditRole:
        if(mModel.contains(index.row())){
            if(index.column() == 0){
                return QVariant::fromValue(mModel[index.row()].first);
            }else{
                return QVariant::fromValue(mModel[index.row()].second);
            }
        }
        break;
    default:
        break;
    }
    return QVariant();
}

bool UdsCanIdMapModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    switch (role) {
    case Qt::EditRole:{
        int row = index.row();
        int column = index.column();
        if(!mModel.contains(row)){
            mModel.insert(row, QPair<QString, QString>());
        }
        if(column == 0){
            mModel[row].first = value.toString();
        }else{
            mModel[row].second = value.toString();
        }
        break;
    }
    default:
        break;
    }
    return true;
}

QVariant UdsCanIdMapModel::headerInfo(int section) const
{
    switch (section) {
    case 0:
        return QStringLiteral("SenderID");
    case 1:
        return QStringLiteral("ReceiveID");
    default:
        return QVariant();
    }
}
