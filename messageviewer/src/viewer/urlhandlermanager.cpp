/*  -*- c++ -*-
    urlhandlermanager.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003      Marc Mutz <mutz@kde.org>
    Copyright (C) 2002-2003, 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    Copyright (c) 2009 Andras Mantia <andras@kdab.net>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License, version 2, as
    published by the Free Software Foundation.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#include "urlhandlermanager.h"
#include "urlhandlermanager_p.h"
#include "messageviewer_debug.h"
#include "interfaces/bodyparturlhandler.h"
#include "utils/mimetype.h"
#include "viewer/viewer_p.h"
#include "messageviewer/messageviewerutil.h"
#include "../utils/messageviewerutil_p.h"
#include "stl_util.h"

#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/PartNodeBodyPart>

#include <MessageCore/StringUtil>
#include <Libkdepim/BroadcastStatus>
#include <LibkdepimAkonadi/OpenEmailAddressJob>

#include <Akonadi/Contact/ContactSearchJob>

#include <KMime/Content>
#include <KEmailAddress>
#include <kmbox/mbox.h>
#include "messageflags.h"

#include <KRun>
#include <KIconLoader>
#include <KLocalizedString>
#include <kmessagebox.h>

#include <QMenu>
#include <QIcon>
#include <QApplication>
#include <QClipboard>
#include <QProcess>
#include <QFile>
#include <QMimeData>
#include <QDrag>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QUrl>

#include <algorithm>

#include <Libkleo/MessageBox>

using std::for_each;
using std::remove;
using std::find;
using namespace MessageViewer;
using namespace MessageCore;

URLHandlerManager *URLHandlerManager::self = nullptr;

//
//
// BodyPartURLHandlerManager
//
//

BodyPartURLHandlerManager::~BodyPartURLHandlerManager()
{
    for_each(mHandlers.begin(), mHandlers.end(),
             [](QVector<const Interface::BodyPartURLHandler *> &handlers) {
        for_each(handlers.begin(), handlers.end(),
                 DeleteAndSetToZero<Interface::BodyPartURLHandler>());
    });
}

void BodyPartURLHandlerManager::registerHandler(
    const Interface::BodyPartURLHandler *handler, const QString &mimeType)
{
    if (!handler) {
        return;
    }
    unregisterHandler(handler);   // don't produce duplicates
    const auto mt = mimeType.toLatin1();
    auto it = mHandlers.find(mt);
    if (it == mHandlers.end()) {
        it = mHandlers.insert(mt, {});
    }
    it->push_back(handler);
}

void BodyPartURLHandlerManager::unregisterHandler(
    const Interface::BodyPartURLHandler *handler)
{
    // don't delete them, only remove them from the list!
    auto it = mHandlers.begin();
    while (it != mHandlers.end()) {
        it->erase(remove(it->begin(), it->end(), handler), it->end());
        if (it->isEmpty()) {
            it = mHandlers.erase(it);
        } else {
            ++it;
        }
    }
}

static KMime::Content *partNodeFromXKMailUrl(const QUrl &url, ViewerPrivate *w, QString *path)
{
    Q_ASSERT(path);

    if (!w || url.scheme() != QLatin1String("x-kmail")) {
        return nullptr;
    }
    const QString urlPath = url.path();

    // urlPath format is: /bodypart/<random number>/<part id>/<path>

    qCDebug(MESSAGEVIEWER_LOG) << "BodyPartURLHandler: urlPath ==" << urlPath;
    if (!urlPath.startsWith(QLatin1String("/bodypart/"))) {
        return nullptr;
    }

    const QStringList urlParts = urlPath.mid(10).split(QLatin1Char('/'));
    if (urlParts.size() != 3) {
        return nullptr;
    }
    //KMime::ContentIndex index( urlParts[1] );
    QByteArray query(urlParts.at(2).toLatin1());
    if (url.hasQuery()) {
        query += "?" + url.query().toLatin1();
    }
    *path = QUrl::fromPercentEncoding(query);
    return w->nodeFromUrl(QUrl(urlParts.at(1)));
}

QVector<const Interface::BodyPartURLHandler *> BodyPartURLHandlerManager::handlersForPart(KMime::Content *node) const
{
    if (auto ct = node->contentType(false)) {
        const auto mimeType = ct->mimeType();
        if (!mimeType.isEmpty()) {
            return mHandlers.value(mimeType);
        }
    }

    return {};
}

bool BodyPartURLHandlerManager::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    QString path;
    KMime::Content *node = partNodeFromXKMailUrl(url, w, &path);
    if (!node) {
        return false;
    }

    MimeTreeParser::PartNodeBodyPart part(nullptr, nullptr,
                                          w->message().data(), node, w->nodeHelper());

    for (const auto &handlers : { handlersForPart(node), mHandlers.value({}) }) {
        for (auto it = handlers.cbegin(), end = handlers.cend(); it != end; ++it) {
            if ((*it)->handleClick(w->viewer(), &part, path)) {
                return true;
            }
        }
    }

    return false;
}

bool BodyPartURLHandlerManager::handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *w) const
{
    QString path;
    KMime::Content *node = partNodeFromXKMailUrl(url, w, &path);
    if (!node) {
        return false;
    }

    MimeTreeParser::PartNodeBodyPart part(nullptr, nullptr,
                                          w->message().data(), node, w->nodeHelper());

    for (const auto &handlers : { handlersForPart(node), mHandlers.value({}) }) {
        for (auto it = handlers.cbegin(), end = handlers.cend(); it != end; ++it) {
            if ((*it)->handleContextMenuRequest(&part, path, p)) {
                return true;
            }
        }
    }
    return false;
}

QString BodyPartURLHandlerManager::statusBarMessage(const QUrl &url, ViewerPrivate *w) const
{
    QString path;
    KMime::Content *node = partNodeFromXKMailUrl(url, w, &path);
    if (!node) {
        return QString();
    }

    MimeTreeParser::PartNodeBodyPart part(nullptr, nullptr,
                                          w->message().data(), node, w->nodeHelper());

    for (const auto &handlers : { handlersForPart(node), mHandlers.value({}) }) {
        for (auto it = handlers.cbegin(), end = handlers.cend(); it != end; ++it) {
            const QString msg = (*it)->statusBarMessage(&part, path);
            if (!msg.isEmpty()) {
                return msg;
            }
        }
    }
    return QString();
}

//
//
// URLHandlerManager
//
//

URLHandlerManager::URLHandlerManager()
{
    registerHandler(new KMailProtocolURLHandler());
    registerHandler(new ExpandCollapseQuoteURLManager());
    registerHandler(new SMimeURLHandler());
    registerHandler(new MailToURLHandler());
    registerHandler(new ContactUidURLHandler());
    registerHandler(new HtmlAnchorHandler());
    registerHandler(new AttachmentURLHandler());
    registerHandler(mBodyPartURLHandlerManager = new BodyPartURLHandlerManager());
    registerHandler(new ShowAuditLogURLHandler());
    registerHandler(new InternalImageURLHandler);
    registerHandler(new KRunURLHandler());
}

URLHandlerManager::~URLHandlerManager()
{
    for_each(mHandlers.begin(), mHandlers.end(),
             DeleteAndSetToZero<MimeTreeParser::URLHandler>());
}

URLHandlerManager *URLHandlerManager::instance()
{
    if (!self) {
        self = new URLHandlerManager();
    }
    return self;
}

void URLHandlerManager::registerHandler(const MimeTreeParser::URLHandler *handler)
{
    if (!handler) {
        return;
    }
    unregisterHandler(handler);   // don't produce duplicates
    mHandlers.push_back(handler);
}

void URLHandlerManager::unregisterHandler(const MimeTreeParser::URLHandler *handler)
{
    // don't delete them, only remove them from the list!
    mHandlers.erase(remove(mHandlers.begin(), mHandlers.end(), handler), mHandlers.end());
}

void URLHandlerManager::registerHandler(const Interface::BodyPartURLHandler *handler, const QString &mimeType)
{
    if (mBodyPartURLHandlerManager) {
        mBodyPartURLHandlerManager->registerHandler(handler, mimeType);
    }
}

void URLHandlerManager::unregisterHandler(const Interface::BodyPartURLHandler *handler)
{
    if (mBodyPartURLHandlerManager) {
        mBodyPartURLHandlerManager->unregisterHandler(handler);
    }
}

bool URLHandlerManager::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    HandlerList::const_iterator end(mHandlers.constEnd());
    for (HandlerList::const_iterator it = mHandlers.constBegin(); it != end; ++it) {
        if ((*it)->handleClick(url, w)) {
            return true;
        }
    }
    return false;
}

bool URLHandlerManager::handleShiftClick(const QUrl &url, ViewerPrivate *window) const
{
    HandlerList::const_iterator end(mHandlers.constEnd());
    for (HandlerList::const_iterator it = mHandlers.constBegin(); it != end; ++it) {
        if ((*it)->handleShiftClick(url, window)) {
            return true;
        }
    }
    return false;
}

bool URLHandlerManager::willHandleDrag(const QUrl &url, ViewerPrivate *window) const
{
    HandlerList::const_iterator end(mHandlers.constEnd());

    for (HandlerList::const_iterator it = mHandlers.constBegin(); it != end; ++it) {
        if ((*it)->willHandleDrag(url, window)) {
            return true;
        }
    }
    return false;
}

bool URLHandlerManager::handleDrag(const QUrl &url, ViewerPrivate *window) const
{
    HandlerList::const_iterator end(mHandlers.constEnd());
    for (HandlerList::const_iterator it = mHandlers.constBegin(); it != end; ++it) {
        if ((*it)->handleDrag(url, window)) {
            return true;
        }
    }
    return false;
}

bool URLHandlerManager::handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *w) const
{
    HandlerList::const_iterator end(mHandlers.constEnd());
    for (HandlerList::const_iterator it = mHandlers.constBegin(); it != end; ++it) {
        if ((*it)->handleContextMenuRequest(url, p, w)) {
            return true;
        }
    }
    return false;
}

QString URLHandlerManager::statusBarMessage(const QUrl &url, ViewerPrivate *w) const
{
    HandlerList::const_iterator end(mHandlers.constEnd());
    for (HandlerList::const_iterator it = mHandlers.constBegin(); it != end; ++it) {
        const QString msg = (*it)->statusBarMessage(url, w);
        if (!msg.isEmpty()) {
            return msg;
        }
    }
    return QString();
}

//
//
// URLHandler
//
//

bool KMailProtocolURLHandler::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    if (url.scheme() == QLatin1String("kmail")) {
        if (!w) {
            return false;
        }
        const QString urlPath(url.path());
        if (urlPath == QLatin1String("showHTML")) {
            w->setDisplayFormatMessageOverwrite(MessageViewer::Viewer::Html);
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1String("goOnline")) {
            w->goOnline();
            return true;
        } else if (urlPath == QLatin1String("goResourceOnline")) {
            w->goResourceOnline();
            return true;
        } else if (urlPath == QLatin1String("loadExternal")) {
            w->setHtmlLoadExtOverride(!w->htmlLoadExtOverride());
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1String("decryptMessage")) {
            w->setDecryptMessageOverwrite(true);
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1String("showSignatureDetails")) {
            w->setShowSignatureDetails(true);
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1String("hideSignatureDetails")) {
            w->setShowSignatureDetails(false);
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1String("showEncryptionDetails")) {
            w->setHideEncryptionDetails(false);
            return true;
        } else if (urlPath == QLatin1String("hideEncryptionDetails")) {
            w->setHideEncryptionDetails(true);
            return true;
        }
    }
    return false;
}

QString KMailProtocolURLHandler::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    const QString schemeStr = url.scheme();
    if (schemeStr == QLatin1String("kmail")) {
        const QString urlPath(url.path());
        if (urlPath == QLatin1String("showHTML")) {
            return i18n("Turn on HTML rendering for this message.");
        } else if (urlPath == QLatin1String("loadExternal")) {
            return i18n("Load external references from the Internet for this message.");
        } else if (urlPath == QLatin1String("goOnline")) {
            return i18n("Work online.");
        } else if (urlPath == QLatin1String("goResourceOnline")) {
            return i18n("Make account online.");
        } else if (urlPath == QLatin1String("decryptMessage")) {
            return i18n("Decrypt message.");
        } else if (urlPath == QLatin1String("showSignatureDetails")) {
            return i18n("Show signature details.");
        } else if (urlPath == QLatin1String("hideSignatureDetails")) {
            return i18n("Hide signature details.");
        } else if (urlPath == QLatin1String("showEncryptionDetails")) {
            return i18n("Show encryption details.");
        } else if (urlPath == QLatin1String("hideEncryptionDetails")) {
            return i18n("Hide encryption details.");
        } else {
            return QString();
        }
    } else if (schemeStr == QLatin1String("help")) {
        return i18n("Open Documentation");
    }
    return QString();
}

bool ExpandCollapseQuoteURLManager::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    //  kmail:levelquote/?num      -> the level quote to collapse.
    //  kmail:levelquote/?-num      -> expand all levels quote.
    if (url.scheme() == QLatin1String("kmail") && url.path() == QLatin1String("levelquote")) {
        const QString levelStr = url.query();
        bool isNumber = false;
        const int levelQuote = levelStr.toInt(&isNumber);
        if (isNumber) {
            w->slotLevelQuote(levelQuote);
        }
        return true;
    }
    return false;
}

bool ExpandCollapseQuoteURLManager::handleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(url);
    Q_UNUSED(window);
    return false;
}

QString ExpandCollapseQuoteURLManager::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    if (url.scheme() == QLatin1String("kmail") && url.path() == QLatin1String("levelquote")) {
        const QString query = url.query();
        if (query.length() >= 1) {
            if (query[ 0 ] == QLatin1Char('-')) {
                return i18n("Expand all quoted text.");
            } else {
                return i18n("Collapse quoted text.");
            }
        }
    }
    return QString();
}

bool foundSMIMEData(const QString &aUrl, QString &displayName, QString &libName, QString &keyId)
{
    static QString showCertMan(QStringLiteral("showCertificate#"));
    displayName.clear();
    libName.clear();
    keyId.clear();
    int i1 = aUrl.indexOf(showCertMan);
    if (-1 < i1) {
        i1 += showCertMan.length();
        int i2 = aUrl.indexOf(QLatin1String(" ### "), i1);
        if (i1 < i2) {
            displayName = aUrl.mid(i1, i2 - i1);
            i1 = i2 + 5;
            i2 = aUrl.indexOf(QLatin1String(" ### "), i1);
            if (i1 < i2) {
                libName = aUrl.mid(i1, i2 - i1);
                i2 += 5;

                keyId = aUrl.mid(i2);
                /*
                int len = aUrl.length();
                if( len > i2+1 ) {
                keyId = aUrl.mid( i2, 2 );
                i2 += 2;
                while( len > i2+1 ) {
                keyId += ':';
                keyId += aUrl.mid( i2, 2 );
                i2 += 2;
                }
                }
                */
            }
        }
    }
    return !keyId.isEmpty();
}

