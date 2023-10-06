/*  -*- c++ -*-
    htmlstatusbar.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2002 Ingo Kloecker <kloecker@kde.org>
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "messageviewer/messageviewerutil.h"
#include <MimeTreeParser/Util>
#include <QLabel>
class QMouseEvent;

namespace MessageViewer
{
/**
 * @short The HTML statusbar widget for use with the reader.
 *
 * The HTML status bar is a small widget that acts as an indicator
 * for the message content. It can be in one of four modes:
 *
 * <dl>
 * <dt><code>Normal</code></dt>
 * <dd>Default. No HTML.</dd>
 * <dt><code>Html</code></dt>
 * <dd>HTML content is being shown. Since HTML mails can mimic all sorts
 *     of KMail markup in the reader, this provides out-of-band information
 *     about the presence of (rendered) HTML.</dd>
 * <dt><code>MultipartPlain</code></dt>
 * <dd>Viewed as plain text with HTML part also available.</dd>
 * <dt><code>MultipartHtml</code></dt>
 * <dd>Viewed as Html with plain text part also available.</dd>
 * </dl>
 *
 * @author Ingo Kloecker <kloecker@kde.org>, Marc Mutz <mutz@kde.org>
 **/
class HtmlStatusBar : public QLabel
{
    Q_OBJECT
public:
    enum UpdateMode {
        NoUpdate,
        Update,
    };

    explicit HtmlStatusBar(QWidget *parent = nullptr);
    ~HtmlStatusBar() override;

    /** @return current mode. */
    [[nodiscard]] MimeTreeParser::Util::HtmlMode mode() const;
    [[nodiscard]] bool isHtml() const;
    [[nodiscard]] bool isNormal() const;

    // Update the status bar, for example when the color scheme changed.
    void update();

    void setAvailableModes(const QList<MimeTreeParser::Util::HtmlMode> &availableModes);
    [[nodiscard]] const QList<MimeTreeParser::Util::HtmlMode> &availableModes();

public Q_SLOTS:
    void setHtmlMode();
    /** Switch to "normal mode". */
    void setNormalMode();
    /** Switch to mode @p m */
    void setMode(MimeTreeParser::Util::HtmlMode m, MessageViewer::HtmlStatusBar::UpdateMode mode = Update);

Q_SIGNALS:
    /** The user has clicked the status bar. */
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString message() const;
    QString toolTip() const;
    QColor bgColor() const;
    QColor fgColor() const;

    MimeTreeParser::Util::HtmlMode mMode;
    QList<MimeTreeParser::Util::HtmlMode> mAvailableModes;
};
}
