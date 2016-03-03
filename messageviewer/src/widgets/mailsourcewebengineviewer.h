/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MAILSOURCEWEBENGINEVIEWER_H
#define MAILSOURCEWEBENGINEVIEWER_H

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
    explicit MailSourceWebEngineViewer(QWidget *parent = Q_NULLPTR);
    ~MailSourceWebEngineViewer();

    void setRawSource(const QString &source);
    void setDisplayedSource(QWebEnginePage *page);
    void setFixedFont();
private:
    QString reformat(const QString &src);
    MailSourceViewTextBrowserWidget *mRawBrowser;
    FindBarSourceView *mFindBar;
#ifndef NDEBUG
    QTabWidget *mTabWidget;
    MailSourceViewTextBrowserWidget *mHtmlBrowser;
#endif
};
}
#endif // MAILSOURCEWEBENGINEVIEWER_H
