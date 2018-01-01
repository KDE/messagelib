/*
   Copyright (C) 2015-2018 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef DEFAULTGRANTLEEHEADERSTYLEINTERFACE_H
#define DEFAULTGRANTLEEHEADERSTYLEINTERFACE_H

#include <messageviewer/headerstyleinterface.h>
#include <grantleetheme/grantleetheme.h>

namespace MessageViewer {
class DefaultGrantleeHeaderStyleInterface : public MessageViewer::HeaderStyleInterface
{
    Q_OBJECT
public:
    explicit DefaultGrantleeHeaderStyleInterface(HeaderStylePlugin *plugin, QObject *parent = nullptr);
    ~DefaultGrantleeHeaderStyleInterface();

    void createAction(KActionMenu *menu, QActionGroup *actionGroup, KActionCollection *ac) override;
    void activateAction() override;
private:
    void slotDefaultGrantleeHeaders();
    GrantleeTheme::Theme mDefaultTheme;
};
}

#endif // DEFAULTGRANTLEEHEADERSTYLEINTERFACE_H
