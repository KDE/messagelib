/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

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
    mManagerKeyProxyModel->setObjectName(QLatin1StringView("mManagerKeyProxyModel"));
    setRootIsDecorated(false);
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(ExtendedSelection);
    setUniformRowHeights(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSortingEnabled(true);
    connect(this, &DKIMManagerKeyTreeView::customContextMenuRequested, this, &DKIMManagerKeyTreeView::slotCustomContextMenuRequested);
}

DKIMManagerKeyTreeView::~DKIMManagerKeyTreeView() = default;

void DKIMManagerKeyTreeView::setFilterStr(const QString &str)
{
    mManagerKeyProxyModel->setFilterText(str);
}

QList<MessageViewer::KeyInfo> DKIMManagerKeyTreeView::keyInfos() const
{
    return mManagerKeyModel ? mManagerKeyModel->keyInfos() : QList<MessageViewer::KeyInfo>();
}

void DKIMManagerKeyTreeView::clear()
{
    if (mManagerKeyModel) {
        mManagerKeyModel->clear();
    }
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
            menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18n("Copy Key"), this, [index, this]() {
                QApplication::clipboard()->setText(mManagerKeyModel->index(index.row()).data(DKIMManagerKeyModel::KeyRole).toString());
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
                               deleteSelectedItems();
                           }
                       });
        menu.addSeparator();
    }
    if (mManagerKeyProxyModel->rowCount() > 0) {
        menu.addAction(i18n("Delete All"), this, [this]() {
            const int answer = KMessageBox::warningTwoActions(this,
                                                              i18n("Do you want to delete all keys?"),
                                                              i18nc("@title:window", "Delete Keys"),
                                                              KStandardGuiItem::del(),
                                                              KStandardGuiItem::cancel());
            if (answer == KMessageBox::ButtonCode::PrimaryAction) {
                mManagerKeyModel->clear();
            }
        });
    }
    if (!menu.isEmpty()) {
        menu.exec(QCursor::pos());
    }
}

void DKIMManagerKeyTreeView::deleteSelectedItems()
{
    const QModelIndexList selectedIndexes = selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        return;
    }
    QStringList lst;
    for (const auto &index : selectedIndexes) {
        const auto info = mManagerKeyProxyModel->mapToSource(mManagerKeyProxyModel->index(index.row(), DKIMManagerKeyModel::KeyRole));
        lst.append(info.data().toString());
    }
    mManagerKeyModel->removeKeyInfos(lst);
}

#include "moc_dkimmanagerkeytreeview.cpp"
