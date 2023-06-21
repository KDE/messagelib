/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagepart.h"

using namespace MessageComposer;

MessagePart::MessagePart(QObject *parent)
    : QObject(parent)
{
}

MessagePart::~MessagePart() = default;

#include "moc_messagepart.cpp"
