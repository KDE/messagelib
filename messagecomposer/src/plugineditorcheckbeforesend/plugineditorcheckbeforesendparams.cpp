/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "plugineditorcheckbeforesendparams.h"

#include <QStringList>

using namespace MessageComposer;

class MessageComposer::PluginEditorCheckBeforeSendParamsPrivate
{
public:
    PluginEditorCheckBeforeSendParamsPrivate()
        : identity(-1),
          isHtml(false)
    {

    }
    QStringList listAddress;
    QString plainText;
    QString subject;
    uint identity;
    bool isHtml;
};

PluginEditorCheckBeforeSendParams::PluginEditorCheckBeforeSendParams()
    : d(new MessageComposer::PluginEditorCheckBeforeSendParamsPrivate)
{

}

PluginEditorCheckBeforeSendParams::PluginEditorCheckBeforeSendParams(const PluginEditorCheckBeforeSendParams &other)
    : d(new MessageComposer::PluginEditorCheckBeforeSendParamsPrivate)
{
    (*this) = other;
}


PluginEditorCheckBeforeSendParams::~PluginEditorCheckBeforeSendParams()
{
    delete d;
}

PluginEditorCheckBeforeSendParams &PluginEditorCheckBeforeSendParams::operator=(const PluginEditorCheckBeforeSendParams &other)
{
    if (this != &other) {
        d->subject = other.subject();
        d->identity = other.identity();
        d->isHtml = other.isHtmlMail();
        d->plainText = other.plainText();
        d->listAddress = other.addresses();
    }
    return *this;
}

bool PluginEditorCheckBeforeSendParams::operator ==(const PluginEditorCheckBeforeSendParams &other) const
{
    return (d->subject == other.subject()) &&
            (d->identity == other.identity()) &&
            (d->isHtml == other.isHtmlMail()) &&
            (d->plainText == other.plainText()) &&
            (d->listAddress == other.addresses());
}

void PluginEditorCheckBeforeSendParams::setSubject(const QString &subject)
{
    d->subject = subject;
}

QString PluginEditorCheckBeforeSendParams::subject() const
{
    return d->subject;
}

void PluginEditorCheckBeforeSendParams::setIdentity(uint currentIdentity)
{
    d->identity = currentIdentity;
}

uint PluginEditorCheckBeforeSendParams::identity() const
{
    return d->identity;
}

bool PluginEditorCheckBeforeSendParams::isHtmlMail() const
{
    return d->isHtml;
}

void PluginEditorCheckBeforeSendParams::setHtmlMail(bool html)
{
    d->isHtml = html;
}

void PluginEditorCheckBeforeSendParams::setPlainText(const QString &text)
{
    d->plainText = text;
}

QString PluginEditorCheckBeforeSendParams::plainText() const
{
    return d->plainText;
}

void PluginEditorCheckBeforeSendParams::setAddresses(const QStringList &lst)
{
    d->listAddress = lst;
}

QStringList PluginEditorCheckBeforeSendParams::addresses() const
{
    return d->listAddress;
}
