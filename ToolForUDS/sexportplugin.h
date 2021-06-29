#ifndef SEXPORTPLUGIN_H
#define SEXPORTPLUGIN_H

#include "ToolForUDS_global.h"

class TOOLFORUDS_EXPORT SExportPlugin : public QObject , public SPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(SPluginInterface)
    Q_PLUGIN_METADATA(IID "yc.mgdtts.interface/1.0")
public:
    QStringList keys() const override;

    QObject * create(const QString &strKey, SMainWindow *pMainWindow, QObject *pParent) override;

    SObject * sobject(const QString &strKey, SObject *pParent) override;
};

#endif // SEXPORTPLUGIN_H
