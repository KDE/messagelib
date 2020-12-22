/*
  SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#ifndef SCAMCHECKSHORTURL_H
#define SCAMCHECKSHORTURL_H

#include "messageviewer_export.h"

#include <QObject>
#include <QUrl>
#include <QStringList>

namespace MessageViewer {
/**
 * @brief The ScamCheckShortUrl class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ScamCheckShortUrl : public QObject
{
    Q_OBJECT
public:
    explicit ScamCheckShortUrl(QObject *parent = nullptr);
    ~ScamCheckShortUrl() override;

    static Q_REQUIRED_RESULT bool isShortUrl(const QUrl &url);

    void expandedUrl(const QUrl &url);

    static void loadLongUrlServices();

private:
    static QStringList sSupportedServices;
};
}

#endif // SCAMCHECKSHORTURL_H
