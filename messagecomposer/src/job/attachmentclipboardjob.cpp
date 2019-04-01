/*
  Copyright (c) 2015-2019 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "attachmentclipboardjob.h"
#include <KLocalizedString>
#include <QApplication>
#include <QClipboard>
#include <QInputDialog>

using namespace MessageComposer;

AttachmentClipBoardJob::AttachmentClipBoardJob(QObject *parent)
    : MessageCore::AttachmentLoadJob(parent)
{
}

AttachmentClipBoardJob::~AttachmentClipBoardJob()
{
}

void AttachmentClipBoardJob::addAttachment(const QByteArray &data, const QString &attachmentName)
{
    MessageCore::AttachmentPart::Ptr attachment = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart());
    if (!data.isEmpty()) {
        attachment->setName(attachmentName);
        attachment->setFileName(attachmentName);
        attachment->setData(data);
        attachment->setMimeType("text/plain");
        // TODO what about the other fields?
    }
    setAttachmentPart(attachment);
    emitResult(); // Success.
}

void AttachmentClipBoardJob::doStart()
{
    QClipboard *clip = QApplication::clipboard();
    const QString clipText = clip->text();
    if (clipText.isEmpty()) {
        setError(KJob::UserDefinedError);
        setErrorText(i18n("No text found in Clipboard"));
        emitResult();
    } else {
        QString attachmentName = QInputDialog::getText(nullptr, i18n("Define Attachment Name"), i18n("Attachment Name:"));
        if (attachmentName.isEmpty()) {
            attachmentName = i18n("Clipboard Text");
        }
        addAttachment(clipText.toUtf8(), attachmentName);
    }
}
