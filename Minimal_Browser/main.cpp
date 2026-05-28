#include "mainwindow.h"
#include <QApplication>

#define HEREDOC(text) #text

static const char styleSheet[] = HEREDOC
(
    QWidget[cssClass='clickme']
    {background-color:#eeeeee;color:#000000;border-radius:5px;padding:2px;}
    QWidget[cssClass='clickme']:hover
    {background-color:#abcdef;}
    QWidget[cssClass='feedme']
    {border-style:solid;border-width:1px;border-color:#cccccc;border-radius:5px;}
);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("datadiode");
    QCoreApplication::setApplicationName("Minimal Browser");
    QCoreApplication::setApplicationVersion("2.5");
    a.setStyleSheet(styleSheet);

    MainWindow w;
    w.show();

    return a.exec();
}
