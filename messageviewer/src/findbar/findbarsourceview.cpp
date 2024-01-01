/* SPDX-FileCopyrightText: 2011-2024 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "findbarsourceview.h"
#include <PimCommon/LineEditWithCompleterNg>

#include <QAction>
#include <QPlainTextEdit>

using namespace MessageViewer;

FindBarSourceView::FindBarSourceView(QPlainTextEdit *view, QWidget *parent)
    : WebEngineViewer::FindBarBase(parent)
    , mView(view)
{
}

FindBarSourceView::~FindBarSourceView() = default;

void FindBarSourceView::searchText(bool backward, bool isAutoSearch)
{
    QTextDocument::FindFlags searchOptions = {};
    if (backward) {
        searchOptions |= QTextDocument::FindBackward;
    }
    if (mCaseSensitiveAct->isChecked()) {
        searchOptions |= QTextDocument::FindCaseSensitively;
    }

    if (isAutoSearch) {
        QTextCursor cursor = mView->textCursor();
        cursor.setPosition(cursor.selectionStart());
        mView->setTextCursor(cursor);
    } else if (!mLastSearchStr.contains(mSearch->text(), Qt::CaseSensitive)) {
        clearSelections();
    }
    mLastSearchStr = mSearch->text();
    const bool found = mView->find(mLastSearchStr, searchOptions);

    setFoundMatch(found);
}

void FindBarSourceView::clearSelections()
{
    QTextCursor textCursor = mView->textCursor();
    textCursor.clearSelection();
    textCursor.setPosition(0);
    mView->setTextCursor(textCursor);

    WebEngineViewer::FindBarBase::clearSelections();
}

void FindBarSourceView::updateHighLight(bool)
{
    clearSelections();
}

void FindBarSourceView::updateSensitivity(bool)
{
    QTextDocument::FindFlags searchOptions = {};
    if (mCaseSensitiveAct->isChecked()) {
        searchOptions |= QTextDocument::FindCaseSensitively;
    }
    mLastSearchStr = mSearch->text();
    const bool found = mView->find(mLastSearchStr, searchOptions);
    setFoundMatch(found);
}

#include "moc_findbarsourceview.cpp"
