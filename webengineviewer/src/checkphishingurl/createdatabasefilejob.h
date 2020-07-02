/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CREATEDATABASEFILEJOB_H
#define CREATEDATABASEFILEJOB_H

#include <QObject>
#include "webengineviewer_export.h"
#include "updatedatabaseinfo.h"

namespace WebEngineViewer {
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
    ~CreateDatabaseFileJob();
    void start();
    void setFileName(const QString &filename);
    bool canStart() const;
    void setUpdateDataBaseInfo(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase);

Q_SIGNALS:
    void finished(bool success, const QString &newStateDatabase, const QString &minimumWaitDurationStr);

private:
    CreateDatabaseFileJobPrivate *const d;
};
}

#endif // CREATEDATABASEFILEJOB_H
