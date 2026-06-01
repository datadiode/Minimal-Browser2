#include "form.h"
#include "ui_form.h"
#include <QFileDialog>
#include <QString>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineHistory>
#include <QtWidgets>
#include <QUrl>
#include <QtPrintSupport>
#include <QPrintDialog>
#include <QPrinter>
#include <QPageLayout>

class Form::ItemDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QAbstractItemView *const view = qobject_cast<QComboBox*>(parent())->view();
        // Use the alternate base color for the selected item to restore the visual feedback of selection which was lost when making the items click-checkable
        painter->fillRect(option.rect, QGuiApplication::palette().brush(
            view->currentIndex() == index ? QPalette::AlternateBase : QPalette::Base));
        QStyledItemDelegate::paint(painter, option, index);
    }
};

Form::Form(QStandardItemModel *model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    // Settings
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    ui->webView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    // GUI tweaks
    ui->back->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowBack));
    ui->forward->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowForward));
    ui->stop->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserStop));
    ui->reload->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
    ui->zoominus->setMinimumWidth(25);
    ui->zoomplus->setMinimumWidth(25);
    ui->addressbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->addressbar->lineEdit()->setPlaceholderText(tr("Enter an address"));
    ui->addressbar->lineEdit()->setClearButtonEnabled(true);
    ui->addressbar->setModel(model);

    // Restore item selectability which was sacrificed to make the items click-checkable
    connect(ui->addressbar->view(), &QAbstractItemView::pressed, this, &Form::onItemPressed);
    ui->addressbar->setItemDelegate(new ItemDelegate(ui->addressbar));

    // Certificate status indicator
    certificateStatus = ui->addressbar->lineEdit()->addAction(QApplication::style()->standardIcon(QStyle::SP_VistaShield), QLineEdit::LeadingPosition);
    connect(certificateStatus, &QAction::triggered, this, &Form::onCertificateStatusTriggered);

    // Page icon
    pageIcon = ui->addressbar->lineEdit()->addAction(QApplication::style()->standardIcon(QStyle::SP_FileIcon), QLineEdit::LeadingPosition);

    connect(ui->webView->page(), &QWebEnginePage::certificateError, this, &Form::onCertificateError);

    on_home_clicked();
}

Form::~Form()
{
    delete ui;
}

void Form::on_addressbar_textActivated(const QString &text)
{
    QModelIndex const index = ui->addressbar->view()->currentIndex();
    QStandardItemModel *const model = qobject_cast<QStandardItemModel*>(ui->addressbar->model());
    if (QStandardItem *const item = model->itemFromIndex(index))
    {
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(item->checkState());
    }

    QUrl url = text;
    QString part = url.scheme();
    if (part == "search")
    {
        url.setHost("duckduckgo.com");
        part = url.path();
        url.setPath("/");
        QUrlQuery query;
        query.addQueryItem("q", part);
        url.setQuery(query);
        part.clear();
    }
    if (part.isEmpty())
    {
        url.setScheme("https");
    }
    part = url.host();
    if (part.isEmpty())
    {
        url.setHost(text);
        url.setPath("/");
    }
    ui->addressbar->setItemText(ui->addressbar->currentIndex(), url.toString());
    ui->webView->load(url);
}

void Form::on_back_clicked()
{
    ui->webView->back();
    ui->addressbar->setEditText(ui->webView->url().toString());
}

void Form::on_forward_clicked()
{
    ui->webView->forward();
    ui->addressbar->setEditText(ui->webView->url().toString());
}

void Form::on_reload_clicked()
{
    ui->webView->reload();
    ui->addressbar->setEditText(ui->webView->url().toString());
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
    ui->webView->setZoomFactor(ui->webView->zoomFactor() + 0.2);
}

void Form::on_zoominus_clicked()
{
    ui->webView->setZoomFactor(ui->webView->zoomFactor() - 0.2);
}

void Form::on_home_clicked()
{
    ui->addressbar->setCurrentIndex(0);
    ui->webView->load(ui->addressbar->currentText());
}

void Form::on_webView_loadStarted()
{
    ui->stop->setDisabled(false);
    certificateStatus->setEnabled(false);
    certificateStatus->setIcon(QApplication::style()->standardIcon(QStyle::SP_VistaShield));
}

