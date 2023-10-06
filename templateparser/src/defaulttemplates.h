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
[[nodiscard]] TEMPLATEPARSER_EXPORT QString defaultNewMessage();
[[nodiscard]] TEMPLATEPARSER_EXPORT QString defaultReply();
[[nodiscard]] TEMPLATEPARSER_EXPORT QString defaultReplyAll();
[[nodiscard]] TEMPLATEPARSER_EXPORT QString defaultForward();
[[nodiscard]] TEMPLATEPARSER_EXPORT QString defaultQuoteString();
}
}
