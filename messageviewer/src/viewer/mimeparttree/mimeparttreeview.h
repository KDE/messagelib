/*
   SPDX-FileCopyrightText: 2014-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QTreeView>

namespace KMime
{
class Content;
}

namespace MessageViewer
{
class MimeTreeModel;
class MimePartTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit MimePartTreeView(QWidget *parent = nullptr);
    ~MimePartTreeView() override;

    MessageViewer::MimeTreeModel *mimePartModel() const;

    void clearModel();
    void setRoot(const KMime::Content *root);

    [[nodiscard]] QList<KMime::Content *> selectedContents() const;

private:
    void saveMimePartTreeConfig();
    void restoreMimePartTreeConfig();
    MimeTreeModel *const mMimePartModel;
};
}
