/*  -*- c++ -*-
    htmlstatusbar.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2002 Ingo Kloecker <kloecker@kde.org>
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "htmlstatusbar.h"
#include "settings/messageviewersettings.h"

#include "MessageCore/MessageCoreSettings"

#include <KConfigGroup>
#include <KLocalizedString>

#include <KSharedConfig>
#include <QMouseEvent>

using namespace MessageViewer;
namespace
{
static const char myHtmlStatusBarConfigGroupName[] = "Reader";
}
HtmlStatusBar::HtmlStatusBar(QWidget *parent)
    : QLabel(parent)
    , mMode(MimeTreeParser::Util::Normal)
{
    setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    setAutoFillBackground(true);
    setCursor(QCursor(Qt::PointingHandCursor));
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

QString HtmlStatusBar::message() const
{
    switch (mode()) {
    case MimeTreeParser::Util::Html: // bold: "HTML Message"
    case MimeTreeParser::Util::MultipartHtml:
        return i18nc("'HTML Message' with html linebreaks between each letter and in bold text.",
                     "<qt><b><br />H<br />T<br />M<br />L<br /> "
                     "<br />M<br />e<br />s<br />s<br />a<br />g<br />e</b></qt>");
    case MimeTreeParser::Util::Normal: // normal: "No HTML Message"
        return i18nc("'No HTML Message' with html linebreaks between each letter.",
                     "<qt><br />N<br />o<br /> "
                     "<br />H<br />T<br />M<br />L<br /> "
                     "<br />M<br />e<br />s<br />s<br />a<br />g<br />e</qt>");
    case MimeTreeParser::Util::MultipartPlain: // normal: "Plain Message"
        return i18nc("'Plain Message' with html linebreaks between each letter.",
                     "<qt><br />P<br />l<br />a<br />i<br />n<br /> "
                     "<br />M<br />e<br />s<br />s<br />a<br />g<br />e<br /></qt>");
    case MimeTreeParser::Util::MultipartIcal: // normal: "Calendar Message"
        return i18nc("'Calendar Message' with html linebreaks between each letter.",
                     "<qt><br />C<br />a<br />l<br />e<br />n<br />d<br />a<br />r<br /> "
                     "<br />M<br />e<br />s<br />s<br />a<br />g<br />e<br /></qt>");
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
        return i18n("Click to toggle between HTML, plain text and calendar.");
    default:
    case MimeTreeParser::Util::Normal:
        break;
    }

    return {};
}

QColor HtmlStatusBar::fgColor() const
{
    KConfigGroup conf(KSharedConfig::openConfig(), QLatin1StringView(myHtmlStatusBarConfigGroupName));
    QColor defaultColor;
    QColor color;
    switch (mode()) {
    case MimeTreeParser::Util::Html:
    case MimeTreeParser::Util::MultipartHtml:
        defaultColor = Qt::white;
        color = defaultColor;
        if (!MessageCore::MessageCoreSettings::self()->useDefaultColors()) {
            color = conf.readEntry("ColorbarForegroundHTML", defaultColor);
        }
        return color;
    case MimeTreeParser::Util::Normal:
    case MimeTreeParser::Util::MultipartPlain:
    case MimeTreeParser::Util::MultipartIcal:
        defaultColor = Qt::black;
        color = defaultColor;
        if (!MessageCore::MessageCoreSettings::self()->useDefaultColors()) {
            color = conf.readEntry("ColorbarForegroundPlain", defaultColor);
        }
        return color;
    }
    return Qt::black;
}

QColor HtmlStatusBar::bgColor() const
{
    KConfigGroup conf(KSharedConfig::openConfig(), QLatin1StringView(myHtmlStatusBarConfigGroupName));

    QColor defaultColor;
    QColor color;
    switch (mode()) {
    case MimeTreeParser::Util::Html:
    case MimeTreeParser::Util::MultipartHtml:
        defaultColor = Qt::black;
        color = defaultColor;
        if (!MessageCore::MessageCoreSettings::self()->useDefaultColors()) {
            color = conf.readEntry("ColorbarBackgroundHTML", defaultColor);
        }
        return color;
    case MimeTreeParser::Util::Normal:
    case MimeTreeParser::Util::MultipartPlain:
    case MimeTreeParser::Util::MultipartIcal:
        defaultColor = Qt::lightGray;
        color = defaultColor;
        if (!MessageCore::MessageCoreSettings::self()->useDefaultColors()) {
            color = conf.readEntry("ColorbarBackgroundPlain", defaultColor);
        }
        return color;
    }
    return Qt::white;
}

#include "moc_htmlstatusbar.cpp"
