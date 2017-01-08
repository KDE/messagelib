/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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

#include "webengineprintmessagebox.h"
#include "webengineviewer_debug.h"
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

using namespace WebEngineViewer;
WebEnginePrintMessageBox::WebEnginePrintMessageBox(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18n("Print Status Information"));
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QLabel *lab = new QLabel(i18n("WebEngine printing support is limited for the moment. An external browser will be used for printing."), this);
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

    mainLayout->addWidget(buttonBox);
}

WebEnginePrintMessageBox::~WebEnginePrintMessageBox()
{

}

void WebEnginePrintMessageBox::slotOpenInBrowser()
{
    Q_EMIT openInBrowser();
    accept();
}
