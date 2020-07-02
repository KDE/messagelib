/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "globalpart.h"

using namespace MessageComposer;

class Q_DECL_HIDDEN GlobalPart::Private
{
public:
    Private()
    {
    }

    QVector<QByteArray> charsets;
    QWidget *parentWidgetForGui = nullptr;
    bool guiEnabled = true;
    bool fallbackCharsetEnabled = false;
    bool allow8Bit = false;
    bool MDNRequested = false;
    bool requestDeleveryConfirmation = false;
};

GlobalPart::GlobalPart(QObject *parent)
    : MessagePart(parent)
    , d(new Private)
{
}

GlobalPart::~GlobalPart()
{
    delete d;
}

bool GlobalPart::isGuiEnabled() const
{
    return d->guiEnabled;
}

void GlobalPart::setGuiEnabled(bool enabled)
{
    d->guiEnabled = enabled;
}

QWidget *GlobalPart::parentWidgetForGui() const
{
    return d->parentWidgetForGui;
}

void GlobalPart::setParentWidgetForGui(QWidget *widget)
{
    d->parentWidgetForGui = widget;
}

bool GlobalPart::isFallbackCharsetEnabled() const
{
    return d->fallbackCharsetEnabled;
}

void GlobalPart::setFallbackCharsetEnabled(bool enabled)
{
    d->fallbackCharsetEnabled = enabled;
}

QVector<QByteArray> GlobalPart::charsets(bool forceFallback) const
{
    QVector<QByteArray> ret = d->charsets;
    if (d->fallbackCharsetEnabled || forceFallback) {
        ret << "us-ascii";
        ret << "utf-8";
    }
    return ret;
}

void GlobalPart::setCharsets(const QVector<QByteArray> &charsets)
{
    d->charsets = charsets;
}

bool GlobalPart::is8BitAllowed() const
{
    return d->allow8Bit;
}

void GlobalPart::set8BitAllowed(bool allowed)
{
    d->allow8Bit = allowed;
}

bool GlobalPart::MDNRequested() const
{
    return d->MDNRequested;
}

void GlobalPart::setMDNRequested(bool requestMDN)
{
    d->MDNRequested = requestMDN;
}

bool GlobalPart::requestDeleveryConfirmation() const
{
    return d->requestDeleveryConfirmation;
}

void GlobalPart::setRequestDeleveryConfirmation(bool value)
{
    d->requestDeleveryConfirmation = value;
}
