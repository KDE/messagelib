/*
  SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
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

AttachmentClipBoardJob::~AttachmentClipBoardJob() = default;

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
        bool ok = false;
        QString attachmentName = QInputDialog::getText(nullptr, i18nc("@title:window", "Define Attachment Name"), i18n("Attachment Name:"), {}, {}, &ok);
        if (ok) {
            if (attachmentName.isEmpty()) {
                attachmentName = i18n("Clipboard Text");
            }
            addAttachment(clipText.toUtf8(), attachmentName);
        }
    }
}

#include "moc_attachmentclipboardjob.cpp"
