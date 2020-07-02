/*
   SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MIMEPARTTREEVIEW_H
#define MIMEPARTTREEVIEW_H

#include <QTreeView>
#include <KMime/Message>

namespace MessageViewer {
class MimeTreeModel;
class MimePartTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit MimePartTreeView(QWidget *parent = nullptr);
    ~MimePartTreeView();

    MessageViewer::MimeTreeModel *mimePartModel() const;

    void clearModel();
    void setRoot(KMime::Content *root);

    Q_REQUIRED_RESULT KMime::Content::List selectedContents() const;

private:
    void slotMimePartDestroyed();
    void saveMimePartTreeConfig();
    void restoreMimePartTreeConfig();
    MimeTreeModel *mMimePartModel = nullptr;
};
}

#endif // MIMEPARTTREEVIEW_H
