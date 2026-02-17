/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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
/*!
 * \class WebEngineViewer::LocalDataBaseManager
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/LocalDataBaseManager
 *
 * \brief The LocalDataBaseManager class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT LocalDataBaseManager : public QObject
{
    Q_OBJECT
public:
    /*! Constructs a LocalDataBaseManager with the given parent. */
    explicit LocalDataBaseManager(QObject *parent = nullptr);
    /*! Destroys the LocalDataBaseManager object. */
    ~LocalDataBaseManager() override;

    /*! Checks the given URL. */
    void checkUrl(const QUrl &url);

    /*! Initializes the database manager. */
    void initialize();

Q_SIGNALS:
    /*! Emitted when a URL check is finished. */
    void checkUrlFinished(const QUrl &url, WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status);

protected:
    /*! Constructs a LocalDataBaseManager with the given implementation. */
    explicit LocalDataBaseManager(LocalDataBaseManagerPrivate *impl, QObject *parent = nullptr);

    LocalDataBaseManagerPrivate *const d;
};
}
