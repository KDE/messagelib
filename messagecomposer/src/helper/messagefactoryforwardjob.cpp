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

#include "messagefactoryforwardjob.h"
#include "settings/messagecomposersettings.h"
#include <TemplateParser/TemplateParserJob>
#include <KIdentityManagement/IdentityManager>

using namespace MessageComposer;

MessageFactoryForwardJob::MessageFactoryForwardJob(QObject *parent)
    : QObject(parent),
      mMsg(nullptr),
      mOrigMsg(nullptr),
      mIdentityManager(nullptr)
{

}

MessageFactoryForwardJob::~MessageFactoryForwardJob()
{

}

void MessageFactoryForwardJob::start()
{
    TemplateParser::TemplateParserJob *parser = new TemplateParser::TemplateParserJob(mMsg, TemplateParser::TemplateParserJob::Forward);
    connect(parser, &TemplateParser::TemplateParserJob::parsingDone, this, &MessageFactoryForwardJob::slotParsingDone);
    parser->setIdentityManager(mIdentityManager);
    parser->setCharsets(MessageComposerSettings::self()->preferredCharsets());
    parser->setSelection(mSelection);
    if (!mTemplate.isEmpty()) {
        parser->process(mTemplate, mOrigMsg);
    } else {
        parser->process(mOrigMsg, mCollection.id());
    }
}

void MessageFactoryForwardJob::slotParsingDone()
{
    Q_EMIT forwardDone(mMsg);
    deleteLater();
}

void MessageFactoryForwardJob::setCollection(const Akonadi::Collection &collection)
{
    mCollection = collection;
}

void MessageFactoryForwardJob::setMsg(const KMime::Message::Ptr &msg)
{
    mMsg = msg;
}

void MessageFactoryForwardJob::setTemplate(const QString &tmpl)
{
    mTemplate = tmpl;
}

void MessageFactoryForwardJob::setSelection(const QString &selection)
{
    mSelection = selection;
}

void MessageFactoryForwardJob::setOrigMsg(const KMime::Message::Ptr &origMsg)
{
    mOrigMsg = origMsg;
}

void MessageFactoryForwardJob::setIdentityManager(KIdentityManagement::IdentityManager *identityManager)
{
    mIdentityManager = identityManager;
}
