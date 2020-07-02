/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef DEFAULTGRANTLEEHEADERSTYLEINTERFACE_H
#define DEFAULTGRANTLEEHEADERSTYLEINTERFACE_H

#include <messageviewer/headerstyleinterface.h>
#include <GrantleeTheme/GrantleeTheme>

namespace MessageViewer {
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

#endif // DEFAULTGRANTLEEHEADERSTYLEINTERFACE_H
