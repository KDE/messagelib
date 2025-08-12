/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "defaultrenderer.h"
using namespace Qt::Literals::StringLiterals;

#include "defaultrenderer_p.h"

#include "utils/messageviewerutil.h"

#include "messageviewer_debug.h"

#include "converthtmltoplaintext.h"
#include "htmlblock.h"
#include "messagepartrendererbase.h"
#include "messagepartrendererfactory.h"
#include "messagepartrenderermanager.h"
#include "utils/iconnamecache.h"
#include "viewer/attachmentstrategy.h"
#include "viewer/csshelperbase.h"

#include "htmlwriter/bufferedhtmlwriter.h"
#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/ObjectTreeParser>

#include <Libkleo/Formatting>
#include <QGpgME/Protocol>

#include <Libkleo/Formatting>
#include <MessageCore/StringUtil>

#include <KEmailAddress>
#include <KIconLoader>
#include <KLocalizedString>

#include <KTextTemplate/Context>
#include <KTextTemplate/Engine>
#include <KTextTemplate/MetaType>
#include <KTextTemplate/Template>
#include <KTextTemplate/TemplateLoader>
#include <QUrl>

using namespace MimeTreeParser;
using namespace MessageViewer;
#ifndef COMPILE_WITH_UNITY_CMAKE_SUPPORT
Q_DECLARE_METATYPE(GpgME::DecryptionResult::Recipient)
Q_DECLARE_METATYPE(GpgME::Key)
Q_DECLARE_METATYPE(const QGpgME::Protocol *)
#endif

DefaultRendererPrivate::DefaultRendererPrivate(CSSHelperBase *cssHelper, const MessagePartRendererFactory *rendererFactory)
    : mCSSHelper(cssHelper)
    , mRendererFactory(rendererFactory)
{
}

DefaultRendererPrivate::~DefaultRendererPrivate() = default;

CSSHelperBase *DefaultRendererPrivate::cssHelper() const
{
    return mCSSHelper;
}

Interface::ObjectTreeSource *DefaultRendererPrivate::source() const
{
    return mMsgPart->source();
}

void DefaultRendererPrivate::renderSubParts(const MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter)
{
    for (const auto &m : msgPart->subParts()) {
        renderFactory(m, htmlWriter);
    }
}

void DefaultRendererPrivate::render(const MessagePartList::Ptr &mp, HtmlWriter *htmlWriter)
{
    HTMLBlock::Ptr rBlock;
    HTMLBlock::Ptr aBlock;

    if (mp->isRoot()) {
        rBlock = HTMLBlock::Ptr(new RootBlock(htmlWriter));
    }

    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }

    renderSubParts(mp, htmlWriter);
}

void DefaultRendererPrivate::render(const MimeMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    HTMLBlock::Ptr aBlock;
    HTMLBlock::Ptr rBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }
    if (mp->isRoot()) {
        rBlock = HTMLBlock::Ptr(new RootBlock(htmlWriter));
    }

    renderSubParts(mp, htmlWriter);
}

void DefaultRendererPrivate::render(const EncapsulatedRfc822MessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    if (!mp->hasSubParts()) {
        return;
    }
    KTextTemplate::Template t = MessagePartRendererManager::self()->loadByName(u"encapsulatedrfc822messagepart.html"_s);
    KTextTemplate::Context c = MessagePartRendererManager::self()->createContext();
    QObject block;

    c.insert(u"block"_s, &block);
    block.setProperty("link", mp->nodeHelper()->asHREF(mp->message().data(), u"body"_s));

    c.insert(u"msgHeader"_s, mCreateMessageHeader(mp->message().data()));
    c.insert(u"content"_s, QVariant::fromValue<KTextTemplateCallback>([this, mp, htmlWriter](KTextTemplate::OutputStream *) {
                 renderSubParts(mp, htmlWriter);
             }));
    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }
    KTextTemplate::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
}

