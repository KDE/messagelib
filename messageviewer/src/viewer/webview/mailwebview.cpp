/*
   Copyright 2010 Thomas McGuire <mcguire@kde.org>

   Copyright 2013-2016 Laurent Montel <monte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mailwebview.h"
#include "messageviewer_debug.h"
#include "scamdetection/scamdetection.h"
#include "scamdetection/scamcheckshorturl.h"
#include "adblock/adblockblockableitemsdialog.h"
#include "webpage.h"
#include "webviewaccesskey.h"

#include <KActionCollection>
#include <QAction>

#include <QCoreApplication>
#include <QContextMenuEvent>
#include <QWebFrame>
#include <QWebElement>
#include <QLabel>
#include <QToolTip>

#include <limits>
#include <cassert>

using namespace boost;
using namespace MessageViewer;

MailWebView::MailWebView(KActionCollection *actionCollection, QWidget *parent)
    : KWebView(parent, false),
      mScamDetection(new ScamDetection),
      mActionCollection(actionCollection)
{
    mWebViewAccessKey = new WebViewAccessKey(this, this);
    mWebViewAccessKey->setActionCollection(mActionCollection);
    setPage(new MessageViewer::WebPage(this));
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    settings()->setAttribute(QWebSettings::JavaEnabled, false);
    settings()->setAttribute(QWebSettings::PluginsEnabled, false);

    const QFontInfo font(QFontDatabase().systemFont(QFontDatabase::GeneralFont));
    settings()->setFontFamily(QWebSettings::StandardFont, font.family());
    settings()->setFontSize(QWebSettings::DefaultFontSize, font.pixelSize());

    connect(page(), &QWebPage::linkHovered,
            this,   &MailWebView::linkHovered);
    connect(this, &QWebView::loadStarted, mWebViewAccessKey, &WebViewAccessKey::hideAccessKeys);
    connect(mScamDetection, &ScamDetection::messageMayBeAScam, this, &MailWebView::messageMayBeAScam);
    connect(page(), &QWebPage::scrollRequested, mWebViewAccessKey, &WebViewAccessKey::hideAccessKeys);
}

MailWebView::~MailWebView()
{
    delete mScamDetection;
}

bool MailWebView::event(QEvent *event)
{
    if (event->type() == QEvent::ContextMenu) {
        // Don't call KWebView::event() here, it will do silly things like selecting the text
        // under the mouse cursor, which we don't want.

        QContextMenuEvent const *contextMenuEvent = static_cast<QContextMenuEvent *>(event);
        const QWebFrame *const frame = page()->currentFrame();
        const QWebHitTestResult hit = frame->hitTestContent(contextMenuEvent->pos());
        qCDebug(MESSAGEVIEWER_LOG) << "Right-clicked URL:" << hit.linkUrl();

        Q_EMIT popupMenu(hit.linkUrl(), ((hit.pixmap().isNull()) ? QUrl() : hit.imageUrl()), mapToGlobal(contextMenuEvent->pos()));
        event->accept();
        return true;
    }
    return KWebView::event(event);
}

void MailWebView::scrollDown(int pixels)
{
    QPoint point = page()->mainFrame()->scrollPosition();
    point.ry() += pixels;
    page()->mainFrame()->setScrollPosition(point);
}

void MailWebView::scrollUp(int pixels)
{
    scrollDown(-pixels);
}

bool MailWebView::isScrolledToBottom() const
{
    const int pos = page()->mainFrame()->scrollBarValue(Qt::Vertical);
    const int max = page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
    return pos == max;
}

void MailWebView::scrollPageDown(int percent)
{
    const qint64 height =  page()->viewportSize().height();
    const qint64 current = page()->mainFrame()->scrollBarValue(Qt::Vertical);
    // do arithmetic in higher precision, and check for overflow:
    const qint64 newPosition = current + height * percent / 100;
    if (newPosition > std::numeric_limits<int>::max()) {
        qCWarning(MESSAGEVIEWER_LOG) << "new position" << newPosition << "exceeds range of 'int'!";
    }
    page()->mainFrame()->setScrollBarValue(Qt::Vertical, newPosition);
}

void MailWebView::scrollPageUp(int percent)
{
    scrollPageDown(-percent);
}

QString MailWebView::selectedText() const
{
    //TODO HTML selection
    /* settings()->setAttribute( QWebSettings::JavascriptEnabled, true );
    QString textSelected = page()->currentFrame()->evaluateJavaScript(
    "var span = document.createElement( 'SPAN' ); span.appendChild( window.getSelection().getRangeAt(0).cloneContents() );
    ).toString();
    settings()->setAttribute( QWebSettings::JavascriptEnabled, false );

    return textSelected;
    */
    return KWebView::selectedText();
}

