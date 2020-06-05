/*
   Copyright (C) 2014-2020 Laurent Montel <montel@kde.org>

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
    //root is either null or a modified tree that we need to clean up
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
        KMime::Content *content = static_cast<KMime::Content *>(index.internalPointer());
        if (content) {
            contents.append(content);
        }
    }
    return contents;
}
