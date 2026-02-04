/* SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "findbarbase.h"
#include "webengineviewer_export.h"

class QWebEngineView;

namespace WebEngineViewer
{
class FindBarWebEngineViewPrivate;
/*!
 * \class WebEngineViewer::FindBarWebEngineView
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/FindBarWebEngineView
 *
 * \brief The FindBarWebEngineView class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT FindBarWebEngineView : public FindBarBase
{
    Q_OBJECT
public:
    /*! Constructs a FindBarWebEngineView for the given web engine view. */
    explicit FindBarWebEngineView(QWebEngineView *view, QWidget *parent = nullptr);
    /*! Destroys the FindBarWebEngineView. */
    ~FindBarWebEngineView() override;

private:
    WEBENGINEVIEWER_NO_EXPORT void clearSelections() override;
    WEBENGINEVIEWER_NO_EXPORT void searchText(bool backward, bool isAutoSearch) override;
    WEBENGINEVIEWER_NO_EXPORT void updateSensitivity(bool sensitivity) override;

private:
    std::unique_ptr<FindBarWebEngineViewPrivate> const d;
};
}
