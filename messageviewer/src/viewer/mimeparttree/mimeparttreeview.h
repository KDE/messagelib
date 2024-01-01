/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMime/Message>
#include <QTreeView>

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
    void setRoot(KMime::Content *root);

    [[nodiscard]] KMime::Content::List selectedContents() const;

private:
    void saveMimePartTreeConfig();
    void restoreMimePartTreeConfig();
    MimeTreeModel *const mMimePartModel;
};
}
