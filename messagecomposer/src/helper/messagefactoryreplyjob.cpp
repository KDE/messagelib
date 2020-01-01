/*
   Copyright (C) 2017-2020 Laurent Montel <montel@kde.org>

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

#include "messagefactoryreplyjob.h"
#include "settings/messagecomposersettings.h"
#include <TemplateParser/TemplateParserJob>
#include <KIdentityManagement/IdentityManager>

using namespace MessageComposer;

MessageFactoryReplyJob::MessageFactoryReplyJob(QObject *parent)
    : QObject(parent)
{
}

MessageFactoryReplyJob::~MessageFactoryReplyJob()
{
}

void MessageFactoryReplyJob::start()
{
    TemplateParser::TemplateParserJob *parser = new TemplateParser::TemplateParserJob(mMsg, (mReplyAll ? TemplateParser::TemplateParserJob::ReplyAll : TemplateParser::TemplateParserJob::Reply), this);
    connect(parser, &TemplateParser::TemplateParserJob::parsingDone, this, &MessageFactoryReplyJob::slotReplyDone);
    parser->setIdentityManager(mIdentityManager);
    parser->setCharsets(MessageComposerSettings::self()->preferredCharsets());
    parser->setWordWrap(MessageComposerSettings::wordWrap(), MessageComposerSettings::lineWrapWidth());
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

void MessageFactoryReplyJob::setIdentityManager(KIdentityManagement::IdentityManager *identityManager)
{
    mIdentityManager = identityManager;
}