bool SMimeURLHandler::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    if (!url.hasFragment()) {
        return false;
    }
    QString displayName, libName, keyId;
    if (!foundSMIMEData(url.path() + QLatin1Char('#')
                        +QUrl::fromPercentEncoding(url.fragment().toLatin1()),
                        displayName, libName, keyId)) {
        return false;
    }
    QStringList lst;
    lst << QStringLiteral("--parent-windowid")
        << QString::number((qlonglong)w->viewer()->mainWindow()->winId())
        << QStringLiteral("--query") << keyId;
    if (!QProcess::startDetached(QStringLiteral("kleopatra"), lst)) {
        KMessageBox::error(w->mMainWindow, i18n("Could not start certificate manager. "
                                                "Please check your installation."),
                           i18n("KMail Error"));
    }
    return true;
}

QString SMimeURLHandler::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    QString displayName, libName, keyId;
    if (!foundSMIMEData(url.path() + QLatin1Char('#')
                        +QUrl::fromPercentEncoding(url.fragment().toLatin1()),
                        displayName, libName, keyId)) {
        return QString();
    }
    return i18n("Show certificate 0x%1", keyId);
}

bool HtmlAnchorHandler::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    if (!url.host().isEmpty() || !url.hasFragment()) {
        return false;
    }

    w->scrollToAnchor(url.fragment());
    return true;
}

