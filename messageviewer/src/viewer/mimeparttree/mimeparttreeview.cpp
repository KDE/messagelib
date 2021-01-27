/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mimeparttreeview.h"
#include "mimetreemodel.h"
#include "settings/messageviewersettings.h"

#include <KMime/Content>

#include <KConfigGroup>
#include <QHeaderView>

using namespace MessageViewer;

MimePartTreeView::MimePartTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setObjectName(QStringLiteral("mMimePartTree"));

    mMimePartModel = new MimeTreeModel(this);
    setModel(mMimePartModel);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(this, &MimePartTreeView::destroyed, this, &MimePartTreeView::slotMimePartDestroyed);
    setContextMenuPolicy(Qt::CustomContextMenu);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(mMimePartModel, &MimeTreeModel::modelReset, this, &MimePartTreeView::expandAll);
    restoreMimePartTreeConfig();
    setDragEnabled(true);
}

MimePartTreeView::~MimePartTreeView()
{
    disconnect(mMimePartModel, &MimeTreeModel::modelReset, this, &MimePartTreeView::expandAll);
    saveMimePartTreeConfig();
}

MimeTreeModel *MimePartTreeView::mimePartModel() const
{
    return mMimePartModel;
}

void MimePartTreeView::restoreMimePartTreeConfig()
{
    KConfigGroup grp(KSharedConfig::openConfig(), "MimePartTree");
    header()->restoreState(grp.readEntry("State", QByteArray()));
}

void MimePartTreeView::saveMimePartTreeConfig()
{
    KConfigGroup grp(KSharedConfig::openConfig(), "MimePartTree");
    grp.writeEntry("State", header()->saveState());
}

void MimePartTreeView::slotMimePartDestroyed()
{
    // root is either null or a modified tree that we need to clean up
    clearModel();
}

void MimePartTreeView::clearModel()
{
    delete mMimePartModel->root();
    mMimePartModel->setRoot(nullptr);
}

void MimePartTreeView::setRoot(KMime::Content *root)
{
    delete mMimePartModel->root();
    mMimePartModel->setRoot(root);
}

KMime::Content::List MimePartTreeView::selectedContents() const
{
    KMime::Content::List contents;
    QItemSelectionModel *selectModel = selectionModel();
    const QModelIndexList selectedRows = selectModel->selectedRows();
    contents.reserve(selectedRows.count());
    for (const QModelIndex &index : selectedRows) {
        auto content = static_cast<KMime::Content *>(index.internalPointer());
        if (content) {
            contents.append(content);
        }
    }
    return contents;
}
