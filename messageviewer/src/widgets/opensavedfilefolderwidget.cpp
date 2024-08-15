/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "opensavedfilefolderwidget.h"

#include <KIO/JobUiDelegateFactory>
#include <KIO/OpenFileManagerWindowJob>
#include <KIO/OpenUrlJob>
#include <KLocalizedString>
#include <KMessageBox>

#include <QTimer>

#include <QAction>
#include <chrono>

using namespace std::chrono_literals;
using namespace MessageViewer;

OpenSavedFileFolderWidget::OpenSavedFileFolderWidget(QWidget *parent)
    : KMessageWidget(parent)
    , mTimer(new QTimer(this))
    , mShowFolderAction(new QAction(i18nc("@action - For we don't know if it's one or many attachments", "Open folder where attachment was saved"), this))
    , mOpenFileAction(new QAction(i18nc("@action", "Open File"), this))
{
    setPosition(KMessageWidget::Header);
    mTimer->setSingleShot(true);
    mTimer->setInterval(5s); // 5 seconds
    connect(mTimer, &QTimer::timeout, this, &OpenSavedFileFolderWidget::slotTimeOut);
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Positive);
    setWordWrap(true);
    auto action = this->findChild<QAction *>(); // should give us the close action...
    if (action) {
        connect(action, &QAction::triggered, this, &OpenSavedFileFolderWidget::slotExplicitlyClosed);
    }

    connect(mShowFolderAction, &QAction::triggered, this, &OpenSavedFileFolderWidget::slotOpenSavedFileFolder);
    addAction(mShowFolderAction);
    connect(mOpenFileAction, &QAction::triggered, this, &OpenSavedFileFolderWidget::slotOpenFile);
    addAction(mOpenFileAction);
}

OpenSavedFileFolderWidget::~OpenSavedFileFolderWidget() = default;

void OpenSavedFileFolderWidget::slotExplicitlyClosed()
{
    if (mTimer->isActive()) {
        mTimer->stop();
    }
}

void OpenSavedFileFolderWidget::setUrls(const QList<QUrl> &urls, FileType fileType)
{
    mUrls = urls;
    switch (fileType) {
    case FileType::Attachment:
        mShowFolderAction->setText(i18ncp("@action", "Open folder where attachment was saved", "Open folder where attachments were saved", mUrls.count()));
        break;
    case FileType::Pdf:
        mShowFolderAction->setText(i18nc("@action", "Open folder where PDF file was saved"));
        break;
    }
}

void OpenSavedFileFolderWidget::slotOpenFile()
{
    for (const auto &url : std::as_const(mUrls)) {
        auto job = new KIO::OpenUrlJob(url);
        job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, this));
        job->setDeleteTemporaryFile(true);
        connect(job, &KIO::OpenUrlJob::result, this, [this](KJob *job) {
            if (job->error() == KIO::ERR_USER_CANCELED) {
                KMessageBox::error(this, i18n("KMail was unable to open the attachment."), job->errorString());
            }
        });
        job->start();
    }
}

void OpenSavedFileFolderWidget::slotOpenSavedFileFolder()
{
    if (!mUrls.isEmpty()) {
        KIO::highlightInFileManager(mUrls);
        slotHideWarning();
    }
}

void OpenSavedFileFolderWidget::slotHideWarning()
{
    if (mTimer->isActive()) {
        mTimer->stop();
    }
    animatedHide();
}

void OpenSavedFileFolderWidget::slotShowWarning()
{
    if (mTimer->isActive()) {
        mTimer->stop();
    }
    mTimer->start();
    animatedShow();
}

void OpenSavedFileFolderWidget::slotTimeOut()
{
    animatedHide();
}

#include "moc_opensavedfilefolderwidget.cpp"
