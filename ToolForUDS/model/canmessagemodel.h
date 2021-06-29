#ifndef CANMESSAGEMODEL_H
#define CANMESSAGEMODEL_H

#include "ToolForUDS_global.h"
#include "ECanVci.h"

class CanMessageModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CanMessageModel(QObject *parent = nullptr);

    // update function
    void insertData(CAN_OBJ data);

    // setting
    void setIsHex(bool enable);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    QVariant headerInfo(int section) const;
    QVariant format(const CAN_OBJ& obj, int column) const;

private:
    QVector<CAN_OBJ>    mModel;
    bool                mIsHex;
};

#endif // CANMESSAGEMODEL_H
