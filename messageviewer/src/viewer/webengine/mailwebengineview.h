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

#ifndef MAILWEBENGINE_H
#define MAILWEBENGINE_H
#include "messageviewer_export.h"
#include <WebEngineViewer/WebEngineView>
#include <boost/function.hpp>
class KActionCollection;
namespace WebEngineViewer
{
class WebHitTestResult;
}
namespace MessageViewer
{
class ViewerPrivate;
class MailWebEngineViewPrivate;
class MESSAGEVIEWER_EXPORT MailWebEngineView : public WebEngineViewer::WebEngineView
{
    Q_OBJECT
public:
    explicit MailWebEngineView(KActionCollection *ac, QWidget *parent = Q_NULLPTR);
    ~MailWebEngineView();

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
    void injectAttachments(const boost::function<QString()> &delayedHtml);

    void scrollToRelativePosition(qreal pos);

    void setAllowExternalContent(bool b);

    QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;
    void toggleFullAddressList(const QString &field, const boost::function<QString()> &delayedHtml);

    void setPrintElementBackground(bool printElementBackground);

    void executeHideShowAttachmentsScripts(bool hide);
    void executeHideShowToAddressScripts(bool hide);
    void executeHideShowCcAddressScripts(bool hide);
    void setLinkHovered(const QUrl &url);
    void setViewer(MessageViewer::ViewerPrivate *viewer);
public Q_SLOTS:
    void slotZoomChanged(qreal zoom);
    void slotShowDetails();

protected:
    void forwardWheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void forwardKeyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void forwardKeyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void forwardMousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void forwardMouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void forwardMouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

    void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;
Q_SIGNALS:
    void wheelZoomChanged(int numSteps);
    void openUrl(const QUrl &url);
    void messageMayBeAScam();
    void formSubmittedForbidden();
    /// Emitted when the user right-clicks somewhere
    /// @param url if an URL was under the cursor, this parameter contains it. Otherwise empty
    /// @param point position where the click happened, in local coordinates
    void popupMenu(const WebEngineViewer::WebHitTestResult &result);
    void pageIsScrolledToBottom(bool);

private Q_SLOTS:
    void handleScrollToAnchor(const QVariant &result);
    void handleIsScrolledToBottom(const QVariant &result);

    void slotWebHitFinished(const WebEngineViewer::WebHitTestResult &result);
    void slotLoadFinished();
private:
    void initializeScripts();
    void runJavaScriptInWordId(const QString &script);
    MailWebEngineViewPrivate *const d;
};
}

#endif // MAILWEBENGINE_H
