/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "printpreviewdialog.h"
#include "printselectpagedialog.h"
#include "printpreviewpagewidget.h"
#include "webengineviewer_debug.h"
#include <KLocalizedString>
#include <QVBoxLayout>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QPointer>

using namespace WebEngineViewer;

PrintPreviewDialog::PrintPreviewDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18n("Preview"));
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    mPrintPreviewWidget = new PrintPreviewPageWidget(this);
    mPrintPreviewWidget->setObjectName(QStringLiteral("printpreviewwidget"));
    layout->addWidget(mPrintPreviewWidget);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    mPrintButton = new QPushButton(i18n("Print"));
    mPrintButton->setObjectName(QStringLiteral("printbutton"));
    connect(mPrintButton, &QPushButton::clicked, this, &PrintPreviewDialog::slotPrint);
    buttonBox->addButton(mPrintButton, QDialogButtonBox::ActionRole);
    buttonBox->setObjectName(QStringLiteral("buttonbox"));
    layout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &PrintPreviewDialog::reject);
    readConfig();
}

PrintPreviewDialog::~PrintPreviewDialog()
{
    writeConfig();
}

void PrintPreviewDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "PrintPreviewDialog");
    group.writeEntry("Size", size());
    group.sync();
}

void PrintPreviewDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "PrintPreviewDialog");

    const QSize size = group.readEntry("Size", QSize(500, 300));
    if (size.isValid()) {
        resize(size);
    }
}

void PrintPreviewDialog::loadFile(const QString &path, bool deleteFile)
{
    if (!mPrintPreviewWidget->loadFile(path, deleteFile)) {
        mPrintButton->setEnabled(false);
    }
}

void PrintPreviewDialog::slotPrint()
{
    QList<int> lstPages;
    if (mPrintPreviewWidget->pageCount() > 1) {
        QPointer<WebEngineViewer::PrintSelectPageDialog> dlg = new WebEngineViewer::PrintSelectPageDialog(this);
        dlg->setPages(mPrintPreviewWidget->pageCount());
        if (dlg->exec()) {
            lstPages = dlg->pages();
        }
        delete dlg;
    } else {
        lstPages.append(0);
    }

    if (!lstPages.isEmpty()) {
        qCDebug(WEBENGINEVIEWER_LOG) << "void PrintPreviewDialog::slotPrint() not implemented yet";
    }

    accept();
}
