/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "setupenv.h"

#include <QStandardPaths>

#include <QDir>
#include <QFile>

void MessageViewer::Test::setupEnv()
{
    qputenv("LC_ALL", "C");
    qputenv("KDEHOME", QFile::encodeName(QDir::homePath() + QLatin1String("/.qttest")).constData());
    QStandardPaths::setTestModeEnabled(true);
}
