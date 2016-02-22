/* Copyright (C) 2016 Laurent Montel <montel@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MESSAGEVIEWER_FINDBAR_MAILWEBENGINEVIEW_H
#define MESSAGEVIEWER_FINDBAR_MAILWEBENGINEVIEW_H

#include "findbarbase.h"
#include "messageviewer_export.h"

class QWebEngineView;

namespace MessageViewer
{
class FindBarWebEngineViewPrivate;
class MESSAGEVIEWER_EXPORT FindBarWebEngineView : public FindBarBase
{
    Q_OBJECT
public:
    explicit FindBarWebEngineView(QWebEngineView *view, QWidget *parent = Q_NULLPTR);
    ~FindBarWebEngineView();

private:
    void clearSelections() Q_DECL_OVERRIDE;
    void searchText(bool backward, bool isAutoSearch) Q_DECL_OVERRIDE;
    void updateHighLight(bool) Q_DECL_OVERRIDE;
    void updateSensitivity(bool sensitivity) Q_DECL_OVERRIDE;

private:
    FindBarWebEngineViewPrivate *const d;
};

}

#endif
