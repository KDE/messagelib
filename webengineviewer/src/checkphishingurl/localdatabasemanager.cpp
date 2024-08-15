/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "localdatabasemanager.h"
#include "localdatabasemanager_p.h"
#include "searchfullhashjob.h"
#include "updatedatabaseinfo.h"
#include "urlhashing.h"
#include "webengineviewer_debug.h"

#include <KSharedConfig>

#include <QCryptographicHash>

using namespace WebEngineViewer;

Q_GLOBAL_STATIC(LocalDataBaseManagerPrivate, s_localDataBaseManager)

LocalDataBaseManager::LocalDataBaseManager(LocalDataBaseManagerPrivate *impl, QObject *parent)
    : QObject(parent)
    , d(impl)
{
    qRegisterMetaType<WebEngineViewer::UpdateDataBaseInfo>();
    qRegisterMetaType<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult>();
    qRegisterMetaType<WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType>();
}

LocalDataBaseManager::LocalDataBaseManager(QObject *parent)
    : LocalDataBaseManager(s_localDataBaseManager, parent)
{
}

LocalDataBaseManager::~LocalDataBaseManager() = default;

void LocalDataBaseManager::initialize()
{
    d->initialize();
}

void LocalDataBaseManager::checkUrl(const QUrl &url)
{
    if (d->mDataBaseOk) {
        // TODO fixme short hash! we don't need to use it.
        WebEngineViewer::UrlHashing urlHashing(url);
        QHash<QByteArray, QByteArray> hashList = urlHashing.hashList();
        QHash<QByteArray, QByteArray> conflictHashs;

        QHashIterator<QByteArray, QByteArray> i(hashList);
        while (i.hasNext()) {
            i.next();
            const QByteArray ba = i.value();
            QByteArray result = d->mFile.searchHash(ba);
            if (ba.contains(result)) {
                conflictHashs.insert(i.key().toBase64(), ba.toBase64());
            }
        }
        if (conflictHashs.isEmpty()) {
            Q_EMIT checkUrlFinished(url, WebEngineViewer::CheckPhishingUrlUtil::Ok);
        } else {
            // qCWarning(WEBENGINEVIEWER_LOG) << " We need to Check Server Database";
            if (d->mNewClientState.isEmpty()) {
                qCWarning(WEBENGINEVIEWER_LOG) << "Database client state is unknown";
                Q_EMIT checkUrlFinished(url, WebEngineViewer::CheckPhishingUrlUtil::Unknown);
            } else {
                auto job = new WebEngineViewer::SearchFullHashJob(this);
                job->setDatabaseState(QStringList() << d->mNewClientState);
                job->setSearchHashs(conflictHashs);
                job->setSearchFullHashForUrl(url);
                connect(job, &SearchFullHashJob::result, this, [this](CheckPhishingUrlUtil::UrlStatus status, const QUrl &url) {
                    Q_EMIT checkUrlFinished(url, status);
                });
                job->start();
            }
        }
    } else {
        qCWarning(WEBENGINEVIEWER_LOG) << "Database not ok";
        Q_EMIT checkUrlFinished(url, WebEngineViewer::CheckPhishingUrlUtil::Unknown);
    }
    if (d->mFile.checkFileChanged()) {
        d->mFile.reload();
    }
}

#include "moc_localdatabasemanager.cpp"
