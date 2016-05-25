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
#include "webengineexporthtmlpagejob.h"
#include "config-webengineviewer.h"
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
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    //TODO add more.
    QLabel *lab = new QLabel(i18n("Printing WebEngine is limited for the moment."), this);
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

void WebEnginePrintMessageBox::setWebEngineView(QWebEngineView *engineView)
{
    mEngineView = engineView;
}

void WebEnginePrintMessageBox::slotOpenInBrowser()
{
    //TODO
}

QWebEngineView *WebEnginePrintMessageBox::engineView() const
{
    return mEngineView;
}
