/*
   Copyright (c) 2017 Laurent Montel <montel@kde.org>

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
#include "config-messagecomposer.h"
#include "settings/messagecomposersettings.h"
#ifdef KDEPIM_TEMPLATEPARSER_ASYNC_BUILD
#include <TemplateParser/TemplateParserJob>
#else
#include <TemplateParser/TemplateParser>
#endif
#include <KIdentityManagement/IdentityManager>

using namespace MessageComposer;

MessageFactoryReplyJob::MessageFactoryReplyJob(QObject *parent)
    : QObject(parent),
      mMsg(nullptr),
      mOrigMsg(nullptr),
      mReplyAll(false),
      mIdentityManager(nullptr)
{

}

MessageFactoryReplyJob::~MessageFactoryReplyJob()
{

}

void MessageFactoryReplyJob::start()
{
#ifdef KDEPIM_TEMPLATEPARSER_ASYNC_BUILD
    TemplateParser::TemplateParserJob *parser = new TemplateParser::TemplateParserJob(mMsg, (mReplyAll ? TemplateParser::TemplateParserJob::ReplyAll : TemplateParser::TemplateParserJob::Reply));
    connect(parser, &TemplateParser::TemplateParserJob::parsingDone, this, &MessageFactoryReplyJob::slotReplyDone);
    parser->setIdentityManager(mIdentityManager);
    parser->setCharsets(MessageComposerSettings::self()->preferredCharsets());
    parser->setWordWrap(MessageComposerSettings::wordWrap(), MessageComposerSettings::lineWrapWidth());
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 1)
    if (MessageComposer::MessageComposerSettings::quoteSelectionOnly()) {
        parser->setSelection(mSelection);
    }
#endif
    parser->setAllowDecryption(true);
    if (!mTemplate.isEmpty()) {
        parser->process(mTemplate, mOrigMsg);
    } else {
        parser->process(mOrigMsg, mCollection);
    }
    slotReplyDone();
#else
    TemplateParser::TemplateParser parser(mMsg, (mReplyAll ? TemplateParser::TemplateParser::ReplyAll : TemplateParser::TemplateParser::Reply));
    parser.setIdentityManager(mIdentityManager);
    parser.setCharsets(MessageComposerSettings::self()->preferredCharsets());
    parser.setWordWrap(MessageComposerSettings::wordWrap(), MessageComposerSettings::lineWrapWidth());
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 1)
    if (MessageComposer::MessageComposerSettings::quoteSelectionOnly()) {
        parser.setSelection(mSelection);
    }
#endif
    parser.setAllowDecryption(true);
    if (!mTemplate.isEmpty()) {
        parser.process(mTemplate, mOrigMsg);
    } else {
        parser.process(mOrigMsg, mCollection);
    }
    slotReplyDone();
#endif

}

void MessageFactoryReplyJob::slotReplyDone()
{
    Q_EMIT replyDone(mMsg);
    deleteLater();
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
