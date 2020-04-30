/*
   Copyright (C) 2014-2020 Laurent Montel <montel@kde.org>

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

#include "opensavedfilefolderwidget.h"

#include <KLocalizedString>
#include <KIO/OpenFileManagerWindowJob>

#include <QTimer>

#include <QAction>

using namespace MessageViewer;

OpenSavedFileFolderWidget::OpenSavedFileFolderWidget(QWidget *parent)
    : KMessageWidget(parent)
{
    mTimer = new QTimer(this);
    mTimer->setSingleShot(true);
    mTimer->setInterval(5000); // 5 seconds
    connect(mTimer, &QTimer::timeout, this, &OpenSavedFileFolderWidget::slotTimeOut);
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Positive);
    setWordWrap(true);
    QAction *action = this->findChild<QAction *>(); // should give us the close action...
    if (action) {
        connect(action, &QAction::triggered, this,
                &OpenSavedFileFolderWidget::slotExplicitlyClosed);
    }

    mShowFolderAction = new QAction(i18n("Open folder where attachment was saved"), this);
    connect(mShowFolderAction, &QAction::triggered, this,
            &OpenSavedFileFolderWidget::slotOpenSavedFileFolder);
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
    switch(fileType) {
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
