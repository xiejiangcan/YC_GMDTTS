#include "emcresultmodel.h"
#include <QColor>

EmcResultModel::EmcResultModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void EmcResultModel::updateModel(const QString &name, const QMap<QString, QVariantMap> &map)
{
    beginResetModel();
    m_model[name] = map;
    endResetModel();
}

QVariant EmcResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal){
        switch (role) {
        case Qt::DisplayRole:
            return hHeaderInfo(section);
        default:
            return QVariant();
        }
    }else{
        switch (role) {
        case Qt::DisplayRole:
            return vHeaderInfo(section);
        default:
            return QVariant();
        }
    }
}

int EmcResultModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_model.size();
}

int EmcResultModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 23;
}

QVariant EmcResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return getValueFromModel(index.row(), index.column()).toDouble();
    case Qt::BackgroundColorRole:
        return getColorFromModel(index.row(), index.column());
    }
    return QVariant();
}

QVariant EmcResultModel::hHeaderInfo(int section) const
{
    switch (section) {
    case 0:
        return QStringLiteral("电源");
    case 1:
        return QStringLiteral("KEYON唤醒源");
    case 2:
        return QStringLiteral("CAN唤醒源");
    case 3:
        return QStringLiteral("ETH_V3P3");
    case 4:
        return QStringLiteral("V3P3_UP");
    case 5:
        return QStringLiteral("VDD_1V25");
    case 6:
        return QStringLiteral("VDD_CNN_0V9");
    case 7:
        return QStringLiteral("VDD_0V9");
    case 8:
        return QStringLiteral("VDDQ_DDR_1V1");
    case 9:
        return QStringLiteral("VDDA_1V8");
    case 10:
        return QStringLiteral("VDD_DDR_1V0");
    case 11:
        return QStringLiteral("VDD_CORE_A0");
    case 12:
        return QStringLiteral("V3P3_J2");
    case 13:
        return QStringLiteral("SYS_IO_1P8V");
    case 14:
        return QStringLiteral("板上温度");
    case 15:
        return QStringLiteral("J2温度");
    case 16:
        return QStringLiteral("J2温度墙");
    case 17:
        return QStringLiteral("J2工作状态");
    case 18:
        return QStringLiteral("限功率状态");
    case 19:
        return QStringLiteral("CAN2");
    case 20:
        return QStringLiteral("CAN3");
    case 21:
        return QStringLiteral("CPU");
    case 22:
        return QStringLiteral("BPU");
    }
}

QVariant EmcResultModel::vHeaderInfo(int section) const
{
    auto iter = m_model.begin();
    while(iter != m_model.end() && section > 0){
        iter++;
        section--;
    }
    if(iter == m_model.end()){
        return QVariant();
    }
    return iter.key();
}

QVariant EmcResultModel::getValueFromModel(int row, int column) const
{
    QString key = vHeaderInfo(row).toString();
    QString prop = hHeaderInfo(column).toString();
    if(m_model.contains(key)
            && m_model[key].contains(prop)
            && m_model[key][prop].contains("error"))
        return m_model[key][prop]["error"];
    return QVariant();
}

QVariant EmcResultModel::getColorFromModel(int row, int column) const
{
    QString key = vHeaderInfo(row).toString();
    QString prop = hHeaderInfo(column).toString();
    if(m_model.contains(key)
            && m_model[key].contains(prop)
            && m_model[key][prop].contains("state")){
        QColor bkColor = m_model[key][prop]["state"].toBool() ?
                    QColor(Qt::green) : QColor(Qt::red);
        return bkColor;
    }
    return QColor(Qt::white);
}
