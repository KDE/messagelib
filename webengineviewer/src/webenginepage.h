/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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

#ifndef WEBENGINEPAGE_H
#define WEBENGINEPAGE_H

#include <QWebEnginePage>
#include "webengineviewer_export.h"
class QWebEngineProfile;
class QWebEngineDownloadItem;
class QPrinter;
namespace WebEngineViewer {
class WebHitTest;
class WEBENGINEVIEWER_EXPORT WebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    /**
     * Constructor.
     *
     * A private QWebEngineProfile, only applying to this QWebEnginePage,
     * will be created to implement browser settings.  It can be accessed via
     * @c profile(), but it should not be shared or reused unless care is
     * taken that the profile is not deleted until all of the QWebEnginePage's
     * belonging to it are deleted first.
     *
     * @param parent The parent object
     **/
    explicit WebEnginePage(QObject *parent = nullptr);

    /**
     * Constructor.
     *
     * The specified QWebEngineProfile will be used.  See the description of
     * @c WebEnginePage(QObject *) and the API documentation of QWebEnginePage
     * for caution regarding the lifetime of the profile.
     *
     * @param profile The profile to be used
     * @param parent The parent object
     * @deprecated Use the single argument constructor, which creates and uses
     * a private profile.
     **/
#ifndef WEBENGINEVIEWER_NO_DEPRECATED
    explicit WEBENGINEVIEWER_DEPRECATED WebEnginePage(QWebEngineProfile *profile, QObject *parent = nullptr);
#endif

    /**
     * Destructor.  If there is a private QWebEngineProfile then it will also
     * be destroyed.
     **/
    virtual ~WebEnginePage() override = default;

    WebEngineViewer::WebHitTest *hitTestContent(const QPoint &pos);

    void saveHtml(QWebEngineDownloadItem *download);
    Q_REQUIRED_RESULT bool execPrintPreviewPage(QPrinter *printer, int timeout);

    QPoint mapToViewport(const QPoint &pos) const;
Q_SIGNALS:
    void urlClicked(const QUrl &url);
    void showConsoleMessage(const QString &message);

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override;

private:
    void init();
};
}

#endif // WEBENGINEPAGE_H
