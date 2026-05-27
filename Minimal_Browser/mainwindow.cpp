//Minimal Browser is a C++ and QT5 browser.
//Copyright (c) 2020 JJ Posti <techtimejourney.net>
//This is free software, and you are welcome to redistribute it under GPL Version 3 or Apache v2 license.
//This forked version is modified from original version 2.5 beta(July 2022).

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QString>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineDownloadRequest>
#include <QtWidgets>
#include <QUrl>
#include <QtPrintSupport>
#include <QPrintDialog>
#include <QPrinter>
#include <QPageLayout>
#include <QCoreApplication>
#include <QProcess>
#include <form.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(QWebEngineProfile::defaultProfile(), &QWebEngineProfile::downloadRequested, this, &MainWindow::downloadRequested);

    on_tabWidget_tabBarDoubleClicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if (index > 0)
    {
        ui->tabWidget->removeTab(index);
    }
}

void MainWindow::on_tabWidget_tabBarDoubleClicked()
{
    ui->tabWidget->addTab(new Form(), QString("Tab %0").arg(ui->tabWidget->count() + 1));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() -1);
}

void MainWindow::downloadRequested(QWebEngineDownloadRequest* download)
{
    QDir downloadDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString fileName = download->suggestedFileName();
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), downloadDir.filePath(fileName));
    if (!filePath.isEmpty())
    {
        download->setDownloadFileName(filePath);
        download->accept();
    }
}
