#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "smainwindow.h"

class MainWindow : public SMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
