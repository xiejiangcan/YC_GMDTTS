#ifndef YFILEOPERATION_H
#define YFILEOPERATION_H

#include <QObject>
#include <QFile>

class YFileOperation : public QObject
{
    Q_OBJECT
public:
    explicit YFileOperation(QObject *parent = nullptr);
    ~YFileOperation();

    // func
    bool IsOpen();
    bool OpenFile(QString fileName = "");
    void CloseFile();
    bool SaveFile(QString filepath);

    bool WriteData(QString str);

protected:
    bool isDirExist(QString fullPath);

    QString isFileExist(QString fullPath);

private:
    QFile mFile;
    QString tmpFilePath;
};

#endif // YFILEOPERATION_H
