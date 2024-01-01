/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "updatedatabaseinfo.h"
#include "webengineviewer_export.h"
#include <QObject>
#include <memory>
namespace WebEngineViewer
{
class CreateDatabaseFileJobPrivate;
/**
 * @brief The CreateDatabaseFileJob class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT CreateDatabaseFileJob : public QObject
{
    Q_OBJECT
public:
    explicit CreateDatabaseFileJob(QObject *parent = nullptr);
    ~CreateDatabaseFileJob() override;
    void start();
    void setFileName(const QString &filename);
    [[nodiscard]] bool canStart() const;
    void setUpdateDataBaseInfo(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase);

Q_SIGNALS:
    void finished(bool success, const QString &newStateDatabase, const QString &minimumWaitDurationStr);

private:
    std::unique_ptr<CreateDatabaseFileJobPrivate> const d;
};
}
