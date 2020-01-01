/*
   Copyright (C) 2016-2020 Laurent Montel <montel@kde.org>

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

using namespace MessageComposer;

class MessageComposer::PluginEditorCheckBeforeSendParamsPrivate
{
public:
    PluginEditorCheckBeforeSendParamsPrivate()
    {
    }

    QString ccAddresses;
    QString bccAddresses;
    QString toAddresses;
    QString plainText;
    QString subject;
    QString defaultDomain;
    int transportId = -1;
    bool isHtml = false;
    uint identity = 0;
    bool hasAttachment = false;
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
        d->defaultDomain = other.defaultDomain();
        d->hasAttachment = other.hasAttachment();
        d->transportId = other.transportId();
        d->bccAddresses = other.bccAddresses();
        d->ccAddresses = other.ccAddresses();
        d->toAddresses = other.toAddresses();
    }
    return *this;
}

bool PluginEditorCheckBeforeSendParams::operator ==(const PluginEditorCheckBeforeSendParams &other) const
{
    return (d->subject == other.subject())
           && (d->identity == other.identity())
           && (d->isHtml == other.isHtmlMail())
           && (d->plainText == other.plainText())
           && (d->defaultDomain == other.defaultDomain())
           && (d->hasAttachment == other.hasAttachment())
           && (d->transportId == other.transportId())
           && (d->bccAddresses == other.bccAddresses())
           && (d->ccAddresses == other.ccAddresses())
           && (d->toAddresses == other.toAddresses());
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

void PluginEditorCheckBeforeSendParams::setBccAddresses(const QString &lst)
{
    d->bccAddresses = lst;
}

QString PluginEditorCheckBeforeSendParams::bccAddresses() const
{
    return d->bccAddresses;
}

void PluginEditorCheckBeforeSendParams::setToAddresses(const QString &lst)
{
    d->toAddresses = lst;
}

QString PluginEditorCheckBeforeSendParams::toAddresses() const
{
    return d->toAddresses;
}

void PluginEditorCheckBeforeSendParams::setCcAddresses(const QString &lst)
{
    d->ccAddresses = lst;
}

QString PluginEditorCheckBeforeSendParams::ccAddresses() const
{
    return d->ccAddresses;
}

void PluginEditorCheckBeforeSendParams::setDefaultDomain(const QString &domain)
{
    d->defaultDomain = domain;
}

QString PluginEditorCheckBeforeSendParams::defaultDomain() const
{
    return d->defaultDomain;
}

bool PluginEditorCheckBeforeSendParams::hasAttachment() const
{
    return d->hasAttachment;
}

void PluginEditorCheckBeforeSendParams::setHasAttachment(bool b)
{
    d->hasAttachment = b;
}

int PluginEditorCheckBeforeSendParams::transportId() const
{
    return d->transportId;
}

void PluginEditorCheckBeforeSendParams::setTransportId(int id)
{
    d->transportId = id;
}
