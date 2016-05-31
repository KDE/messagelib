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

#include "printconfiguredialog.h"
#include "printconfigurewidget.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <KLocalizedString>

using namespace WebEngineViewer;

PrintConfigureDialog::PrintConfigureDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18n("Print Setup"));
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    mConfigureWidget = new PrintConfigureWidget(this);
    mConfigureWidget->setObjectName(QStringLiteral("configurewidget"));
    mainLayout->addWidget(mConfigureWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->setObjectName(QStringLiteral("buttonBox"));
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &PrintConfigureDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &PrintConfigureDialog::reject);
}

PrintConfigureDialog::~PrintConfigureDialog()
{

}

QPageLayout PrintConfigureDialog::currentPageLayout() const
{
    return mConfigureWidget->currentPageLayout();
}
