#ifndef UDSCANIDMAPMODEL_H
#define UDSCANIDMAPMODEL_H

#include <QAbstractTableModel>
#include <QMap>

class UdsCanIdMapModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit UdsCanIdMapModel(QObject *parent = nullptr);

    // init
    void initData(const QMap<QString, QVariant>& map);

    // getting
    QMap<QString, QVariant> getModel() const;

    // setting
    void insertNewData(const QString& key = "", const QString& value = "");

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

protected:
    QVariant headerInfo(int section) const;

private:
    QMap<int, QPair<QString, QString>> mModel;
};

#endif // UDSCANIDMAPMODEL_H
