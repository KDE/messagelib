/*  -*- c++ -*-
    htmlstatusbar.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2002 Ingo Kloecker <kloecker@kde.org>
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2013-2025 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "htmlstatusbar.h"
#include "settings/messageviewersettings.h"

#include "MessageCore/MessageCoreSettings"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QMouseEvent>
#include <WebEngineViewer/WebEngineManageScript>

#include "viewer/webengine/mailwebengineview.h"

static constexpr int scrollBarWidth = 20;
static constexpr int maxWidthHtml = 1600;

using namespace MessageViewer;
using namespace Qt::StringLiterals;

namespace
{
static const char myHtmlStatusBarConfigGroupName[] = "Reader";
}
HtmlStatusBar::HtmlStatusBar(MailWebEngineView *viewer, QWidget *parent)
    : QPushButton(parent)
    , mMode(MimeTreeParser::Util::Normal)
    , mViewer(viewer)
{
    setCursor(QCursor(Qt::PointingHandCursor));
    setContentsMargins(6, 6, 6, 6);
    update();
}

HtmlStatusBar::~HtmlStatusBar() = default;

MimeTreeParser::Util::HtmlMode HtmlStatusBar::mode() const
{
    return mMode;
}

bool HtmlStatusBar::isHtml() const
{
    return mode() == MimeTreeParser::Util::Html;
}

bool HtmlStatusBar::isNormal() const
{
    return mode() == MimeTreeParser::Util::Normal;
}

void HtmlStatusBar::update()
{
    QPalette pal = palette();
    pal.setColor(backgroundRole(), bgColor());
    pal.setColor(foregroundRole(), fgColor());
    setPalette(pal);
    setText(message());
    setToolTip(toolTip());
    constexpr int extraWidth = 10;
    constexpr int overlap = -4;
    constexpr int htmlPadding = 40;

    setGeometry(std::min(maxWidthHtml + htmlPadding, parentWidget()->width()) - sizeHint().width() + overlap - extraWidth - scrollBarWidth,
                -overlap,
                sizeHint().width() + extraWidth,
                sizeHint().height());
}

void HtmlStatusBar::setNormalMode()
{
    setMode(MimeTreeParser::Util::Normal);
}

void HtmlStatusBar::setHtmlMode()
{
    setMode(MimeTreeParser::Util::Html);
}

void HtmlStatusBar::setAvailableModes(const QList<MimeTreeParser::Util::HtmlMode> &availableModes)
{
    mAvailableModes = availableModes;
}

const QList<MimeTreeParser::Util::HtmlMode> &HtmlStatusBar::availableModes()
{
    return mAvailableModes;
}

void HtmlStatusBar::setMode(MimeTreeParser::Util::HtmlMode m, UpdateMode mode)
{
    if (mMode != m) {
        mMode = m;
        if (mode == Update) {
            update();
        }
    }
}

void HtmlStatusBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        Q_EMIT clicked();
    }
}

void HtmlStatusBar::resizeEvent(QResizeEvent *event)
{
    const QString code = u"const root = document.querySelector(':root'); root.style.setProperty('--html-max-width', '"_s + QString::number(maxWidthHtml)
        + u"px'); root.style.setProperty('--html-indicator-width', '"_s + QString::number(width() + scrollBarWidth) + u"px');"_s;
    mViewer->page()->runJavaScript(code, WebEngineViewer::WebEngineManageScript::scriptWordId());
}

QString HtmlStatusBar::message() const
{
    switch (mode()) {
    case MimeTreeParser::Util::Html: // bold: "HTML Message"
    case MimeTreeParser::Util::MultipartHtml:
        return i18nc("@label", "Show Plain Message");
    case MimeTreeParser::Util::Normal: // normal: "No HTML Message"
        return i18nc("@label", "Show HTML Message");
    case MimeTreeParser::Util::MultipartPlain: // normal: "Plain Message"
        return i18nc("@label", "Plain Message");
    case MimeTreeParser::Util::MultipartIcal: // normal: "Calendar Message"
        return i18nc("@label", "Calendar Message");
    default:
        return {};
    }
}

QString HtmlStatusBar::toolTip() const
{
    switch (mode()) {
    case MimeTreeParser::Util::Html:
    case MimeTreeParser::Util::MultipartHtml:
    case MimeTreeParser::Util::MultipartPlain:
    case MimeTreeParser::Util::MultipartIcal:
        return i18n("Click to toggle between HTML, plain text, and calendar.")
            + i18nc("@info:tooltip", "Warning: HTML email content can hide or modify this email headers.");
    default:
    case MimeTreeParser::Util::Normal:
        break;
    }

    return {};
}

QColor HtmlStatusBar::fgColor() const
{
    KConfigGroup conf(KSharedConfig::openConfig(), QLatin1StringView(myHtmlStatusBarConfigGroupName));
    KColorScheme scheme;
    switch (mode()) {
    case MimeTreeParser::Util::Html:
    case MimeTreeParser::Util::MultipartHtml:
        return scheme.foreground(KColorScheme::NegativeText).color();
    case MimeTreeParser::Util::Normal:
    case MimeTreeParser::Util::MultipartPlain:
    case MimeTreeParser::Util::MultipartIcal:
        return scheme.foreground(KColorScheme::NormalText).color();
    default:
        return scheme.foreground(KColorScheme::NormalText).color();
    }
}

QColor HtmlStatusBar::bgColor() const
{
    KConfigGroup conf(KSharedConfig::openConfig(), QLatin1StringView(myHtmlStatusBarConfigGroupName));

    KColorScheme scheme;
    switch (mode()) {
    case MimeTreeParser::Util::Html:
    case MimeTreeParser::Util::MultipartHtml:
        return scheme.background(KColorScheme::NegativeBackground).color();
    case MimeTreeParser::Util::Normal:
    case MimeTreeParser::Util::MultipartPlain:
    case MimeTreeParser::Util::MultipartIcal:
        return scheme.background(KColorScheme::NormalBackground).color();
    default:
        return scheme.background(KColorScheme::NormalBackground).color();
    }
}

#include "moc_htmlstatusbar.cpp"
