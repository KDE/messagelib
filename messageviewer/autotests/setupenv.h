/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGECORE_SETUPENV_H
#define MESSAGECORE_SETUPENV_H

namespace MessageViewer
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
