/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "opensavedfilefolderwidget.h"

#include <KIO/OpenFileManagerWindowJob>
#include <KLocalizedString>

#include <QTimer>

#include <QAction>

using namespace MessageViewer;

OpenSavedFileFolderWidget::OpenSavedFileFolderWidget(QWidget *parent)
    : KMessageWidget(parent)
    , mTimer(new QTimer(this))
    , mShowFolderAction(new QAction(i18nc("@action", "Open folder where attachment was saved"), this))
{
    mTimer->setSingleShot(true);
    mTimer->setInterval(5000); // 5 seconds
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
}

OpenSavedFileFolderWidget::~OpenSavedFileFolderWidget()
{
}

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
        mShowFolderAction->setText(i18np("Open folder where attachment was saved", "Open folder where attachments were saved", mUrls.count()));
        break;
    case FileType::Pdf:
        mShowFolderAction->setText(i18n("Open folder where PDF file was saved"));
        break;
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
