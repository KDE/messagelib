/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeywidget.h"
#include "dkimmanagerkey.h"
#include "dkimmanagerkeymodel.h"
#include "dkimmanagerkeytreeview.h"

#include <KLineEditEventHandler>
#include <KLocalizedString>
#include <QHeaderView>
#include <QLineEdit>
#include <QVBoxLayout>

using namespace MessageViewer;
DKIMManagerKeyWidget::DKIMManagerKeyWidget(QWidget *parent)
    : QWidget(parent)
    , mDKIMManagerKeyTreeView(new DKIMManagerKeyTreeView(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainlayout"));
    mainLayout->setContentsMargins({});

    mDKIMManagerKeyTreeView->setObjectName(QLatin1StringView("mDKIMManagerKeyTreeView"));

    auto searchLineEdit = new QLineEdit(this);
    KLineEditEventHandler::catchReturnKey(searchLineEdit);
    searchLineEdit->setObjectName(QLatin1StringView("searchlineedit"));
    searchLineEdit->setClearButtonEnabled(true);
    searchLineEdit->setPlaceholderText(i18nc("@info:placeholder", "Search..."));
    mainLayout->addWidget(searchLineEdit);
    connect(searchLineEdit, &QLineEdit::textChanged, mDKIMManagerKeyTreeView, &DKIMManagerKeyTreeView::setFilterStr);

    mainLayout->addWidget(mDKIMManagerKeyTreeView);
}

DKIMManagerKeyWidget::~DKIMManagerKeyWidget() = default;

QByteArray DKIMManagerKeyWidget::saveHeaders() const
{
    return mDKIMManagerKeyTreeView->header()->saveState();
}

void DKIMManagerKeyWidget::restoreHeaders(const QByteArray &header)
{
    mDKIMManagerKeyTreeView->header()->restoreState(header);
}

void DKIMManagerKeyWidget::loadKeys()
{
    auto model = new DKIMManagerKeyModel(this);
    model->setKeyInfos(DKIMManagerKey::self()->keys());
    mDKIMManagerKeyTreeView->setKeyModel(model);
}

void DKIMManagerKeyWidget::saveKeys()
{
    DKIMManagerKey::self()->saveKeys(mDKIMManagerKeyTreeView->keyInfos());
}

void DKIMManagerKeyWidget::resetKeys()
{
    mDKIMManagerKeyTreeView->clear();
    loadKeys();
}

#include "moc_dkimmanagerkeywidget.cpp"
