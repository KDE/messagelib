/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "setupenv.h"
#include "messagepartthemes/default/messagepartrendererfactory.h"

#include <QStandardPaths>

#include <QIcon>
#include <QLocale>

void MessageViewer::Test::setupEnv()
{
    qputenv("TZ", "UTC");
    QStandardPaths::setTestModeEnabled(true);
    QIcon::setThemeName(QStringLiteral("breeze"));
    QLocale::setDefault(QLocale(QStringLiteral("en_US")));

    // disable plugin loading, so kdepim-addons doesn't interfere with our tests
    MessagePartRendererFactory::instance()->setPluginPath(QString());
}
