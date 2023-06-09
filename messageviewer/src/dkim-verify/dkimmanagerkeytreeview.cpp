/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeytreeview.h"
#include "dkimmanagerkeymodel.h"
#include "dkimmanagerkeyproxymodel.h"
#include <KLocalizedString>
#include <KMessageBox>

#include <QApplication>
#include <QClipboard>
#include <QMenu>

using namespace MessageViewer;
DKIMManagerKeyTreeView::DKIMManagerKeyTreeView(QWidget *parent)
    : QTreeView(parent)
    , mManagerKeyProxyModel(new DKIMManagerKeyProxyModel(this))
{
    mManagerKeyProxyModel->setObjectName(QStringLiteral("mManagerKeyProxyModel"));
    setRootIsDecorated(false);
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(ExtendedSelection);
    setUniformRowHeights(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &DKIMManagerKeyTreeView::customContextMenuRequested, this, &DKIMManagerKeyTreeView::slotCustomContextMenuRequested);
}

DKIMManagerKeyTreeView::~DKIMManagerKeyTreeView() = default;

void DKIMManagerKeyTreeView::setFilterStr(const QString &str)
{
    // mManagerKeyProxyModel->setFilterText(str);
    // TODO
}

void DKIMManagerKeyTreeView::setKeyModel(DKIMManagerKeyModel *model)
{
    mManagerKeyModel = model;
    mManagerKeyProxyModel->setSourceModel(mManagerKeyModel);
    setModel(mManagerKeyProxyModel);
}

void DKIMManagerKeyTreeView::slotCustomContextMenuRequested(const QPoint &pos)
{
    const QModelIndex idx = indexAt(pos);
    const QModelIndex index = mManagerKeyProxyModel->mapToSource(idx);
    QMenu menu(this);
    const QModelIndexList selectedIndexes = selectionModel()->selectedRows();
    const auto selectedItemCount{selectedIndexes.count()};

    if (index.isValid()) {
        if (selectedItemCount == 1) {
            menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18n("Copy Key"), this, [index]() {
                // QApplication::clipboard()->setText(item->text(2));
            });
            menu.addSeparator();
        }

        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")),
                       i18np("Remove Key", "Remove Keys", selectedItemCount),
                       this,
                       [this, selectedItemCount]() {
                           const int answer =
                               KMessageBox::questionTwoActions(this,
                                                               i18np("Do you want to delete this key?", "Do you want to delete these keys?", selectedItemCount),
                                                               i18np("Delete Key", "Delete Keys", selectedItemCount),
                                                               KStandardGuiItem::del(),
                                                               KStandardGuiItem::cancel());
                           if (answer == KMessageBox::ButtonCode::PrimaryAction) {
#if 0
                               const auto selectedItems = mTreeWidget->selectedItems();
                               for (QTreeWidgetItem *item : selectedItems) {
                                   delete item;
                               }
#endif
                           }
                       });
        menu.addSeparator();
    }
#if 0
    if (mTreeWidget->topLevelItemCount() > 0) {
        menu.addAction(i18n("Delete All"), this, [this]() {
            const int answer = KMessageBox::warningTwoActions(this,
                                                              i18n("Do you want to delete all keys?"),
                                                              i18n("Delete Keys"),
                                                              KStandardGuiItem::del(),
                                                              KStandardGuiItem::cancel());
            if (answer == KMessageBox::ButtonCode::PrimaryAction) {
                mManagerKeyModel->clear();
            }
        });
    }
#endif
    if (!menu.isEmpty()) {
        menu.exec(QCursor::pos());
    }
}
