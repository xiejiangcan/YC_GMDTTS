#include "yfileoperation.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QStandardPaths>

//const QString _tmpFilePath = QCoreApplication::applicationDirPath() + "/data/";
const QString _tmpFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        + "/EMC/";

YFileOperation::YFileOperation(QObject *parent)
    : QObject(parent)
{
    if(isDirExist(_tmpFilePath)){
        qDebug() << _tmpFilePath;
    }
    if(isDirExist(QApplication::applicationDirPath() + "/data/")){
        qDebug() << QApplication::applicationDirPath() + "/data/";
    }
}

YFileOperation::~YFileOperation()
{
    CloseFile();
}

bool YFileOperation::IsOpen()
{
    return mFile.isOpen();
}

bool YFileOperation::OpenFile(QString fileName)
{
    CloseFile();
    if(fileName.isEmpty()){
        fileName = "default.tsf";
    }
    tmpFilePath = _tmpFilePath + fileName;
    mFile.setFileName(tmpFilePath);
    return mFile.open(QFile::WriteOnly | QFile::Truncate);
}

void YFileOperation::CloseFile()
{
    if(mFile.isOpen()){
        mFile.close();
    }
}

bool YFileOperation::SaveFile(QString filepath)
{
    return QFile::copy(tmpFilePath, filepath);
}

bool YFileOperation::WriteData(QString str)
{
    if(mFile.isOpen()){
        auto data = str.toUtf8();
        auto size = mFile.write(data);
        return size == data.size();
    }
    return false;
}


bool YFileOperation::isDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists())
        return true;
    else
        return dir.mkpath(fullPath);
}

QString YFileOperation::isFileExist(QString fullPath)
{
    QFileInfo fileInfo(fullPath);
    if(fileInfo.isFile()){
        QString path = fileInfo.path();
        QString baseName = fileInfo.baseName();
        if(baseName.contains('(')){
            QStringList nameList = baseName.split('(');
            QString pro = nameList[0];
            QString tmp = nameList[1];
            nameList = tmp.split(')');
            QString back = nameList[1];
            int num = nameList[0].toInt();
            num ++;
            return isFileExist(path + "/" + pro + "("
                               + QString::number(num) + ")"
                               + back + "." + fileInfo.completeSuffix());
        }else{
            return isFileExist(path + "/" + baseName
                               + "(1)." + fileInfo.completeSuffix());
        }
   }else
        return fullPath;

}
