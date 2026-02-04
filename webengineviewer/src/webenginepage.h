/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QWebEnginePage>
class QWebEngineProfile;
class QWebEngineDownloadRequest;
namespace WebEngineViewer
{
class WebHitTest;
/*!
 * \class WebEngineViewer::WebEnginePage
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/WebEnginePage
 *
 * \brief The WebEnginePage class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    /*!
     * Constructor.
     *
     * A private QWebEngineProfile, only applying to this QWebEnginePage,
     * will be created to implement browser settings.  It can be accessed via
     * \\ profile(), but it should not be shared or reused unless care is
     * taken that the profile is not deleted until all of the QWebEnginePage's
     * belonging to it are deleted first.
     *
     * \a parent The parent object
     **/
    explicit WebEnginePage(QObject *parent = nullptr);
    /*!
     * Destructor.  If there is a private QWebEngineProfile then it will also
     * be destroyed.
     **/
    ~WebEnginePage() override;
    /*! Performs a hit test on the content at the specified position. */
    [[nodiscard]] WebEngineViewer::WebHitTest *hitTestContent(const QPoint &pos);

    /*! Saves the HTML content to the specified download request. */
    void saveHtml(QWebEngineDownloadRequest *download);

    /*! Maps the given point to the viewport. */
    [[nodiscard]] QPoint mapToViewport(const QPoint &pos) const;
Q_SIGNALS:
    /*! Emitted when a URL is clicked. */
    void urlClicked(const QUrl &url);
    /*! Emitted when a console message should be displayed. */
    void showConsoleMessage(const QString &message);

protected:
    /*! Accepts or rejects a navigation request. */
    [[nodiscard]] bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;
    /*! Handles JavaScript console messages. */
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override;
    /*! Filters events for the object. */
    [[nodiscard]] bool eventFilter(QObject *obj, QEvent *event) override;
    /*! Returns a script to refresh CSS variables. */
    [[nodiscard]] QString refreshCssVariablesScript();

private:
    WEBENGINEVIEWER_NO_EXPORT void init();
    class Private;
    std::unique_ptr<Private> d;
};
}
