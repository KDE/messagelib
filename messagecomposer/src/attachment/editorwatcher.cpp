/*
    SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "editorwatcher.h"
#include <config-messagecomposer.h>

#include "messagecomposer_debug.h"

#include <KApplicationTrader>
#include <KIO/DesktopExecParser>
#include <KLocalizedString>
#include <KMessageBox>
#include <KOpenWithDialog>
#include <KProcess>

#include <QSocketNotifier>

#include <cassert>
#include <memory>

// inotify stuff taken from kdelibs/kio/kio/kdirwatch.cpp
#ifdef HAVE_SYS_INOTIFY_H
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

using namespace MessageComposer;

EditorWatcher::EditorWatcher(const QUrl &url, const QString &mimeType, OpenWithOption option, QObject *parent, QWidget *parentWidget)
    : QObject(parent)
    , mUrl(url)
    , mMimeType(mimeType)
    , mParentWidget(parentWidget)
    , mOpenWithOption(option)
{
    assert(mUrl.isLocalFile());
    mTimer.setSingleShot(true);
    connect(&mTimer, &QTimer::timeout, this, &EditorWatcher::checkEditDone);
}

EditorWatcher::~EditorWatcher()
{
#ifdef HAVE_SYS_INOTIFY_H
    ::close(mInotifyFd);
#endif
}

EditorWatcher::ErrorEditorWatcher EditorWatcher::start()
{
    // find an editor
    QList<QUrl> list;
    list.append(mUrl);
    KService::Ptr offer = KApplicationTrader::preferredService(mMimeType);
    if ((mOpenWithOption == OpenWithDialog) || !offer) {
        std::unique_ptr<KOpenWithDialog> dlg(new KOpenWithDialog(list, i18n("Edit with:"), QString(), mParentWidget));
        const int dlgrc = dlg->exec();
        if (dlgrc) {
            offer = dlg->service();
        }
        if (!dlgrc) {
            return Canceled;
        }
        if (!offer) {
            return NoServiceFound;
        }
    }

#ifdef HAVE_SYS_INOTIFY_H
    // monitor file
    mInotifyFd = inotify_init();
    if (mInotifyFd > 0) {
        (void)fcntl(mInotifyFd, F_SETFD, FD_CLOEXEC);
        mInotifyWatch = inotify_add_watch(mInotifyFd, mUrl.path().toLatin1().constData(), IN_CLOSE | IN_OPEN | IN_MODIFY | IN_ATTRIB);
        if (mInotifyWatch >= 0) {
            auto sn = new QSocketNotifier(mInotifyFd, QSocketNotifier::Read, this);
            connect(sn, &QSocketNotifier::activated, this, &EditorWatcher::inotifyEvent);
            mHaveInotify = true;
            mFileModified = false;
        }
    } else {
        qCWarning(MESSAGECOMPOSER_LOG()) << "Failed to activate INOTIFY!";
    }
#endif

    // start the editor
    KIO::DesktopExecParser parser(*offer, list);
    parser.setUrlsAreTempFiles(false);
    const QStringList params = parser.resultingArguments();
    mEditor = new KProcess(this);
    mEditor->setProgram(params);
    connect(mEditor, &KProcess::finished, this, &EditorWatcher::editorExited);
    mEditor->start();
    if (!mEditor->waitForStarted()) {
        return CannotStart;
    }
    mEditorRunning = true;

    mEditTime.start();
    return NoError;
}

bool EditorWatcher::fileChanged() const
{
    return mFileModified;
}

QUrl EditorWatcher::url() const
{
    return mUrl;
}

void EditorWatcher::inotifyEvent()
{
    assert(mHaveInotify);

#ifdef HAVE_SYS_INOTIFY_H
    int pending = -1;
    int offsetStartRead = 0; // where we read into buffer
    char buf[8192];
    assert(mInotifyFd > -1);
    ioctl(mInotifyFd, FIONREAD, &pending);

    while (pending > 0) {
        const int bytesToRead = qMin(pending, (int)sizeof(buf) - offsetStartRead);

        int bytesAvailable = read(mInotifyFd, &buf[offsetStartRead], bytesToRead);
        pending -= bytesAvailable;
        bytesAvailable += offsetStartRead;
        offsetStartRead = 0;

        int offsetCurrent = 0;
        while (bytesAvailable >= (int)sizeof(struct inotify_event)) {
            const struct inotify_event *const event = (struct inotify_event *)&buf[offsetCurrent];
            const int eventSize = sizeof(struct inotify_event) + event->len;
            if (bytesAvailable < eventSize) {
                break;
            }

            bytesAvailable -= eventSize;
            offsetCurrent += eventSize;
            if (event->mask & IN_OPEN) {
                mFileOpen = true;
            }
            if (event->mask & IN_CLOSE) {
                mFileOpen = false;
            }
            if (event->mask & (IN_MODIFY | IN_ATTRIB)) {
                mFileModified = true;
            }
        }
        if (bytesAvailable > 0) {
            // copy partial event to beginning of buffer
            memmove(buf, &buf[offsetCurrent], bytesAvailable);
            offsetStartRead = bytesAvailable;
        }
    }
#endif
    mTimer.start(500);
}

void EditorWatcher::editorExited()
{
    mEditorRunning = false;
    mTimer.start(500);
}

void EditorWatcher::checkEditDone()
{
    if (mEditorRunning || (mFileOpen && mHaveInotify) || mDone) {
        return;
    }

    static QStringList readOnlyMimeTypes;
    if (readOnlyMimeTypes.isEmpty()) {
        readOnlyMimeTypes << QStringLiteral("message/rfc822") << QStringLiteral("application/pdf");
    }

    // protect us against double-deletion by calling this method again while
    // the subeventloop of the message box is running
    mDone = true;

    // check if it's a mime type that's mostly handled read-only
    const bool isReadOnlyMimeType = (readOnlyMimeTypes.contains(mMimeType) || mMimeType.startsWith(QLatin1StringView("image/")));

    // nobody can edit that fast, we seem to be unable to detect
    // when the editor will be closed
    if (mEditTime.elapsed() <= 3000 && !isReadOnlyMimeType) {
        KMessageBox::information(mParentWidget,
                                 i18n("KMail is unable to detect when the chosen editor is closed. "
                                      "To avoid data loss, editing the attachment will be aborted."),
                                 i18nc("@title:window", "Unable to edit attachment"),
                                 QStringLiteral("UnableToEditAttachment"));
    }

    Q_EMIT editDone(this);
    deleteLater();
}

#include "moc_editorwatcher.cpp"