QString MailToURLHandler::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    if (url.scheme() == QLatin1String("mailto")) {
        return KEmailAddress::decodeMailtoUrl(url);
    }
    return QString();
}

static QString searchFullEmailByUid(const QString &uid)
{
    QString fullEmail;
    Akonadi::ContactSearchJob *job = new Akonadi::ContactSearchJob();
    job->setLimit(1);
    job->setQuery(Akonadi::ContactSearchJob::ContactUid, uid,
                  Akonadi::ContactSearchJob::ExactMatch);
    job->exec();
    const KContacts::Addressee::List res = job->contacts();
    if (!res.isEmpty()) {
        KContacts::Addressee addr = res.at(0);
        fullEmail = addr.fullEmail();
    }
    return fullEmail;
}

static void runKAddressBook(const QUrl &url)
{
    KPIM::OpenEmailAddressJob *job = new KPIM::OpenEmailAddressJob(url.path(), nullptr);
    job->start();
}

bool ContactUidURLHandler::handleClick(const QUrl &url, ViewerPrivate *) const
{
    if (url.scheme() == QLatin1String("uid")) {
        runKAddressBook(url);
        return true;
    } else {
        return false;
    }
}

bool ContactUidURLHandler::handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *) const
{
    if (url.scheme() != QLatin1String("uid") || url.path().isEmpty()) {
        return false;
    }

    QMenu menu;
    QAction *open
        = menu.addAction(QIcon::fromTheme(QStringLiteral("view-pim-contacts")),
                         i18n("&Open in Address Book"));
#ifndef QT_NO_CLIPBOARD
    QAction *copy
        = menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")),
                         i18n("&Copy Email Address"));
