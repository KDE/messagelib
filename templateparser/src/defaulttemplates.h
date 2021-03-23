/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#pragma once

#include "templateparser_export.h"
#include <QString>

namespace TemplateParser
{
/** Default new/reply/forward templates. */
namespace DefaultTemplates
{
TEMPLATEPARSER_EXPORT Q_REQUIRED_RESULT QString defaultNewMessage();
TEMPLATEPARSER_EXPORT Q_REQUIRED_RESULT QString defaultReply();
TEMPLATEPARSER_EXPORT Q_REQUIRED_RESULT QString defaultReplyAll();
TEMPLATEPARSER_EXPORT Q_REQUIRED_RESULT QString defaultForward();
TEMPLATEPARSER_EXPORT Q_REQUIRED_RESULT QString defaultQuoteString();
}
}

