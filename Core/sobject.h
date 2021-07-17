#ifndef SOBJECT_H
#define SOBJECT_H

#include "core_global.h"

enum ENUM_SIGNAL_TYPE
{
    SIGNAL_CAN_MESSAGE = 0
};

class CORE_EXPORT SObject : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief The PropertyT struct
     */
    struct PropertyT
    {
        QObject*    m_object = nullptr;                  ///< 对象地址
        QString     m_property = "";                     ///< 对象属性
        PropertyT(QObject* object = nullptr,
                  const QString& property = "")
            : m_object(object),
              m_property(property)
        {}
    };

    /**
     * @brief The PropertyInfo struct
     */
    struct PropertyInfo
    {
        uint                    m_version = 0;               ///< 修改版本
        QMap<QString, QString>  m_attribute;                 ///< 属性映射
        QObject*                m_changedBy = nullptr;       ///< 修改者地址
    };

    struct RootParamT
    {
        int                                         m_timerIDForCheck = 0;  ///< 计时器ID
        QMap<SObject*, QMap<QString, PropertyT>>    m_changed;              ///< 被修改的属性容器
        QMutex                                      m_mutex;                ///< 保护锁
    };

public:
    SObject(SObject* parent = nullptr);
    virtual ~SObject();

    // gettings
    QVariant propertyEx(const QString& strPath);
    QMap<QString, PropertyInfo>& mutablePropertyInfo();
    const QMap<QString, PropertyInfo>& propertyInfo() const;
    QReadWriteLock& lock();

    // settings
    bool setPropertyP(const char* name, const QVariant& value,
                     const QObject* changedBy = nullptr);
    bool setPropertyP(const QString& name, const QVariant& value,
                     const QObject* changedBy = nullptr);
    void setPropertyS(const QString& name, const QVariant &value,
                      const QObject* changedBy = nullptr);
    bool setPropertyEx(const QString& strPath, const QVariant& varValue,
                       const QObject* changedBy = nullptr);
    void setPropertyByText(const QString& name, const QString& value,
                           const QObject* changedBy = nullptr);
    void setPropertyInfo(const QMap<QString, PropertyInfo>& propertyInfo);
    void setSObjectName(const QString& name);
    void setAsRoot(bool bRoot);

    // function
    void changeProperty(const QString& strPropName,
                        const QObject* pFromObj,
                        const QString& strFromProp = "");
    SObject* find(const QString& strPath);
    SObject* clone() const;
    QDomElement toXML(const QString& strRootName = "object");
    void fromXML(const QDomElement& eRoot);
    bool saveFile(const QString& strFileName);
    bool openFile(const QString& strFileName,
                  QMap<QString, QVariantMap>* pIDs = nullptr);
    void updateProperty(const QString& strPropName);

    // static function
    static void toXML(QDomElement& eParent, const SObject& obj);
    static void fromXML(SObject& obj, const QDomElement& eParent,
                        QMap<QString, QVariantMap>* pIDs = nullptr);
    static QVariant stringToVariant(const QString& strTypeName,
                                    const QString& strValue);

    static QString variantToString(const QVariant& varValue,
                                   QString &strTypeName);
    static void clone(SObject& des, const SObject &src);
    static QStringList supportedTypes();

protected:
    void timerEvent(QTimerEvent *event) override;
    void customEvent(QEvent *event) override;

    SObject* root();

signals:
    void signalNotifed(uint signType, QVariant data);
    void propertyChanged(SObject* pObj, const QString& strPropName,
                         const SObject::PropertyT& propChangedBy);
private:
    QMap<QString, PropertyInfo> m_propertyInfo;         ///< 属性信息
    QReadWriteLock              m_lock;                 ///< 读写锁
    RootParamT*                 m_rootParam = nullptr;  ///< 根对象才有的参数
};
Q_DECLARE_METATYPE(SObject::PropertyT);
#endif // SOBJECT_H
