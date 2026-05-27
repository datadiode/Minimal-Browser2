#include "form.h"
#include "ui_form.h"
#include <QFileDialog>
#include <QKeyEvent>
#include <QString>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QtWidgets>
#include <QUrl>
#include <QtPrintSupport>
#include <QPrintDialog>
#include <QPrinter>
#include <QPageLayout>
#include <QCoreApplication>
#include <QProcess>

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    //Settings
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled,true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled,true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::SpatialNavigationEnabled,true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled,true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages,true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled,true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled,true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::WebGLEnabled,true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows,true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    //GUI tweaks
    ui->back->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowBack));
    ui->forward->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowForward));
    ui->stop->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserStop));
    ui->reload->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
    ui->zoominus->setMinimumWidth(25);
    ui->zoomplus->setMinimumWidth(25);

    on_home_clicked();
}

Form::~Form()
{
    delete ui;
}

void Form::on_addressbar_returnPressed()
{
    QUrl url = ui->addressbar->text();
    QString part = url.scheme();
    url.setScheme("https");
    if (part == "search")
    {
        url.setHost("duckduckgo.com");
        part = url.path();
        url.setPath("/");
        QUrlQuery query;
        query.addQueryItem("q", part);
        url.setQuery(query);
    }
    ui->webView->load(url);
}

void Form::on_back_clicked()
{
    ui->webView->back();
    ui->addressbar->setText(ui->webView->url().toString());

}

void Form::on_forward_clicked()
{
    ui->webView->forward();
    ui->addressbar->setText(ui->webView->url().toString());

}

void Form::on_reload_clicked()
{
    ui->webView->reload();
    ui->addressbar->setText(ui->webView->url().toString());

}

void Form::on_print_clicked()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() != QDialog::Accepted) return;
    ui->webView->print(&printer);
}

void Form::on_stop_clicked()
{
    ui->webView->stop();
}

void Form::on_zoomplus_clicked()
{
     ui->webView->setZoomFactor(ui->webView->zoomFactor()+.2);
}

void Form::on_zoominus_clicked()
{
    ui->webView->setZoomFactor(ui->webView->zoomFactor()-.2);
}

void Form::on_home_clicked()
{
    ui->webView->load(QUrl("https://duckduckgo.com/"));
    ui->addressbar->setText(ui->webView->url().toString());
}

void Form::on_webView_loadFinished(bool arg1)
{
    if (arg1)
    {
        ui->addressbar->setText(ui->webView->url().toString());
    }
}
