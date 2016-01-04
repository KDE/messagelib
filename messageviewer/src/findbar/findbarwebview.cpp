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

#include "findbarwebview.h"
#include "PimCommon/LineEditWithCompleter"
#include <QWebView>

#include <KLocalizedString>
#include <QMenu>
using namespace MessageViewer;

class MessageViewer::FindBarWebViewPrivate
{
public:
    FindBarWebViewPrivate()
        : mView(Q_NULLPTR),
          mHighlightAll(Q_NULLPTR),
          mFindInSelection(Q_NULLPTR)
    {

    }
    QWebView *mView;
    QAction *mHighlightAll;
    QAction *mFindInSelection;
};

FindBarWebView::FindBarWebView(QWebView *view, QWidget *parent)
    : FindBarBase(parent),
      d(new MessageViewer::FindBarWebViewPrivate)
{
    d->mView = view;
    QMenu *options = optionsMenu();
    d->mHighlightAll = options->addAction(i18n("Highlight all matches"));
    d->mHighlightAll->setCheckable(true);
    d->mFindInSelection = options->addAction(i18n("Find in selection first"));
    d->mFindInSelection->setCheckable(true);
    connect(d->mHighlightAll, &QAction::toggled, this, &FindBarWebView::slotHighlightAllChanged);
    connect(d->mFindInSelection, &QAction::toggled, this, &FindBarWebView::slotFindSelectionFirstChanged);
}

FindBarWebView::~FindBarWebView()
{
    delete d;
}

void FindBarWebView::searchText(bool backward, bool isAutoSearch)
{
    QWebPage::FindFlags searchOptions = QWebPage::FindWrapsAroundDocument;

    if (backward) {
        searchOptions |= QWebPage::FindBackward;
    }
    if (mCaseSensitiveAct->isChecked()) {
        searchOptions |= QWebPage::FindCaseSensitively;
    }
    if (d->mHighlightAll->isChecked()) {
        searchOptions |= QWebPage::HighlightAllOccurrences;
    }
    if (d->mFindInSelection->isChecked()) {
        searchOptions |= QWebPage::FindBeginsInSelection;
    }
    const QString searchWord(mSearch->text());
    if (!isAutoSearch && !mLastSearchStr.contains(searchWord, Qt::CaseSensitive)) {
        clearSelections();
    }
    d->mView->findText(QString(), QWebPage::HighlightAllOccurrences); //Clear an existing highligh

    mLastSearchStr = searchWord;
    const bool found = d->mView->findText(mLastSearchStr, searchOptions);

    setFoundMatch(found);
}

void FindBarWebView::updateHighLight(bool highLight)
{
    bool found = false;
    if (highLight) {
        QWebPage::FindFlags searchOptions = QWebPage::FindWrapsAroundDocument;
        if (mCaseSensitiveAct->isChecked()) {
            searchOptions |= QWebPage::FindCaseSensitively;
        }
        searchOptions |= QWebPage::HighlightAllOccurrences;
        found = d->mView->findText(mLastSearchStr, searchOptions);
    } else {
        found = d->mView->findText(QString(), QWebPage::HighlightAllOccurrences);
    }
    setFoundMatch(found);
}

void FindBarWebView::updateSensitivity(bool sensitivity)
{
    QWebPage::FindFlags searchOptions = QWebPage::FindWrapsAroundDocument;
    if (sensitivity) {
        searchOptions |= QWebPage::FindCaseSensitively;
        d->mView->findText(QString(), QWebPage::HighlightAllOccurrences); //Clear an existing highligh
    }
    if (d->mHighlightAll->isChecked()) {
        searchOptions |= QWebPage::HighlightAllOccurrences;
    }
    const bool found = d->mView->findText(mLastSearchStr, searchOptions);
    setFoundMatch(found);
}

void FindBarWebView::slotFindSelectionFirstChanged(bool findSectionFirst)
{
    QWebPage::FindFlags searchOptions = QWebPage::FindWrapsAroundDocument;
    if (findSectionFirst) {
        searchOptions |= QWebPage::FindBeginsInSelection;
        d->mView->findText(QString(), QWebPage::HighlightAllOccurrences); //Clear an existing highligh
    }
    if (d->mHighlightAll->isChecked()) {
        searchOptions |= QWebPage::HighlightAllOccurrences;
    }
    const bool found = d->mView->findText(mLastSearchStr, searchOptions);
    setFoundMatch(found);
}

void FindBarWebView::clearSelections()
{
    //WEBKIT: TODO: Find a way to unselect last selection
    // http://bugreports.qt.nokia.com/browse/QTWEBKIT-80
    d->mView->findText(QString(), QWebPage::HighlightAllOccurrences);
    mLastSearchStr.clear();
    FindBarBase::clearSelections();
}

