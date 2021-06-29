#ifndef ABSTRACTHANDLE_H
#define ABSTRACTHANDLE_H

#include <QObject>

class AbstractHandle : public QObject
{
    Q_OBJECT
public:
    explicit AbstractHandle(QObject *parent = nullptr);
    virtual ~AbstractHandle();

    virtual bool Open() = 0;
    virtual bool Close() = 0;

private:
    Q_DISABLE_COPY(AbstractHandle)
};

#endif // ABSTRACTHANDLE_H
