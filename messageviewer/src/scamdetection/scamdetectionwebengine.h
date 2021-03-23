/*
  SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
#include <QVariant>
class QWebEnginePage;
namespace MessageViewer
{
class ScamDetectionWebEnginePrivate;
/**
 * @brief The ScamDetectionWebEngine class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ScamDetectionWebEngine : public QObject
{
    Q_OBJECT
public:
    explicit ScamDetectionWebEngine(QObject *parent = nullptr);
    ~ScamDetectionWebEngine() override;

    void scanPage(QWebEnginePage *page);

public Q_SLOTS:
    void showDetails();

private Q_SLOTS:
    void handleScanPage(const QVariant &result);

Q_SIGNALS:
    void messageMayBeAScam();
    void resultScanDetection(bool foundScam);

private:
    ScamDetectionWebEnginePrivate *const d;
};
}
