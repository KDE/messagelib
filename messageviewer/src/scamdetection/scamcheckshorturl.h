/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer_export.h"

#include <QObject>
#include <QStringList>
#include <QUrl>

namespace MessageViewer
{
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

    [[nodiscard]] static bool isShortUrl(const QUrl &url);

    void expandedUrl(const QUrl &url);

    static void loadLongUrlServices();

private:
    static QStringList sSupportedServices;
};
}
