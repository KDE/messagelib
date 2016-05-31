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
#include "printpreviewpagewidget.h"
#include "webengineviewer_debug.h"
#include <KLocalizedString>
#include <QVBoxLayout>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QPushButton>


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
    QPushButton *printButton = new QPushButton(i18n("Print"));
    printButton->setObjectName(QStringLiteral("printbutton"));
    connect(printButton, &QPushButton::clicked, this, &PrintPreviewDialog::slotPrint);
    buttonBox->addButton(printButton, QDialogButtonBox::ActionRole);
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
    mPrintPreviewWidget->loadFile(path, deleteFile);
}

void PrintPreviewDialog::slotPrint()
{
    qCDebug(WEBENGINEVIEWER_LOG) << "void PrintPreviewDialog::slotPrint() not implemented yet";
    //TODO add dialog box to select page.

    //TODO select page ?
    //mPrintPreviewWidget->print();
    //TODO
    accept();
}
