/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagepart.h"

using namespace MessageComposer;

class Q_DECL_HIDDEN MessagePart::Private
{
public:
    Private()
    {
    }
};

MessagePart::MessagePart(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

MessagePart::~MessagePart()
{
    delete d;
}
