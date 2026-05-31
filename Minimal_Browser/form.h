#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QWebEngineCertificateError>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

private slots:
    void on_addressbar_textActivated(const QString &text);
    void on_back_clicked();
    void on_forward_clicked();
    void on_reload_clicked();
    void on_print_clicked();
    void on_stop_clicked();
    void on_zoomplus_clicked();
    void on_zoominus_clicked();
    void on_home_clicked();
    void on_webView_loadStarted();
    void on_webView_loadFinished(bool ok);
    void on_webView_iconChanged(const QIcon &icon);
    void onCertificateError(QWebEngineCertificateError certificateError);
    void onCertificateStatusTriggered();

private:
    Ui::Form *ui;
    QAction *certificateStatus;
    QAction *pageIcon;
};

#endif // FORM_H