void DefaultRendererPrivate::render(const HtmlMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    KTextTemplate::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(u"htmlmessagepart.html"_s);
    KTextTemplate::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
    QObject block;

    c.insert(u"block"_s, &block);

    auto preferredMode = mp->source()->preferredMode();
    const bool isHtmlPreferred = (preferredMode == MimeTreeParser::Util::Html) || (preferredMode == MimeTreeParser::Util::MultipartHtml);
    block.setProperty("htmlMail", isHtmlPreferred);
    block.setProperty("loadExternal", htmlLoadExternal());
    block.setProperty("isPrinting", isPrinting());
    {
        // laurent: FIXME port to async method webengine
        const Util::HtmlMessageInfo messageInfo = Util::processHtml(mp->bodyHtml());

        if (isHtmlPreferred) {
            mp->nodeHelper()->setNodeDisplayedEmbedded(mp->content(), true);
            htmlWriter->setExtraHead(messageInfo.extraHead);
            htmlWriter->setStyleBody(Util::parseBodyStyle(messageInfo.bodyStyle));
        }

        block.setProperty("containsExternalReferences", Util::containsExternalReferences(messageInfo.htmlSource, messageInfo.extraHead));
        c.insert(u"content"_s, messageInfo.htmlSource);
    }

    {
        ConvertHtmlToPlainText convert;
        convert.setHtmlString(mp->bodyHtml());
        QString plaintext = convert.generatePlainText();
        plaintext.replace(u'\n', u"<br>"_s);
        c.insert(u"plaintext"_s, plaintext);
    }
    mp->source()->setHtmlMode(MimeTreeParser::Util::Html,
                              QList<MimeTreeParser::Util::HtmlMode>() << MimeTreeParser::Util::Html << MimeTreeParser::Util::Normal);

    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }
    KTextTemplate::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
}

void DefaultRendererPrivate::renderEncrypted(const EncryptedMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    KMime::Content *node = mp->content();
    const auto metaData = *mp->partMetaData();
    KTextTemplate::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(u"encryptedmessagepart.html"_s);
    KTextTemplate::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
    QObject block;
    if (node || mp->hasSubParts()) {
        c.insert(u"content"_s, QVariant::fromValue<KTextTemplateCallback>([this, mp, htmlWriter](KTextTemplate::OutputStream *) {
                     HTMLBlock::Ptr rBlock;
                     if (mp->content() && mp->isRoot()) {
                         rBlock = HTMLBlock::Ptr(new RootBlock(htmlWriter));
                     }
                     renderSubParts(mp, htmlWriter);
                 }));
    } else if (!metaData.inProgress) {
        c.insert(u"content"_s, QVariant::fromValue<KTextTemplateCallback>([this, mp, htmlWriter](KTextTemplate::OutputStream *) {
                     renderWithFactory<MimeTreeParser::MessagePart>(mp, htmlWriter);
                 }));
    }
    c.insert(u"cryptoProto"_s, QVariant::fromValue(mp->cryptoProto()));
    if (!mp->decryptRecipients().empty()) {
        c.insert(u"decryptedRecipients"_s, QVariant::fromValue(mp->decryptRecipients()));
    }
    c.insert(u"block"_s, &block);

    block.setProperty("isPrinting", isPrinting());
    block.setProperty("detailHeader", showEncryptionDetails());
    block.setProperty("inProgress", metaData.inProgress);
    block.setProperty("isDecrypted", mp->decryptMessage());
    block.setProperty("isDecryptable", metaData.isDecryptable);
    block.setProperty("decryptIcon", QUrl::fromLocalFile(IconNameCache::instance()->iconPath(u"document-decrypt"_s, KIconLoader::Small)).url());
    block.setProperty("errorText", metaData.errorText);
    block.setProperty("noSecKey", mp->isNoSecKey());
    block.setProperty("isCompliant", metaData.isCompliant);
    block.setProperty("compliance", metaData.compliance);
    KTextTemplate::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
}