#endif

    QAction *a = menu.exec(p);
    if (a == open) {
        runKAddressBook(url);
#ifndef QT_NO_CLIPBOARD
    } else if (a == copy) {
        const QString fullEmail = searchFullEmailByUid(url.path());
        if (!fullEmail.isEmpty()) {
            QClipboard *clip = QApplication::clipboard();
            clip->setText(fullEmail, QClipboard::Clipboard);
            clip->setText(fullEmail, QClipboard::Selection);
            KPIM::BroadcastStatus::instance()->setStatusMsg(i18n("Address copied to clipboard."));
        }
#endif
    }

    return true;
}

QString ContactUidURLHandler::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    if (url.scheme() == QLatin1String("uid")) {
        return i18n("Lookup the contact in KAddressbook");
    } else {
        return QString();
    }
}

KMime::Content *AttachmentURLHandler::nodeForUrl(const QUrl &url, ViewerPrivate *w) const
{
    if (!w || !w->mMessage) {
        return nullptr;
    }
    if (url.scheme() == QLatin1String("attachment")) {
        KMime::Content *node = w->nodeFromUrl(url);
        return node;
    }
    return nullptr;
}

bool AttachmentURLHandler::attachmentIsInHeader(const QUrl &url) const
{
    bool inHeader = false;
    QUrlQuery query(url);
    const QString place = query.queryItemValue(QStringLiteral("place")).toLower();
    if (!place.isNull()) {
        inHeader = (place == QLatin1String("header"));
    }
    return inHeader;
}

