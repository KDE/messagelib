/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <WebEngineViewer/BlockTrackingUrlInterceptor>
#include <WebEngineViewer/WebEngineView>

class KActionCollection;
namespace WebEngineViewer
{
class WebHitTestResult;
}
namespace MessageViewer
{
class ViewerPrivate;
class MailWebEngineViewPrivate;
/**
 * @brief The MailWebEngineView class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MailWebEngineView : public WebEngineViewer::WebEngineView
{
    Q_OBJECT
public:
    explicit MailWebEngineView(KActionCollection *ac, QWidget *parent = nullptr);
    ~MailWebEngineView() override;

    void scrollUp(int pixels);
    void scrollDown(int pixels);

    void selectAll();
    void scamCheck();

    void saveMainFrameScreenshotInFile(const QString &filename);
    void showAccessKeys();
    void hideAccessKeys();
    void isScrolledToBottom();

    void setElementByIdVisible(const QString &id, bool visible);
    void removeAttachmentMarking(const QString &id);
    void markAttachment(const QString &id, const QString &style);
    void scrollToAnchor(const QString &anchor);
    void scrollPageDown(int percent);
    void scrollPageUp(int percent);

    void scrollToRelativePosition(qreal pos);

    void setAllowExternalContent(bool b);

    [[nodiscard]] QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;

    void setPrintElementBackground(bool printElementBackground);

    void setLinkHovered(const QUrl &url);
    void setViewer(MessageViewer::ViewerPrivate *viewer);
    void readConfig();

    static void initializeCustomScheme();

public Q_SLOTS:
    void slotZoomChanged(qreal zoom);
    void slotShowDetails();

protected:
    void forwardWheelEvent(QWheelEvent *event) override;
    void forwardKeyPressEvent(QKeyEvent *event) override;
    void forwardKeyReleaseEvent(QKeyEvent *event) override;
    void forwardMousePressEvent(QMouseEvent *event) override;
    void forwardMouseMoveEvent(QMouseEvent *event) override;
    void forwardMouseReleaseEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *e) override;

    void contextMenuEvent(QContextMenuEvent *e) override;
Q_SIGNALS:
    void wheelZoomChanged(int numSteps);
    void openUrl(const QUrl &url);
    void urlBlocked(const QUrl &url);
    void messageMayBeAScam();
    void formSubmittedForbidden();
    /// Emitted when the user right-clicks somewhere
    void popupMenu(const WebEngineViewer::WebHitTestResult &result);
    void pageIsScrolledToBottom(bool);

private Q_SLOTS:
    MESSAGEVIEWER_NO_EXPORT void handleScrollToAnchor(const QVariant &result);
    MESSAGEVIEWER_NO_EXPORT void handleIsScrolledToBottom(const QVariant &result);

    MESSAGEVIEWER_NO_EXPORT void slotWebHitFinished(const WebEngineViewer::WebHitTestResult &result);
    MESSAGEVIEWER_NO_EXPORT void slotLoadFinished();

private:
    MESSAGEVIEWER_NO_EXPORT void runJavaScriptInWordId(const QString &script);
    std::unique_ptr<MailWebEngineViewPrivate> const d;
};
}
