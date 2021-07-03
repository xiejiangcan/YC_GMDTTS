#ifndef CORE_GLOBAL_H
#define CORE_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QMap>
#include <QMutex>
#include <QReadWriteLock>
#include <QTimerEvent>
#include <QEvent>
#include <QMetaType>
#include <QPluginLoader>
#include <QScriptEngine>
#include <QThread>
#include <QDebug>
#include <QTime>
#include <QCoreApplication>
#include <QApplication>
#include <QFile>
#include <QTextCodec>
#include <QBuffer>
#include <QVariant>
#include <QDir>
#include <QCloseEvent>
#include <QSettings>
#include <QVector>

// ui
#include <QWidget>
#include <QMainWindow>
#include <QMenu>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QDockWidget>
#include <QDialog>
#include <QStatusBar>
#include <QScrollArea>
#include <QMenuBar>
#include <QGroupBox>
#include <QSplitter>
#include <QGridLayout>
#include <QFormLayout>
#include <QStackedLayout>
#include <QMessageBox>
#include <QTreeWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QComboBox>

#if defined(CORE_LIBRARY)
#  define CORE_EXPORT Q_DECL_EXPORT
#else
#  define CORE_EXPORT Q_DECL_IMPORT
#endif

class SMainWindow;
class SObject;

class CORE_EXPORT SPluginInterface
{
public:
    /**
     * @brief Destructor
     *
     */
    virtual ~SPluginInterface() { }

    /**
     * @brief Get all of the key in this plugin
     *
     * @return QStringList  Keys' list
     */
    virtual QStringList keys() const = 0;

    /**
     * @brief Create an object from the plugin
     *
     * @param strKey        The key specify the key which be created class. It references the returnned keys from function keys().
     * @param pMainWindow   The main window handle
     * @param pParent       The parent object handle
     * @return QObject      Created object handle. If it's failed. It's NULL.
     */
    virtual QObject* create(const QString &strKey, SMainWindow *pMainWindow, QObject* pParent) = 0;

    /**
     * @brief Create a SObject from the plugin
     *
     * @param strKey    The key specify the key which be created SObject. It references the returnned keys from function keys().
     * @param pParent   The parent SObject handle
     * @return SObject  Created SObject handle. If it's failed. It's NULL.
     */
    virtual SObject* sobject(const QString &strKey, SObject* pParent) = 0;
};
Q_DECLARE_INTERFACE(SPluginInterface, "yc.mgdtts.interface/1.0")

// script attr
#define STR_TYPE_W      "_w"               ///< widget
#define STR_TYPE_P      "_P"               ///< project
#define STR_TYPE_O      "_O"               ///< object

// instance name
#define STR_PROROOT     "project_root"
#define STR_ACTION      "action"
#define STR_CENTRAL     "central"
#define STR_DOCK        "dock"
#define STR_MENU        "menu"
#define STR_ITEM        "item"
#define STR_CHILDREN    "children"
#define STR_POPWIDGET   "pop_widget"
#define STR_STATUSBAR   "statusbar"
#define STR_TOOLBAR     "toolbar"
#define STR_PROJECT     "project"
#define STR_OBJ         "object"
#define STR_SOBJ        "sobject"
#define STR_ICON        "icon"
#define STR_CODE        "code"
#define STR_SIGNAL      "signal"
#define STR_BOARD       "board"
#define STR_PART        "part"
#define STR_COM         "communication"
#define STR_COMMAND     "command"
#define STR_RES         "resource"
#define STR_UI          "ui"
#define STR_COM_ROOT    "communication_root"
#define STR_ICON_ROOT   "icon_root"
#define STR_ACTION_ROOT "action_root"
#define STR_MENU_ROOT   "menu_root"
#define STR_DOCK_ROOT   "dock_root"
#define STR_DATA_ROOT   "data_root"
#define STR_BORAD_ROOT  "board_root"
#define STR_PART_ROOT   "part_root"
#define STR_SIGNAL_ROOT "signal_root"
#define STR_POPWIDGET_ROOT  "pop_widget_root"
#define STR_MENU_ITEM   "menu_item"
#define STR_SUB_MENU    "sub_menu"
#define STR_WIDGET_CHILDREN "widget_children"
#define STR_CENTRAL_WIDGET  "central_widget"
#define STR_REMOVABLE_PARAM    "removable_param"

// property name
#define STR_AUTHOR      "author"
#define STR_PATH        "path"
#define STR_COMMENT     "comment"

