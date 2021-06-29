﻿#ifndef TOOLFORUDS_GLOBAL_H
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

#if defined(TOOLFORUDS_LIBRARY)
#  define TOOLFORUDS_EXPORT Q_DECL_EXPORT
#else
#  define TOOLFORUDS_EXPORT Q_DECL_IMPORT
#endif

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

#define MSG_BUFF            "msg_buff"
#define BUFF_NUM            "buff_num"
#define BUFF_               "buff_"

#endif // TOOLFORUDS_GLOBAL_H
