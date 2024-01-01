/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QDialog>
class QTabWidget;
class QWebEnginePage;

namespace MessageViewer
{
class FindBarSourceView;
class MailSourceViewTextBrowserWidget;
class MailSourceWebEngineViewer : public QDialog
{
    Q_OBJECT
public:
    explicit MailSourceWebEngineViewer(QWidget *parent = nullptr);
    ~MailSourceWebEngineViewer() override;

    void setRawSource(const QString &source);
    void setDisplayedSource(QWebEnginePage *page);
    void setFixedFont();

private:
    void readConfig();
    void writeConfig();
    MailSourceViewTextBrowserWidget *const mRawBrowser;
    QTabWidget *mTabWidget = nullptr;
    MailSourceViewTextBrowserWidget *mHtmlBrowser = nullptr;
    bool mShowHtmlSource = false;
};
}
