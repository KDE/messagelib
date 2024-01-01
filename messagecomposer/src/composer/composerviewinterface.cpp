/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "composerviewinterface.h"
#include "composerviewbase.h"
#include "snippet/convertsnippetvariablesutil.h"
#include <KIO/Global>
#include <MessageComposer/AttachmentModel>
using namespace MessageComposer;
ComposerViewInterface::ComposerViewInterface(ComposerViewBase *composerView)
    : mComposerView(composerView)
{
}

ComposerViewInterface::~ComposerViewInterface() = default;

QString ComposerViewInterface::subject() const
{
    if (mComposerView) {
        return mComposerView->subject();
    }
    return {};
}

QString ComposerViewInterface::to() const
{
    if (mComposerView) {
        return mComposerView->to();
    }
    return {};
}

QString ComposerViewInterface::cc() const
{
    if (mComposerView) {
        return mComposerView->cc();
    }
    return {};
}

QString ComposerViewInterface::bcc() const
{
    if (mComposerView) {
        return mComposerView->bcc();
    }
    return {};
}

QString ComposerViewInterface::from() const
{
    if (mComposerView) {
        return mComposerView->from();
    }
    return {};
}

QString ComposerViewInterface::replyTo() const
{
    if (mComposerView) {
        return mComposerView->replyTo();
    }
    return {};
}

MessageComposer::ComposerAttachmentInterface ComposerViewInterface::attachments() const
{
    MessageComposer::ComposerAttachmentInterface attachmentInterface;
    if (mComposerView) {
        const int countElement = mComposerView->attachmentModel()->attachments().count();
        attachmentInterface.setCount(countElement);
        QStringList fileNames;
        QStringList nameAndSize;
        QStringList names;

        fileNames.reserve(countElement);
        nameAndSize.reserve(countElement);
        names.reserve(countElement);
        const auto attachments = mComposerView->attachmentModel()->attachments();
        for (const MessageCore::AttachmentPart::Ptr &attachment : attachments) {
            fileNames.append(attachment->fileName());
            names.append(attachment->name());
            nameAndSize.append(QStringLiteral("%1 (%2)").arg(attachment->name(), KIO::convertSize(attachment->size())));
        }
        attachmentInterface.setNames(names);
        attachmentInterface.setNamesAndSize(nameAndSize);
        attachmentInterface.setFileNames(fileNames);
    }
    return attachmentInterface;
}

QString ComposerViewInterface::shortDate() const
{
    return MessageComposer::ConvertSnippetVariablesUtil::shortDate();
}

QString ComposerViewInterface::longDate() const
{
    return MessageComposer::ConvertSnippetVariablesUtil::longDate();
}

QString ComposerViewInterface::shortTime() const
{
    return MessageComposer::ConvertSnippetVariablesUtil::shortTime();
}

QString ComposerViewInterface::longTime() const
{
    return MessageComposer::ConvertSnippetVariablesUtil::longTime();
}

QString ComposerViewInterface::insertDayOfWeek() const
{
    return MessageComposer::ConvertSnippetVariablesUtil::insertDayOfWeek();
}
