/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "webengineprintmessagebox.h"
#include "webengineviewer_debug.h"
#include "webengineexporthtmlpagejob.h"
#include <webengineviewer/config-webengineviewer.h>
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWebEngineView>

using namespace WebEngineViewer;
WebEnginePrintMessageBox::WebEnginePrintMessageBox(QWidget *parent)
    : QDialog(parent),
      mEngineView(Q_NULLPTR)
{
    setWindowTitle(i18n("Print Status Information"));
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QLabel *lab = new QLabel(i18n("Printing WebEngine is limited for the moment. So printing support will done by an external browser to print it."), this);
    lab->setWordWrap(true);
    lab->setAlignment(Qt::AlignHCenter);
    lab->setObjectName(QStringLiteral("webengineprintmessageboxlabel"));
    mainLayout->addWidget(lab);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    buttonBox->setObjectName(QStringLiteral("buttonbox"));
    connect(buttonBox, &QDialogButtonBox::rejected, this, &WebEnginePrintMessageBox::reject);
    QPushButton *openInBrowser = new QPushButton(i18n("Open In Browser"), this);
    connect(openInBrowser, &QPushButton::clicked, this, &WebEnginePrintMessageBox::slotOpenInBrowser);
    openInBrowser->setObjectName(QStringLiteral("openinbrowser"));
    buttonBox->addButton(openInBrowser, QDialogButtonBox::ActionRole);

#ifdef WEBENGINEVIEWER_PRINTPREVIEW_SUPPORT
    QPushButton *openInPreviewDialogBox = new QPushButton(i18n("Print Preview"), this);
    connect(openInPreviewDialogBox, &QPushButton::clicked, this, &WebEnginePrintMessageBox::slotPrintPreview);
    openInPreviewDialogBox->setObjectName(QStringLiteral("openprintpreview"));
    buttonBox->addButton(openInPreviewDialogBox, QDialogButtonBox::ActionRole);
#endif

    mainLayout->addWidget(buttonBox);
}

WebEnginePrintMessageBox::~WebEnginePrintMessageBox()
{

}

void WebEnginePrintMessageBox::setWebEngineView(QWebEngineView *engineView)
{
    mEngineView = engineView;
}

void WebEnginePrintMessageBox::slotOpenInBrowser()
{
    if (mEngineView) {
        //AutoDelete
        WebEngineViewer::WebEngineExportHtmlPageJob *job = new WebEngineViewer::WebEngineExportHtmlPageJob(this);
        job->setEngineView(mEngineView);
        connect(job, &WebEngineExportHtmlPageJob::failed, this, &WebEnginePrintMessageBox::slotExportHtmlPageFailed);
        connect(job, &WebEngineExportHtmlPageJob::success, this, &WebEnginePrintMessageBox::slotExportHtmlPageSuccess);
        job->start();
    } else {
        qCDebug(WEBENGINEVIEWER_LOG) << "WebEngineView not defined";
    }
}

void WebEnginePrintMessageBox::slotExportHtmlPageSuccess(const QString &filename)
{
    Q_EMIT openInBrowser(filename);
    accept();
}

void WebEnginePrintMessageBox::slotExportHtmlPageFailed()
{
    qCDebug(WEBENGINEVIEWER_LOG) << "Impossible to export html page";
    accept();
}

QWebEngineView *WebEnginePrintMessageBox::engineView() const
{
    return mEngineView;
}

void WebEnginePrintMessageBox::slotPrintPreview()
{
    Q_EMIT openPrintPreview();
    accept();
}