#define STR_SW_FLAG     "_is_swidget"
#define STR_UI_CLOSE    "_ui_closable"
#define STR_UI_TITLE    "_ui_title"
#define STR_UI_FLOAT    "_ui_floating"
#define STR_UI_AREA     "_ui_area"
#define STR_UI_ICON     "_ui_icon"
#define STR_UI_SHOW     "_ui_show"
#define STR_UI_VISIBLE  "_ui_visible"
#define STR_UI_STR      "_ui_stretch"
#define STR_UI_LEFT     "_ui_left"
#define STR_UI_INDEX    "_ui_index"
#define STR_UI_CLASS    "_ui_class"
#define STR_UI_ALIGN    "_ui_alignment"
#define STR_UI_LAYOUT   "_ui_layout"
#define STR_UI_LAYOUT_P "_ui_layout_param"
#define STR_UI_BACKIMG  "_ui_back_image"
#define STR_UI_MINW     "_ui_min_width"
#define STR_UI_MINH     "_ui_min_height"
#define STR_UI_ALLOW_AREAS  "_ui_allowed_areas"
#define STR_UI_ORIENTATION  "_ui_orientation"

#define STR_PLUGIN      "__plugin"
#define STR_PTR_W       "__widget"
#define STR_PTR_OBJ     "__sobj"
#define STR_ID          "__id"
#define STR_INS_TYPE    "__type"

#define STR_ATTR        "attribute"
#define STR_PROP        "property"
#define STR_DATA        "data"
#define STR_TYPE        "type"
#define STR_VALUE       "value"
#define STR_DATASOURCE  "DataSource"
#define STR_DATAOUT     "DataOut"
#define STR_PARAMOUT    "ParamOut"
#define STR_SIGNALOUT   "signalOut"
#define STR_RESULTOUT   "resultOut"
#define STR_CHECKABLE   "checkable"
#define STR_CHANGEDBY   "changed_by"
#define STR_ROLE        "role"
#define STR_LIMIT       "limit"
#define STR_TIP         "tip"
#define STR_ERROR       "error"
#define STR_FUNCTION    "function"
#define STR_PARAMS      "params"
#define STR_ROLE_PARAM  "role_param"
#define STR_ACT_REL     "release_action"
#define STR_ACT_INIT    "init_action"
#define STR_PATH        "path"
#define STR_NAME        "name"
#define STR_LAYOUT      "layout"
#define STR_ENABLE      "enable"
#define STR_SHOW_STATUSBAR  "show_statusbar"
#define STR_TIMER_INTER     "timer_interval"
#define STR_TIMER_COUNT     "timer_counter"
#define STR_ACTION_TYPE     "action_type"
#define STR_ACTION_ICON     "action_icon"
#define STR_ACTION_CODE     "action_code"

#define STR_TOOLBAR_VISIBLE     "toolbar_visible"
#define STR_STATUSBAR_VISIBLE   "statusbar_visible"

// property value
// _ui_area
#define STR_ALL         "all"
#define STR_RIGHT       "right"
#define STR_LEFT        "left"
#define STR_TOP         "top"
#define STR_BOTTOM      "bottom"
// show
#define STR_MAX         QStringLiteral("max")
#define STR_MIN         QStringLiteral("min")
#define STR_FULL        QStringLiteral("full_screen")
// __type
#define STR_INSTANCE    QStringLiteral("instance")
#define STR_CONTAINER   QStringLiteral("container")
// _ui_class
#define STR_WIDGET      QStringLiteral("widget")
#define STR_GROUP       QStringLiteral("group")
#define STR_SCROLL      QStringLiteral("scroll")
#define STR_DIALOG      QStringLiteral("dialog")
#define STR_SPLIT       QStringLiteral("split")
#define STR_TAB         QStringLiteral("tab")
// _ui_alignment
#define STR_ALIGN_LEFT      QStringLiteral("align_left")
#define STR_ALIGN_RIGHT     QStringLiteral("align_right")
#define STR_ALIGN_CENTER    QStringLiteral("align_center")
#define STR_ALIGN_NORTH     QStringLiteral("align_north")
#define STR_ALIGN_SOUTH     QStringLiteral("align_south")
#define STR_ALIGN_WEST      QStringLiteral("align_west")
#define STR_ALIGN_EAST      QStringLiteral("align_east")
// _ui_layout_params
#define STR_LAYOUT_COL      QStringLiteral("layout_col")
#define STR_LAYOUT_ROW      QStringLiteral("layout_raw")
// _ui_layout
#define STR_LAYOUT_GRID     QStringLiteral("layout_grid")
#define STR_LAYOUT_VBOX     QStringLiteral("layout_vbox")
#define STR_LAYOUT_HBOX     QStringLiteral("layout_hbox")
#define STR_LAYOUT_FORM     QStringLiteral("layout_form")
#define STR_LAYOUT_STACK    QStringLiteral("layout_stack")
#define STR_LAYOUT_GEOMETRY QStringLiteral("layout_geometry")
#define STR_LAYOUT_SPLIT    QStringLiteral("layout_split")
#define STR_LAYOUT_TAB      QStringLiteral("layout_tab")
// ui size type
#define STR_SIZE_FIX        "size_fixed"
// role
#define STR_ROLE_ACTION     "action"
#define STR_ROLE_MAPPING    "mapping"
#define STR_ROLE_IMAGE      "image"
// enable
#define STR_BOOL_TRUE       "true"
#define STR_BOOL_FALSE      "false"
// type
#define STR_TYPE_BOOL       "bool"
#define STR_TYPE_INT        "int"
#define STR_TYPE_UINT       "uint"
#define STR_TYPE_DOUB       "double"
#define STR_TYPE_CHAR       "char"
#define STR_TYPE_QSTR       "QString"
#define STR_TYPE_FLOAT      "float"
#define STR_TYPE_QLL        "qlonglong"
#define STR_TYPE_SHORT      "short"
#define STR_TYPE_QULL       "qulonglong"
#define STR_TYPE_USHORT     "ushort"
#define STR_TYPE_UCHAR      "uchar"
#define STR_TYPE_QIMG       "qimage"
#define STR_TYPE_QPIXMAP    "qPixmap"
#define STR_TYPE_QBYTEARRAY "QByteArray"

