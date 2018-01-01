/* Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>
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

#include "findbarwebengineview.h"
#include <PimCommon/LineEditWithCompleterNg>
#include <QWebEngineView>
#include <QAction>

using namespace WebEngineViewer;

class WebEngineViewer::FindBarWebEngineViewPrivate
{
public:
    FindBarWebEngineViewPrivate()
    {
    }

    QWebEngineView *mView = nullptr;
};

FindBarWebEngineView::FindBarWebEngineView(QWebEngineView *view, QWidget *parent)
    : FindBarBase(parent)
    , d(new WebEngineViewer::FindBarWebEngineViewPrivate)
{
    d->mView = view;
}

FindBarWebEngineView::~FindBarWebEngineView()
{
    delete d;
}

void FindBarWebEngineView::searchText(bool backward, bool isAutoSearch)
{
    QWebEnginePage::FindFlags searchOptions;

    if (backward) {
        searchOptions |= QWebEnginePage::FindBackward;
    }
    if (mCaseSensitiveAct->isChecked()) {
        searchOptions |= QWebEnginePage::FindCaseSensitively;
    }
    const QString searchWord(mSearch->text());
    if (!isAutoSearch && !mLastSearchStr.contains(searchWord, Qt::CaseSensitive)) {
        clearSelections();
    }
    d->mView->findText(QString()); //Clear an existing highligh
    mLastSearchStr = searchWord;
    d->mView->findText(mLastSearchStr, searchOptions, [this](bool found) {
        setFoundMatch(found);
    });
}

void FindBarWebEngineView::updateSensitivity(bool sensitivity)
{
    QWebEnginePage::FindFlags searchOptions;
    if (sensitivity) {
        searchOptions |= QWebEnginePage::FindCaseSensitively;
        d->mView->findText(QString()); //Clear an existing highligh
    }
    d->mView->findText(QString(), searchOptions, [this](bool found) {
        setFoundMatch(found);
    });
}

void FindBarWebEngineView::clearSelections()
{
    d->mView->findText(QString());
    mLastSearchStr.clear();
    FindBarBase::clearSelections();
}
