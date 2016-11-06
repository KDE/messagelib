/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MAILSOURCEWEBENGINEVIEWER_H
#define MAILSOURCEWEBENGINEVIEWER_H

#include <QDialog>
#include "config-messageviewer.h"
class QTabWidget;
class QWebEnginePage;

#ifdef KDEPIM_KF5SYNTAXHIGHLIGHTING_SUPPORT
#include <KSyntaxHighlighting/Repository>
#endif

namespace MessageViewer
{
class FindBarSourceView;
class MailSourceViewTextBrowserWidget;
class MailSourceWebEngineViewer : public QDialog
{
    Q_OBJECT
public:
    explicit MailSourceWebEngineViewer(QWidget *parent = Q_NULLPTR);
    ~MailSourceWebEngineViewer();

    void setRawSource(const QString &source);
    void setDisplayedSource(QWebEnginePage *page);
    void setFixedFont();
private:
#ifdef KDEPIM_KF5SYNTAXHIGHLIGHTING_SUPPORT
    KSyntaxHighlighting::Repository mRepo;
#endif
    MailSourceViewTextBrowserWidget *mRawBrowser;
    FindBarSourceView *mFindBar;
#ifndef NDEBUG
    QTabWidget *mTabWidget;
    MailSourceViewTextBrowserWidget *mHtmlBrowser;
#endif
};
}
#endif // MAILSOURCEWEBENGINEVIEWER_H
