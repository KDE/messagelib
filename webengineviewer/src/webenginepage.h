/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QWebEnginePage>
class QWebEngineProfile;
class QWebEngineDownloadItem;
class QPrinter;
namespace WebEngineViewer
{
class WebHitTest;
/**
 * @brief The WebEnginePage class
 * @author Laurent Montel <montel@kde.org>
 */
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

    Q_REQUIRED_RESULT QPoint mapToViewport(const QPoint &pos) const;
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

