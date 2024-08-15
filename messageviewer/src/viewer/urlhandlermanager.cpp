/*  -*- c++ -*-
    urlhandlermanager.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2002-2003, 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "urlhandlermanager.h"
#include "../utils/messageviewerutil_p.h"
#include "interfaces/bodyparturlhandler.h"
#include "messageviewer/messageviewerutil.h"
#include "messageviewer_debug.h"
#include "stl_util.h"
#include "urlhandlermanager_p.h"
#include "utils/mimetype.h"
#include "viewer/viewer_p.h"

#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/PartNodeBodyPart>

#include <Akonadi/OpenEmailAddressJob>
#include <MessageCore/StringUtil>
#include <PimCommon/BroadcastStatus>

#include <Akonadi/ContactSearchJob>

#include <Akonadi/MessageFlags>
#include <KEmailAddress>
#include <KMbox/MBox>
#include <KMime/Content>

#include <KIconLoader>
#include <KLocalizedString>
#include <KMessageBox>

#include <QApplication>
#include <QClipboard>
#include <QDrag>
#include <QFile>
#include <QIcon>
#include <QMenu>
#include <QMimeData>
#include <QMimeDatabase>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>
#include <QUrlQuery>

#include <algorithm>

#include <Libkleo/AuditLogEntry>
#include <Libkleo/AuditLogViewer>
#include <chrono>

using namespace std::chrono_literals;

using std::for_each;
using std::remove;
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
    for_each(mHandlers.begin(), mHandlers.end(), [](QList<const Interface::BodyPartURLHandler *> &handlers) {
        for_each(handlers.begin(), handlers.end(), DeleteAndSetToZero<Interface::BodyPartURLHandler>());
    });
}

void BodyPartURLHandlerManager::registerHandler(const Interface::BodyPartURLHandler *handler, const QString &mimeType)
{
    if (!handler) {
        return;
    }
    unregisterHandler(handler); // don't produce duplicates
    const auto mt = mimeType.toLatin1();
    auto it = mHandlers.find(mt);
    if (it == mHandlers.end()) {
        it = mHandlers.insert(mt, {});
    }
    it->push_back(handler);
}

void BodyPartURLHandlerManager::unregisterHandler(const Interface::BodyPartURLHandler *handler)
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

    if (!w || url.scheme() != QLatin1StringView("x-kmail")) {
        return nullptr;
    }
    const QString urlPath = url.path();

    // urlPath format is: /bodypart/<random number>/<part id>/<path>

    qCDebug(MESSAGEVIEWER_LOG) << "BodyPartURLHandler: urlPath ==" << urlPath;
    if (!urlPath.startsWith(QLatin1StringView("/bodypart/"))) {
        return nullptr;
    }

    const QStringList urlParts = urlPath.mid(10).split(QLatin1Char('/'));
    if (urlParts.size() != 3) {
        return nullptr;
    }
    // KMime::ContentIndex index( urlParts[1] );
    QByteArray query(urlParts.at(2).toLatin1());
    if (url.hasQuery()) {
        query += "?" + url.query().toLatin1();
    }
    *path = QUrl::fromPercentEncoding(query);
    return w->nodeFromUrl(QUrl(urlParts.at(1)));
}

QList<const Interface::BodyPartURLHandler *> BodyPartURLHandlerManager::handlersForPart(KMime::Content *node) const
{
    if (auto ct = node->contentType(false)) {
        auto mimeType = ct->mimeType();
        if (!mimeType.isEmpty()) {
            // Bug 390900
            if (mimeType == "text/x-vcard") {
                mimeType = "text/vcard";
            }
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

    MimeTreeParser::PartNodeBodyPart part(nullptr, nullptr, w->message().data(), node, w->nodeHelper());

    for (const auto &handlers : {handlersForPart(node), mHandlers.value({})}) {
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

    MimeTreeParser::PartNodeBodyPart part(nullptr, nullptr, w->message().data(), node, w->nodeHelper());

    for (const auto &handlers : {handlersForPart(node), mHandlers.value({})}) {
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
        return {};
    }

    MimeTreeParser::PartNodeBodyPart part(nullptr, nullptr, w->message().data(), node, w->nodeHelper());

    for (const auto &handlers : {handlersForPart(node), mHandlers.value({})}) {
        for (auto it = handlers.cbegin(), end = handlers.cend(); it != end; ++it) {
            const QString msg = (*it)->statusBarMessage(&part, path);
            if (!msg.isEmpty()) {
                return msg;
            }
        }
    }
    return {};
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
    registerHandler(new EmbeddedImageURLHandler());
}

URLHandlerManager::~URLHandlerManager()
{
    for_each(mHandlers.begin(), mHandlers.end(), DeleteAndSetToZero<MimeTreeParser::URLHandler>());
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
    unregisterHandler(handler); // don't produce duplicates
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
    return {};
}

//
//
// URLHandler
//
//

bool KMailProtocolURLHandler::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    if (url.scheme() == QLatin1StringView("kmail")) {
        if (!w) {
            return false;
        }
        const QString urlPath(url.path());
        if (urlPath == QLatin1StringView("showHTML")) {
            w->setDisplayFormatMessageOverwrite(MessageViewer::Viewer::Html);
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1StringView("goOnline")) {
            w->goOnline();
            return true;
        } else if (urlPath == QLatin1StringView("goResourceOnline")) {
            w->goResourceOnline();
            return true;
        } else if (urlPath == QLatin1StringView("loadExternal")) {
            w->setHtmlLoadExtOverride(!w->htmlLoadExtOverride());
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1StringView("decryptMessage")) {
            w->setDecryptMessageOverwrite(true);
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1StringView("showSignatureDetails")) {
            w->setShowSignatureDetails(true);
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1StringView("hideSignatureDetails")) {
            w->setShowSignatureDetails(false);
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1StringView("showEncryptionDetails")) {
            w->setShowEncryptionDetails(true);
            w->update(MimeTreeParser::Force);
            return true;
        } else if (urlPath == QLatin1StringView("hideEncryptionDetails")) {
            w->setShowEncryptionDetails(false);
            w->update(MimeTreeParser::Force);
            return true;
        }
    }
    return false;
}

QString KMailProtocolURLHandler::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    const QString schemeStr = url.scheme();
    if (schemeStr == QLatin1StringView("kmail")) {
        const QString urlPath(url.path());
        if (urlPath == QLatin1StringView("showHTML")) {
            return i18n("Turn on HTML rendering for this message.");
        } else if (urlPath == QLatin1StringView("loadExternal")) {
            return i18n("Load external references from the Internet for this message.");
        } else if (urlPath == QLatin1StringView("goOnline")) {
            return i18n("Work online.");
        } else if (urlPath == QLatin1StringView("goResourceOnline")) {
            return i18n("Make account online.");
        } else if (urlPath == QLatin1StringView("decryptMessage")) {
            return i18n("Decrypt message.");
        } else if (urlPath == QLatin1StringView("showSignatureDetails")) {
            return i18n("Show signature details.");
        } else if (urlPath == QLatin1StringView("hideSignatureDetails")) {
            return i18n("Hide signature details.");
        } else if (urlPath == QLatin1StringView("showEncryptionDetails")) {
            return i18n("Show encryption details.");
        } else if (urlPath == QLatin1StringView("hideEncryptionDetails")) {
            return i18n("Hide encryption details.");
        } else {
            return {};
        }
    } else if (schemeStr == QLatin1StringView("help")) {
        return i18n("Open Documentation");
    }
    return {};
}

bool ExpandCollapseQuoteURLManager::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    //  kmail:levelquote/?num      -> the level quote to collapse.
    //  kmail:levelquote/?-num      -> expand all levels quote.
    if (url.scheme() == QLatin1StringView("kmail") && url.path() == QLatin1StringView("levelquote")) {
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
    Q_UNUSED(url)
    Q_UNUSED(window)
    return false;
}

QString ExpandCollapseQuoteURLManager::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    if (url.scheme() == QLatin1StringView("kmail") && url.path() == QLatin1StringView("levelquote")) {
        const QString query = url.query();
        if (query.length() >= 1) {
            if (query[0] == QLatin1Char('-')) {
                return i18n("Expand all quoted text.");
            } else {
                return i18n("Collapse quoted text.");
            }
        }
    }
    return {};
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
        int i2 = aUrl.indexOf(QLatin1StringView(" ### "), i1);
        if (i1 < i2) {
            displayName = aUrl.mid(i1, i2 - i1);
            i1 = i2 + 5;
            i2 = aUrl.indexOf(QLatin1StringView(" ### "), i1);
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
    QString displayName;
    QString libName;
    QString keyId;
    if (!foundSMIMEData(url.path() + QLatin1Char('#') + QUrl::fromPercentEncoding(url.fragment().toLatin1()), displayName, libName, keyId)) {
        return false;
    }
    QStringList lst;
    lst << QStringLiteral("--parent-windowid") << QString::number(static_cast<qlonglong>(w->viewer()->mainWindow()->winId())) << QStringLiteral("--query")
        << keyId;
#ifdef Q_OS_WIN
    QString exec = QStandardPaths::findExecutable(QStringLiteral("kleopatra.exe"), {QCoreApplication::applicationDirPath()});
    if (exec.isEmpty()) {
        exec = QStandardPaths::findExecutable(QStringLiteral("kleopatra.exe"));
    }
#else
    const QString exec = QStandardPaths::findExecutable(QStringLiteral("kleopatra"));
#endif
    if (exec.isEmpty()) {
        qCWarning(MESSAGEVIEWER_LOG) << "Could not find kleopatra executable in PATH";
        KMessageBox::error(w->mMainWindow,
                           i18n("Could not start certificate manager. "
                                "Please check your installation."),
                           i18n("KMail Error"));
        return false;
    }
    QProcess::startDetached(exec, lst);
    return true;
}

QString SMimeURLHandler::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    QString displayName;
    QString libName;
    QString keyId;
    if (!foundSMIMEData(url.path() + QLatin1Char('#') + QUrl::fromPercentEncoding(url.fragment().toLatin1()), displayName, libName, keyId)) {
        return {};
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
    if (url.scheme() == QLatin1StringView("mailto")) {
        return KEmailAddress::decodeMailtoUrl(url);
    }
    return {};
}

static QString searchFullEmailByUid(const QString &uid)
{
    QString fullEmail;
    auto job = new Akonadi::ContactSearchJob();
    job->setLimit(1);
    job->setQuery(Akonadi::ContactSearchJob::ContactUid, uid, Akonadi::ContactSearchJob::ExactMatch);
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
    auto job = new Akonadi::OpenEmailAddressJob(url.path(), nullptr);
    job->start();
}

bool ContactUidURLHandler::handleClick(const QUrl &url, ViewerPrivate *) const
{
    if (url.scheme() == QLatin1StringView("uid")) {
        runKAddressBook(url);
        return true;
    } else {
        return false;
    }
}

bool ContactUidURLHandler::handleContextMenuRequest(const QUrl &url, const QPoint &p, ViewerPrivate *) const
{
    if (url.scheme() != QLatin1StringView("uid") || url.path().isEmpty()) {
        return false;
    }

    QMenu menu;
    QAction *open = menu.addAction(QIcon::fromTheme(QStringLiteral("view-pim-contacts")), i18n("&Open in Address Book"));
#ifndef QT_NO_CLIPBOARD
    QAction *copy = menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18n("&Copy Email Address"));
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
            PimCommon::BroadcastStatus::instance()->setStatusMsg(i18n("Address copied to clipboard."));
        }
#endif
    }

    return true;
}

QString ContactUidURLHandler::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    if (url.scheme() == QLatin1StringView("uid")) {
        return i18n("Lookup the contact in KAddressbook");
    } else {
        return {};
    }
}

KMime::Content *AttachmentURLHandler::nodeForUrl(const QUrl &url, ViewerPrivate *w) const
{
    if (!w || !w->mMessage) {
        return nullptr;
    }
    if (url.scheme() == QLatin1StringView("attachment")) {
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
        inHeader = (place == QLatin1StringView("header"));
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
    // if (shouldShowDialog || w->nodeHelper()->isNodeDisplayedHidden(node)) {
    w->openAttachment(node, w->nodeHelper()->tempFileUrlFromNode(node));
    //}

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
        KMime::Message::Ptr message(new KMime::Message);
        message->setContent(node->parent()->bodyAsMessage()->encodedContent());
        message->parse();
        Akonadi::Item item;
        item.setPayload<KMime::Message::Ptr>(message);
        Akonadi::MessageFlags::copyMessageFlags(*message, item);
        item.setMimeType(KMime::Message::mimeType());
        QUrl newUrl;
        if (MessageViewer::Util::saveMessageInMboxAndGetUrl(newUrl, Akonadi::Item::List() << item, window->viewer())) {
            window->viewer()->showOpenAttachmentFolderWidget(QList<QUrl>() << newUrl);
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
        KMime::Message::Ptr message(new KMime::Message);
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
        f.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);
        const QString icon = Util::iconPathForContent(node, KIconLoader::Small);
        auto drag = new QDrag(window->viewer());
        auto mimeData = new QMimeData();
        mimeData->setUrls(QList<QUrl>() << tUrl);
        drag->setMimeData(mimeData);
        if (!icon.isEmpty()) {
            drag->setPixmap(QIcon::fromTheme(icon).pixmap(16, 16));
        }
        drag->exec();
        return true;
    } else {
#endif
        return false;
    }
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
        return {};
    }
    const QString name = MimeTreeParser::NodeHelper::fileName(node);
    if (!name.isEmpty()) {
        return i18n("Attachment: %1", name);
    } else if (dynamic_cast<KMime::Message *>(node)) {
        if (node->header<KMime::Headers::Subject>()) {
            return i18n("Encapsulated Message (Subject: %1)", node->header<KMime::Headers::Subject>()->asUnicodeString());
        } else {
            return i18n("Encapsulated Message");
        }
    }
    return i18n("Unnamed attachment");
}

static QString extractAuditLog(const QUrl &url)
{
    if (url.scheme() != QLatin1StringView("kmail") || url.path() != QLatin1StringView("showAuditLog")) {
        return {};
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
    Kleo::AuditLogViewer::showAuditLog(w->mMainWindow, Kleo::AuditLogEntry{auditLog, GpgME::Error{}}, auditLog);
    return true;
}

bool ShowAuditLogURLHandler::handleContextMenuRequest(const QUrl &url, const QPoint &, ViewerPrivate *w) const
{
    Q_UNUSED(w)
    // disable RMB for my own links:
    return !extractAuditLog(url).isEmpty();
}

QString ShowAuditLogURLHandler::statusBarMessage(const QUrl &url, ViewerPrivate *) const
{
    if (extractAuditLog(url).isEmpty()) {
        return {};
    } else {
        return i18n("Show GnuPG Audit Log for this operation");
    }
}

bool ShowAuditLogURLHandler::handleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(url)
    Q_UNUSED(window)
    return true;
}

bool InternalImageURLHandler::handleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(window)
    Q_UNUSED(url)

    // This will only be called when willHandleDrag() was true. Return false here, that will
    // notify ViewerPrivate::eventFilter() that no drag was started.
    return false;
}

bool InternalImageURLHandler::willHandleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(window)
    if (url.scheme() == QLatin1StringView("data") && url.path().startsWith(QLatin1StringView("image"))) {
        return true;
    }

    const QString imagePath =
        QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("libmessageviewer/pics/"), QStandardPaths::LocateDirectory);
    return url.path().contains(imagePath);
}

bool KRunURLHandler::handleClick(const QUrl &url, ViewerPrivate *w) const
{
    const QString scheme(url.scheme());
    if ((scheme == QLatin1StringView("http")) || (scheme == QLatin1StringView("https")) || (scheme == QLatin1StringView("ftp"))
        || (scheme == QLatin1StringView("file")) || (scheme == QLatin1StringView("ftps")) || (scheme == QLatin1StringView("sftp"))
        || (scheme == QLatin1StringView("help")) || (scheme == QLatin1StringView("vnc")) || (scheme == QLatin1StringView("smb"))
        || (scheme == QLatin1StringView("fish")) || (scheme == QLatin1StringView("news")) || (scheme == QLatin1StringView("tel"))
        || (scheme == QLatin1StringView("geo")) || (scheme == QLatin1StringView("sms"))) {
        PimCommon::BroadcastStatus::instance()->setTransientStatusMsg(i18n("Opening URL..."));
        QTimer::singleShot(2s, PimCommon::BroadcastStatus::instance(), &PimCommon::BroadcastStatus::reset);

        QMimeDatabase mimeDb;
        auto mime = mimeDb.mimeTypeForUrl(url);
        if (mime.name() == QLatin1StringView("application/x-desktop") || mime.name() == QLatin1StringView("application/x-executable")
            || mime.name() == QLatin1StringView("application/x-ms-dos-executable") || mime.name() == QLatin1StringView("application/x-shellscript")) {
            if (KMessageBox::warningTwoActions(
                    nullptr,
                    xi18nc("@info", "Do you really want to execute <filename>%1</filename>?", url.toDisplayString(QUrl::PreferLocalFile)),
                    QString(),
                    KGuiItem(i18nc("@action:button", "Execute")),
                    KStandardGuiItem::cancel())
                != KMessageBox::ButtonCode::PrimaryAction) {
                return true;
            }
        }
        w->checkPhishingUrl();
        return true;
    } else {
        return false;
    }
}

bool EmbeddedImageURLHandler::handleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(url)
    Q_UNUSED(window)
    return false;
}

bool EmbeddedImageURLHandler::willHandleDrag(const QUrl &url, ViewerPrivate *window) const
{
    Q_UNUSED(window)
    return url.scheme() == QLatin1StringView("cid");
}