bool MailWebView::hasVerticalScrollBar() const
{
    return page()->mainFrame()->scrollBarGeometry(Qt::Vertical).isValid();
}

double MailWebView::relativePosition() const
{
    if (hasVerticalScrollBar()) {
        const double pos = page()->mainFrame()->scrollBarValue(Qt::Vertical);
        const int height = page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
        return height ? pos / height : 0.0;
    } else {
        return 0.0;
    }
}

void MailWebView::scrollToRelativePosition(double pos)
{
    // FIXME: This doesn't work, Qt resets the scrollbar value somewhere in the event handler.
    //        Using a singleshot timer wouldn't work either, since that introduces visible scrolling.
    const int max = page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
    page()->currentFrame()->setScrollBarValue(Qt::Vertical, max * pos);
}

void MailWebView::selectAll()
{
    page()->triggerAction(QWebPage::SelectAll);
}

void MailWebView::clearSelection()
{
    //This is an ugly hack to remove the selection, I found no other way to do it with QWebView
    QMouseEvent event(QEvent::MouseButtonPress, QPoint(10, 10), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(page(), &event);
    QMouseEvent event2(QEvent::MouseButtonRelease, QPoint(10, 10), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(page(), &event2);
}

// Checks if the given node has a child node that is a DIV which has an ID attribute
// with the value specified here
static bool has_parent_div_with_id(const QWebElement &start, const QString &id)
{
    if (start.isNull()) {
        return false;
    }

    if (start.tagName().toLower() == QLatin1String("div")) {
        if (start.attribute(QStringLiteral("id"), QString()) == id) {
            return true;
        }
    }

    return has_parent_div_with_id(start.parent(), id);
}

bool MailWebView::isAttachmentInjectionPoint(const QPoint &global) const
{
    // for QTextBrowser, can be implemented as 'return false'
    const QPoint local = page()->view()->mapFromGlobal(global);
    const QWebHitTestResult hit = page()->currentFrame()->hitTestContent(local);
    return has_parent_div_with_id(hit.enclosingBlockElement(), QStringLiteral("attachmentInjectionPoint"));
}

void MailWebView::injectAttachments(const function<QString()> &delayedHtml)
{
    // for QTextBrowser, can be implemented empty
    QWebElement doc = page()->currentFrame()->documentElement();
    QWebElement injectionPoint = doc.findFirst(QStringLiteral("*#attachmentInjectionPoint"));
    if (injectionPoint.isNull()) {
        return;
    }

    const QString html = delayedHtml();
    if (html.isEmpty()) {
        return;
    }

    assert(injectionPoint.tagName().toLower() == QLatin1String("div"));
    injectionPoint.setInnerXml(html);
}

void MailWebView::scrollToAnchor(const QString &anchor)
{
    page()->mainFrame()->scrollToAnchor(anchor);
}

bool MailWebView::removeAttachmentMarking(const QString &id)
{
    QWebElement doc = page()->mainFrame()->documentElement();
    QWebElement attachmentDiv = doc.findFirst(QLatin1String("*#") + id);
    if (attachmentDiv.isNull()) {
        return false;
    }
    attachmentDiv.removeAttribute(QStringLiteral("style"));
    return true;
}

void MailWebView::markAttachment(const QString &id, const QString &style)
{
    QWebElement doc = page()->mainFrame()->documentElement();
    QWebElement attachmentDiv = doc.findFirst(QLatin1String("*#") + id);
    if (!attachmentDiv.isNull()) {
        attachmentDiv.setAttribute(QStringLiteral("style"), style);
    }
}

QString MailWebView::htmlSource() const
{
    return page()->mainFrame()->documentElement().toOuterXml();
}

QUrl MailWebView::linkOrImageUrlAt(const QPoint &global) const
{
    const QPoint local = page()->view()->mapFromGlobal(global);
    const QWebHitTestResult hit = page()->currentFrame()->hitTestContent(local);
    if (!hit.linkUrl().isEmpty()) {
        return hit.linkUrl();
    } else if (!hit.imageUrl().isEmpty()) {
        return hit.imageUrl();
    } else {
        return QUrl();
    }
}

void MailWebView::setScrollBarPolicy(Qt::Orientation orientation, Qt::ScrollBarPolicy policy)
{
    page()->mainFrame()->setScrollBarPolicy(orientation, policy);
}

Qt::ScrollBarPolicy MailWebView::scrollBarPolicy(Qt::Orientation orientation) const
{
    return page()->mainFrame()->scrollBarPolicy(orientation);
}

bool MailWebView::replaceInnerHtml(const QString &id, const function<QString()> &delayedHtml)
{
    QWebElement doc = page()->currentFrame()->documentElement();
    QWebElement tag = doc.findFirst(QLatin1String("*#") + id);
    if (tag.isNull()) {
        return false;
    }
    tag.setInnerXml(delayedHtml());
    return true;
}

void MailWebView::setElementByIdVisible(const QString &id, bool visible)
{
    QWebElement doc = page()->currentFrame()->documentElement();
    QWebElement e = doc.findFirst(QLatin1String("*#") + id);
    Q_ASSERT(!e.isNull());

    if (visible) {
        e.removeAttribute(QStringLiteral("display"));
    } else {
        e.setStyleProperty(QStringLiteral("display"), QStringLiteral("none"));
    }
}

void MailWebView::keyReleaseEvent(QKeyEvent *e)
{
    if (MessageViewer::MessageViewerSettings::self()->accessKeyEnabled()) {
        mWebViewAccessKey->keyReleaseEvent(e);
    }
    KWebView::keyReleaseEvent(e);
}

void MailWebView::keyPressEvent(QKeyEvent *e)
{
    if (e && hasFocus()) {
        if (MessageViewer::MessageViewerSettings::self()->accessKeyEnabled()) {
            mWebViewAccessKey->keyPressEvent(e);
        }
    }
    KWebView::keyPressEvent(e);
}

void MailWebView::wheelEvent(QWheelEvent *e)
{
    if (MessageViewer::MessageViewerSettings::self()->accessKeyEnabled()) {
        mWebViewAccessKey->wheelEvent(e);
    }
    KWebView::wheelEvent(e);
}

void MailWebView::resizeEvent(QResizeEvent *e)
{
    if (MessageViewer::MessageViewerSettings::self()->accessKeyEnabled()) {
        mWebViewAccessKey->resizeEvent(e);
    }
    KWebView::resizeEvent(e);
}

void MailWebView::scamCheck()
{
    QWebFrame *mainFrame = page()->mainFrame();
    mScamDetection->scanPage(mainFrame);
}

void MailWebView::slotShowDetails()
{
    mScamDetection->showDetails();
}

void MailWebView::slotZoomChanged(qreal zoom)
{
    setZoomFactor(zoom);
}

void MailWebView::slotZoomTextOnlyChanged(bool b)
{
    settings()->setAttribute(QWebSettings::ZoomTextOnly, b);
}

void MailWebView::saveMainFrameScreenshotInFile(const QString &filename)
{
    QWebFrame *frame = page()->mainFrame();
    QImage image(frame->contentsSize(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    frame->documentElement().render(&painter);
    painter.end();
    image.save(filename);
}

void MailWebView::openBlockableItemsDialog()
{
    QPointer<AdBlockBlockableItemsDialog> dlg = new AdBlockBlockableItemsDialog(this);
    dlg->setWebFrame(page()->mainFrame());
    if (dlg->exec()) {
        dlg->saveFilters();
    }
    delete dlg;
}

void MailWebView::expandUrl(const QUrl &url)
{
    mScamDetection->scamCheckShortUrl()->expandedUrl(url);
}

bool MailWebView::isAShortUrl(const QUrl &url) const
{
    return mScamDetection->scamCheckShortUrl()->isShortUrl(url);
}

#include "moc_mailwebview.cpp"
