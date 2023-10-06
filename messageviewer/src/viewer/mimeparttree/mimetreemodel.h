/*
    SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QAbstractItemModel>
#include <memory>
namespace KMime
{
class Content;
}

namespace MessageViewer
{
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
        UserRole = Qt::UserRole + 100,
    };
    explicit MimeTreeModel(QObject *parent = nullptr);
    ~MimeTreeModel() override;

    void setRoot(KMime::Content *root);

    KMime::Content *root();

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QStringList mimeTypes() const override;

private:
    class MimeTreeModelPrivate;
    std::unique_ptr<MimeTreeModelPrivate> const d;
};
}
