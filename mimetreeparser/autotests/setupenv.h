/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MIMETREEPARSER_TESTS_SETUPENV_H
#define MIMETREEPARSER_TESTS_SETUPENV_H

namespace MimeTreeParser
{
namespace Test
{
/**
 * setup a environment variables for tests:
 * * set LC_ALL to C
 * * set KDEHOME
 */
void setupEnv();
}
}

#endif
