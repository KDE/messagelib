/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparserjob.h"
#include "customtemplates_kfg.h"
#include "globalsettings_templateparser.h"
#include "templateparserextracthtmlinfo.h"
#include "templateparserjob_p.h"
#include "templatesconfiguration.h"
#include "templatesconfiguration_kfg.h"
#include "templatesutil.h"
#include "templatesutil_p.h"

#include <MessageCore/ImageCollector>
#include <MessageCore/StringUtil>

#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/SimpleObjectTreeSource>

#include <KIdentityManagement/Identity>
#include <KIdentityManagement/IdentityManager>

#include "templateparser_debug.h"
#include <KCharsets>
#include <KLocalizedString>
#include <KMessageBox>
#include <KProcess>
#include <KShell>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QRegularExpression>
#include <QTextCodec>

namespace
{
Q_DECL_CONSTEXPR inline int pipeTimeout()
{
    return 15 * 1000;
}

static QTextCodec *selectCharset(const QStringList &charsets, const QString &text)
{
    for (const QString &name : charsets) {
        // We use KCharsets::codecForName() instead of QTextCodec::codecForName() here, because
        // the former knows us-ascii is latin1.
        bool ok = true;
        QTextCodec *codec = nullptr;
        if (name == QLatin1String("locale")) {
            codec = QTextCodec::codecForLocale();
        } else {
            codec = KCharsets::charsets()->codecForName(name, ok);
        }
        if (!ok || !codec) {
            qCWarning(TEMPLATEPARSER_LOG) << "Could not get text codec for charset" << name;
            continue;
        }
        if (codec->canEncode(text)) {
            // Special check for us-ascii (needed because us-ascii is not exactly latin1).
            if (name == QLatin1String("us-ascii") && !KMime::isUsAscii(text)) {
                continue;
            }
            qCDebug(TEMPLATEPARSER_LOG) << "Chosen charset" << name << codec->name();
            return codec;
        }
    }
    qCDebug(TEMPLATEPARSER_LOG) << "No appropriate charset found.";
    return KCharsets::charsets()->codecForName(QStringLiteral("utf-8"));
}
}

using namespace TemplateParser;

TemplateParserJobPrivate::TemplateParserJobPrivate(const KMime::Message::Ptr &amsg, const TemplateParserJob::Mode amode)
    : mMsg(amsg)
    , mMode(amode)
{
    mEmptySource = new MimeTreeParser::SimpleObjectTreeSource;
    mEmptySource->setDecryptMessage(mAllowDecryption);

    mOtp = new MimeTreeParser::ObjectTreeParser(mEmptySource);
    mOtp->setAllowAsync(false);
}

TemplateParserJobPrivate::~TemplateParserJobPrivate()
{
    delete mEmptySource;
    delete mOtp;
}

void TemplateParserJobPrivate::setAllowDecryption(const bool allowDecryption)
{
    mAllowDecryption = allowDecryption;
    mEmptySource->setDecryptMessage(mAllowDecryption);
}

TemplateParserJob::TemplateParserJob(const KMime::Message::Ptr &amsg, const Mode amode, QObject *parent)
    : QObject(parent)
    , d(new TemplateParserJobPrivate(amsg, amode))
{
}

TemplateParserJob::~TemplateParserJob() = default;

void TemplateParserJob::setSelection(const QString &selection)
{
    d->mSelection = selection;
}

void TemplateParserJob::setAllowDecryption(const bool allowDecryption)
{
    d->setAllowDecryption(allowDecryption);
}

bool TemplateParserJob::shouldStripSignature() const
{
    // Only strip the signature when replying, it should be preserved when forwarding
    return (d->mMode == Reply || d->mMode == ReplyAll) && TemplateParserSettings::self()->stripSignature();
}

void TemplateParserJob::setIdentityManager(KIdentityManagement::IdentityManager *ident)
{
    d->m_identityManager = ident;
}

void TemplateParserJob::setCharsets(const QStringList &charsets)
{
    d->mCharsets = charsets;
}

int TemplateParserJob::parseQuotes(const QString &prefix, const QString &str, QString &quote)
{
    int pos = prefix.length();
    int len;
    const int str_len = str.length();

    // Also allow the german lower double-quote sign as quote separator, not only
    // the standard ASCII quote ("). This fixes bug 166728.
    const QList<QChar> quoteChars = {QLatin1Char('"'), 0x201C};

    QChar prev(QChar::Null);

    pos++;
    len = pos;

    while (pos < str_len) {
        const QChar c = str[pos];

        pos++;
        len++;

        if (!prev.isNull()) {
            quote.append(c);
            prev = QChar::Null;
        } else {
            if (c == QLatin1Char('\\')) {
                prev = c;
            } else if (quoteChars.contains(c)) {
                break;
            } else {
                quote.append(c);
            }
        }
    }

    return len;
}

void TemplateParserJob::process(const KMime::Message::Ptr &aorig_msg, qint64 afolder)
{
    if (aorig_msg == nullptr) {
        qCDebug(TEMPLATEPARSER_LOG) << "aorig_msg == 0!";
        Q_EMIT parsingDone(d->mForceCursorPosition);
        deleteLater();
        return;
    }

    d->mOrigMsg = aorig_msg;
    d->mFolder = afolder;
    const QString tmpl = findTemplate();
    if (tmpl.isEmpty()) {
        Q_EMIT parsingDone(d->mForceCursorPosition);
        deleteLater();
        return;
    }
    processWithTemplate(tmpl);
}

void TemplateParserJob::process(const QString &tmplName, const KMime::Message::Ptr &aorig_msg, qint64 afolder)
{
    d->mForceCursorPosition = false;
    d->mOrigMsg = aorig_msg;
    d->mFolder = afolder;
    const QString tmpl = findCustomTemplate(tmplName);
    processWithTemplate(tmpl);
}

void TemplateParserJob::processWithIdentity(uint uoid, const KMime::Message::Ptr &aorig_msg, qint64 afolder)
{
    d->mIdentity = uoid;
    process(aorig_msg, afolder);
}