bool AttachmentURLHandler::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    KMime::Content *node = nodeForUrl(url, w);
    if (!node) {
        return false;
    }
    const bool inHeader = attachmentIsInHeader(url);
    const bool shouldShowDialog = !w->nodeHelper()->isNodeDisplayedEmbedded(node) || !inHeader;
    if (inHeader) {
        w->scrollToAttachment(node);
    }
    if (shouldShowDialog || w->nodeHelper()->isNodeDisplayedHidden(node)) {
        w->openAttachment(node, w->nodeHelper()->tempFileUrlFromNode(node));
    }

    return true;
}

bool AttachmentURLHandler::handleShiftClick(const QUrl &url, ViewerPrivate *window) const
{
    KMime::Content *node = nodeForUrl(url, window);
    if (!node) {
        return false;
    }
    if (!window) {
        return false;
    }
    if (node->contentType()->mimeType() == "text/x-moz-deleted") {
        return false;
    }

    const bool isEncapsulatedMessage = node->parent() && node->parent()->bodyIsMessage();
    if (isEncapsulatedMessage) {
        KMime::Message::Ptr message = KMime::Message::Ptr(new KMime::Message);
        message->setContent(node->parent()->bodyAsMessage()->encodedContent());
        message->parse();
        Akonadi::Item item;
        item.setPayload<KMime::Message::Ptr>(message);
        Akonadi::MessageFlags::copyMessageFlags(*message, item);
        item.setMimeType(KMime::Message::mimeType());
        QUrl url;
        if (MessageViewer::Util::saveMessageInMboxAndGetUrl(url, Akonadi::Item::List() << item, window->viewer())) {
            window->viewer()->showOpenAttachmentFolderWidget(QList<QUrl>() << url);
        }
    } else {
        QList<QUrl> urlList;
        if (Util::saveContents(window->viewer(), KMime::Content::List() << node, urlList)) {
            window->viewer()->showOpenAttachmentFolderWidget(urlList);
        }
    }

    return true;
}

