#include "sevent.h"


SEvent::SEvent(QEvent::Type type)
    : QEvent(type)
{

}

QVariantMap &SEvent::mutableParams()
{
    return m_params;
}

const QVariantMap &SEvent::params() const
{
    return m_params;
}

void SEvent::setParam(const QString &name, const QVariant &value)
{
    m_params[name] = value;
}