#define STR_DEFAULT         ""
#define STR_CUT_CHAR        QStringLiteral("@")
#define CUT_CHAR            '@'

#define STR_LAYOUTS         "grid,vbox,hbox,stack,form,geometry"
#define STR_AREAS           "left,right,bottom,top"
#define STR_ORIENTATIONS    "horizontal,vertical"
// orientations
#define STR_HORIZONTAL      "horizontal"
#define STR_VERTICAL        "vertical"

// role value
#define STR_ROLE_SELECTMUL  "select_mul"
#define STR_ROLE_SELECTSIG  "select_signle"
#define STR_ROLE_LAYOUT_P   "layout_param"
#define STR_ROLE_LAYOUT_V   "layout_value"
#define STR_MAP_IMAGE       "map2image"
#define STR_MAP_ACTION      "map2action"
#define STR_MAP_OBJECT      "map2object"
#define STR_MAP_PROPERTY    "map2property"

#define STR_CURRENT_ITEM    "_current_item"
#define MAIN_WND_NAME       "mainwindow"
#define RECENT_PROJECTS     "recent_projects"

template <class T>
/**
 * @brief
 *
 */
struct SDataBuffer{
    quint64 capacity; /**< TODO */
    quint64 size; /**< TODO */
    T* data; /**< TODO */

    /**
     * @brief
     *
     */
    SDataBuffer(){
        capacity = 0;
        size = 0;
        data = NULL;
    }
    /**
     * @brief
     *
     */
    void release(){
        if(capacity > 0
                && data != NULL)
            free(data);
        capacity = 0;
        size = 0;
        data = NULL;
    }
    /**
     * @brief
     *
     * @param uSize
     */
    void resize(quint64 uSize){
        if(capacity >= uSize){
            size = uSize;
            return;
        }
        release();
        data = (T*)malloc(sizeof(T) * uSize);
        capacity = uSize;
        size = uSize;
    }
    /**
     * @brief
     *
     */
    void zero(){
        memset(data, 0, size * sizeof(T));
    }
};

template <class T>
struct ThreadSafeBuffer{
    volatile quint64 capacity; /**< TODO */
    volatile quint64 mSize; /**< TODO */
    T* data; /**< TODO */

    ThreadSafeBuffer(){
        capacity = 1000;
        mSize = 0;
        data = (T*)malloc(sizeof(T) * capacity);
    }

    int size()
    {
        return mSize;
    }

    void release(){
        if(capacity > 0
                && data != NULL)
            free(data);
        capacity = 0;
        mSize = 0;
        data = NULL;
    }

    void resize(quint64 uSize){
        if(capacity >= uSize){
            mSize = uSize;
            return;
        }
        release();
        data = (T*)malloc(sizeof(T) * uSize);
        capacity = uSize;
        mSize = uSize;
    }
    bool append(T newData){
        if(data == NULL){
            capacity = 1000;
            mSize = 1;
            data = (T*)malloc(sizeof(T) * capacity);
            memcpy(data, &newData, sizeof(T));
            return true;
        }else{
            if(mSize>=capacity)
                return false;
            else{
                memcpy(data+mSize, &newData, sizeof(T));
                mSize++;
                return true;
            }
        }
    }

