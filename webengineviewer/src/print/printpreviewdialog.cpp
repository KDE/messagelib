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

#include "printpreviewdialog.h"
#include "printpreviewpagewidget.h"

#include <KLocalizedString>
#include <QVBoxLayout>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QDialogButtonBox>

using namespace WebEngineViewer;

PrintPreviewDialog::PrintPreviewDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    mPrintPreviewWidget = new PrintPreviewPageWidget(this);
    mPrintPreviewWidget->setObjectName(QStringLiteral("printpreviewwidget"));
    layout->addWidget(mPrintPreviewWidget);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    buttonBox->setObjectName(QStringLiteral("buttonbox"));
    layout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &PrintPreviewDialog::reject);
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

void PrintPreviewDialog::loadFile(const QString &path)
{
    mPrintPreviewWidget->loadFile(path);
}
