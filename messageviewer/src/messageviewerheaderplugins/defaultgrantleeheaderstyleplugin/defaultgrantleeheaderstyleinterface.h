/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <GrantleeTheme/GrantleeTheme>
#include <messageviewer/headerstyleinterface.h>

namespace MessageViewer
{
class DefaultGrantleeHeaderStyleInterface : public MessageViewer::HeaderStyleInterface
{
    Q_OBJECT
public:
    explicit DefaultGrantleeHeaderStyleInterface(HeaderStylePlugin *plugin, QObject *parent = nullptr);
    ~DefaultGrantleeHeaderStyleInterface() override;

    void createAction(KActionMenu *menu, QActionGroup *actionGroup, KActionCollection *ac) override;
    void activateAction() override;

private:
    void slotDefaultGrantleeHeaders();
    GrantleeTheme::Theme mDefaultTheme;
};
}