bool AttachmentURLHandler::willHandleDrag(const QUrl &url, ViewerPrivate *window) const
{
    return nodeForUrl(url, window) != nullptr;
}

bool AttachmentURLHandler::handleDrag(const QUrl &url, ViewerPrivate *window) const
{
#ifndef QT_NO_DRAGANDDROP
    KMime::Content *node = nodeForUrl(url, window);
    if (!node) {
        return false;
    }
    if (node->contentType()->mimeType() == "text/x-moz-deleted") {
        return false;
    }
    QString fileName;
    QUrl tUrl;
    const bool isEncapsulatedMessage = node->parent() && node->parent()->bodyIsMessage();
    if (isEncapsulatedMessage) {
        KMime::Message::Ptr message = KMime::Message::Ptr(new KMime::Message);
        message->setContent(node->parent()->bodyAsMessage()->encodedContent());
        message->parse();
        Akonadi::Item item;
        item.setPayload<KMime::Message::Ptr>(message);
        Akonadi::MessageFlags::copyMessageFlags(*message, item);
        item.setMimeType(KMime::Message::mimeType());
        fileName = window->nodeHelper()->writeFileToTempFile(node, Util::generateMboxFileName(item));

        KMBox::MBox mbox;
        QFile::remove(fileName);

        if (!mbox.load(fileName)) {
            qCWarning(MESSAGEVIEWER_LOG) << "MBOX: Impossible to open file";
            return false;
        }
        mbox.appendMessage(item.payload<KMime::Message::Ptr>());

        if (!mbox.save()) {
            qCWarning(MESSAGEVIEWER_LOG) << "MBOX: Impossible to save file";
            return false;
        }
        tUrl = QUrl::fromLocalFile(fileName);
    } else {
        if (node->header<KMime::Headers::Subject>()) {
            if (!node->contents().isEmpty()) {
                node = node->contents().constLast();
                fileName = window->nodeHelper()->writeNodeToTempFile(node);
                tUrl = QUrl::fromLocalFile(fileName);
            }
        }
        if (fileName.isEmpty()) {
            tUrl = window->nodeHelper()->tempFileUrlFromNode(node);
            fileName = tUrl.path();
        }
    }
    if (!fileName.isEmpty()) {
        QFile f(fileName);
        f.setPermissions(
            QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::ReadGroup
            | QFile::ReadOther);
        const QString icon = Util::iconPathForContent(node, KIconLoader::Small);
        QDrag *drag = new QDrag(window->viewer());
        QMimeData *mimeData = new QMimeData();
        mimeData->setUrls(QList<QUrl>() << tUrl);
        drag->setMimeData(mimeData);
        if (!icon.isEmpty()) {
            drag->setPixmap(QIcon::fromTheme(icon).pixmap(16, 16));
        }
        drag->start();
        return true;
    } else
#endif
    return false;
}

bool AttachmentURLHandler::handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *w) const
{
    KMime::Content *node = nodeForUrl(url, w);
    if (!node) {
        return false;
    }
    // PENDING(romain_kdab) : replace with toLocalFile() ?
    w->showAttachmentPopup(node, w->nodeHelper()->tempFileUrlFromNode(node).path(), p);
    return true;
}

