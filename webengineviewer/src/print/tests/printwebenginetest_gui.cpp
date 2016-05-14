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


#include "printwebenginetest_gui.h"
#include "../printwebengineviewjob.h"
#include "../printconfiguredialog.h"
#include "../printpreviewdialog.h"

#include <QApplication>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWebEngineView>

PrintWebEngineTest_Gui::PrintWebEngineTest_Gui(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    mWebEngine = new QWebEngineView(this);
    mainLayout->addWidget(mWebEngine);
    mWebEngine->load(QUrl(QStringLiteral("http://www.kde.org")));

    QPushButton *printButton = new QPushButton(QStringLiteral("Print"), this);
    mainLayout->addWidget(printButton);
    connect(printButton, &QPushButton::clicked, this, &PrintWebEngineTest_Gui::slotPrint);
}

PrintWebEngineTest_Gui::~PrintWebEngineTest_Gui()
{

}

void PrintWebEngineTest_Gui::slotPrint()
{
    WebEngineViewer::PrintConfigureDialog dlg(this);
    if (dlg.exec()) {
        const QPageLayout pageLayout = dlg.currentPageLayout();
        WebEngineViewer::PrintWebEngineViewJob *job = new WebEngineViewer::PrintWebEngineViewJob(this);
        job->setEngineView(mWebEngine);
        job->setPageLayout(pageLayout);
        connect(job, &WebEngineViewer::PrintWebEngineViewJob::failed, this, &PrintWebEngineTest_Gui::slotPdfFailed);
        connect(job, &WebEngineViewer::PrintWebEngineViewJob::success, this, &PrintWebEngineTest_Gui::slotPdfCreated);
        job->start();
        //Don't delete it, it's autodelete
    }
}

void PrintWebEngineTest_Gui::slotPdfCreated(const QString &filename)
{
    WebEngineViewer::PrintPreviewDialog dlg(this);
    dlg.loadFile(filename);
    dlg.exec();
    //TODO delete filename
}

void PrintWebEngineTest_Gui::slotPdfFailed()
{
    qDebug() << "void PrintWebEngineTest_Gui::slotPdfFailed()";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    PrintWebEngineTest_Gui *w = new PrintWebEngineTest_Gui;
    w->show();
    const int ret = app.exec();
    return ret;
}
