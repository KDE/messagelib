/*
  This file is part of KMail, the KDE mail client.
  SPDX-FileCopyrightText: 2011-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGELISTUTIL_H
#define MESSAGELISTUTIL_H

#include <messagelist_export.h>
#include <QString>
#include <QColor>

namespace Akonadi {
class Item;
}
namespace MessageList {
namespace Util {
MESSAGELIST_EXPORT void deleteConfig(const QString &collectionId);
MESSAGELIST_EXPORT QColor unreadDefaultMessageColor();
MESSAGELIST_EXPORT QColor importantDefaultMessageColor();
MESSAGELIST_EXPORT QColor todoDefaultMessageColor();
/// Returns the first few lines of the actual email text if available.
MESSAGELIST_EXPORT QString contentSummary(const Akonadi::Item &item);
}
}

#endif /* MESSAGELISTUTIL_H */
