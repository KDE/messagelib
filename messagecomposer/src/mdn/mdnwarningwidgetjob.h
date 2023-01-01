/*
  SPDX-FileCopyrightText: 2022-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <Akonadi/Item>
#include <KMime/MDN>
#include <QObject>
namespace MessageComposer
{
/**
 * @brief The MDNWarningWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT MDNWarningWidgetJob : public QObject
{
    Q_OBJECT
public:
    explicit MDNWarningWidgetJob(QObject *parent = nullptr);
    ~MDNWarningWidgetJob() override;

    Q_REQUIRED_RESULT bool start();

    Q_REQUIRED_RESULT const Akonadi::Item &item() const;
    void setItem(const Akonadi::Item &newItem);

    Q_REQUIRED_RESULT bool canStart() const;

Q_SIGNALS:
    void showMdnInfo(const QPair<QString, bool> &mdnInfo, KMime::MDN::SendingMode s);

private:
    Akonadi::Item mItem;
};
}
