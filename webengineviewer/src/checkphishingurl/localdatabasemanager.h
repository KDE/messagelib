/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
#include <QUrl>
#include <WebEngineViewer/CheckPhishingUrlUtil>
namespace WebEngineViewer
{
class WebEngineView;
class LocalDataBaseManagerPrivate;
/**
 * @brief The LocalDataBaseManager class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT LocalDataBaseManager : public QObject
{
    Q_OBJECT
public:
    explicit LocalDataBaseManager(QObject *parent = nullptr);
    ~LocalDataBaseManager() override;

    void checkUrl(const QUrl &url);

    void initialize();

Q_SIGNALS:
    void checkUrlFinished(const QUrl &url, WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status);

protected:
    explicit LocalDataBaseManager(LocalDataBaseManagerPrivate *impl, QObject *parent = nullptr);

    LocalDataBaseManagerPrivate *const d;
};
}