void DefaultRendererPrivate::renderSigned(const SignedMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    KMime::Content *node = mp->content();
    const auto metaData = *mp->partMetaData();
    auto cryptoProto = mp->cryptoProto();

    const bool isSMIME = cryptoProto && (cryptoProto == QGpgME::smime());
    KTextTemplate::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(u"signedmessagepart.html"_s);
    KTextTemplate::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
    QObject block;

    if (node) {
        c.insert(u"content"_s, QVariant::fromValue<KTextTemplateCallback>([this, mp, htmlWriter](KTextTemplate::OutputStream *) {
                     HTMLBlock::Ptr rBlock;
                     if (mp->isRoot()) {
                         rBlock = HTMLBlock::Ptr(new RootBlock(htmlWriter));
                     }
                     renderSubParts(mp, htmlWriter);
                 }));
    } else if (!metaData.inProgress) {
        c.insert(u"content"_s, QVariant::fromValue<KTextTemplateCallback>([this, mp, htmlWriter](KTextTemplate::OutputStream *) {
                     renderWithFactory<MimeTreeParser::MessagePart>(mp, htmlWriter);
                 }));
    }
    c.insert(u"cryptoProto"_s, QVariant::fromValue(cryptoProto));
    c.insert(u"block"_s, &block);

    block.setProperty("inProgress", metaData.inProgress);
    block.setProperty("errorText", metaData.errorText);

    block.setProperty("detailHeader", showSignatureDetails());
    block.setProperty("isPrinting", isPrinting());
    block.setProperty("technicalProblem", metaData.technicalProblem);
    block.setProperty("keyId", metaData.keyId);
    if (metaData.creationTime.isValid()) { // should be handled inside grantlee but currently not possible see: https://bugs.kde.org/363475
        block.setProperty("creationTime", QLocale().toString(metaData.creationTime, QLocale::ShortFormat));
    }
    block.setProperty("isGoodSignature", metaData.isGoodSignature);
    block.setProperty("isCompliant", metaData.isCompliant);
    block.setProperty("compliance", metaData.compliance);
    block.setProperty("isSMIME", isSMIME);

    QString startKeyHREF;
    {
        QString keyWithWithoutURL;
        if (cryptoProto) {
            startKeyHREF = u"<a href=\"key:#%1\">"_s.arg(QString::fromLatin1(metaData.keyId));

            keyWithWithoutURL = u"%1%2</a>"_s.arg(startKeyHREF, QString::fromLatin1(QByteArray(QByteArrayLiteral("0x") + metaData.keyId)));
        } else {
            keyWithWithoutURL = QLatin1StringView("0x") + QString::fromUtf8(metaData.keyId);
        }
        block.setProperty("keyWithWithoutURL", keyWithWithoutURL);
    }

    QString statusStr;
    QString mClass;

    const auto signatures = metaData.verificationResult.signatures();
    if (metaData.inProgress) {
        mClass = u"signInProgress"_s;
    } else {
        Q_ASSERT(!signatures.empty());
        const auto signature = signatures.front(); // TODO add support for multiple signature
        const auto summary = signature.summary();

        statusStr = Kleo::Formatting::prettySignature(signature, {});

        if (summary & GpgME::Signature::Summary::Red) {
            mClass = u"signErr"_s;
        } else if (summary & GpgME::Signature::Summary::Valid) {
            mClass = u"signOkKeyOk"_s;
        } else {
            mClass = u"signWarn"_s;
        }
    }

    block.setProperty("statusStr", statusStr);
    block.setProperty("signClass", mClass);
    KTextTemplate::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
}

void DefaultRendererPrivate::render(const SignedMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    const auto metaData = *mp->partMetaData();
    if (metaData.isSigned || metaData.inProgress) {
        HTMLBlock::Ptr aBlock;
        if (mp->isAttachment()) {
            aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
        }
        renderSigned(mp, htmlWriter);
        return;
    }

    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }
    if (mp->hasSubParts()) {
        renderSubParts(mp, htmlWriter);
    } else if (!metaData.inProgress) {
        renderWithFactory<MimeTreeParser::MessagePart>(mp, htmlWriter);
    }
}

void DefaultRendererPrivate::render(const EncryptedMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    const auto metaData = *mp->partMetaData();

    if (metaData.isEncrypted || metaData.inProgress) {
        HTMLBlock::Ptr aBlock;
        if (mp->isAttachment()) {
            aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
        }
        renderEncrypted(mp, htmlWriter);
        return;
    }

    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }

    if (mp->hasSubParts()) {
        renderSubParts(mp, htmlWriter);
    } else if (!metaData.inProgress) {
        renderWithFactory<MimeTreeParser::MessagePart>(mp, htmlWriter);
    }
}

