/*
    Copyright (c) 2007 Volker Krause <vkrause@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef MAILVIEWER_MIMETREEMODEL_H
#define MAILVIEWER_MIMETREEMODEL_H

#include <QAbstractItemModel>

namespace KMime {
class Content;
}

namespace MessageViewer {
/**
  A model representing the mime part tree of a message.
*/
class MimeTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Role {
        ContentIndexRole = Qt::UserRole + 1,
        ContentRole,
        MimeTypeRole,
        MainBodyPartRole,
        AlternativeBodyPartRole,
        UserRole = Qt::UserRole + 100
    };
    explicit MimeTreeModel(QObject *parent = nullptr);
    ~MimeTreeModel();

    void setRoot(KMime::Content *root);

    KMime::Content *root();

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QStringList mimeTypes() const override;
private:
    class Private;
    Private *const d;
};
}

#endif