MimeTreeParser::MessagePart::Ptr toplevelTextNode(MimeTreeParser::MessagePart::Ptr messageTree)
{
    const auto subParts = messageTree->subParts();
    for (const auto &mp : subParts) {
        auto text = mp.dynamicCast<MimeTreeParser::TextMessagePart>();
        const auto attach = mp.dynamicCast<MimeTreeParser::AttachmentMessagePart>();
        if (text && !attach) {
            // TextMessagePart can have several subparts cause of PGP inline, we search for the first part with content
            const auto mpSubParts{mp->subParts()};
            for (const auto &sub : mpSubParts) {
                if (!sub->text().trimmed().isEmpty()) {
                    return sub;
                }
            }
            return text;
        } else if (const auto html = mp.dynamicCast<MimeTreeParser::HtmlMessagePart>()) {
            return html;
        } else if (const auto alternative = mp.dynamicCast<MimeTreeParser::AlternativeMessagePart>()) {
            return alternative;
        } else {
            auto ret = toplevelTextNode(mp);
            if (ret) {
                return ret;
            }
        }
    }
    return MimeTreeParser::MessagePart::Ptr();
}

void TemplateParserJob::processWithTemplate(const QString &tmpl)
{
    d->mOtp->parseObjectTree(d->mOrigMsg.data());

    const auto mp = toplevelTextNode(d->mOtp->parsedPart());
    if (!mp) {
        qCWarning(TEMPLATEPARSER_LOG) << "Invalid message! mp is null ";
        Q_EMIT parsingFailed();
        return;
    }

    QString plainText = mp->plaintextContent();
    QString htmlElement;

    if (mp->isHtml()) {
        htmlElement = d->mOtp->htmlContent();
        if (plainText.isEmpty()) { // HTML-only mails
            plainText = htmlElement;
        }
    } else { // plain mails only
        QString htmlReplace = plainText.toHtmlEscaped();
        htmlReplace.replace(QLatin1Char('\n'), QStringLiteral("<br />"));
        htmlElement = QStringLiteral("<html><head></head><body>%1</body></html>\n").arg(htmlReplace);
    }

    auto job = new TemplateParserExtractHtmlInfo(this);
    connect(job, &TemplateParserExtractHtmlInfo::finished, this, &TemplateParserJob::slotExtractInfoDone);

    job->setHtmlForExtractingTextPlain(plainText);
    job->setTemplate(tmpl);

    job->setHtmlForExtractionHeaderAndBody(htmlElement);
    job->start();
}

void TemplateParserJob::setReplyAsHtml(bool replyAsHtml)
{
    d->mReplyAsHtml = replyAsHtml;
}

