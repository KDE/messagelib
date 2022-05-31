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
Q_REQUIRED_RESULT TEMPLATEPARSER_EXPORT QString defaultNewMessage();
Q_REQUIRED_RESULT TEMPLATEPARSER_EXPORT QString defaultReply();
Q_REQUIRED_RESULT TEMPLATEPARSER_EXPORT QString defaultReplyAll();
Q_REQUIRED_RESULT TEMPLATEPARSER_EXPORT QString defaultForward();
Q_REQUIRED_RESULT TEMPLATEPARSER_EXPORT QString defaultQuoteString();
}
}
