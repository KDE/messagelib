/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandwidget.h"
#include "searchlinecommandflowlayout.h"
#include <KLocalizedString>
#include <QEvent>
#include <QPushButton>
#include <QVBoxLayout>
using namespace MessageList::Core;
SearchLineCommandWidget::SearchLineCommandWidget(QWidget *parent)
    : QFrame{parent}
{
    auto flowLayout = new SearchLineCommandFlowLayout(this);
    flowLayout->setObjectName(QStringLiteral("flowLayout"));
    flowLayout->setContentsMargins({});
    flowLayout->setSpacing(0);
    fillWidgets();
    Q_ASSERT(!mButtonsList.isEmpty());
    for (const auto &info : std::as_const(mButtonsList)) {
        flowLayout->addWidget(createPushButton(info.needSpace, info.i18n, info.identifier));
    }
    setAutoFillBackground(true);
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

SearchLineCommandWidget::~SearchLineCommandWidget() = default;

bool SearchLineCommandWidget::eventFilter(QObject *o, QEvent *e)
{
    if (o == mAlignWidget && (e->type() == QEvent::Move || e->type() == QEvent::Resize)) {
        reposition();
    }
    return QFrame::eventFilter(o, e);
}

void SearchLineCommandWidget::resizeEvent(QResizeEvent *ev)
{
    reposition();
    QWidget::resizeEvent(ev);
}

void SearchLineCommandWidget::setAlignWidget(QWidget *w)
{
    if (w == mAlignWidget) {
        return;
    }

    if (mAlignWidget) {
        mAlignWidget->removeEventFilter(this);
    }

    mAlignWidget = w;

    if (mAlignWidget) {
        mAlignWidget->installEventFilter(this);
    }
    reposition();
}

void SearchLineCommandWidget::reposition()
{
    if (!mAlignWidget) {
        return;
    }
    // p is in the alignWidget's coordinates
    QPoint p;
    // We are always above the alignWidget, right-aligned with it for
    // LTR locales, and left-aligned for RTL locales (default value=0).
    if (layoutDirection() == Qt::LeftToRight) {
        p.setX(mAlignWidget->width() - width());
    }
    p.setY(-height());
    // Position in the toplevelwidget's coordinates
    QPoint pTopLevel = mAlignWidget->mapTo(topLevelWidget(), p);
    // Position in the widget's parentWidget coordinates
    QPoint pParent = parentWidget()->mapFrom(topLevelWidget(), pTopLevel);
    // Move 'this' to that position.
    move(pParent);
    qDebug() << "pParent  " << pParent;
}

QPushButton *SearchLineCommandWidget::createPushButton(bool needSpace, const QString &i18nStr, const QString &commandStr)
{
    auto pushButton = new QPushButton(i18nStr, this);
    pushButton->setObjectName(commandStr);
    connect(pushButton, &QPushButton::clicked, this, [this, commandStr, needSpace]() {
        const QString str = commandStr + (needSpace ? QStringLiteral(" ") : QString());
        Q_EMIT insertCommand(str);
    });
    return pushButton;
}

void SearchLineCommandWidget::fillWidgets()
{
    mButtonsList = {
        {false, QStringLiteral("subject:"), i18nc("@action:button", "Subject")},
        {false, QStringLiteral("from:"), i18nc("@action:button", "From")},
        {false, QStringLiteral("to:"), i18nc("@action:button", "To")},
        {false, QStringLiteral("cc:"), i18nc("@action:button", "Cc")},
        {false, QStringLiteral("bcc:"), i18nc("@action:button", "Bcc")},
        {true, QStringLiteral("has:attachment"), i18nc("@action:button", "Has Attachment")},
        {true, QStringLiteral("has:invitation"), i18nc("@action:button", "Has Invitation")},
        {true, QStringLiteral("is:read"), i18nc("@action:button", "Read")},
        {true, QStringLiteral("is:unread"), i18nc("@action:button", "Unread")},
        {true, QStringLiteral("is:important"), i18nc("@action:button", "Important")},
        {true, QStringLiteral("is:ignored"), i18nc("@action:button", "Ignored")},
        {true, QStringLiteral("is:ham"), i18nc("@action:button", "Ham")},
        {true, QStringLiteral("is:spam"), i18nc("@action:button", "Spam")},
        {true, QStringLiteral("is:watched"), i18nc("@action:button", "Watched")},
        {true, QStringLiteral("is:replied"), i18nc("@action:button", "Replied")},
        {true, QStringLiteral("is:forwarded"), i18nc("@action:button", "Forwarded")},
        {false, QStringLiteral("smaller:"), i18nc("@action:button", "Smaller")},
        {false, QStringLiteral("larger:"), i18nc("@action:button", "Larger")},
        {false, QStringLiteral("size:"), i18nc("@action:button", "Size")},
        {false, QStringLiteral("older_than:"), i18nc("@action:button", "Older")},
        {false, QStringLiteral("newer_than:"), i18nc("@action:button", "Newer")},
    };
}

#include "moc_searchlinecommandwidget.cpp"
