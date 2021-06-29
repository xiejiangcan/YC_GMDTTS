#ifndef SEVENT_H
#define SEVENT_H

#include "core_global.h"

class CORE_EXPORT SEvent : public QEvent
{
public:
    SEvent(Type type);

    // gettings
    QVariantMap& mutableParams();
    const QVariantMap& params() const;

    // settings
    void setParam(const QString& name, const QVariant& value);

private:
    QVariantMap m_params;
};

#endif // SEVENT_H
