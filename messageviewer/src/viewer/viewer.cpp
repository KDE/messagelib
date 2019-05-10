/*
  This file is part of KMail, the KDE mail client.
  Copyright (c) 1997 Markus Wuebben <markus.wuebben@kde.org>
  Copyright (C) 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  Copyright (c) 2009 Andras Mantia <andras@kdab.net>
  Copyright (C) 2013-2019 Laurent Montel <montel@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

// define this to copy all html that is written to the readerwindow to
// filehtmlwriter.out in the current working directory
//#define KMAIL_READER_HTML_DEBUG 1

#include "viewer.h"
#include "viewer_p.h"
#include "widgets/configurewidget.h"
#include "csshelper.h"
#include "settings/messageviewersettings.h"
#include "viewer/webengine/mailwebengineview.h"
#include <WebEngineViewer/WebHitTestResult>
#include <WebEngineViewer/WebEngineManageScript>
#include "viewer/mimeparttree/mimetreemodel.h"
#include "viewer/mimeparttree/mimeparttreeview.h"
#include "widgets/zoomactionmenu.h"

#include <Akonadi/KMime/MessageParts>
#include <AkonadiCore/itemfetchjob.h>
#include <AkonadiCore/itemfetchscope.h>
#include <QUrl>

#include <mailtransportakonadi/errorattribute.h>

#include <KLocalizedString>

#include <QAction>

namespace MessageViewer {
class AbstractMessageLoadedHandler::Private
{
public:
    Akonadi::Session *mSession = nullptr;
};

AbstractMessageLoadedHandler::AbstractMessageLoadedHandler()
    : d(new Private)
{
}

AbstractMessageLoadedHandler::~AbstractMessageLoadedHandler()
{
    delete d;
}

void AbstractMessageLoadedHandler::setSession(Akonadi::Session *session)
{
    d->mSession = session;
}

Akonadi::Session *AbstractMessageLoadedHandler::session() const
{
    return d->mSession;
}

Viewer::Viewer(QWidget *aParent, QWidget *mainWindow, KActionCollection *actionCollection)
    : QWidget(aParent)
    , d_ptr(new ViewerPrivate(this, mainWindow, actionCollection))
{
    initialize();
}

Viewer::~Viewer()
{
    //the d_ptr is automatically deleted
}

void Viewer::initialize()
{
    connect(d_ptr, &ViewerPrivate::displayPopupMenu,
            this, &Viewer::displayPopupMenu);
    connect(d_ptr, &ViewerPrivate::popupMenu,
            this, &Viewer::popupMenu);
    connect(d_ptr, SIGNAL(urlClicked(Akonadi::Item,QUrl)),
            SIGNAL(urlClicked(Akonadi::Item,QUrl)));
    connect(d_ptr, &ViewerPrivate::requestConfigSync, this, &Viewer::requestConfigSync);
    connect(d_ptr, &ViewerPrivate::makeResourceOnline, this, &Viewer::makeResourceOnline);
    connect(d_ptr, &ViewerPrivate::showReader,
            this, &Viewer::showReader);
    connect(d_ptr, &ViewerPrivate::showMessage, this, &Viewer::showMessage);
    connect(d_ptr, &ViewerPrivate::replyMessageTo, this, &Viewer::replyMessageTo);
    connect(d_ptr, &ViewerPrivate::showStatusBarMessage,
            this, &Viewer::showStatusBarMessage);
    connect(d_ptr, &ViewerPrivate::itemRemoved,
            this, &Viewer::itemRemoved);
    connect(d_ptr, &ViewerPrivate::changeDisplayMail, this, &Viewer::slotChangeDisplayMail);
    connect(d_ptr, &ViewerPrivate::moveMessageToTrash, this, &Viewer::moveMessageToTrash);
    connect(d_ptr, &ViewerPrivate::pageIsScrolledToBottom, this, &Viewer::pageIsScrolledToBottom);
    connect(d_ptr, &ViewerPrivate::printingFinished, this, &Viewer::printingFinished);
    connect(d_ptr, &ViewerPrivate::zoomChanged, this, &Viewer::zoomChanged);

    connect(d_ptr, &ViewerPrivate::showNextMessage, this, &Viewer::showNextMessage);
    connect(d_ptr, &ViewerPrivate::showPreviousMessage, this, &Viewer::showPreviousMessage);

    setMessage(KMime::Message::Ptr(), MimeTreeParser::Delayed);
}

void Viewer::changeEvent(QEvent *event)
{
    Q_D(Viewer);
    if (event->type() == QEvent::FontChange) {
        d->slotGeneralFontChanged();
    }
    QWidget::changeEvent(event);
}

void Viewer::setMessage(const KMime::Message::Ptr &message, MimeTreeParser::UpdateMode updateMode)
{
    Q_D(Viewer);
    if (message == d->message()) {
        return;
    }
    d->setMessage(message, updateMode);
}

void Viewer::setMessageItem(const Akonadi::Item &item, MimeTreeParser::UpdateMode updateMode)
{
    Q_D(Viewer);
    if (d->messageItem() == item) {
        return;
    }
    if (!item.isValid() || item.loadedPayloadParts().contains(Akonadi::MessagePart::Body)) {
        d->setMessageItem(item, updateMode);
    } else {
        Akonadi::ItemFetchJob *job = createFetchJob(item);
        connect(job, &Akonadi::ItemFetchJob::result, [ d](KJob *job) {
                d->itemFetchResult(job);
            });
        d->displaySplashPage(i18n("Loading message..."));
    }
}

QString Viewer::messagePath() const
{
    Q_D(const Viewer);
    return d->mMessagePath;
}

void Viewer::setMessagePath(const QString &path)
{
    Q_D(Viewer);
    d->mMessagePath = path;
}

void Viewer::displaySplashPage(const QString &templateName, const QVariantHash &data, const QByteArray &domain)
{
    Q_D(Viewer);
    d->displaySplashPage(templateName, data, domain);
}

void Viewer::enableMessageDisplay()
{
    Q_D(Viewer);
    d->enableMessageDisplay();
}

void Viewer::printMessage(const Akonadi::Item &msg)
{
    Q_D(Viewer);
    d->printMessage(msg);
}

void Viewer::printPreviewMessage(const Akonadi::Item &message)
{
    Q_D(Viewer);
    d->printPreviewMessage(message);
}

void Viewer::printPreview()
{
    Q_D(Viewer);
    d->slotPrintPreview();
}

void Viewer::print()
{
    Q_D(Viewer);
    d->slotPrintMessage();
}

void Viewer::resizeEvent(QResizeEvent *)
{
    Q_D(Viewer);
    if (!d->mResizeTimer.isActive()) {
        //
        // Combine all resize operations that are requested as long a
        // the timer runs.
        //
        d->mResizeTimer.start(100);
    }
}

void Viewer::closeEvent(QCloseEvent *e)
{
    Q_D(Viewer);
    QWidget::closeEvent(e);
    d->writeConfig();
}

void Viewer::slotAttachmentSaveAs()
{
    Q_D(Viewer);
    d->slotAttachmentSaveAs();
}

void Viewer::slotAttachmentSaveAll()
{
    Q_D(Viewer);
    d->slotAttachmentSaveAll();
}

void Viewer::slotSaveMessage()
{
    Q_D(Viewer);
    d->slotSaveMessage();
}

void Viewer::slotScrollUp()
{
    Q_D(Viewer);
    d->mViewer->scrollUp(10);
}

void Viewer::slotScrollDown()
{
    Q_D(Viewer);
    d->mViewer->scrollDown(10);
}

void Viewer::atBottom()
{
    Q_D(const Viewer);
    d->mViewer->isScrolledToBottom();
}

void Viewer::slotJumpDown()
{
    Q_D(Viewer);
    d->mViewer->scrollPageDown(100);
}

void Viewer::slotScrollPrior()
{
    Q_D(Viewer);
    d->mViewer->scrollPageUp(80);
}

void Viewer::slotScrollNext()
{
    Q_D(Viewer);
    d->mViewer->scrollPageDown(80);
}

QString Viewer::selectedText() const
{
    Q_D(const Viewer);
    return d->mViewer->selectedText();
}

Viewer::DisplayFormatMessage Viewer::displayFormatMessageOverwrite() const
{
    Q_D(const Viewer);
    return d->displayFormatMessageOverwrite();
}

void Viewer::setDisplayFormatMessageOverwrite(Viewer::DisplayFormatMessage format)
{
    Q_D(Viewer);
    d->setDisplayFormatMessageOverwrite(format);
}

void Viewer::setHtmlLoadExtDefault(bool loadExtDefault)
{
    Q_D(Viewer);
    d->setHtmlLoadExtDefault(loadExtDefault);
}

void Viewer::setHtmlLoadExtOverride(bool loadExtOverride)
{
    Q_D(Viewer);
    d->setHtmlLoadExtOverride(loadExtOverride);
}

bool Viewer::htmlLoadExtOverride() const
{
    Q_D(const Viewer);
    return d->htmlLoadExtOverride();
}

bool Viewer::htmlMail() const
{
    Q_D(const Viewer);
    return d->htmlMail();
}

bool Viewer::htmlLoadExternal() const
{
    Q_D(const Viewer);
    return d->htmlLoadExternal();
}

bool Viewer::isFixedFont() const
{
    Q_D(const Viewer);
    return d->mUseFixedFont;
}

void Viewer::setUseFixedFont(bool useFixedFont)
{
    Q_D(Viewer);
    d->setUseFixedFont(useFixedFont);
}

QWidget *Viewer::mainWindow()
{
    Q_D(Viewer);
    return d->mMainWindow;
}

void Viewer::setDecryptMessageOverwrite(bool overwrite)
{
    Q_D(Viewer);
    d->setDecryptMessageOverwrite(overwrite);
}

KMime::Message::Ptr Viewer::message() const
{
    Q_D(const Viewer);
    return d->mMessage;
}

Akonadi::Item Viewer::messageItem() const
{
    Q_D(const Viewer);
    return d->mMessageItem;
}

bool Viewer::event(QEvent *e)
{
    Q_D(Viewer);
    if (e->type() == QEvent::PaletteChange) {
        d->recreateCssHelper();
        d->update(MimeTreeParser::Force);
        e->accept();
        return true;
    }
    return QWidget::event(e);
}

void Viewer::slotFind()
{
    Q_D(Viewer);
    d->slotFind();
}

const AttachmentStrategy *Viewer::attachmentStrategy() const
{
    Q_D(const Viewer);
    return d->attachmentStrategy();
}

void Viewer::setAttachmentStrategy(const AttachmentStrategy *strategy)
{
    Q_D(Viewer);
    d->setAttachmentStrategy(strategy);
}

QString Viewer::overrideEncoding() const
{
    Q_D(const Viewer);
    return d->overrideEncoding();
}

void Viewer::setOverrideEncoding(const QString &encoding)
{
    Q_D(Viewer);
    d->setOverrideEncoding(encoding);
}

CSSHelper *Viewer::cssHelper() const
{
    Q_D(const Viewer);
    return d->cssHelper();
}

KToggleAction *Viewer::toggleFixFontAction() const
{
    Q_D(const Viewer);
    return d->mToggleFixFontAction;
}

bool Viewer::mimePartTreeIsEmpty() const
{
    Q_D(const Viewer);
    return d->mimePartTreeIsEmpty();
}

KToggleAction *Viewer::toggleMimePartTreeAction() const
{
    Q_D(const Viewer);
    return d->mToggleMimePartTreeAction;
}

QAction *Viewer::selectAllAction() const
{
    Q_D(const Viewer);
    return d->mSelectAllAction;
}

QAction *Viewer::viewSourceAction() const
{
    Q_D(const Viewer);
    return d->mViewSourceAction;
}

QAction *Viewer::copyURLAction() const
{
    Q_D(const Viewer);
    return d->mCopyURLAction;
}

QAction *Viewer::copyAction() const
{
    Q_D(const Viewer);
    return d->mCopyAction;
}

QAction *Viewer::speakTextAction() const
{
    Q_D(const Viewer);
    return d->mSpeakTextAction;
}

QAction *Viewer::copyImageLocation() const
{
    Q_D(const Viewer);
    return d->mCopyImageLocation;
}

QAction *Viewer::saveAsAction() const
{
    Q_D(const Viewer);
    return d->mSaveMessageAction;
}

QAction *Viewer::urlOpenAction() const
{
    Q_D(const Viewer);
    return d->mUrlOpenAction;
}

bool Viewer::printingMode() const
{
    Q_D(const Viewer);
    return d->printingMode();
}

void Viewer::setPrinting(bool enable)
{
    Q_D(Viewer);
    d->setPrinting(enable);
}

void Viewer::writeConfig(bool force)
{
    Q_D(Viewer);
    d->writeConfig(force);
}

QUrl Viewer::urlClicked() const
{
    Q_D(const Viewer);
    return d->mClickedUrl;
}

QUrl Viewer::imageUrlClicked() const
{
    Q_D(const Viewer);
    return d->imageUrl();
}

void Viewer::update(MimeTreeParser::UpdateMode updateMode)
{
    Q_D(Viewer);
    d->update(updateMode);
}

void Viewer::setMessagePart(KMime::Content *aMsgPart)
{
    Q_D(Viewer);
    d->setMessagePart(aMsgPart);
}

void Viewer::clear(MimeTreeParser::UpdateMode updateMode)
{
    setMessage(KMime::Message::Ptr(), updateMode);
}

void Viewer::slotShowMessageSource()
{
    Q_D(Viewer);
    d->slotShowMessageSource();
}

void Viewer::readConfig()
{
    Q_D(Viewer);
    d->readConfig();
}

QAbstractItemModel *Viewer::messageTreeModel() const
{
#ifndef QT_NO_TREEVIEW
    return d_func()->mMimePartTree->mimePartModel();
#else
    return nullptr;
#endif
}

Akonadi::ItemFetchJob *Viewer::createFetchJob(const Akonadi::Item &item)
{
    Q_D(Viewer);
    Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(item, d->mSession);
    job->fetchScope().fetchAllAttributes();
    job->fetchScope().setAncestorRetrieval(Akonadi::ItemFetchScope::Parent);
    job->fetchScope().fetchFullPayload(true);
    job->fetchScope().setFetchRelations(true);   // needed to know if we have notes or not
    job->fetchScope().fetchAttribute<MailTransport::ErrorAttribute>();
    return job;
}

void Viewer::addMessageLoadedHandler(AbstractMessageLoadedHandler *handler)
{
    Q_D(Viewer);

    if (!handler) {
        return;
    }

    handler->setSession(d->mSession);
    d->mMessageLoadedHandlers.insert(handler);
}

void Viewer::removeMessageLoadedHandler(AbstractMessageLoadedHandler *handler)
{
    Q_D(Viewer);

    d->mMessageLoadedHandlers.remove(handler);
}

void Viewer::deleteMessage()
{
    Q_D(Viewer);
    Q_EMIT deleteMessage(d->messageItem());
}

void Viewer::selectAll()
{
    Q_D(Viewer);
    d->selectAll();
}

void Viewer::copySelectionToClipboard()
{
    Q_D(Viewer);
    d->slotCopySelectedText();
}

void Viewer::setZoomFactor(qreal zoomFactor)
{
    Q_D(Viewer);
    d->mZoomActionMenu->setZoomFactor(zoomFactor);
}

void Viewer::slotZoomReset()
{
    Q_D(Viewer);
    d->mZoomActionMenu->slotZoomReset();
}

void Viewer::slotZoomIn()
{
    Q_D(Viewer);
    d->mZoomActionMenu->slotZoomIn();
}

void Viewer::slotZoomOut()
{
    Q_D(Viewer);
    d->mZoomActionMenu->slotZoomOut();
}

QAction *Viewer::findInMessageAction() const
{
    Q_D(const Viewer);
    return d->mFindInMessageAction;
}

void Viewer::slotChangeDisplayMail(Viewer::DisplayFormatMessage mode, bool loadExternal)
{
    if ((htmlLoadExtOverride() != loadExternal) || (displayFormatMessageOverwrite() != mode)) {
        setHtmlLoadExtOverride(loadExternal);
        setDisplayFormatMessageOverwrite(mode);
        update(MimeTreeParser::Force);
    }
}

QAction *Viewer::saveMessageDisplayFormatAction() const
{
    Q_D(const Viewer);
    return d->mSaveMessageDisplayFormat;
}

QAction *Viewer::resetMessageDisplayFormatAction() const
{
    Q_D(const Viewer);
    return d->mResetMessageDisplayFormat;
}

KToggleAction *Viewer::disableEmoticonAction() const
{
    Q_D(const Viewer);
    return d->mDisableEmoticonAction;
}

void Viewer::saveMainFrameScreenshotInFile(const QString &filename)
{
    Q_D(Viewer);
    d->saveMainFrameScreenshotInFile(filename);
}

KActionMenu *Viewer::shareServiceUrlMenu() const
{
    Q_D(const Viewer);
    return d->mShareServiceUrlMenu;
}

HeaderStylePlugin *Viewer::headerStylePlugin() const
{
    Q_D(const Viewer);
    return d->mHeaderStylePlugin;
}

void Viewer::setPluginName(const QString &pluginName)
{
    Q_D(Viewer);
    d->setPluginName(pluginName);
}

void Viewer::showOpenAttachmentFolderWidget(const QList<QUrl> &urls)
{
    Q_D(Viewer);
    d->showOpenAttachmentFolderWidget(urls);
}

QList<QAction *> Viewer::viewerPluginActionList(ViewerPluginInterface::SpecificFeatureTypes features)
{
    Q_D(Viewer);
    return d->viewerPluginActionList(features);
}

QList<QAction *> Viewer::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result)
const
{
    Q_D(const Viewer);
    return d->interceptorUrlActions(result);
}

void Viewer::runJavaScript(const QString &code)
{
    Q_D(Viewer);
    d->mViewer->page()->runJavaScript(code, WebEngineViewer::WebEngineManageScript::scriptWordId());
}

void Viewer::setPrintElementBackground(bool printElementBackground)
{
    Q_D(Viewer);
    d->mViewer->setPrintElementBackground(printElementBackground);
}

bool Viewer::showSignatureDetails() const
{
    Q_D(const Viewer);
    return d->showSignatureDetails();
}

void Viewer::setShowSignatureDetails(bool showDetails)
{
    Q_D(Viewer);
    d->setShowSignatureDetails(showDetails);
}

bool Viewer::showEncryptionDetails() const
{
    Q_D(const Viewer);
    return d->showEncryptionDetails();
}

void Viewer::hasMultiMessages(bool messages)
{
    Q_D(Viewer);
    d->hasMultiMessages(messages);
}

void Viewer::setShowEncryptionDetails(bool showDetails)
{
    Q_D(Viewer);
    d->setShowEncryptionDetails(showDetails);
}

qreal Viewer::webViewZoomFactor() const
{
    Q_D(const Viewer);
    return d->webViewZoomFactor();
}

void Viewer::setWebViewZoomFactor(qreal factor)
{
    Q_D(Viewer);
    d->setWebViewZoomFactor(factor);
}
}
