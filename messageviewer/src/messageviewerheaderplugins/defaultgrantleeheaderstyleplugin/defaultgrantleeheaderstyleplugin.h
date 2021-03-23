/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer/headerstyleplugin.h"

#include <QVariant>

namespace MessageViewer
{
class GrantleeHeaderStyle;
class DefaultGrantleeHeaderStylePlugin : public MessageViewer::HeaderStylePlugin
{
    Q_OBJECT
public:
    explicit DefaultGrantleeHeaderStylePlugin(QObject *parent = nullptr, const QList<QVariant> & = QList<QVariant>());
    ~DefaultGrantleeHeaderStylePlugin() override;

    Q_REQUIRED_RESULT HeaderStyle *headerStyle() const override;
    Q_REQUIRED_RESULT HeaderStrategy *headerStrategy() const override;
    Q_REQUIRED_RESULT HeaderStyleInterface *createView(KActionMenu *menu, QActionGroup *actionGroup, KActionCollection *ac, QObject *parent = nullptr) override;
    Q_REQUIRED_RESULT QString name() const override;

private:
    void initializePlugin();
    GrantleeHeaderStyle *const mHeaderStyle;
    HeaderStrategy *const mHeaderStrategy;
};
}
