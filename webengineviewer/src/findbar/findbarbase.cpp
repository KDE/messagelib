/* SPDX-FileCopyrightText: 2010 Torgny Nyblom <nyblom@kde.org>
 * SPDX-FileCopyrightText: 2010-2024 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "findbarbase.h"
#include <KStatefulBrush>

#include <PimCommon/LineEditWithCompleterNg>

#include <KColorScheme>
#include <KLocalizedString>
#include <QIcon>
#include <QLineEdit>

#include <QApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTimer>
#include <QToolButton>

using namespace WebEngineViewer;

FindBarBase::FindBarBase(QWidget *parent)
    : QWidget(parent)
{
    auto lay = new QHBoxLayout(this);
    lay->setContentsMargins({});

    auto closeBtn = new QToolButton(this);
    closeBtn->setIcon(QIcon::fromTheme(QStringLiteral("dialog-close")));
    closeBtn->setObjectName(QLatin1StringView("close"));
    closeBtn->setIconSize(QSize(16, 16));
    closeBtn->setToolTip(i18nc("@info:tooltip", "Close"));

#ifndef QT_NO_ACCESSIBILITY
    closeBtn->setAccessibleName(i18n("Close"));
#endif

    closeBtn->setAutoRaise(true);
    lay->addWidget(closeBtn);

    auto label = new QLabel(i18nc("Find text", "F&ind:"), this);
    lay->addWidget(label);

    mSearch = new PimCommon::LineEditWithCompleterNg(this);
    mSearch->setObjectName(QLatin1StringView("searchline"));
    mSearch->setToolTip(i18nc("@info:tooltip", "Text to search for"));
    mSearch->setClearButtonEnabled(true);
    label->setBuddy(mSearch);
    lay->addWidget(mSearch);

    mFindNextBtn = new QPushButton(QIcon::fromTheme(QStringLiteral("go-down-search")), i18nc("Find and go to the next search match", "Next"), this);
    mFindNextBtn->setToolTip(i18nc("@info:tooltip", "Jump to next match"));
    mFindNextBtn->setObjectName(QLatin1StringView("findnext"));
    lay->addWidget(mFindNextBtn);
    mFindNextBtn->setEnabled(false);

    mFindPrevBtn = new QPushButton(QIcon::fromTheme(QStringLiteral("go-up-search")), i18nc("Find and go to the previous search match", "Previous"), this);
    mFindPrevBtn->setToolTip(i18nc("@info:tooltip", "Jump to previous match"));
    mFindPrevBtn->setObjectName(QLatin1StringView("findprevious"));
    lay->addWidget(mFindPrevBtn);
    mFindPrevBtn->setEnabled(false);

    auto optionsBtn = new QPushButton(this);
    optionsBtn->setText(i18n("Options"));
    optionsBtn->setToolTip(i18nc("@info:tooltip", "Modify search behavior"));
    mOptionsMenu = new QMenu(optionsBtn);
    mCaseSensitiveAct = mOptionsMenu->addAction(i18n("Case sensitive"));
    mCaseSensitiveAct->setCheckable(true);

    optionsBtn->setMenu(mOptionsMenu);
    lay->addWidget(optionsBtn);

    connect(closeBtn, &QToolButton::clicked, this, &FindBarBase::closeBar);
    connect(mFindNextBtn, &QPushButton::clicked, this, &FindBarBase::findNext);
    connect(mFindPrevBtn, &QPushButton::clicked, this, &FindBarBase::findPrev);
    connect(mCaseSensitiveAct, &QAction::toggled, this, &FindBarBase::caseSensitivityChanged);
    connect(mSearch, &QLineEdit::textChanged, this, &FindBarBase::autoSearch);
    connect(mSearch, &QLineEdit::returnPressed, this, &FindBarBase::findNext);

    mStatus = new QLabel(this);
    mStatus->setObjectName(QLatin1StringView("status"));
    mStatus->setTextFormat(Qt::PlainText);
    QFontMetrics fm(mStatus->font());
    mNotFoundString = i18n("Phrase not found");
    mStatus->setFixedWidth(fm.boundingRect(mNotFoundString).width());
    lay->addWidget(mStatus);

    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    hide();
    qGuiApp->installEventFilter(this);
}

FindBarBase::~FindBarBase() = default;

QMenu *FindBarBase::optionsMenu() const
{
    return mOptionsMenu;
}

QString FindBarBase::text() const
{
    return mSearch->text();
}

void FindBarBase::setText(const QString &text)
{
    mSearch->setText(text);
    addToCompletion(text);
}

void FindBarBase::focusAndSetCursor()
{
    setFocus();
    mStatus->clear();
    mSearch->selectAll();
    mSearch->setFocus();
}

void FindBarBase::slotClearSearch()
{
    clearSelections();
}

void FindBarBase::autoSearch(const QString &str)
{
    const bool isNotEmpty = (!str.isEmpty());
    mFindPrevBtn->setEnabled(isNotEmpty);
    mFindNextBtn->setEnabled(isNotEmpty);
    if (isNotEmpty) {
        QTimer::singleShot(0, this, [this]() {
            slotSearchText();
        });
    } else {
        mStatus->clear();
        clearSelections();
    }
}

void FindBarBase::slotSearchText(bool backward, bool isAutoSearch)
{
    searchText(backward, isAutoSearch);
}

void FindBarBase::updatePalette()
{
    KStatefulBrush bgBrush(KColorScheme::View, KColorScheme::PositiveBackground);
    mPositiveBackground = QStringLiteral("QLineEdit{ background-color:%1 }").arg(bgBrush.brush(mSearch->palette()).color().name());
    bgBrush = KStatefulBrush(KColorScheme::View, KColorScheme::NegativeBackground);
    mNegativeBackground = QStringLiteral("QLineEdit{ background-color:%1 }").arg(bgBrush.brush(mSearch->palette()).color().name());
}

void FindBarBase::setFoundMatch(bool match)
{
#ifndef QT_NO_STYLE_STYLESHEET
    QString styleSheet;

    if (!mSearch->text().isEmpty()) {
        if (mNegativeBackground.isEmpty()) {
            updatePalette();
        }
        if (match) {
            styleSheet = mPositiveBackground;
            mStatus->clear();
        } else {
            styleSheet = mNegativeBackground;
            mStatus->setText(mNotFoundString);
        }
    }
    mSearch->setStyleSheet(styleSheet);
#endif
}

void FindBarBase::searchText(bool backward, bool isAutoSearch)
{
    Q_UNUSED(backward)
    Q_UNUSED(isAutoSearch)
}

void FindBarBase::addToCompletion(const QString &text)
{
    mSearch->addCompletionItem(text);
}

void FindBarBase::findNext()
{
    searchText(false, false);
    addToCompletion(mLastSearchStr);
}

void FindBarBase::findPrev()
{
    searchText(true, false);
    addToCompletion(mLastSearchStr);
}

void FindBarBase::caseSensitivityChanged(bool b)
{
    updateSensitivity(b);
}

void FindBarBase::updateSensitivity(bool)
{
}

void FindBarBase::slotHighlightAllChanged(bool b)
{
    updateHighLight(b);
}

void FindBarBase::updateHighLight(bool)
{
}

void FindBarBase::clearSelections()
{
    setFoundMatch(false);
}

void FindBarBase::closeBar()
{
    // Make sure that all old searches are cleared
    mSearch->clear();
    clearSelections();
    mSearch->clearFocus();
    Q_EMIT hideFindBar();
}

bool FindBarBase::event(QEvent *e)
{
    // Close the bar when pressing Escape.
    // Not using a QShortcut for this because it could conflict with
    // window-global actions (e.g. Emil Sedgh binds Esc to "close tab").
    // With a shortcut override we can catch this before it gets to kactions.
    const bool shortCutOverride = (e->type() == QEvent::ShortcutOverride);
    if (shortCutOverride || e->type() == QEvent::KeyPress) {
        auto kev = static_cast<QKeyEvent *>(e);
        if (kev->key() == Qt::Key_Escape) {
            if (shortCutOverride) {
                e->accept();
                return true;
            }
            e->accept();
            closeBar();
            return true;
        } else if (kev->key() == Qt::Key_Enter || kev->key() == Qt::Key_Return) {
            e->accept();
            if (shortCutOverride) {
                return true;
            }
            if (mSearch->text().isEmpty()) {
                return true;
            }
            if (kev->modifiers() & Qt::ShiftModifier) {
                findPrev();
            } else if (kev->modifiers() == Qt::NoModifier) {
                findNext();
            }
            return true;
        }
    }
    return QWidget::event(e);
}

bool FindBarBase::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    if (event->type() == QEvent::ApplicationPaletteChange) {
        updatePalette();
    }
    return false;
}

#include "moc_findbarbase.cpp"
