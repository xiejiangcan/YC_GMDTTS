#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
//#include "vld.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    if(argc > 1)
        w.slotOpenProject(argv[1]);
    else{
        QStringList lstPro;
        QSettings setting;

        lstPro = setting.value("mainwindow/recent_projects").toString().split(',');
        if(!lstPro.isEmpty()
                && !lstPro.first().isEmpty())
            w.slotOpenProject(lstPro.first());
    }
    w.show();
    return a.exec();
}
