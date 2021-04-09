/* SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "findbarwebengineview.h"
#include <PimCommon/LineEditWithCompleterNg>
#include <QAction>
#include <QWebEngineView>

using namespace WebEngineViewer;

class WebEngineViewer::FindBarWebEngineViewPrivate
{
public:
    FindBarWebEngineViewPrivate(QWebEngineView *view)
        : mView(view)
    {
    }

    QWebEngineView *const mView;
};

FindBarWebEngineView::FindBarWebEngineView(QWebEngineView *view, QWidget *parent)
    : FindBarBase(parent)
    , d(new WebEngineViewer::FindBarWebEngineViewPrivate(view))
{
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
    d->mView->findText(QString()); // Clear an existing highlight
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
        d->mView->findText(QString()); // Clear an existing highlight
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
