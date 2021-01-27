/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagefactoryforwardjob.h"
#include "settings/messagecomposersettings.h"
#include <KIdentityManagement/IdentityManager>
#include <TemplateParser/TemplateParserJob>

using namespace MessageComposer;

MessageFactoryForwardJob::MessageFactoryForwardJob(QObject *parent)
    : QObject(parent)
{
}

MessageFactoryForwardJob::~MessageFactoryForwardJob()
{
}

void MessageFactoryForwardJob::start()
{
    auto parser = new TemplateParser::TemplateParserJob(mMsg, TemplateParser::TemplateParserJob::Forward, this);
    connect(parser, &TemplateParser::TemplateParserJob::parsingDone, this, &MessageFactoryForwardJob::slotParsingDone);
    connect(parser, &TemplateParser::TemplateParserJob::parsingFailed, this, &MessageFactoryForwardJob::slotParsingDone);
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
