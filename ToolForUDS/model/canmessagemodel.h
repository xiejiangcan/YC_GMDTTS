#ifndef CANMESSAGEMODEL_H
#define CANMESSAGEMODEL_H

#include "ToolForUDS_global.h"
#include "tool/ycanhandle.h"
#include <QTimer>

class CanMessageModel : public QAbstractTableModel
{
    Q_OBJECT
    struct ModelData{
        QString devID;
        QString type;
        CAN_OBJ obj;
    };

public:
    explicit CanMessageModel(QObject *parent = nullptr);

    // update function
    void insertData(CAN_OBJ data, QString type, QString devID = "default dev");
    void insertData(const CAN_MESSAGE_PACKAGE& msg);

    void updateModel();

    // setting
    void setIsHex(bool enable);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool saveToFile(QString filepath);

protected:
    QVariant headerInfo(int section) const;
    QVariant format(const ModelData &obj, int column) const;

private:
    QVector<ModelData>      mModel;
    bool                    mIsHex = true;
    int                     mFreshCount = 100;
    QTimer                  mTimer;
};

#endif // CANMESSAGEMODEL_H