void DefaultRendererPrivate::render(const AlternativeMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }

    auto mode = mp->preferredMode();
    if (mode == MimeTreeParser::Util::MultipartPlain && mp->text().trimmed().isEmpty()) {
        const auto availableModes = mp->availableModes();
        for (const auto m : availableModes) {
            if (m != MimeTreeParser::Util::MultipartPlain) {
                mode = m;
                break;
            }
        }
    }
    MimeMessagePart::Ptr part(mp->childParts().first());
    if (mp->childParts().contains(mode)) {
        part = mp->childParts()[mode];
    }

    render(part, htmlWriter);
}

void DefaultRendererPrivate::render(const CertMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    const GpgME::ImportResult &importResult(mp->importResult());
    KTextTemplate::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(u"certmessagepart.html"_s);
    KTextTemplate::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
    QObject block;

    c.insert(u"block"_s, &block);
    block.setProperty("importError", Kleo::Formatting::errorAsString(importResult.error()));
    block.setProperty("nImp", importResult.numImported());
    block.setProperty("nUnc", importResult.numUnchanged());
    block.setProperty("nSKImp", importResult.numSecretKeysImported());
    block.setProperty("nSKUnc", importResult.numSecretKeysUnchanged());

    QVariantList keylist;
    const auto imports = importResult.imports();

    auto end(imports.end());
    for (auto it = imports.begin(); it != end; ++it) {
        auto key(new QObject(mp.data()));
        key->setProperty("error", Kleo::Formatting::errorAsString(it->error()));
        key->setProperty("status", (*it).status());
        key->setProperty("fingerprint", QLatin1StringView((*it).fingerprint()));
        keylist << QVariant::fromValue(key);
    }

    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }
    KTextTemplate::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
}

bool DefaultRendererPrivate::renderWithFactory(const QMetaObject *mo, const MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter)
{
    if (!mRendererFactory) {
        return false;
    }
    for (auto r : mRendererFactory->renderersForPart(mo, msgPart)) {
        if (r->render(msgPart, htmlWriter, this)) {
            return true;
        }
    }
    return false;
}

