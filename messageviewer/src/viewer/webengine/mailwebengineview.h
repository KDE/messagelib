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

#ifndef MAILWEBENGINE_H
#define MAILWEBENGINE_H
#include "messageviewer_export.h"
#include <WebEngineViewer/WebEngineView>
#include <boost/function.hpp>
#include <MessageViewer/BlockMailTrackingUrlInterceptor>
class QPrinter;
class KActionCollection;
namespace WebEngineViewer {
class WebHitTestResult;
}
namespace MessageViewer {
class ViewerPrivate;
class MailWebEngineViewPrivate;
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

    Q_REQUIRED_RESULT QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;

    void setPrintElementBackground(bool printElementBackground);

    void setLinkHovered(const QUrl &url);
    void setViewer(MessageViewer::ViewerPrivate *viewer);
    bool execPrintPreviewPage(QPrinter *printer, int timeout);
    void readConfig();
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
    void messageMayBeAScam();
    void formSubmittedForbidden();
    void mailTrackingFound(const MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList &);
    /// Emitted when the user right-clicks somewhere
    void popupMenu(const WebEngineViewer::WebHitTestResult &result);
    void pageIsScrolledToBottom(bool);

private Q_SLOTS:
    void handleScrollToAnchor(const QVariant &result);
    void handleIsScrolledToBottom(const QVariant &result);

    void slotWebHitFinished(const WebEngineViewer::WebHitTestResult &result);
    void slotLoadFinished();
private:
    void runJavaScriptInWordId(const QString &script);
    MailWebEngineViewPrivate *const d;
};
}

#endif // MAILWEBENGINE_H
