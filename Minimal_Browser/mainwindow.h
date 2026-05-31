#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QWebEngineDownloadRequest;
class QStandardItemModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_tabWidget_tabCloseRequested(int index);
    void on_tabWidget_tabBarDoubleClicked();
    void downloadRequested(QWebEngineDownloadRequest* download);

private:
    QStandardItemModel *const model;
    Ui::MainWindow *const ui;
};

#endif // MAINWINDOW_H
