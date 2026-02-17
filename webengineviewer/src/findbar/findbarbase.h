/* SPDX-FileCopyrightText: 2010 Torgny Nyblom <nyblom@kde.org>
 * SPDX-FileCopyrightText: 2010-2026 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "webengineviewer_export.h"
#include <QWidget>
class QAction;
class QPushButton;
class QMenu;
class QLabel;
namespace PimCommon
{
class LineEditWithCompleterNg;
}
namespace WebEngineViewer
{
/*!
 * \class WebEngineViewer::FindBarBase
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/FindBarBase
 *
 * \brief The FindBarBase class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT FindBarBase : public QWidget
{
    Q_OBJECT
public:
    /*! Constructs a FindBarBase widget with the given parent. */
    explicit FindBarBase(QWidget *parent = nullptr);
    /*! Destroys the FindBarBase widget. */
    ~FindBarBase() override;

    /*! Returns the search text. */
    [[nodiscard]] QString text() const;
    /*! Sets the search text. */
    void setText(const QString &text);
    /*! Focuses the search field and sets cursor. */
    void focusAndSetCursor();

protected:
    /*! Handles events for the widget. */
    [[nodiscard]] bool event(QEvent *e) override;
    /*! Filters events for objects. */
    [[nodiscard]] bool eventFilter(QObject *obj, QEvent *event) override;
    /*! Clears text selections. */
    virtual void clearSelections();
    /*! Updates highlighting. */
    virtual void updateHighLight(bool);
    /*! Searches for text with specified direction and auto-search setting. */
    virtual void searchText(bool backward, bool isAutoSearch);
    /*! Updates the sensitivity of search controls. */
    virtual void updateSensitivity(bool);

    /*! Sets whether a match was found. */
    void setFoundMatch(bool match);
    /*! Returns the options menu. */
    [[nodiscard]] QMenu *optionsMenu() const;

public Q_SLOTS:
    /*! Finds the next match. */
    void findNext();
    /*! Finds the previous match. */
    void findPrev();
    /*! Performs auto search. */
    void autoSearch(const QString &str);
    /*! Performs text search. */
    void slotSearchText(bool backward = false, bool isAutoSearch = true);
    /*! Closes the find bar. */
    void closeBar();

Q_SIGNALS:
    /*! Emitted when the find bar should be hidden. */
    void hideFindBar();

protected Q_SLOTS:
    /*! Handles case sensitivity changes. */
    void caseSensitivityChanged(bool);
    /*! Handles highlight all changes. */
    void slotHighlightAllChanged(bool);
    /*! Clears the search. */
    void slotClearSearch();

protected:
    void updatePalette();
    void addToCompletion(const QString &text);
    QString mNotFoundString;
    QString mPositiveBackground;
    QString mNegativeBackground;
    QString mLastSearchStr;
    PimCommon::LineEditWithCompleterNg *mSearch = nullptr;
    QAction *mCaseSensitiveAct = nullptr;

    QPushButton *mFindPrevBtn = nullptr;
    QPushButton *mFindNextBtn = nullptr;
    QMenu *mOptionsMenu = nullptr;
    QLabel *mStatus = nullptr;
};
}
