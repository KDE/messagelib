/*
    SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QUrl>

#include <QElapsedTimer>
#include <QObject>
#include <QTimer>
class KProcess;

namespace MessageComposer
{
/**
  Starts an editor for the given URL and emits an signal when
  editing has been finished. Both, the editor process as well
  as the edited file are watched to work with as many as possible
  editors.
*/
class EditorWatcher : public QObject
{
    Q_OBJECT
public:
    enum OpenWithOption {
        OpenWithDialog,
        NoOpenWithDialog,
    };
    enum ErrorEditorWatcher {
        Unknown = 0,
        Canceled,
        NoServiceFound,
        CannotStart,
        NoError,
    };

    /**
     * Constructs an EditorWatcher.
     * @param url the given URL.
     * @param mimeType the data MIME type.
     * @param option the open option.
     * @param parent the parent object of this EditorWatcher, which will take care of deleting
     *               this EditorWatcher if the parent is deleted.
     * @param parentWidget the parent widget of this EditorWatcher, which will be used as the parent
     *                     widget for message dialogs.
     */
    explicit EditorWatcher(const QUrl &url, const QString &mimeType, OpenWithOption option, QObject *parent, QWidget *parentWidget);

    ~EditorWatcher() override;
    [[nodiscard]] ErrorEditorWatcher start();
    [[nodiscard]] bool fileChanged() const;
    [[nodiscard]] QUrl url() const;
Q_SIGNALS:
    void editDone(MessageComposer::EditorWatcher *watcher);

private:
    void editorExited();
    void inotifyEvent();
    void checkEditDone();
    const QUrl mUrl;
    const QString mMimeType;
    QTimer mTimer;
    QElapsedTimer mEditTime;

    KProcess *mEditor = nullptr;
    QWidget *const mParentWidget;

    int mInotifyFd = -1;
    int mInotifyWatch = -1;
    const OpenWithOption mOpenWithOption;
    bool mHaveInotify = false;
    bool mFileOpen = false;
    bool mEditorRunning = false;
    bool mFileModified = true;
    bool mDone = false;
};
}
