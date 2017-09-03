/*
   Copyright (C) 2014-2017 Laurent Montel <montel@kde.org>

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

#ifndef ATTACHMENTEDITJOB_H
#define ATTACHMENTEDITJOB_H

#include <QObject>
#include <AkonadiCore/Item>
#include <QHash>
#include <kmime/kmime_message.h>
namespace KMime {
class Content;
}
namespace Akonadi {
class Session;
}
namespace MessageViewer {
class EditorWatcher;
class AttachmentEditJob : public QObject
{
    Q_OBJECT
public:
    explicit AttachmentEditJob(Akonadi::Session *session, QObject *parent = nullptr);
    ~AttachmentEditJob();

    void setMainWindow(QWidget *mainWindow);

    void setMessageItem(const Akonadi::Item &messageItem);

    bool addAttachment(KMime::Content *node, bool showWarning);

    void canDeleteJob();

    void setMessage(const KMime::Message::Ptr &message);

Q_SIGNALS:
    void refreshMessage(const Akonadi::Item &item);
private:
    void slotAttachmentEditDone(MessageViewer::EditorWatcher *editorWatcher);
    void slotItemModifiedResult(KJob *);
    void removeEditorWatcher(MessageViewer::EditorWatcher *editorWatcher, const QString &name);
    QHash<EditorWatcher *, KMime::Content *> mEditorWatchers;
    Akonadi::Item mMessageItem;
    KMime::Message::Ptr mMessage;
    bool mShowWarning = false;
    QWidget *mMainWindow = nullptr;
    Akonadi::Session *mSession = nullptr;
};
}

#endif // ATTACHMENTEDITJOB_H
