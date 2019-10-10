/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#include "dkimmanagerkeywidget.h"
#include "dkimmanagerkey.h"

#include <QTreeWidget>
#include <QVBoxLayout>
#include <KLocalizedString>
#include <QMenu>

using namespace MessageViewer;
DKIMManagerKeyWidget::DKIMManagerKeyWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mTreeWidget = new QTreeWidget(this);
    mTreeWidget->setObjectName(QStringLiteral("treewidget"));
    mTreeWidget->setRootIsDecorated(false);
    mTreeWidget->setHeaderLabels({i18n("SDID"), i18n("Selector"), i18n("DKIM Key")});
    mainLayout->addWidget(mTreeWidget);
    mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mTreeWidget->setAlternatingRowColors(true);
    connect(mTreeWidget, &QTreeWidget::customContextMenuRequested, this, &DKIMManagerKeyWidget::customContextMenuRequested);
}

DKIMManagerKeyWidget::~DKIMManagerKeyWidget()
{
}

void DKIMManagerKeyWidget::customContextMenuRequested(const QPoint &)
{
    QTreeWidgetItem *item = mTreeWidget->currentItem();
    QMenu menu(this);
    if (item) {
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove Key"), this, [this, item]() {
            delete item;
        });
        menu.addSeparator();
    }
    menu.addAction(i18n("Delete All"), this, [this]() {
        mTreeWidget->clear();
    });
    menu.exec(QCursor::pos());
}

void DKIMManagerKeyWidget::loadKeys()
{
    const QVector<MessageViewer::KeyInfo> lst = DKIMManagerKey::self()->keys();
    for (const MessageViewer::KeyInfo &key : lst) {
        QTreeWidgetItem *item = new QTreeWidgetItem(mTreeWidget);
        item->setText(0, key.domain);
        item->setText(1, key.selector);
        item->setText(2, key.keyValue);
        item->setToolTip(2, key.keyValue);
    }
}

void DKIMManagerKeyWidget::saveKeys()
{
    QVector<MessageViewer::KeyInfo> lst;
    for (int i = 0, total = mTreeWidget->topLevelItemCount(); i < total; ++i) {
        QTreeWidgetItem *item = mTreeWidget->topLevelItem(i);
        const MessageViewer::KeyInfo info {item->text(2), item->text(1), item->text(0)};
        lst.append(info);
    }
    DKIMManagerKey::self()->saveKeys(lst);
}

void DKIMManagerKeyWidget::resetKeys()
{
    mTreeWidget->clear();
    loadKeys();
}