QString AttachmentURLHandler::statusBarMessage(const QUrl &url, ViewerPrivate *w) const
{
    KMime::Content *node = nodeForUrl(url, w);
    if (!node) {
        return QString();
    }
    const QString name = MimeTreeParser::NodeHelper::fileName(node);
    if (!name.isEmpty()) {
        return i18n("Attachment: %1", name);
    } else if (dynamic_cast<KMime::Message *>(node)) {
        if (node->header<KMime::Headers::Subject>()) {
            return i18n("Encapsulated Message (Subject: %1)",
                        node->header<KMime::Headers::Subject>()->asUnicodeString());
        } else {
            return i18n("Encapsulated Message");
        }
    }
    return i18n("Unnamed attachment");
}

static QString extractAuditLog(const QUrl &url)
{
    if (url.scheme() != QLatin1String("kmail")
        || url.path() != QLatin1String("showAuditLog")) {
        return QString();
    }
    QUrlQuery query(url);
    Q_ASSERT(!query.queryItemValue(QStringLiteral("log")).isEmpty());
    return query.queryItemValue(QStringLiteral("log"));
}

bool ShowAuditLogURLHandler::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    const QString auditLog = extractAuditLog(url);
    if (auditLog.isEmpty()) {
        return false;
    }
    Kleo::MessageBox::auditLog(w->mMainWindow, auditLog);
    return true;
}

bool ShowAuditLogURLHandler::handleContextMenuRequest(const QUrl &url, const QPoint &, ViewerPrivate *w) const
{
    Q_UNUSED(w);
    // disable RMB for my own links:
    return !extractAuditLog(url).isEmpty();
}

QString ShowAuditLogURLHandler::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    if (extractAuditLog(url).isEmpty()) {
        return QString();
    } else {
        return i18n("Show GnuPG Audit Log for this operation");
    }
}

bool ShowAuditLogURLHandler::handleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(url);
    Q_UNUSED(window);
    return true;
}

bool InternalImageURLHandler::handleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(window);
    Q_UNUSED(url);

    // This will only be called when willHandleDrag() was true. Return false here, that will
    // notify ViewerPrivate::eventFilter() that no drag was started.
    return false;
}

bool InternalImageURLHandler::willHandleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(window);
    if (url.scheme() == QLatin1String("data") && url.path().startsWith(QLatin1String("image"))) {
        return true;
    }

    const QString imagePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral(
                                                         "libmessageviewer/pics/"),
                                                     QStandardPaths::LocateDirectory);
    return url.path().contains(imagePath);
}

bool KRunURLHandler::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    const QString scheme(url.scheme());
    if ((scheme == QLatin1String("http")) || (scheme == QLatin1String("https"))
        || (scheme == QLatin1String("ftp")) || (scheme == QLatin1String("file"))
        || (scheme == QLatin1String("ftps")) || (scheme == QLatin1String("sftp"))
        || (scheme == QLatin1String("help")) || (scheme == QLatin1String("vnc"))
        || (scheme == QLatin1String("smb")) || (scheme == QLatin1String("fish"))
        || (scheme == QLatin1String("news"))) {
        KPIM::BroadcastStatus::instance()->setTransientStatusMsg(i18n("Opening URL..."));
        QTimer::singleShot(2000, KPIM::BroadcastStatus::instance(), &KPIM::BroadcastStatus::reset);

        QMimeDatabase mimeDb;
        auto mime = mimeDb.mimeTypeForUrl(url);
        if (mime.name() == QLatin1String("application/x-desktop")
            || mime.name() == QLatin1String("application/x-executable")
            || mime.name() == QLatin1String("application/x-ms-dos-executable")
            || mime.name() == QLatin1String("application/x-shellscript")) {
            if (KMessageBox::warningYesNo(nullptr,
                                          xi18nc("@info",
                                                 "Do you really want to execute <filename>%1</filename>?",
                                                 url.toDisplayString(QUrl::PreferLocalFile)),
                                          QString(), KGuiItem(i18n("Execute")),
                                          KStandardGuiItem::cancel()) != KMessageBox::Yes) {
                return true;
            }
        }
        w->checkPhishingUrl();
        return true;
    } else {
        return false;
    }
}
