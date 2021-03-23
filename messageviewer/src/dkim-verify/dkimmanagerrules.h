/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <KSharedConfig>
#include <MessageViewer/DKIMRule>
#include <QObject>
#include <QVector>
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

    Q_REQUIRED_RESULT QVector<DKIMRule> rules() const;

    void saveRules(const QVector<DKIMRule> &lst);

    void clear();

    Q_REQUIRED_RESULT int importRules(const QString &fileName);
    void exportRules(const QString &fileName, const QVector<DKIMRule> &lst = {});

    Q_REQUIRED_RESULT bool isEmpty() const;

private:
    Q_REQUIRED_RESULT int loadRules(const QString &fileName = QString());
    void save(const QString &fileName = QString(), const QVector<DKIMRule> &lst = {});
    Q_REQUIRED_RESULT QStringList ruleGroups(const KSharedConfig::Ptr &config) const;
    QVector<DKIMRule> mRules;
};
}

