/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef PRINTWEBENGINEVIEWJOB_H
#define PRINTWEBENGINEVIEWJOB_H

#include "webengineviewer_export.h"

#include <QObject>
#include <QPageLayout>
#include <QTemporaryFile>

class QWebEngineView;
namespace WebEngineViewer
{
class WEBENGINEVIEWER_EXPORT PrintWebEngineViewJob : public QObject
{
    Q_OBJECT
public:
    explicit PrintWebEngineViewJob(QObject *parent = Q_NULLPTR);
    ~PrintWebEngineViewJob();

    void start();

    QWebEngineView *engineView() const;
    void setEngineView(QWebEngineView *engineView);

    void setPageLayout(const QPageLayout &pageLayout);

    QPageLayout pageLayout() const;

Q_SIGNALS:
    void failed();
    void success(const QString &temporaryFileName);

private Q_SLOTS:
    void slotHandlePdfPrinted(const QByteArray &result);

private:
    void unableToPrint();
    QPageLayout mPageLayout;
    QTemporaryFile mTemporaryFile;
    QWebEngineView *mEngineView;
};
}
#endif // PRINTWEBENGINEVIEWJOB_H
