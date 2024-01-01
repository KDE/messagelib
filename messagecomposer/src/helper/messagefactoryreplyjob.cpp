/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagefactoryreplyjob.h"
#include "settings/messagecomposersettings.h"
#include <TemplateParser/TemplateParserJob>

using namespace MessageComposer;

MessageFactoryReplyJob::MessageFactoryReplyJob(QObject *parent)
    : QObject(parent)
{
}

MessageFactoryReplyJob::~MessageFactoryReplyJob() = default;

void MessageFactoryReplyJob::start()
{
    auto parser =
        new TemplateParser::TemplateParserJob(mMsg, (mReplyAll ? TemplateParser::TemplateParserJob::ReplyAll : TemplateParser::TemplateParserJob::Reply), this);
    connect(parser, &TemplateParser::TemplateParserJob::parsingDone, this, &MessageFactoryReplyJob::slotReplyDone);
    connect(parser, &TemplateParser::TemplateParserJob::parsingFailed, this, &MessageFactoryReplyJob::slotReplyDone);
    parser->setIdentityManager(mIdentityManager);
    parser->setWordWrap(MessageComposerSettings::wordWrap(), MessageComposerSettings::lineWrapWidth());
    parser->setReplyAsHtml(mReplyAsHtml);
    if (MessageComposer::MessageComposerSettings::quoteSelectionOnly()) {
        parser->setSelection(mSelection);
    }
    parser->setAllowDecryption(true);
    if (!mTemplate.isEmpty()) {
        parser->process(mTemplate, mOrigMsg);
    } else {
        parser->process(mOrigMsg, mCollection.id());
    }
}

void MessageFactoryReplyJob::slotReplyDone()
{
    Q_EMIT replyDone(mMsg, mReplyAll);
    deleteLater();
}

void MessageFactoryReplyJob::setCollection(const Akonadi::Collection &collection)
{
    mCollection = collection;
}

void MessageFactoryReplyJob::setReplyAsHtml(bool replyAsHtml)
{
    mReplyAsHtml = replyAsHtml;
}

void MessageFactoryReplyJob::setReplyAll(bool replyAll)
{
    mReplyAll = replyAll;
}

void MessageFactoryReplyJob::setMsg(const KMime::Message::Ptr &msg)
{
    mMsg = msg;
}

void MessageFactoryReplyJob::setTemplate(const QString &tmpl)
{
    mTemplate = tmpl;
}

void MessageFactoryReplyJob::setSelection(const QString &selection)
{
    mSelection = selection;
}

void MessageFactoryReplyJob::setOrigMsg(const KMime::Message::Ptr &origMsg)
{
    mOrigMsg = origMsg;
}

void MessageFactoryReplyJob::setIdentityManager(KIdentityManagementCore::IdentityManager *identityManager)
{
    mIdentityManager = identityManager;
}

#include "moc_messagefactoryreplyjob.cpp"
