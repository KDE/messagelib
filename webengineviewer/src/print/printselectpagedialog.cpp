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

#include "printselectpagedialog.h"
#include "printselectpagewidget.h"

#include <KLocalizedString>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QVBoxLayout>
#include <QDialogButtonBox>

using namespace WebEngineViewer;

PrintSelectPageDialog::PrintSelectPageDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mPrintSelectPage = new PrintSelectPageWidget(this);
    mPrintSelectPage->setObjectName(QStringLiteral("printselectpage"));
    mainLayout->addWidget(mPrintSelectPage);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, this);
    buttonBox->setObjectName(QStringLiteral("buttonbox"));
    mainLayout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PrintSelectPageDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &PrintSelectPageDialog::reject);
    readConfig();
}

PrintSelectPageDialog::~PrintSelectPageDialog()
{
    writeConfig();
}

void PrintSelectPageDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "PrintSelectPageDialog");
    group.writeEntry("Size", size());
    group.sync();
}

void PrintSelectPageDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "PrintSelectPageDialog");

    const QSize size = group.readEntry("Size", QSize(500, 300));
    if (size.isValid()) {
        resize(size);
    }
}


QList<int> PrintSelectPageDialog::pages() const
{
    return mPrintSelectPage->pages();
}

void PrintSelectPageDialog::setPages(int pages)
{
    mPrintSelectPage->setPages(pages);
}
