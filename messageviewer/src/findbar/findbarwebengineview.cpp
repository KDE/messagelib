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

#include "findbarwebengineview.h"
#include "PimCommon/LineEditWithCompleter"
#include <QWebEngineView>

#include <KLocalizedString>
#include <QMenu>
using namespace MessageViewer;

class MessageViewer::FindBarWebEngineViewPrivate
{
public:
    FindBarWebEngineViewPrivate()
        : mView(Q_NULLPTR),
          mHighlightAll(Q_NULLPTR),
          mFindInSelection(Q_NULLPTR)
    {

    }
    QWebEngineView *mView;
    QAction *mHighlightAll;
    QAction *mFindInSelection;
};

FindBarWebEngineView::FindBarWebEngineView(QWebEngineView *view, QWidget *parent)
    : FindBarBase(parent),
      d(new MessageViewer::FindBarWebEngineViewPrivate)
{
    d->mView = view;
    QMenu *options = optionsMenu();
    d->mHighlightAll = options->addAction(i18n("Highlight all matches"));
    d->mHighlightAll->setCheckable(true);
    d->mFindInSelection = options->addAction(i18n("Find in selection first"));
    d->mFindInSelection->setCheckable(true);
    connect(d->mHighlightAll, &QAction::toggled, this, &FindBarWebEngineView::slotHighlightAllChanged);
    connect(d->mFindInSelection, &QAction::toggled, this, &FindBarWebEngineView::slotFindSelectionFirstChanged);
}

FindBarWebEngineView::~FindBarWebEngineView()
{
    delete d;
}

void FindBarWebEngineView::searchText(bool backward, bool isAutoSearch)
{
    QWebEnginePage::FindFlags searchOptions; //TODO not implemented = QWebEnginePage::FindWrapsAroundDocument;

    if (backward) {
        searchOptions |= QWebEnginePage::FindBackward;
    }
    if (mCaseSensitiveAct->isChecked()) {
        searchOptions |= QWebEnginePage::FindCaseSensitively;
    }
#if 0 //Not supported yet
    if (d->mHighlightAll->isChecked()) {
        searchOptions |= QWebEnginePage::HighlightAllOccurrences;
    }
    if (d->mFindInSelection->isChecked()) {
        searchOptions |= QWebEnginePage::FindBeginsInSelection;
    }
#endif
    const QString searchWord(mSearch->text());
    if (!isAutoSearch && !mLastSearchStr.contains(searchWord, Qt::CaseSensitive)) {
        clearSelections();
    }
#if 0 //TODO fix me
    d->mView->findText(QString(), QWebEnginePage::HighlightAllOccurrences); //Clear an existing highligh
#endif
    mLastSearchStr = searchWord;
    d->mView->findText(mLastSearchStr, searchOptions, [this](bool found) {
        setFoundMatch(found);
    });
}

void FindBarWebEngineView::updateHighLight(bool highLight)
{
    bool found = false;
    if (highLight) {
        QWebEnginePage::FindFlags searchOptions; //TODO not supported = QWebEnginePage::FindWrapsAroundDocument;
        if (mCaseSensitiveAct->isChecked()) {
            searchOptions |= QWebEnginePage::FindCaseSensitively;
        }
        //TODO not supported searchOptions |= QWebEnginePage::HighlightAllOccurrences;
        d->mView->findText(mLastSearchStr, searchOptions, [this](bool found) {
            setFoundMatch(found);
        });
    } else {
#if 0 //TODO
        d->mView->findText(QString(), searchOptions, [this](bool found) {
            setFoundMatch(found);
        });
#endif
    }
    setFoundMatch(found);
}

void FindBarWebEngineView::updateSensitivity(bool sensitivity)
{
#if 0
    QWebEnginePage::FindFlags searchOptions; //TODO not supported = QWebEnginePage::FindWrapsAroundDocument;
    if (sensitivity) {
        searchOptions |= QWebPage::FindCaseSensitively;
        d->mView->findText(QString(), QWebEnginePage::HighlightAllOccurrences); //Clear an existing highligh
    }
    if (d->mHighlightAll->isChecked()) {
        searchOptions |= QWebEnginePage::HighlightAllOccurrences;
    }
    const bool found = d->mView->findText(mLastSearchStr, searchOptions);
    setFoundMatch(found);
#endif
}

void FindBarWebEngineView::slotFindSelectionFirstChanged(bool findSectionFirst)
{
#if 0 //TODO not supported
    QWebPage::FindFlags searchOptions = QWebEnginePage::FindWrapsAroundDocument;
    if (findSectionFirst) {
        searchOptions |= QWebEnginePage::FindBeginsInSelection;
        d->mView->findText(QString(), QWebPage::HighlightAllOccurrences); //Clear an existing highligh
    }
    if (d->mHighlightAll->isChecked()) {
        searchOptions |= QWebEnginePage::HighlightAllOccurrences;
    }
    const bool found = d->mView->findText(mLastSearchStr, searchOptions);
    setFoundMatch(found);
#endif
}

void FindBarWebEngineView::clearSelections()
{
    //TODO fix me
#if 0
    //WEBKIT: TODO: Find a way to unselect last selection
    // http://bugreports.qt.nokia.com/browse/QTWEBKIT-80
    d->mView->findText(QString(), QWebEnginePage::HighlightAllOccurrences);
#endif
    mLastSearchStr.clear();
    FindBarBase::clearSelections();
}

