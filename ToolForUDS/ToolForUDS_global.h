#ifndef TOOLFORUDS_GLOBAL_H
#define TOOLFORUDS_GLOBAL_H

#include <QtCore/qglobal.h>

// core
#include "core_global.h"
#include "swidget.h"
#include "sobject.h"
#include "smainwindow.h"
#include "sthread.h"

// third lib
#include "ECanVci.h"

// qt
#include <QTimerEvent>
#include <QListWidget>
#include <QTableView>
#include <QDateTime>
#include <QRadioButton>
#include <QCheckBox>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QSpacerItem>


#if defined(TOOLFORUDS_LIBRARY)
#  define TOOLFORUDS_EXPORT Q_DECL_EXPORT
#else
#  define TOOLFORUDS_EXPORT Q_DECL_IMPORT
#endif

#define EMC_WIDGET  "emc_widget"
#define EMC_TABLE   "emc_table"
#define CAN_MESSAGE "can_message"
#define LOG_LIST    "log_list"
#define USB_CAN     "usb_can"
#define UDS_WIDGET  "uds_widget"

#define LOG_SAVE        "log_save"
#define LOG_SAVE_FILE   "save_file"

#define IS_HEX          "is_hex"
#define IS_AUTO         "auto_scroll"

#define CAN_DEVICE_TYPE     "device_type"
#define CAN_CHANNEL         "can_channel"
#define CAN_BAUD            "can_baud"
#define CAN_HANDLELST       "can_handlelst"
#define CAN_OPENSTATE       "can_openstate"

#define MSG_BUFF            "msg_buff"
#define RECEIVE_BUFF       "receive_buff"
#define SEND_BUFF_NUM       "send_buff_num"
#define SEND_BUFF_          "send_buff_"

#define SERVER_FILE         "server_file"
#define MAP_CAN_ID          "map_can_id"
#define LAST_CAN_ID         "last_can_id"
#define LAST_DATA           "last_data"

#define EMC_FILEPATH        "emc_filepath"
#define EMC_DATAFREQ        "emc_dataFreq"
#define EMC_DEVIND          "emc_devind"
#define EMC_DEVCHAN         "emc_devchan"
#define EMC_RESULT          "emc_result"


#endif // TOOLFORUDS_GLOBAL_H