    bool insert(int index,T newData)
    {
        if(index< 0)
            return false;
        if(data == NULL){
            capacity = 1000;
            mSize = 1;
            data = (T*)malloc(sizeof(T) * capacity);
            memcpy(data, &newData, sizeof(T));
            return true;
        }else{
            memcpy(data+index, &newData, sizeof(T));
            return true;
        }
    }

    T first()
    {
        return data[0];
    }

    bool removeFirst()
    {
        if(data == NULL || (mSize<=0))
            return false;
        mSize--;
        memmove(data, data+1, sizeof(T)*mSize);
        return true;
    }

    void zero(){
        memset(data, 0, mSize * sizeof(T));
    }

    bool isEmpty()
    {
        if(mSize== 0)
            return true;
        return false;
    }
};
template <typename InterfaceType>
/**
 * @brief
 *
 * @param strPluginDir
 * @param QMap<QString
 * @param mapPlugin
 * @return int
 */
int loadPlugin(const QString& strPluginDir, QMap<QString, InterfaceType*>& mapPlugin)
{
    QDir pluginsDir(strPluginDir);
    InterfaceType *pInterface;

    foreach (QString fileName, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
       loadPlugin(strPluginDir + "/" + fileName, mapPlugin);
    }
    pluginsDir.setNameFilters(QStringList() << "*.dll"<< "*.a" << "*.so");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
       QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
       if ((pInterface =
                   qobject_cast<InterfaceType *>(loader.instance())) != NULL)
       {
           QStringList lstIDs = pInterface->ids();
           for(int nIndex = 0; nIndex < lstIDs.size(); nIndex++)
           {
               mapPlugin[lstIDs[nIndex]] = pInterface;
           }
       }
    }
    return mapPlugin.size();
}

/**
 * @brief
 *
 * @param pObj
 * @param strType
 * @param strFileName
 * @param strComment
 * @param strAuthor
 * @return bool
 */
bool CORE_EXPORT saveSObject(
        SObject *pObj,
        const QString& strType,
        const QString& strFileName,
        const QString& strComment = "",
        const QString& strAuthor = ""
        );

/**
 * @brief
 *
 * @param strFileName
 * @param strType
 * @param strComment
 * @param strAuthor
 * @param parent
 * @return SObject
 */
CORE_EXPORT SObject* readSObject(
        const QString& strFileName,
        QString& strType,
        QString& strComment,
        QString& strAuthor,
        SObject *parent = NULL
        );

/**
 * @brief
 *
 * @param strName
 * @return bool
 */
bool CORE_EXPORT isValidName(const QString& strName);

/**
 * @brief
 *
 * @param strSrc
 * @param strName
 * @param strType
 * @param uID
 * @return int
 */
int CORE_EXPORT splitNameTypeID(const QString& strSrc, QString& strName, QString& strType, uint& uID);

/**
 * @brief
 *
 * @param pSrcObj
 * @param szIDName
 * @param strType
 * @param QMap<QString
 * @param mapExist
 * @param bInstance
 * @return QString
 */
QString CORE_EXPORT assignID(QObject* pSrcObj, const char* szIDName, const QString& strType, QMap<QString, QVariantMap>& mapExist, bool bInstance = false);
/**
 * @brief
 *
 * @param pSrcObj
 * @param QMap<QString
 * @param mapDes
 * @param strType
 */
void CORE_EXPORT analyseObjectID(QObject* pSrcObj, QMap<QString, QVariantMap>& mapDes, const QString strType = "sobject");

/**
 * @brief
 *
 * @param vl
 * @param ba
 */
void CORE_EXPORT vl2ba(const QVariantList& vl, QByteArray& ba);
/**
 * @brief
 *
 * @param ba
 * @param vl
 */
void CORE_EXPORT ba2vl(const QByteArray& ba, QVariantList& vl);

void CORE_EXPORT addWidgetParam(SObject *pSrc);

CORE_EXPORT SObject* createContainerSObject(const QString& strType, const QString& strName, QMap<QString, QVariantMap> &mapIDs, SObject* pParent = NULL);

CORE_EXPORT SObject* createProjectSObject(QMap<QString, QVariantMap> &mapIDs, const QString& strProjectName = "New Project");




#endif // CORE_GLOBAL_H
