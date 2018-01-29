/*
  Copyright (C) 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Copyright (c) 2010 Leo Franchi <lfranchi@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "setupenv.h"
#include "messagepartthemes/default/messagepartrendererfactory.h"

#include <QStandardPaths>

#include <QDir>
#include <QFile>
#include <QIcon>
#include <QLocale>

void MessageViewer::Test::setupEnv()
{
    qputenv("KDEHOME", QFile::encodeName(QDir::homePath() + QString::fromLatin1(
                                            "/.qttest")).constData());
    qputenv("TZ", "UTC");
    QStandardPaths::setTestModeEnabled(true);
    QIcon::setThemeName(QStringLiteral("breeze"));
    QLocale::setDefault(QLocale(QStringLiteral("en_US")));

    // disable plugin loading, so kdepim-addons doesn't interfere with our tests
    MessagePartRendererFactory::instance()->setPluginPath(QString());
}
