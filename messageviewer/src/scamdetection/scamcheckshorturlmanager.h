/*
  SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#ifndef SCAMCHECKSHORTURLMANAGER_H
#define SCAMCHECKSHORTURLMANAGER_H

#include <QObject>
#include "messageviewer_export.h"
namespace MessageViewer {
class ScamCheckShortUrl;
class ScamCheckShortUrlManagerPrivate;
/**
 * @brief The ScamCheckShortUrlManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ScamCheckShortUrlManager : public QObject
{
    Q_OBJECT
public:
    explicit ScamCheckShortUrlManager(QObject *parent = nullptr);
    ~ScamCheckShortUrlManager();
    static ScamCheckShortUrlManager *self();
    Q_REQUIRED_RESULT ScamCheckShortUrl *scamCheckShortUrl() const;
private:
    ScamCheckShortUrlManagerPrivate *const d;
};
}
#endif // SCAMCHECKSHORTURLMANAGER_H
