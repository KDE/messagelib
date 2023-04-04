/*
   SPDX-FileCopyrightText: 2019-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <KSharedConfig>
#include <MessageViewer/DKIMRule>
#include <QList>
#include <QObject>
namespace MessageViewer
{
/**
 * @brief The DKIMManagerRules class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManagerRules : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerRules(QObject *parent = nullptr);
    ~DKIMManagerRules() override;
    static DKIMManagerRules *self();

    void addRule(const DKIMRule &rule);

    Q_REQUIRED_RESULT QList<DKIMRule> rules() const;

    void saveRules(const QList<DKIMRule> &lst);

    void clear();

    Q_REQUIRED_RESULT int importRules(const QString &fileName);
    void exportRules(const QString &fileName, const QList<DKIMRule> &lst = {});

    Q_REQUIRED_RESULT bool isEmpty() const;

private:
    Q_REQUIRED_RESULT MESSAGEVIEWER_NO_EXPORT int loadRules(const QString &fileName = QString());
    MESSAGEVIEWER_NO_EXPORT void save(const QString &fileName = QString(), const QList<DKIMRule> &lst = {});
    Q_REQUIRED_RESULT MESSAGEVIEWER_NO_EXPORT QStringList ruleGroups(const KSharedConfig::Ptr &config) const;
    QList<DKIMRule> mRules;
};
}
