#ifndef EMCRESULTMODEL_H
#define EMCRESULTMODEL_H

#include <QAbstractTableModel>

class EmcResultModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit EmcResultModel(QObject *parent = nullptr);

    void updateModel(const QString &name, const QMap<QString, QVariantMap>& map);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    QVariant hHeaderInfo(int section) const;
    QVariant vHeaderInfo(int section) const;
    QVariant getValueFromModel(int row, int column) const;
    QVariant getColorFromModel(int row, int column) const;

private:
    QMap<QString, QMap<QString, QVariantMap>> m_model;
};

#endif // EMCRESULTMODEL_H
