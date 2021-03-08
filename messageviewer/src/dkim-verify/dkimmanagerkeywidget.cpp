/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeywidget.h"
#include "dkimmanagerkey.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <KTreeWidgetSearchLine>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidget>
#include <QVBoxLayout>

using namespace MessageViewer;
DKIMManagerKeyWidget::DKIMManagerKeyWidget(QWidget *parent)
    : QWidget(parent)
    , mTreeWidget(new QTreeWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setContentsMargins({});

    mTreeWidget->setObjectName(QStringLiteral("treewidget"));
    mTreeWidget->setRootIsDecorated(false);
    mTreeWidget->setHeaderLabels({i18n("SDID"), i18n("Selector"), i18n("DKIM Key"), i18n("Inserted"), i18n("Last Used")});
    mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mTreeWidget->setAlternatingRowColors(true);

    auto searchLineEdit = new KTreeWidgetSearchLine(this, mTreeWidget);
    searchLineEdit->setObjectName(QStringLiteral("searchlineedit"));
    searchLineEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(searchLineEdit);

    mainLayout->addWidget(mTreeWidget);
    connect(mTreeWidget, &QTreeWidget::customContextMenuRequested, this, &DKIMManagerKeyWidget::slotCustomContextMenuRequested);
}

DKIMManagerKeyWidget::~DKIMManagerKeyWidget()
{
}

QByteArray DKIMManagerKeyWidget::saveHeaders() const
{
    return mTreeWidget->header()->saveState();
}

void DKIMManagerKeyWidget::restoreHeaders(const QByteArray &header)
{
    mTreeWidget->header()->restoreState(header);
}

void DKIMManagerKeyWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = mTreeWidget->itemAt(pos);
    QMenu menu(this);
    if (item) {
        const int selectedItemsCount{mTreeWidget->selectedItems().count()};
        if (selectedItemsCount == 1) {
            menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18n("Copy Key"), this, [item]() {
                QApplication::clipboard()->setText(item->text(2));
            });
            menu.addSeparator();
        }
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")),
                       i18np("Remove Key", "Remove Keys", selectedItemsCount),
                       this,
                       [this, selectedItemsCount]() {
                           if (KMessageBox::Yes
                               == KMessageBox::warningYesNo(this,
                                                            i18np("Do you want to delete this key?", "Do you want to delete these keys?", selectedItemsCount),
                                                            i18np("Delete Key", "Delete Keys", selectedItemsCount))) {
                               const auto selectedItems = mTreeWidget->selectedItems();
                               for (QTreeWidgetItem *item : selectedItems) {
                                   delete item;
                               }
                           }
                       });
        menu.addSeparator();
    }
    if (mTreeWidget->topLevelItemCount() > 0) {
        menu.addAction(i18n("Delete All"), this, [this]() {
            if (KMessageBox::Yes == KMessageBox::warningYesNo(this, i18n("Do you want to delete all keys?"), i18n("Delete Keys"))) {
                mTreeWidget->clear();
            }
        });
    }
    if (!menu.isEmpty()) {
        menu.exec(QCursor::pos());
    }
}

void DKIMManagerKeyWidget::loadKeys()
{
    const QVector<MessageViewer::KeyInfo> lst = DKIMManagerKey::self()->keys();
    for (const MessageViewer::KeyInfo &key : lst) {
        auto item = new QTreeWidgetItem(mTreeWidget);
        item->setText(0, key.domain);
        item->setText(1, key.selector);
        item->setText(2, key.keyValue);
        item->setText(3, key.storedAtDateTime.toString());
        item->setText(4, key.lastUsedDateTime.toString());
        item->setToolTip(2, key.keyValue);
    }
    mTreeWidget->setSortingEnabled(true);
    mTreeWidget->header()->setSortIndicatorShown(true);
    mTreeWidget->header()->setSectionsClickable(true);
    mTreeWidget->sortByColumn(0, Qt::AscendingOrder);
}

void DKIMManagerKeyWidget::saveKeys()
{
    QVector<MessageViewer::KeyInfo> lst;
    lst.reserve(mTreeWidget->topLevelItemCount());
    for (int i = 0, total = mTreeWidget->topLevelItemCount(); i < total; ++i) {
        QTreeWidgetItem *item = mTreeWidget->topLevelItem(i);
        const MessageViewer::KeyInfo info{item->text(2),
                                          item->text(1),
                                          item->text(0),
                                          QDateTime::fromString(item->text(3)),
                                          QDateTime::fromString(item->text(4))};
        lst.append(info);
    }
    DKIMManagerKey::self()->saveKeys(lst);
}

void DKIMManagerKeyWidget::resetKeys()
{
    mTreeWidget->clear();
    loadKeys();
}
