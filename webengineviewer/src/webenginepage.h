/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

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
     * Destructor.  If there is a private QWebEngineProfile then it will also
     * be destroyed.
     **/
    ~WebEnginePage() override;

    [[nodiscard]] WebEngineViewer::WebHitTest *hitTestContent(const QPoint &pos);

    void saveHtml(QWebEngineDownloadRequest *download);

    [[nodiscard]] QPoint mapToViewport(const QPoint &pos) const;
Q_SIGNALS:
    void urlClicked(const QUrl &url);
    void showConsoleMessage(const QString &message);

protected:
    [[nodiscard]] bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override;
    [[nodiscard]] bool eventFilter(QObject *obj, QEvent *event) override;
    [[nodiscard]] QString refreshCssVariablesScript();

private:
    WEBENGINEVIEWER_NO_EXPORT void init();
    class Private;
    std::unique_ptr<Private> d;
};
}