void Form::on_webView_loadFinished(bool ok)
{
    ui->stop->setDisabled(true);
    ui->back->setDisabled(!ui->webView->history()->canGoBack());
    ui->forward->setDisabled(!ui->webView->history()->canGoForward());
    if (ok)
    {
        QUrl const url = ui->webView->url();
        QString const host = url.host();
        ui->addressbar->setEditText(url.toString());
        QSettings settings;
        settings.beginGroup("knownhosts");
        if (url.scheme() == "https")
        {
            certificateStatus->setEnabled(true);
            if (settings.contains(host))
            {
                certificateStatus->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning));
            }
        }
    }
}

void Form::on_webView_iconChanged(const QIcon &icon)
{
    pageIcon->setIcon(icon.isNull() ? QApplication::style()->standardIcon(QStyle::SP_FileIcon) : icon);
}

void Form::onCertificateError(QWebEngineCertificateError certificateError)
{
    QString const host = certificateError.url().host();

    QSettings settings;
    settings.beginGroup("knownhosts");

    QList<QSslCertificate> known = QSslCertificate::fromData(settings.value(host).toByteArray());
    QList<QSslCertificate> const chain = certificateError.certificateChain();
    QString detailedText;
    for (int i = 0; i < chain.count(); ++i)
    {
        QSslCertificate const cert = chain.at(i);
        if (!cert.isNull() && !known.contains(cert))
        {
            known.append(cert);
            detailedText.append("Thumbprint: ");
            detailedText.append(cert.digest(QCryptographicHash::Sha1).toHex().toUpper());
            detailedText.append("\n");
            detailedText.append(cert.toText());
            detailedText.append("\n");
        }
    }

    if (detailedText.isEmpty())
    {
        certificateError.acceptCertificate();
        return;
    }

    QMessageBox msgbox(this);
    msgbox.setIcon(QMessageBox::Warning);
    msgbox.setWindowTitle(host);
    msgbox.setText(certificateError.description());
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox.setDefaultButton(QMessageBox::No);
    msgbox.setCheckBox(new QCheckBox(tr("Remember my decision")));
    msgbox.setDetailedText(detailedText);
    if (QTextEdit *const textEdit = msgbox.findChild<QTextEdit*>())
    {
        textEdit->setFrameStyle(QFrame::NoFrame);
        textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        textEdit->setLineWrapMode(QTextEdit::NoWrap);
        textEdit->setWordWrapMode(QTextOption::NoWrap);
        QFont const font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        textEdit->setFont(font);
        QFontMetrics const metrics(font);
        textEdit->setFixedSize
        (
            96 * metrics.averageCharWidth(),
            32 * metrics.lineSpacing()
        );
    }
    msgbox.setInformativeText(tr("Do you want to ignore the error?"));

    QRect rect = msgbox.geometry();
    rect.moveTopLeft
    (
        window()->geometry().topLeft() + QPoint
        (
            QApplication::style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
            QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight) +
            QApplication::style()->pixelMetric(QStyle::PM_LayoutTopMargin)
        )
    );
    msgbox.setGeometry(rect);

    if (msgbox.exec() == QMessageBox::Yes)
    {
        QByteArray pems;
        if (msgbox.checkBox()->isChecked())
        {
            for (int i = 0; i < known.count(); ++i)
                pems += known.at(i).toPem() + '\n';
        }
        settings.setValue(host, pems);
        certificateError.acceptCertificate();
    }
}

void Form::onCertificateStatusTriggered()
{
    QString const host = ui->webView->url().host();

    QSettings settings;
    settings.beginGroup("knownhosts");

    if (settings.contains(host) &&
        QMessageBox::question(this, host, tr("Do you want to forget the stored certificate for this host?")) == QMessageBox::Yes)
    {
        settings.setValue(host, QByteArray());
    }
}

void Form::onItemPressed(const QModelIndex& index)
{
    QStandardItemModel *const model = qobject_cast<QStandardItemModel*>(ui->addressbar->model());
    if (QStandardItem *const item = model->itemFromIndex(index))
    {
        ui->addressbar->hidePopup();
        ui->addressbar->setCurrentIndex(item->row());
        emit ui->addressbar->textActivated(ui->addressbar->currentText());
    }
}