void TemplateParserJob::slotExtractInfoDone(const TemplateParserExtractHtmlInfoResult &result)
{
    d->mExtractHtmlInfoResult = result;
    const QString tmpl = d->mExtractHtmlInfoResult.mTemplate;
    const int tmpl_len = tmpl.length();
    QString plainBody;
    QString htmlBody;

    bool dnl = false;
    auto definedLocale = QLocale();
    for (int i = 0; i < tmpl_len; ++i) {
        QChar c = tmpl[i];
        // qCDebug(TEMPLATEPARSER_LOG) << "Next char: " << c;
        if (c == QLatin1Char('%')) {
            const QString cmd = tmpl.mid(i + 1);

            if (cmd.startsWith(QLatin1Char('-'))) {
                // dnl
                qCDebug(TEMPLATEPARSER_LOG) << "Command: -";
                dnl = true;
                i += 1;
            } else if (cmd.startsWith(QLatin1String("REM="))) {
                // comments
                qCDebug(TEMPLATEPARSER_LOG) << "Command: REM=";
                QString q;
                const int len = parseQuotes(QStringLiteral("REM="), cmd, q);
                i += len;
            } else if (cmd.startsWith(QLatin1String("LANGUAGE="))) {
                QString q;
                const int len = parseQuotes(QStringLiteral("LANGUAGE="), cmd, q);
                i += len;
                if (!q.isEmpty()) {
                    definedLocale = QLocale(q);
                }
            } else if (cmd.startsWith(QLatin1String("DICTIONARYLANGUAGE="))) {
                QString q;
                const int len = parseQuotes(QStringLiteral("DICTIONARYLANGUAGE="), cmd, q);
                i += len;
                if (!q.isEmpty()) {
                    auto header = new KMime::Headers::Generic("X-KMail-Dictionary");
                    header->fromUnicodeString(q, "utf-8");
                    d->mMsg->setHeader(header);
                }
            } else if (cmd.startsWith(QLatin1String("INSERT=")) || cmd.startsWith(QLatin1String("PUT="))) {
                QString q;
                int len = 0;
                if (cmd.startsWith(QLatin1String("INSERT="))) {
                    // insert content of specified file as is
                    qCDebug(TEMPLATEPARSER_LOG) << "Command: INSERT=";
                    len = parseQuotes(QStringLiteral("INSERT="), cmd, q);
                } else {
                    // insert content of specified file as is
                    qCDebug(TEMPLATEPARSER_LOG) << "Command: PUT=";
                    len = parseQuotes(QStringLiteral("PUT="), cmd, q);
                }
                i += len;
                QString path = KShell::tildeExpand(q);
                QFileInfo finfo(path);
                if (finfo.isRelative()) {
                    path = QDir::homePath() + QLatin1Char('/') + q;
                }
                QFile file(path);
                if (file.open(QIODevice::ReadOnly)) {
                    const QByteArray content = file.readAll();
                    const QString str = QString::fromLocal8Bit(content.constData(), content.size());
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                } else if (d->mDebug) {
                    KMessageBox::error(nullptr, i18nc("@info", "Cannot insert content from file %1: %2", path, file.errorString()));
                }
            } else if (cmd.startsWith(QLatin1String("SYSTEM="))) {
                // insert content of specified file as is
                qCDebug(TEMPLATEPARSER_LOG) << "Command: SYSTEM=";
                QString q;
                const int len = parseQuotes(QStringLiteral("SYSTEM="), cmd, q);
                i += len;
                const QString pipe_cmd = q;
                const QString str = pipe(pipe_cmd, QString());
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("QUOTEPIPE="))) {
                // pipe message body through command and insert it as quotation
                qCDebug(TEMPLATEPARSER_LOG) << "Command: QUOTEPIPE=";
                QString q;
                const int len = parseQuotes(QStringLiteral("QUOTEPIPE="), cmd, q);
                i += len;
                const QString pipe_cmd = q;
                if (d->mOrigMsg) {
                    const QString plainStr = pipe(pipe_cmd, plainMessageText(shouldStripSignature(), NoSelectionAllowed));
                    QString plainQuote = quotedPlainText(plainStr);
                    if (plainQuote.endsWith(QLatin1Char('\n'))) {
                        plainQuote.chop(1);
                    }
                    plainBody.append(plainQuote);

                    const QString htmlStr = pipe(pipe_cmd, htmlMessageText(shouldStripSignature(), NoSelectionAllowed));
                    const QString htmlQuote = quotedHtmlText(htmlStr);
                    htmlBody.append(htmlQuote);
                }
            } else if (cmd.startsWith(QLatin1String("QUOTE"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: QUOTE";
                i += strlen("QUOTE");
                if (d->mOrigMsg) {
                    QString plainQuote = quotedPlainText(plainMessageText(shouldStripSignature(), SelectionAllowed));
                    if (plainQuote.endsWith(QLatin1Char('\n'))) {
                        plainQuote.chop(1);
                    }
                    plainBody.append(plainQuote);

                    const QString htmlQuote = quotedHtmlText(htmlMessageText(shouldStripSignature(), SelectionAllowed));
                    htmlBody.append(htmlQuote);
                }
            } else if (cmd.startsWith(QLatin1String("FORCEDPLAIN"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FORCEDPLAIN";
                d->mQuotes = ReplyAsPlain;
                i += strlen("FORCEDPLAIN");
            } else if (cmd.startsWith(QLatin1String("FORCEDHTML"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FORCEDHTML";
                d->mQuotes = ReplyAsHtml;
                i += strlen("FORCEDHTML");
            } else if (cmd.startsWith(QLatin1String("QHEADERS"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: QHEADERS";
                i += strlen("QHEADERS");
                if (d->mOrigMsg) {
                    const QString headerStr = QString::fromLatin1(MessageCore::StringUtil::headerAsSendableString(d->mOrigMsg));
                    QString plainQuote = quotedPlainText(headerStr);
                    if (plainQuote.endsWith(QLatin1Char('\n'))) {
                        plainQuote.chop(1);
                    }
                    plainBody.append(plainQuote);

                    const QString htmlQuote = quotedHtmlText(headerStr);
                    const QString str = plainTextToHtml(htmlQuote);
                    htmlBody.append(str);
                }
            } else if (cmd.startsWith(QLatin1String("HEADERS"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: HEADERS";
                i += strlen("HEADERS");
                if (d->mOrigMsg) {
                    const QString str = QString::fromLatin1(MessageCore::StringUtil::headerAsSendableString(d->mOrigMsg));
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("TEXTPIPE="))) {
                // pipe message body through command and insert it as is
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TEXTPIPE=";
                QString q;
                const int len = parseQuotes(QStringLiteral("TEXTPIPE="), cmd, q);
                i += len;
                const QString pipe_cmd = q;
                if (d->mOrigMsg) {
                    const QString plainStr = pipe(pipe_cmd, plainMessageText(shouldStripSignature(), NoSelectionAllowed));
                    plainBody.append(plainStr);

                    const QString htmlStr = pipe(pipe_cmd, htmlMessageText(shouldStripSignature(), NoSelectionAllowed));
                    htmlBody.append(htmlStr);
                }
            } else if (cmd.startsWith(QLatin1String("MSGPIPE="))) {
                // pipe full message through command and insert result as is
                qCDebug(TEMPLATEPARSER_LOG) << "Command: MSGPIPE=";
                QString q;
                const int len = parseQuotes(QStringLiteral("MSGPIPE="), cmd, q);
                i += len;
                if (d->mOrigMsg) {
                    const QString str = pipe(q, QString::fromLatin1(d->mOrigMsg->encodedContent()));
                    plainBody.append(str);

                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("BODYPIPE="))) {
                // pipe message body generated so far through command and insert result as is
                qCDebug(TEMPLATEPARSER_LOG) << "Command: BODYPIPE=";
                QString q;
                const int len = parseQuotes(QStringLiteral("BODYPIPE="), cmd, q);
                i += len;
                const QString pipe_cmd = q;
                const QString plainStr = pipe(q, plainBody);
                plainBody.append(plainStr);

                const QString htmlStr = pipe(pipe_cmd, htmlBody);
                const QString body = plainTextToHtml(htmlStr);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("CLEARPIPE="))) {
                // pipe message body generated so far through command and
                // insert result as is replacing current body
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CLEARPIPE=";
                QString q;
                const int len = parseQuotes(QStringLiteral("CLEARPIPE="), cmd, q);
                i += len;
                const QString pipe_cmd = q;
                const QString plainStr = pipe(pipe_cmd, plainBody);
                plainBody = plainStr;

                const QString htmlStr = pipe(pipe_cmd, htmlBody);
                htmlBody = htmlStr;

                auto header = new KMime::Headers::Generic("X-KMail-CursorPos");
                header->fromUnicodeString(QString::number(0), "utf-8");
                d->mMsg->setHeader(header);
            } else if (cmd.startsWith(QLatin1String("TEXT"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TEXT";
                i += strlen("TEXT");
                if (d->mOrigMsg) {
                    const QString plainStr = plainMessageText(shouldStripSignature(), NoSelectionAllowed);
                    plainBody.append(plainStr);

                    const QString htmlStr = htmlMessageText(shouldStripSignature(), NoSelectionAllowed);
                    htmlBody.append(htmlStr);
                }
            } else if (cmd.startsWith(QLatin1String("OTEXTSIZE"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTEXTSIZE";
                i += strlen("OTEXTSIZE");
                if (d->mOrigMsg) {
                    const QString str = QStringLiteral("%1").arg(d->mOrigMsg->body().length());
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OTEXT"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTEXT";
                i += strlen("OTEXT");
                if (d->mOrigMsg) {
                    const QString plainStr = plainMessageText(shouldStripSignature(), NoSelectionAllowed);
                    plainBody.append(plainStr);

                    const QString htmlStr = htmlMessageText(shouldStripSignature(), NoSelectionAllowed);
                    htmlBody.append(htmlStr);
                }
            } else if (cmd.startsWith(QLatin1String("OADDRESSEESADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OADDRESSEESADDR";
                i += strlen("OADDRESSEESADDR");
                if (d->mOrigMsg) {
                    const QString to = d->mOrigMsg->to()->asUnicodeString();
                    const QString cc = d->mOrigMsg->cc()->asUnicodeString();
                    if (!to.isEmpty()) {
                        const QString toLine = i18nc("@item:intext email To", "To:") + QLatin1Char(' ') + to;
                        plainBody.append(toLine);
                        const QString body = plainTextToHtml(toLine);
                        htmlBody.append(body);
                    }
                    if (!to.isEmpty() && !cc.isEmpty()) {
                        plainBody.append(QLatin1Char('\n'));
                        const QString str = plainTextToHtml(QString(QLatin1Char('\n')));
                        htmlBody.append(str);
                    }
                    if (!cc.isEmpty()) {
                        const QString ccLine = i18nc("@item:intext email CC", "CC:") + QLatin1Char(' ') + cc;
                        plainBody.append(ccLine);
                        const QString str = plainTextToHtml(ccLine);
                        htmlBody.append(str);
                    }
                }
            } else if (cmd.startsWith(QLatin1String("CCADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CCADDR";
                i += strlen("CCADDR");
                const QString str = d->mMsg->cc()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("CCNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CCNAME";
                i += strlen("CCNAME");
                const QString str = d->mMsg->cc()->displayString();
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("CCFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CCFNAME";
                i += strlen("CCFNAME");
                const QString str = d->mMsg->cc()->displayString();
                const QString firstNameFromEmail = TemplateParser::Util::getFirstNameFromEmail(str);
                plainBody.append(firstNameFromEmail);
                const QString body = plainTextToHtml(firstNameFromEmail);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("CCLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CCLNAME";
                i += strlen("CCLNAME");
                const QString str = d->mMsg->cc()->displayString();
                plainBody.append(TemplateParser::Util::getLastNameFromEmail(str));
                const QString body = plainTextToHtml(TemplateParser::Util::getLastNameFromEmail(str));
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("TOADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TOADDR";
                i += strlen("TOADDR");
                const QString str = d->mMsg->to()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("TONAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TONAME";
                i += strlen("TONAME");
                const QString str = (d->mMsg->to()->displayString());
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("TOFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TOFNAME";
                i += strlen("TOFNAME");
                const QString str = d->mMsg->to()->displayString();
                const QString firstNameFromEmail = TemplateParser::Util::getFirstNameFromEmail(str);
                plainBody.append(firstNameFromEmail);
                const QString body = plainTextToHtml(firstNameFromEmail);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("TOLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TOLNAME";
                i += strlen("TOLNAME");
                const QString str = d->mMsg->to()->displayString();
                plainBody.append(TemplateParser::Util::getLastNameFromEmail(str));
                const QString body = plainTextToHtml(TemplateParser::Util::getLastNameFromEmail(str));
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("TOLIST"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TOLIST";
                i += strlen("TOLIST");
                const QString str = d->mMsg->to()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("FROMADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FROMADDR";
                i += strlen("FROMADDR");
                const QString str = d->mMsg->from()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("FROMNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FROMNAME";
                i += strlen("FROMNAME");
                const QString str = d->mMsg->from()->displayString();
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("FROMFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FROMFNAME";
                i += strlen("FROMFNAME");
                const QString str = d->mMsg->from()->displayString();
                const QString firstNameFromEmail = TemplateParser::Util::getFirstNameFromEmail(str);
                plainBody.append(firstNameFromEmail);
                const QString body = plainTextToHtml(firstNameFromEmail);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("FROMLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FROMLNAME";
                i += strlen("FROMLNAME");
                const QString str = d->mMsg->from()->displayString();
                plainBody.append(TemplateParser::Util::getLastNameFromEmail(str));
                const QString body = plainTextToHtml(TemplateParser::Util::getLastNameFromEmail(str));
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("FULLSUBJECT")) || cmd.startsWith(QLatin1String("FULLSUBJ"))) {
                if (cmd.startsWith(QLatin1String("FULLSUBJ"))) {
                    qCDebug(TEMPLATEPARSER_LOG) << "Command: FULLSUBJ";
                    i += strlen("FULLSUBJ");
                } else {
                    qCDebug(TEMPLATEPARSER_LOG) << "Command: FULLSUBJECT";
                    i += strlen("FULLSUBJECT");
                }
                const QString str = d->mMsg->subject()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("MSGID"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: MSGID";
                i += strlen("MSGID");
                const QString str = d->mMsg->messageID()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("OHEADER="))) {
                // insert specified content of header from original message
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OHEADER=";
                QString q;
                const int len = parseQuotes(QStringLiteral("OHEADER="), cmd, q);
                i += len;
                if (d->mOrigMsg) {
                    const QString hdr = q;
                    QString str;
                    if (auto hrdMsgOrigin = d->mOrigMsg->headerByType(hdr.toLocal8Bit().constData())) {
                        str = hrdMsgOrigin->asUnicodeString();
                    }
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("HEADER="))) {
                // insert specified content of header from current message
                qCDebug(TEMPLATEPARSER_LOG) << "Command: HEADER=";
                QString q;
                const int len = parseQuotes(QStringLiteral("HEADER="), cmd, q);
                i += len;
                const QString hdr = q;
                QString str;
                if (auto hrdMsgOrigin = d->mOrigMsg->headerByType(hdr.toLocal8Bit().constData())) {
                    str = hrdMsgOrigin->asUnicodeString();
                }
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("HEADER( "))) {
                // insert specified content of header from current message
                qCDebug(TEMPLATEPARSER_LOG) << "Command: HEADER(";
                QRegularExpressionMatch match;
                const int res = cmd.indexOf(QRegularExpression(QStringLiteral("^HEADER\\((.+)\\)")), 0, &match);
                if (res != 0) {
                    // something wrong
                    i += strlen("HEADER( ");
                } else {
                    i += match.capturedLength(0); // length of HEADER(<space> + <space>)
                    const QString hdr = match.captured(1).trimmed();
                    QString str;
                    if (auto hrdMsgOrigin = d->mOrigMsg->headerByType(hdr.toLocal8Bit().constData())) {
                        str = hrdMsgOrigin->asUnicodeString();
                    }
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OCCADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OCCADDR";
                i += strlen("OCCADDR");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->cc()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OCCNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OCCNAME";
                i += strlen("OCCNAME");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->cc()->displayString();
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OCCFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OCCFNAME";
                i += strlen("OCCFNAME");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->cc()->displayString();
                    const QString firstNameFromEmail = TemplateParser::Util::getFirstNameFromEmail(str);
                    plainBody.append(firstNameFromEmail);
                    const QString body = plainTextToHtml(firstNameFromEmail);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OCCLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OCCLNAME";
                i += strlen("OCCLNAME");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->cc()->displayString();
                    plainBody.append(TemplateParser::Util::getLastNameFromEmail(str));
                    const QString body = plainTextToHtml(TemplateParser::Util::getLastNameFromEmail(str));
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OTOADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTOADDR";
                i += strlen("OTOADDR");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->to()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OTONAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTONAME";
                i += strlen("OTONAME");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->to()->displayString();
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OTOFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTOFNAME";
                i += strlen("OTOFNAME");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->to()->displayString();
                    const QString firstNameFromEmail = TemplateParser::Util::getFirstNameFromEmail(str);
                    plainBody.append(firstNameFromEmail);
                    const QString body = plainTextToHtml(firstNameFromEmail);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OTOLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTOLNAME";
                i += strlen("OTOLNAME");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->to()->displayString();
                    plainBody.append(TemplateParser::Util::getLastNameFromEmail(str));
                    const QString body = plainTextToHtml(TemplateParser::Util::getLastNameFromEmail(str));
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OTOLIST"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTOLIST";
                i += strlen("OTOLIST");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->to()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OTO"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTO";
                i += strlen("OTO");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->to()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OFROMADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OFROMADDR";
                i += strlen("OFROMADDR");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->from()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OFROMNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OFROMNAME";
                i += strlen("OFROMNAME");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->from()->displayString();
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OFROMFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OFROMFNAME";
                i += strlen("OFROMFNAME");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->from()->displayString();
                    const QString firstNameFromEmail = TemplateParser::Util::getFirstNameFromEmail(str);
                    plainBody.append(firstNameFromEmail);
                    const QString body = plainTextToHtml(firstNameFromEmail);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OFROMLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OFROMLNAME";
                i += strlen("OFROMLNAME");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->from()->displayString();
                    plainBody.append(TemplateParser::Util::getLastNameFromEmail(str));
                    const QString body = plainTextToHtml(TemplateParser::Util::getLastNameFromEmail(str));
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OFULLSUBJECT")) || cmd.startsWith(QLatin1String("OFULLSUBJ"))) {
                if (cmd.startsWith(QLatin1String("OFULLSUBJECT"))) {
                    qCDebug(TEMPLATEPARSER_LOG) << "Command: OFULLSUBJECT";
                    i += strlen("OFULLSUBJECT");
                } else {
                    qCDebug(TEMPLATEPARSER_LOG) << "Command: OFULLSUBJ";
                    i += strlen("OFULLSUBJ");
                }
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->subject()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OMSGID"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OMSGID";
                i += strlen("OMSGID");
                if (d->mOrigMsg) {
                    const QString str = d->mOrigMsg->messageID()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("DATEEN"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DATEEN";
                i += strlen("DATEEN");
                const QDateTime date = QDateTime::currentDateTime();
                QLocale locale(QLocale::C);
                const QString str = locale.toString(date.date(), QLocale::LongFormat);
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("DATESHORT"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DATESHORT";
                i += strlen("DATESHORT");
                const QDateTime date = QDateTime::currentDateTime();
                const QString str = definedLocale.toString(date.date(), QLocale::ShortFormat);
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("DATE"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DATE";
                i += strlen("DATE");
                const QDateTime date = QDateTime::currentDateTime();
                const QString str = definedLocale.toString(date.date(), QLocale::LongFormat);
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("DOW"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DOW";
                i += strlen("DOW");
                const QDateTime date = QDateTime::currentDateTime();
                const QString str = definedLocale.dayName(date.date().dayOfWeek(), QLocale::LongFormat);
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("TIMELONGEN"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TIMELONGEN";
                i += strlen("TIMELONGEN");
                const QDateTime date = QDateTime::currentDateTime();
                QLocale locale(QLocale::C);
                const QString str = locale.toString(date.time(), QLocale::LongFormat);
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("TIMELONG"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TIMELONG";
                i += strlen("TIMELONG");
                const QDateTime date = QDateTime::currentDateTime();
                const QString str = definedLocale.toString(date.time(), QLocale::LongFormat);
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("TIME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TIME";
                i += strlen("TIME");
                const QDateTime date = QDateTime::currentDateTime();
                const QString str = definedLocale.toString(date.time(), QLocale::ShortFormat);
                plainBody.append(str);
                const QString body = plainTextToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QLatin1String("ODATEEN"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: ODATEEN";
                i += strlen("ODATEEN");
                if (d->mOrigMsg) {
                    const QDateTime date = d->mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = QLocale::c().toString(date.date(), QLocale::LongFormat);
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("ODATESHORT"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: ODATESHORT";
                i += strlen("ODATESHORT");
                if (d->mOrigMsg) {
                    const QDateTime date = d->mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = definedLocale.toString(date.date(), QLocale::ShortFormat);
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("ODATE"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: ODATE";
                i += strlen("ODATE");
                if (d->mOrigMsg) {
                    const QDateTime date = d->mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = definedLocale.toString(date.date(), QLocale::LongFormat);
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("ODOW"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: ODOW";
                i += strlen("ODOW");
                if (d->mOrigMsg) {
                    const QDateTime date = d->mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = definedLocale.dayName(date.date().dayOfWeek(), QLocale::LongFormat);
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OTIMELONGEN"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTIMELONGEN";
                i += strlen("OTIMELONGEN");
                if (d->mOrigMsg) {
                    const QDateTime date = d->mOrigMsg->date()->dateTime().toLocalTime();
                    QLocale locale(QLocale::C);
                    const QString str = locale.toString(date.time(), QLocale::LongFormat);
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OTIMELONG"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTIMELONG";
                i += strlen("OTIMELONG");
                if (d->mOrigMsg) {
                    const QDateTime date = d->mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = definedLocale.toString(date.time(), QLocale::LongFormat);
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("OTIME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTIME";
                i += strlen("OTIME");
                if (d->mOrigMsg) {
                    const QDateTime date = d->mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = definedLocale.toString(date.time(), QLocale::ShortFormat);
                    plainBody.append(str);
                    const QString body = plainTextToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QLatin1String("BLANK"))) {
                // do nothing
                qCDebug(TEMPLATEPARSER_LOG) << "Command: BLANK";
                i += strlen("BLANK");
            } else if (cmd.startsWith(QLatin1String("NOP"))) {
                // do nothing
                qCDebug(TEMPLATEPARSER_LOG) << "Command: NOP";
                i += strlen("NOP");
            } else if (cmd.startsWith(QLatin1String("CLEAR"))) {
                // clear body buffer; not too useful yet
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CLEAR";
                i += strlen("CLEAR");
                plainBody.clear();
                htmlBody.clear();
                auto header = new KMime::Headers::Generic("X-KMail-CursorPos");
                header->fromUnicodeString(QString::number(0), "utf-8");
                d->mMsg->setHeader(header);
            } else if (cmd.startsWith(QLatin1String("DEBUGOFF"))) {
                // turn off debug
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DEBUGOFF";
                i += strlen("DEBUGOFF");
                d->mDebug = false;
            } else if (cmd.startsWith(QLatin1String("DEBUG"))) {
                // turn on debug
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DEBUG";
                i += strlen("DEBUG");
                d->mDebug = true;
            } else if (cmd.startsWith(QLatin1String("CURSOR"))) {
                // turn on debug
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CURSOR";
                int oldI = i;
                i += strlen("CURSOR");
                auto header = new KMime::Headers::Generic("X-KMail-CursorPos");
                header->fromUnicodeString(QString::number(plainBody.length()), "utf-8");
                /* if template is:
                 *  FOOBAR
                 *  %CURSOR
                 *
                 * Make sure there is an empty line for the cursor otherwise it will be placed at the end of FOOBAR
                 */
                if (oldI > 0 && tmpl[oldI - 1] == QLatin1Char('\n') && i == tmpl_len - 1) {
                    plainBody.append(QLatin1Char('\n'));
                }
                d->mMsg->setHeader(header);
                d->mForceCursorPosition = true;
                // FIXME HTML part for header remaining
            } else if (cmd.startsWith(QLatin1String("SIGNATURE"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: SIGNATURE";
                i += strlen("SIGNATURE");
                plainBody.append(getPlainSignature());
                htmlBody.append(getHtmlSignature());
            } else {
                // wrong command, do nothing
                plainBody.append(c);
                htmlBody.append(c);
            }
        } else if (dnl && (c == QLatin1Char('\n') || c == QLatin1Char('\r'))) {
            // skip
            if ((tmpl.size() > i + 1)
                && ((c == QLatin1Char('\n') && tmpl[i + 1] == QLatin1Char('\r')) || (c == QLatin1Char('\r') && tmpl[i + 1] == QLatin1Char('\n')))) {
                // skip one more
                i += 1;
            }
            dnl = false;
        } else {
            plainBody.append(c);
            if (c == QLatin1Char('\n') || c == QLatin1Char('\r')) {
                htmlBody.append(QLatin1String("<br />"));
                htmlBody.append(c);
                if (tmpl.size() > i + 1
                    && ((c == QLatin1Char('\n') && tmpl[i + 1] == QLatin1Char('\r')) || (c == QLatin1Char('\r') && tmpl[i + 1] == QLatin1Char('\n')))) {
                    htmlBody.append(tmpl[i + 1]);
                    plainBody.append(tmpl[i + 1]);
                    i += 1;
                }
            } else {
                htmlBody.append(c);
            }
        }
    }
    // Clear the HTML body if FORCEDPLAIN has set ReplyAsPlain, OR if,
    // there is no use of FORCED command but a configure setting has ReplyUsingHtml disabled,
    // OR the original mail has no HTML part.
    const KMime::Content *content = d->mOrigMsg->mainBodyPart("text/html");
    if (d->mQuotes == ReplyAsPlain || (!d->mReplyAsHtml && TemplateParserSettings::self()->replyUsingVisualFormat())
        || !TemplateParserSettings::self()->replyUsingVisualFormat() || (!content || !content->hasContent())) {
        htmlBody.clear();
    } else {
        makeValidHtml(htmlBody);
    }
    if (d->mMode == NewMessage && plainBody.isEmpty() && !d->mExtractHtmlInfoResult.mPlainText.isEmpty()) {
        plainBody = d->mExtractHtmlInfoResult.mPlainText;
    }

    addProcessedBodyToMessage(plainBody, htmlBody);
    Q_EMIT parsingDone(d->mForceCursorPosition);
    deleteLater();
}

QString TemplateParserJob::getPlainSignature() const
{
    const KIdentityManagement::Identity &identity = d->m_identityManager->identityForUoid(d->mIdentity);

    if (identity.isNull()) {
        return QString();
    }

    KIdentityManagement::Signature signature = const_cast<KIdentityManagement::Identity &>(identity).signature();

    if (signature.type() == KIdentityManagement::Signature::Inlined && signature.isInlinedHtml()) {
        return signature.toPlainText();
    } else {
        return signature.rawText();
    }
}

// TODO If %SIGNATURE command is on, then override it with signature from
// "KMail configure->General->identity->signature".
// There should be no two signatures.
QString TemplateParserJob::getHtmlSignature() const
{
    const KIdentityManagement::Identity &identity = d->m_identityManager->identityForUoid(d->mIdentity);
    if (identity.isNull()) {
        return QString();
    }

    KIdentityManagement::Signature signature = const_cast<KIdentityManagement::Identity &>(identity).signature();

    if (!signature.isInlinedHtml()) {
        signature = signature.rawText().toHtmlEscaped();
        return signature.rawText().replace(QLatin1Char('\n'), QStringLiteral("<br />"));
    }
    return signature.rawText();
}

void TemplateParserJob::addProcessedBodyToMessage(const QString &plainBody, const QString &htmlBody) const
{
    MessageCore::ImageCollector ic;
    ic.collectImagesFrom(d->mOrigMsg.data());

    // Now, delete the old content and set the new content, which
    // is either only the new text or the new text with some attachments.
    const auto parts = d->mMsg->contents();
    for (KMime::Content *content : parts) {
        d->mMsg->removeContent(content, true /*delete*/);
    }

    // Set To and CC from the template
    if (!d->mTo.isEmpty()) {
        d->mMsg->to()->fromUnicodeString(d->mMsg->to()->asUnicodeString() + QLatin1Char(',') + d->mTo, "utf-8");
    }

    if (!d->mCC.isEmpty()) {
        d->mMsg->cc()->fromUnicodeString(d->mMsg->cc()->asUnicodeString() + QLatin1Char(',') + d->mCC, "utf-8");
    }

    d->mMsg->contentType()->clear(); // to get rid of old boundary

    // const QByteArray boundary = KMime::multiPartBoundary();
    KMime::Content *const mainTextPart = htmlBody.isEmpty() ? createPlainPartContent(plainBody) : createMultipartAlternativeContent(plainBody, htmlBody);
    mainTextPart->assemble();

    KMime::Content *textPart = mainTextPart;
    if (!ic.images().empty()) {
        textPart = createMultipartRelated(ic, mainTextPart);
        textPart->assemble();
    }

    // If we have some attachments, create a multipart/mixed mail and
    // add the normal body as well as the attachments
    KMime::Content *mainPart = textPart;
    if (d->mMode == Forward) {
        auto attachments = d->mOrigMsg->attachments();
        attachments += d->mOtp->nodeHelper()->attachmentsOfExtraContents();
        if (!attachments.isEmpty()) {
            mainPart = createMultipartMixed(attachments, textPart);
            mainPart->assemble();
        }
    }

    d->mMsg->setBody(mainPart->encodedBody());
    d->mMsg->setHeader(mainPart->contentType());
    d->mMsg->setHeader(mainPart->contentTransferEncoding());
    d->mMsg->assemble();
    d->mMsg->parse();
}

KMime::Content *TemplateParserJob::createMultipartMixed(const QVector<KMime::Content *> &attachments, KMime::Content *textPart) const
{
    auto mixedPart = new KMime::Content(d->mMsg.data());
    const QByteArray boundary = KMime::multiPartBoundary();
    auto contentType = mixedPart->contentType();
    contentType->setMimeType("multipart/mixed");
    contentType->setBoundary(boundary);
    mixedPart->contentTransferEncoding()->setEncoding(KMime::Headers::CE7Bit);
    mixedPart->addContent(textPart);

    int attachmentNumber = 1;
    for (KMime::Content *attachment : attachments) {
        mixedPart->addContent(attachment);
        // If the content type has no name or filename parameter, add one, since otherwise the name
        // would be empty in the attachment view of the composer, which looks confusing
        if (auto ct = attachment->contentType(false)) {
            if (!ct->hasParameter(QStringLiteral("name")) && !ct->hasParameter(QStringLiteral("filename"))) {
                ct->setParameter(QStringLiteral("name"), i18nc("@item:intext", "Attachment %1", attachmentNumber));
            }
        }
        ++attachmentNumber;
    }
    return mixedPart;
}

KMime::Content *TemplateParserJob::createMultipartRelated(const MessageCore::ImageCollector &ic, KMime::Content *mainTextPart) const
{
    auto relatedPart = new KMime::Content(d->mMsg.data());
    const QByteArray boundary = KMime::multiPartBoundary();
    auto contentType = relatedPart->contentType();
    contentType->setMimeType("multipart/related");
    contentType->setBoundary(boundary);
    relatedPart->contentTransferEncoding()->setEncoding(KMime::Headers::CE7Bit);
    relatedPart->addContent(mainTextPart);
    for (KMime::Content *image : ic.images()) {
        qCWarning(TEMPLATEPARSER_LOG) << "Adding" << image->contentID() << "as an embedded image";
        relatedPart->addContent(image);
    }
    return relatedPart;
}

KMime::Content *TemplateParserJob::createPlainPartContent(const QString &plainBody) const
{
    auto textPart = new KMime::Content(d->mMsg.data());
    auto ct = textPart->contentType(true);
    ct->setMimeType("text/plain");
    QTextCodec *charset = selectCharset(d->mCharsets, plainBody);
    ct->setCharset(charset->name());
    textPart->contentTransferEncoding()->setEncoding(KMime::Headers::CE8Bit);
    textPart->fromUnicodeString(plainBody);
    return textPart;
}

KMime::Content *TemplateParserJob::createMultipartAlternativeContent(const QString &plainBody, const QString &htmlBody) const
{
    auto multipartAlternative = new KMime::Content(d->mMsg.data());
    multipartAlternative->contentType()->setMimeType("multipart/alternative");
    const QByteArray boundary = KMime::multiPartBoundary();
    multipartAlternative->contentType(false)->setBoundary(boundary); // Already created

    KMime::Content *textPart = createPlainPartContent(plainBody);
    multipartAlternative->addContent(textPart);

    auto htmlPart = new KMime::Content(d->mMsg.data());
    htmlPart->contentType(true)->setMimeType("text/html");
    QTextCodec *charset = selectCharset(d->mCharsets, htmlBody);
    htmlPart->contentType(false)->setCharset(charset->name()); // Already created
    htmlPart->contentTransferEncoding()->setEncoding(KMime::Headers::CE8Bit);
    htmlPart->fromUnicodeString(htmlBody);
    multipartAlternative->addContent(htmlPart);

    return multipartAlternative;
}

QString TemplateParserJob::findCustomTemplate(const QString &tmplName)
{
    CTemplates t(tmplName);
    d->mTo = t.to();
    d->mCC = t.cC();
    const QString content = t.content();
    if (!content.isEmpty()) {
        return content;
    } else {
        return findTemplate();
    }
}

QString TemplateParserJob::findTemplate()
{
    // qCDebug(TEMPLATEPARSER_LOG) << "Trying to find template for mode" << mode;

    QString tmpl;

    qCDebug(TEMPLATEPARSER_LOG) << "Folder identify found:" << d->mFolder;
    if (d->mFolder >= 0) { // only if a folder was found
        QString fid = QString::number(d->mFolder);
        Templates fconf(fid);
        if (fconf.useCustomTemplates()) { // does folder use custom templates?
            switch (d->mMode) {
            case NewMessage:
                tmpl = fconf.templateNewMessage();
                break;
            case Reply:
                tmpl = fconf.templateReply();
                break;
            case ReplyAll:
                tmpl = fconf.templateReplyAll();
                break;
            case Forward:
                tmpl = fconf.templateForward();
                break;
            default:
                qCDebug(TEMPLATEPARSER_LOG) << "Unknown message mode:" << d->mMode;
                return QString();
            }
            d->mQuoteString = fconf.quoteString();
            if (!tmpl.isEmpty()) {
                return tmpl; // use folder-specific template
            }
        }
    }

    if (!d->mIdentity) { // find identity message belongs to
        d->mIdentity = identityUoid(d->mMsg);
        if (!d->mIdentity && d->mOrigMsg) {
            d->mIdentity = identityUoid(d->mOrigMsg);
        }
        d->mIdentity = d->m_identityManager->identityForUoidOrDefault(d->mIdentity).uoid();
        if (!d->mIdentity) {
            qCDebug(TEMPLATEPARSER_LOG) << "Oops! No identity for message";
        }
    }
    qCDebug(TEMPLATEPARSER_LOG) << "Identity found:" << d->mIdentity;

    QString iid;
    if (d->mIdentity) {
        iid = TemplatesConfiguration::configIdString(d->mIdentity); // templates ID for that identity
    } else {
        iid = QStringLiteral("IDENTITY_NO_IDENTITY"); // templates ID for no identity
    }

    Templates iconf(iid);
    if (iconf.useCustomTemplates()) { // does identity use custom templates?
        switch (d->mMode) {
        case NewMessage:
            tmpl = iconf.templateNewMessage();
            break;
        case Reply:
            tmpl = iconf.templateReply();
            break;
        case ReplyAll:
            tmpl = iconf.templateReplyAll();
            break;
        case Forward:
            tmpl = iconf.templateForward();
            break;
        default:
            qCDebug(TEMPLATEPARSER_LOG) << "Unknown message mode:" << d->mMode;
            return QString();
        }
        d->mQuoteString = iconf.quoteString();
        if (!tmpl.isEmpty()) {
            return tmpl; // use identity-specific template
        }
    }

    switch (d->mMode) { // use the global template
    case NewMessage:
        tmpl = TemplateParserSettings::self()->templateNewMessage();
        break;
    case Reply:
        tmpl = TemplateParserSettings::self()->templateReply();
        break;
    case ReplyAll:
        tmpl = TemplateParserSettings::self()->templateReplyAll();
        break;
    case Forward:
        tmpl = TemplateParserSettings::self()->templateForward();
        break;
    default:
        qCDebug(TEMPLATEPARSER_LOG) << "Unknown message mode:" << d->mMode;
        return QString();
    }

    d->mQuoteString = TemplateParserSettings::self()->quoteString();
    return tmpl;
}

QString TemplateParserJob::pipe(const QString &cmd, const QString &buf)
{
    KProcess process;
    bool success;

    process.setOutputChannelMode(KProcess::SeparateChannels);
    process.setShellCommand(cmd);
    process.start();
    if (process.waitForStarted(pipeTimeout())) {
        bool finished = false;
        if (!buf.isEmpty()) {
            process.write(buf.toLatin1());
        }
        if (buf.isEmpty() || process.waitForBytesWritten(pipeTimeout())) {
            if (!buf.isEmpty()) {
                process.closeWriteChannel();
            }
            if (process.waitForFinished(pipeTimeout())) {
                success = (process.exitStatus() == QProcess::NormalExit);
                finished = true;
            } else {
                finished = false;
                success = false;
            }
        } else {
            success = false;
            finished = false;
        }

        // The process has started, but did not finish in time. Kill it.
        if (!finished) {
            process.kill();
        }
    } else {
        success = false;
    }

    if (!success && d->mDebug) {
        KMessageBox::error(nullptr, xi18nc("@info", "Pipe command <command>%1</command> failed.", cmd));
    }

    if (success) {
        return QTextCodec::codecForLocale()->toUnicode(process.readAllStandardOutput());
    } else {
        return QString();
    }
}

void TemplateParserJob::setWordWrap(bool wrap, int wrapColWidth)
{
    d->mWrap = wrap;
    d->mColWrap = wrapColWidth;
}

QString TemplateParserJob::plainMessageText(bool aStripSignature, AllowSelection isSelectionAllowed) const
{
    if (!d->mSelection.isEmpty() && (isSelectionAllowed == SelectionAllowed)) {
        return d->mSelection;
    }

    if (!d->mOrigMsg) {
        return QString();
    }
    const auto mp = toplevelTextNode(d->mOtp->parsedPart());
    QString result = mp->plaintextContent();
    if (result.isEmpty()) {
        result = d->mExtractHtmlInfoResult.mPlainText;
    }
    if (aStripSignature) {
        result = MessageCore::StringUtil::stripSignature(result);
    }

    return result;
}

QString TemplateParserJob::htmlMessageText(bool aStripSignature, AllowSelection isSelectionAllowed)
{
    if (!d->mSelection.isEmpty() && (isSelectionAllowed == SelectionAllowed)) {
        // TODO implement d->mSelection for HTML
        return d->mSelection;
    }
    d->mHeadElement = d->mExtractHtmlInfoResult.mHeaderElement;
    const QString bodyElement = d->mExtractHtmlInfoResult.mBodyElement;
    if (!bodyElement.isEmpty()) {
        if (aStripSignature) {
            // FIXME strip signature works partially for HTML mails
            return MessageCore::StringUtil::stripSignature(bodyElement);
        }
        return bodyElement;
    }

    if (aStripSignature) {
        // FIXME strip signature works partially for HTML mails
        return MessageCore::StringUtil::stripSignature(d->mExtractHtmlInfoResult.mHtmlElement);
    }
    return d->mExtractHtmlInfoResult.mHtmlElement;
}

QString TemplateParserJob::quotedPlainText(const QString &selection) const
{
    QString content = TemplateParser::Util::removeSpaceAtBegin(selection);

    const QString indentStr = MessageCore::StringUtil::formatQuotePrefix(d->mQuoteString, d->mOrigMsg->from()->displayString());
    if (TemplateParserSettings::self()->smartQuote() && d->mWrap) {
        content = MessageCore::StringUtil::smartQuote(content, d->mColWrap - indentStr.length());
    }
    content.replace(QLatin1Char('\n'), QLatin1Char('\n') + indentStr);
    content.prepend(indentStr);
    content += QLatin1Char('\n');

    return content;
}

QString TemplateParserJob::quotedHtmlText(const QString &selection) const
{
    QString content = selection;
    // TODO 1) look for all the variations of <br>  and remove the blank lines
    // 2) implement vertical bar for quoted HTML mail.
    // 3) After vertical bar is implemented, If a user wants to edit quoted message,
    // then the <blockquote> tags below should open and close as when required.

    // Add blockquote tag, so that quoted message can be differentiated from normal message
    content = QLatin1String("<blockquote>") + content + QLatin1String("</blockquote>");
    return content;
}

uint TemplateParserJob::identityUoid(const KMime::Message::Ptr &msg) const
{
    QString idString;
    if (auto hrd = msg->headerByType("X-KMail-Identity")) {
        idString = hrd->asUnicodeString().trimmed();
    }
    bool ok = false;
    unsigned int id = idString.toUInt(&ok);

    if (!ok || id == 0) {
        id = d->m_identityManager->identityForAddress(msg->to()->asUnicodeString() + QLatin1String(", ") + msg->cc()->asUnicodeString()).uoid();
    }

    return id;
}

bool TemplateParserJob::isHtmlSignature() const
{
    const KIdentityManagement::Identity &identity = d->m_identityManager->identityForUoid(d->mIdentity);

    if (identity.isNull()) {
        return false;
    }

    const KIdentityManagement::Signature signature = const_cast<KIdentityManagement::Identity &>(identity).signature();

    return signature.isInlinedHtml();
}

QString TemplateParserJob::plainTextToHtml(const QString &body)
{
    QString str = body;
    str = str.toHtmlEscaped();
    str.replace(QLatin1Char('\n'), QStringLiteral("<br />\n"));
    return str;
}

void TemplateParserJob::makeValidHtml(QString &body)
{
    if (body.isEmpty()) {
        return;
    }

    QRegularExpression regEx;

    regEx.setPattern(QStringLiteral("<html.*?>"));
    if (!body.contains(regEx)) {
        regEx.setPattern(QStringLiteral("<body.*?>"));
        if (!body.contains(regEx)) {
            body = QLatin1String("<body>") + body + QLatin1String("<br/></body>");
        }
        regEx.setPattern(QStringLiteral("<head.*?>"));
        if (!body.contains(regEx)) {
            body = QLatin1String("<head>") + d->mHeadElement + QLatin1String("</head>") + body;
        }
        body = QLatin1String("<html>") + body + QLatin1String("</html>");
    }
}
