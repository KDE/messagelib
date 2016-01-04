/* Copyright (C) 2011-2015 Laurent Montel <montel@kde.org>
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

#ifndef MESSAGEVIEWER_FINDBAR_MAILWEBVIEW_H
#define MESSAGEVIEWER_FINDBAR_MAILWEBVIEW_H

#include "findbarbase.h"
#include "messageviewer_export.h"

class QWebView;

namespace MessageViewer
{
class FindBarWebViewPrivate;
class MESSAGEVIEWER_EXPORT FindBarWebView : public FindBarBase
{
    Q_OBJECT
public:
    explicit FindBarWebView(QWebView *view, QWidget *parent = Q_NULLPTR);
    ~FindBarWebView();

private:
    void clearSelections() Q_DECL_OVERRIDE;
    void searchText(bool backward, bool isAutoSearch) Q_DECL_OVERRIDE;
    void updateHighLight(bool) Q_DECL_OVERRIDE;
    void updateSensitivity(bool sensitivity) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void slotFindSelectionFirstChanged(bool findSectionFirst);
private:
    FindBarWebViewPrivate *const d;
};

}

#endif
