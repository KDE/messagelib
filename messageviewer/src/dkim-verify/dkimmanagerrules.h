/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] QList<DKIMRule> rules() const;

    void saveRules(const QList<DKIMRule> &lst);

    void clear();

    [[nodiscard]] int importRules(const QString &fileName);
    void exportRules(const QString &fileName, const QList<DKIMRule> &lst = {});

    [[nodiscard]] bool isEmpty() const;

private:
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT int loadRules(const QString &fileName = QString());
    MESSAGEVIEWER_NO_EXPORT void save(const QString &fileName = QString(), const QList<DKIMRule> &lst = {});
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QStringList ruleGroups(const KSharedConfig::Ptr &config) const;
    QList<DKIMRule> mRules;
};
}
