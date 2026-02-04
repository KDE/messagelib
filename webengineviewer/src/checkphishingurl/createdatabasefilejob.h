/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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
/*!
 * \class WebEngineViewer::CreateDatabaseFileJob
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/CreateDatabaseFileJob
 *
 * \brief The CreateDatabaseFileJob class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT CreateDatabaseFileJob : public QObject
{
    Q_OBJECT
public:
    /*! Constructs a CreateDatabaseFileJob object. */
    explicit CreateDatabaseFileJob(QObject *parent = nullptr);
    /*! Destroys the CreateDatabaseFileJob object. */
    ~CreateDatabaseFileJob() override;
    /*! Starts the database file creation job. */
    void start();
    /*! Sets the file name for the database file. */
    void setFileName(const QString &filename);
    /*! Returns whether the job can be started. */
    [[nodiscard]] bool canStart() const;
    /*! Sets the update database information. */
    void setUpdateDataBaseInfo(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase);

Q_SIGNALS:
    /*! Emitted when the database file creation job has finished. */
    void finished(bool success, const QString &newStateDatabase, const QString &minimumWaitDurationStr);

private:
    std::unique_ptr<CreateDatabaseFileJobPrivate> const d;
};
}
