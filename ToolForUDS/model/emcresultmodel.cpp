#include "emcresultmodel.h"
#include <QColor>
#include <QTime>

EmcResultModel::EmcResultModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_timer.start(1000);
    connect(&m_timer, &QTimer::timeout, this, &EmcResultModel::slotTimeout);
}

void EmcResultModel::updateModel(const QString &name, const QMap<QString, QVariantMap> &map)
{
    if(m_timeoutList.contains(name)){
        m_timeoutList.removeOne(name);
    }
    if(!m_model.contains(name)){
        m_timeMaskMap[name] = 0;
    }
    beginResetModel();
    m_model[name] = map;
    endResetModel();
}

void EmcResultModel::setDevTimeout(const QString &name)
{
    if(m_model.contains(name)){
        beginResetModel();
        m_timeoutList.append(name);
        endResetModel();
    }
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
        case Qt::BackgroundColorRole:
            return vHeaderColor(section);
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

    return 20;
}

QVariant EmcResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return getValueFromModel(index.row(), index.column());
    case Qt::BackgroundColorRole:
        return getColorFromModel(index.row(), index.column());
    }
    return QVariant();
}

QString EmcResultModel::formateFromSecond(uint second)
{
    uint temp, day, hour, min, sec;
    day = second/(24*60*60);
    temp = second%(24*60*60);
    hour = temp/(60*60);
    temp = temp%(60*60);
    min = temp/60;
    sec = temp%60;

    return QString("%1-%2-%3-%4")
            .arg(day,2).arg(hour,2).arg(min,2).arg(sec,2);
}

QVariant EmcResultModel::hHeaderInfo(int section) const
{
    switch (section) {
    case 0:
        return QStringLiteral("计时器");
    case 1:
        return QStringLiteral("电源");
    case 2:
        return QStringLiteral("KEYON唤醒源");
    case 3:
        return QStringLiteral("CAN唤醒源");
    case 4:
        return QStringLiteral("ETH_V3P3");
    case 5:
        return QStringLiteral("V3P3_UP");
    case 6:
        return QStringLiteral("VDD_1V25");
    case 7:
        return QStringLiteral("VDD_CNN_0V9");
    case 8:
        return QStringLiteral("VDD_0V9");
    case 9:
        return QStringLiteral("VDDQ_DDR_1V1");
    case 10:
        return QStringLiteral("VDDA_1V8");
    case 11:
        return QStringLiteral("VDD_DDR_1V0");
    case 12:
        return QStringLiteral("VDD_CORE_A0");
//    case 12:
//        return QStringLiteral("V3P3_J2");
    case 13:
        return QStringLiteral("SYS_IO_1P8V");
    case 14:
        return QStringLiteral("板上温度");
    case 15:
        return QStringLiteral("J2温度");
//    case 16:
//        return QStringLiteral("J2温度墙");
//    case 17:
//        return QStringLiteral("J2工作状态");
//    case 14:
//        return QStringLiteral("限功率状态");
    case 16:
        return QStringLiteral("CAN2");
    case 17:
        return QStringLiteral("CAN3");
    case 18:
        return QStringLiteral("CPU");
    case 19:
        return QStringLiteral("BPU");
    default:
        return QStringLiteral("");
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

QVariant EmcResultModel::vHeaderColor(int section) const
{
    QString title = vHeaderInfo(section).toString();
    if(m_timeoutList.contains(title)){
        return QColor(Qt::red);
    }
    return QVariant();
}

QVariant EmcResultModel::getValueFromModel(int row, int column) const
{
    QString key = vHeaderInfo(row).toString();
    if(column == 0){
        uint second = m_timeMaskMap[key].toUInt();
        QString res = formateFromSecond(second);
        return res;
    }
    QString prop = hHeaderInfo(column).toString();
    if(m_model.contains(key)
            && m_model[key].contains(prop)
            && m_model[key][prop].contains("error"))
        return m_model[key][prop]["error"];
    return QVariant();
}

QVariant EmcResultModel::getColorFromModel(int row, int column) const
{
    if(column == 0)
        return QColor(Qt::white);
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

void EmcResultModel::slotTimeout()
{
    auto begin = m_timeMaskMap.begin();
    auto end = m_timeMaskMap.end();
    while(begin != end){
        int time = begin.value().toInt() + 1;
        (*begin).setValue(time);
        ++begin;
    }
    beginResetModel();
    endResetModel();
}