void DefaultRendererPrivate::renderFactory(const MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter)
{
    const QString className = QString::fromUtf8(msgPart->metaObject()->className());

    if (isHiddenHint(msgPart)) {
        const QByteArray cid = msgPart->content()->contentID()->identifier();
        auto mp = msgPart.dynamicCast<MimeTreeParser::TextMessagePart>();
        if (!cid.isEmpty() && mp) {
            const QString fileName = mp->temporaryFilePath();
            const QString href = QUrl::fromLocalFile(fileName).url();
            htmlWriter->embedPart(cid, href);
        }
    }

    if (renderWithFactory(msgPart, htmlWriter)) {
        return;
    }

    if (className == QLatin1StringView("MimeTreeParser::MessagePartList")) {
        auto mp = msgPart.dynamicCast<MessagePartList>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1StringView("MimeTreeParser::MimeMessagePart")) {
        auto mp = msgPart.dynamicCast<MimeMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1StringView("MimeTreeParser::EncapsulatedRfc822MessagePart")) {
        auto mp = msgPart.dynamicCast<EncapsulatedRfc822MessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1StringView("MimeTreeParser::HtmlMessagePart")) {
        auto mp = msgPart.dynamicCast<HtmlMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1StringView("MimeTreeParser::SignedMessagePart")) {
        auto mp = msgPart.dynamicCast<SignedMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1StringView("MimeTreeParser::EncryptedMessagePart")) {
        auto mp = msgPart.dynamicCast<EncryptedMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1StringView("MimeTreeParser::AlternativeMessagePart")) {
        auto mp = msgPart.dynamicCast<AlternativeMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1StringView("MimeTreeParser::CertMessagePart")) {
        auto mp = msgPart.dynamicCast<CertMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else {
        qCWarning(MESSAGEVIEWER_LOG) << "We got a unknown classname, using default behaviour for " << className;
    }
}

bool DefaultRendererPrivate::isHiddenHint(const MimeTreeParser::MessagePart::Ptr &msgPart)
{
    auto mp = msgPart.dynamicCast<MimeTreeParser::MessagePart>();
    auto content = msgPart->content();

    if (!mp || !content) {
        return false;
    }

    if (mShowOnlyOneMimePart && mMsgPart.data() == msgPart->parentPart()) {
        if (mMsgPart->subParts().at(0) == msgPart.data()) {
            return false;
        }
    }

    if (msgPart->nodeHelper()->isNodeDisplayedHidden(content)) {
        return true;
    }

    const AttachmentStrategy *const as = mAttachmentStrategy;
    const bool defaultHidden(as && as->defaultDisplay(content) == AttachmentStrategy::None);
    auto preferredMode = source()->preferredMode();
    bool isHtmlPreferred = (preferredMode == MimeTreeParser::Util::Html) || (preferredMode == MimeTreeParser::Util::MultipartHtml);

    QByteArray mediaType("text");
    if (content->contentType(false) && !content->contentType(false)->mediaType().isEmpty() && !content->contentType(false)->subType().isEmpty()) {
        mediaType = content->contentType(false)->mediaType();
    }
    const bool isTextPart = (mediaType == QByteArrayLiteral("text"));

    bool defaultAsIcon = true;
    if (!mp->neverDisplayInline()) {
        if (as) {
            defaultAsIcon = as->defaultDisplay(content) == AttachmentStrategy::AsIcon;
        }
    }

    // neither image nor text -> show as icon
    if (!mp->isImage() && !isTextPart) {
        defaultAsIcon = true;
    }

    bool hidden(false);
    if (isTextPart) {
        hidden = defaultHidden;
    } else {
        if (mp->isImage() && isHtmlPreferred && content->parent() && content->parent()->contentType(false)->subType() == "related") {
            hidden = true;
        } else {
            hidden = defaultHidden && content->parent();
            hidden |= defaultAsIcon && defaultHidden;
        }
    }
    msgPart->nodeHelper()->setNodeDisplayedHidden(content, hidden);
    return hidden;
}

MimeTreeParser::IconType DefaultRendererPrivate::displayHint(const MimeTreeParser::MessagePart::Ptr &msgPart)
{
    auto mp = msgPart.dynamicCast<MimeTreeParser::TextMessagePart>();
    auto content = msgPart->content();

    if (!content || !mp) {
        return MimeTreeParser::IconType::NoIcon;
    }

    const AttachmentStrategy *const as = mAttachmentStrategy;
    const bool defaultDisplayHidden(as && as->defaultDisplay(content) == AttachmentStrategy::None);
    const bool defaultDisplayInline(as && as->defaultDisplay(content) == AttachmentStrategy::Inline);
    const bool defaultDisplayAsIcon(as && as->defaultDisplay(content) == AttachmentStrategy::AsIcon);
    const bool showOnlyOneMimePart(mShowOnlyOneMimePart);
    auto preferredMode = source()->preferredMode();
    bool isHtmlPreferred = (preferredMode == MimeTreeParser::Util::Html) || (preferredMode == MimeTreeParser::Util::MultipartHtml);

    QByteArray mediaType("text");
    if (content->contentType(false) && !content->contentType(false)->mediaType().isEmpty() && !content->contentType(false)->subType().isEmpty()) {
        mediaType = content->contentType(false)->mediaType();
    }
    const bool isTextPart = (mediaType == QByteArrayLiteral("text"));

    bool defaultAsIcon = true;
    if (!mp->neverDisplayInline()) {
        if (as) {
            defaultAsIcon = defaultDisplayAsIcon;
        }
    }
    if (mp->isImage() && showOnlyOneMimePart && !mp->neverDisplayInline()) {
        defaultAsIcon = false;
    }

    // neither image nor text -> show as icon
    if (!mp->isImage() && !isTextPart) {
        defaultAsIcon = true;
    }

    if (isTextPart) {
        if (as && !defaultDisplayInline) {
            return MimeTreeParser::IconExternal;
        }
        return MimeTreeParser::NoIcon;
    } else {
        if (mp->isImage() && isHtmlPreferred && content->parent() && content->parent()->contentType(false)->subType() == "related") {
            return MimeTreeParser::IconInline;
        }

        if (defaultDisplayHidden && !showOnlyOneMimePart && content->parent()) {
            return MimeTreeParser::IconInline;
        }

        if (defaultAsIcon) {
            return MimeTreeParser::IconExternal;
        } else if (mp->isImage()) {
            return MimeTreeParser::IconInline;
        }
    }

    return MimeTreeParser::NoIcon;
}

bool DefaultRendererPrivate::showEmoticons() const
{
    return mShowEmoticons;
}

bool DefaultRendererPrivate::isPrinting() const
{
    return mIsPrinting;
}

bool DefaultRendererPrivate::htmlLoadExternal() const
{
    return mHtmlLoadExternal;
}

bool DefaultRendererPrivate::showExpandQuotesMark() const
{
    return mShowExpandQuotesMark;
}

bool DefaultRendererPrivate::showOnlyOneMimePart() const
{
    return mShowOnlyOneMimePart;
}

bool DefaultRendererPrivate::showSignatureDetails() const
{
    return mShowSignatureDetails;
}

bool DefaultRendererPrivate::showEncryptionDetails() const
{
    return mShowEncryptionDetails;
}

int DefaultRendererPrivate::levelQuote() const
{
    return mLevelQuote;
}

DefaultRenderer::DefaultRenderer(CSSHelperBase *cssHelper)
    : d(new DefaultRendererPrivate(cssHelper, MessagePartRendererFactory::instance()))
{
}

DefaultRenderer::~DefaultRenderer() = default;

void DefaultRenderer::setShowOnlyOneMimePart(bool onlyOneMimePart)
{
    d->mShowOnlyOneMimePart = onlyOneMimePart;
}

void DefaultRenderer::setAttachmentStrategy(const AttachmentStrategy *strategy)
{
    d->mAttachmentStrategy = strategy;
}

void DefaultRenderer::setShowEmoticons(bool showEmoticons)
{
    d->mShowEmoticons = showEmoticons;
}

void DefaultRenderer::setIsPrinting(bool isPrinting)
{
    d->mIsPrinting = isPrinting;
}

void DefaultRenderer::setShowExpandQuotesMark(bool showExpandQuotesMark)
{
    d->mShowExpandQuotesMark = showExpandQuotesMark;
}

void DefaultRenderer::setShowEncryptionDetails(bool showEncryptionDetails)
{
    d->mShowEncryptionDetails = showEncryptionDetails;
}

void DefaultRenderer::setShowSignatureDetails(bool showSignatureDetails)
{
    d->mShowSignatureDetails = showSignatureDetails;
}

void DefaultRenderer::setLevelQuote(int levelQuote)
{
    d->mLevelQuote = levelQuote;
}

void DefaultRenderer::setHtmlLoadExternal(bool htmlLoadExternal)
{
    d->mHtmlLoadExternal = htmlLoadExternal;
}

void DefaultRenderer::setCreateMessageHeader(const std::function<QString(KMime::Message *)> &createMessageHeader)
{
    d->mCreateMessageHeader = createMessageHeader;
}

static QString renderTreeHelper(const MimeTreeParser::MessagePart::Ptr &messagePart, QString indent)
{
    QString ret = u"%1 * %3\n"_s.arg(indent, QString::fromUtf8(messagePart->metaObject()->className()));
    indent += u' ';
    for (const auto &subPart : messagePart->subParts()) {
        ret += renderTreeHelper(subPart, indent);
    }
    return ret;
}

void DefaultRenderer::render(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer)
{
    qCDebug(MESSAGEVIEWER_LOG) << "MimeTreeParser structure:";
    qCDebug(MESSAGEVIEWER_LOG) << qPrintable(renderTreeHelper(msgPart, QString()));
    d->mMsgPart = msgPart;
    d->renderFactory(d->mMsgPart, writer);
}
