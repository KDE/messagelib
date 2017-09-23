/*
    This file is part of KMail, the KDE mail client.
    Copyright (c) 2017 Sandro Knauß <sknauss@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#include "messagepartrendererfactory.h"
#include "messagepartrendererfactory_p.h"

#include "messagepartrendererbase.h"
#include "messageviewer_debug.h"

#include "plugins/attachmentmessagepartrenderer.h"
#include "plugins/messagepartrenderer.h"
#include "plugins/textmessagepartrenderer.h"

using namespace MessageViewer;

void MessagePartRendererFactoryPrivate::setup()
{
    if (mAll.isEmpty()) {
        initalize_builtin_renderers();
        loadPlugins();
    }
}

void MessagePartRendererFactoryPrivate::loadPlugins()
{
    qCDebug(MESSAGEVIEWER_LOG) << "plugin loading is not enabled in libmimetreeparser";
}

void MessagePartRendererFactoryPrivate::initalize_builtin_renderers()
{
    insert(QStringLiteral("MimeTreeParser::MessagePart"), new MessagePartRenderer());
    insert(QStringLiteral("MimeTreeParser::TextMessagePart"), new TextMessagePartRenderer());
    insert(QStringLiteral("MimeTreeParser::AttachmentMessagePart"), new AttachmentMessagePartRenderer());
}

void MessagePartRendererFactoryPrivate::insert(const QString &type, MessagePartRendererBase *formatter)
{
    if (type.isEmpty() || !formatter) {
        return;
    }

    mAll[type].insert(mAll[type].begin(), formatter);
}

MessagePartRendererFactory::MessagePartRendererFactory()
    : d(new MessagePartRendererFactoryPrivate)
{
}

MessagePartRendererFactory::~MessagePartRendererFactory()
{
}

MessagePartRendererFactory* MessagePartRendererFactory::instance()
{
    static std::unique_ptr<MessagePartRendererFactory> singeltonRendererFactory(new MessagePartRendererFactory);
    return singeltonRendererFactory.get();
}

std::vector<MessagePartRendererBase *> MessagePartRendererFactory::typeRegistry(
    const QString &type) const
{
    d->setup();
    Q_ASSERT(!d->mAll.isEmpty());
    return d->mAll.value(type);
}
