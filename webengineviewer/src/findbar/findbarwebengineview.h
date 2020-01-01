/* Copyright (C) 2016-2020 Laurent Montel <montel@kde.org>
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

#ifndef WEBENGINEVIEWER_FINDBAR_WEBENGINEVIEW_H
#define WEBENGINEVIEWER_FINDBAR_WEBENGINEVIEW_H

#include "findbarbase.h"
#include "webengineviewer_export.h"

class QWebEngineView;

namespace WebEngineViewer {
class FindBarWebEngineViewPrivate;
/**
 * @brief The FindBarWebEngineView class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT FindBarWebEngineView : public FindBarBase
{
    Q_OBJECT
public:
    explicit FindBarWebEngineView(QWebEngineView *view, QWidget *parent = nullptr);
    ~FindBarWebEngineView() override;

private:
    void clearSelections() override;
    void searchText(bool backward, bool isAutoSearch) override;
    void updateSensitivity(bool sensitivity) override;

private:
    FindBarWebEngineViewPrivate *const d;
};
}

#endif
