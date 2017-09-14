/*
   Copyright (C) 2017 Laurent Montel <montel@kde.org>

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

#include "templateparserjob.h"
#include "templateparserextracthtmlinfo.h"
#include "globalsettings_templateparser.h"
#include "customtemplates_kfg.h"
#include "templatesconfiguration_kfg.h"
#include "templatesconfiguration.h"

#include <MessageCore/ImageCollector>
#include <MessageCore/StringUtil>

#include <MimeTreeParser/ObjectTreeParser>

#include <KIdentityManagement/Identity>
#include <KIdentityManagement/IdentityManager>

#include <KCharsets>
#include <KLocalizedString>
#include <KMessageBox>
#include <KProcess>
#include <KShell>
#include "templateparser_debug.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QLocale>

namespace {
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

TemplateParserJob::TemplateParserJob(const KMime::Message::Ptr &amsg, const Mode amode)
    : mMode(amode)
    , mIdentity(0)
    , mAllowDecryption(true)
    , mDebug(false)
    , mQuoteString(QStringLiteral("> "))
    , m_identityManager(nullptr)
    , mWrap(true)
    , mColWrap(80)
    , mQuotes(ReplyAsOriginalMessage)
    , mForceCursorPosition(false)
{
    mMsg = amsg;

    mEmptySource = new MessageViewer::EmptySource;
    mEmptySource->setAllowDecryption(mAllowDecryption);

    mOtp = new MimeTreeParser::ObjectTreeParser(mEmptySource);
    mOtp->setAllowAsync(false);
}

TemplateParserJob::~TemplateParserJob()
{
    delete mEmptySource;
    delete mOtp;
}

void TemplateParserJob::setSelection(const QString &selection)
{
    mSelection = selection;
}

void TemplateParserJob::setAllowDecryption(const bool allowDecryption)
{
    mAllowDecryption = allowDecryption;
    mEmptySource->setAllowDecryption(mAllowDecryption);
}

bool TemplateParserJob::shouldStripSignature() const
{
    // Only strip the signature when replying, it should be preserved when forwarding
    return (mMode == Reply || mMode == ReplyAll) && TemplateParserSettings::self()->stripSignature();
}

void TemplateParserJob::setIdentityManager(KIdentityManagement::IdentityManager *ident)
{
    m_identityManager = ident;
}

void TemplateParserJob::setCharsets(const QStringList &charsets)
{
    m_charsets = charsets;
}

int TemplateParserJob::parseQuotes(const QString &prefix, const QString &str, QString &quote)
{
    int pos = prefix.length();
    int len;
    const int str_len = str.length();

    // Also allow the german lower double-quote sign as quote separator, not only
    // the standard ASCII quote ("). This fixes bug 166728.
    const QList< QChar > quoteChars = {QLatin1Char('"'), 0x201C};

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

QString TemplateParserJob::getFirstName(const QString &str)
{
    // simple logic:
    // if there is ',' in name, than format is 'Last, First'
    // else format is 'First Last'
    // last resort -- return 'name' from 'name@domain'
    int sep_pos;
    QString res;
    if ((sep_pos = str.indexOf(QLatin1Char('@'))) > 0) {
        int i;
        for (i = (sep_pos - 1); i >= 0; --i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                res.prepend(c);
            } else {
                break;
            }
        }
    } else if ((sep_pos = str.indexOf(QLatin1Char(','))) > 0) {
        int i;
        bool begin = false;
        const int strLength(str.length());
        for (i = sep_pos; i < strLength; ++i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                begin = true;
                res.append(c);
            } else if (begin) {
                break;
            }
        }
    } else {
        int i;
        const int strLength(str.length());
        for (i = 0; i < strLength; ++i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                res.append(c);
            } else {
                break;
            }
        }
    }
    return res;
}

QString TemplateParserJob::getLastName(const QString &str)
{
    // simple logic:
    // if there is ',' in name, than format is 'Last, First'
    // else format is 'First Last'
    int sep_pos;
    QString res;
    if ((sep_pos = str.indexOf(QLatin1Char(','))) > 0) {
        int i;
        for (i = sep_pos; i >= 0; --i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                res.prepend(c);
            } else {
                break;
            }
        }
    } else {
        if ((sep_pos = str.indexOf(QLatin1Char(' '))) > 0) {
            bool begin = false;
            const int strLength(str.length());
            for (int i = sep_pos; i < strLength; ++i) {
                QChar c = str[i];
                if (c.isLetterOrNumber()) {
                    begin = true;
                    res.append(c);
                } else if (begin) {
                    break;
                }
            }
        }
    }
    return res;
}

void TemplateParserJob::process(const KMime::Message::Ptr &aorig_msg, qint64 afolder)
{
    if (aorig_msg == nullptr) {
        qCDebug(TEMPLATEPARSER_LOG) << "aorig_msg == 0!";
        Q_EMIT parsingDone(mForceCursorPosition);
        return;
    }

    mOrigMsg = aorig_msg;
    mFolder = afolder;
    const QString tmpl = findTemplate();
    if (tmpl.isEmpty()) {
        Q_EMIT parsingDone(mForceCursorPosition);
        return;
    }
    processWithTemplate(tmpl);
}

void TemplateParserJob::process(const QString &tmplName, const KMime::Message::Ptr &aorig_msg, qint64 afolder)
{
    mForceCursorPosition = false;
    mOrigMsg = aorig_msg;
    mFolder = afolder;
    const QString tmpl = findCustomTemplate(tmplName);
    processWithTemplate(tmpl);
}

void TemplateParserJob::processWithIdentity(uint uoid, const KMime::Message::Ptr &aorig_msg, qint64 afolder)
{
    mIdentity = uoid;
    process(aorig_msg, afolder);
}

void TemplateParserJob::processWithTemplate(const QString &tmpl)
{
    mOtp->parseObjectTree(mOrigMsg.data());

    TemplateParserExtractHtmlInfo *job = new TemplateParserExtractHtmlInfo(this);
    connect(job, &TemplateParserExtractHtmlInfo::finished, this, &TemplateParserJob::slotExtractInfoDone);

    QString plainText = mOtp->plainTextContent();
    if (plainText.isEmpty()) {   //HTML-only mails
        plainText = mOtp->htmlContent();
    }

    job->setHtmlForExtractingTextPlain(plainText);
    job->setTemplate(tmpl);

    QString mHtmlElement = mOtp->htmlContent();

    if (mHtmlElement.isEmpty()) {   //plain mails only
        QString htmlReplace = mOtp->plainTextContent().toHtmlEscaped();
        htmlReplace = htmlReplace.replace(QLatin1Char('\n'), QStringLiteral("<br />"));
        mHtmlElement = QStringLiteral("<html><head></head><body>%1</body></html>\n").arg(htmlReplace);
    }

    job->setHtmlForExtractionHeaderAndBody(mHtmlElement);
    job->start();
}

void TemplateParserJob::slotExtractInfoDone(const TemplateParserExtractHtmlInfoResult &result)
{
    mExtractHtmlInfoResult = result;
    const QString tmpl = mExtractHtmlInfoResult.mTemplate;
    const int tmpl_len = tmpl.length();
    QString plainBody, htmlBody;

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
            } else if (cmd.startsWith(QStringLiteral("REM="))) {
                // comments
                qCDebug(TEMPLATEPARSER_LOG) << "Command: REM=";
                QString q;
                const int len = parseQuotes(QStringLiteral("REM="), cmd, q);
                i += len;
            } else if (cmd.startsWith(QStringLiteral("LANGUAGE="))) {
                QString q;
                const int len = parseQuotes(QStringLiteral("LANGUAGE="), cmd, q);
                i += len;
                if (!q.isEmpty()) {
                    definedLocale = QLocale(q);
                }
            } else if (cmd.startsWith(QStringLiteral("DICTIONARYLANGUAGE="))) {
                QString q;
                const int len = parseQuotes(QStringLiteral("DICTIONARYLANGUAGE="), cmd, q);
                i += len;
                if (!q.isEmpty()) {
                    KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-Dictionary");
                    header->fromUnicodeString(q, "utf-8");
                    mMsg->setHeader(header);
                }
            } else if (cmd.startsWith(QStringLiteral("INSERT=")) || cmd.startsWith(QStringLiteral("PUT="))) {
                QString q;
                int len = 0;
                if (cmd.startsWith(QStringLiteral("INSERT="))) {
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
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                } else if (mDebug) {
                    KMessageBox::error(
                        nullptr,
                        i18nc("@info",
                              "Cannot insert content from file %1: %2", path, file.errorString()));
                }
            } else if (cmd.startsWith(QStringLiteral("SYSTEM="))) {
                // insert content of specified file as is
                qCDebug(TEMPLATEPARSER_LOG) << "Command: SYSTEM=";
                QString q;
                const int len = parseQuotes(QStringLiteral("SYSTEM="), cmd, q);
                i += len;
                const QString pipe_cmd = q;
                const QString str = pipe(pipe_cmd, QString());
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("QUOTEPIPE="))) {
                // pipe message body through command and insert it as quotation
                qCDebug(TEMPLATEPARSER_LOG) << "Command: QUOTEPIPE=";
                QString q;
                const int len = parseQuotes(QStringLiteral("QUOTEPIPE="), cmd, q);
                i += len;
                const QString pipe_cmd = q;
                if (mOrigMsg) {
                    const QString plainStr
                        = pipe(pipe_cmd, plainMessageText(shouldStripSignature(), NoSelectionAllowed));
                    QString plainQuote = quotedPlainText(plainStr);
                    if (plainQuote.endsWith(QLatin1Char('\n'))) {
                        plainQuote.chop(1);
                    }
                    plainBody.append(plainQuote);

                    const QString htmlStr
                        = pipe(pipe_cmd, htmlMessageText(shouldStripSignature(), NoSelectionAllowed));
                    const QString htmlQuote = quotedHtmlText(htmlStr);
                    htmlBody.append(htmlQuote);
                }
            } else if (cmd.startsWith(QStringLiteral("QUOTE"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: QUOTE";
                i += strlen("QUOTE");
                if (mOrigMsg) {
                    QString plainQuote
                        = quotedPlainText(plainMessageText(shouldStripSignature(), SelectionAllowed));
                    if (plainQuote.endsWith(QLatin1Char('\n'))) {
                        plainQuote.chop(1);
                    }
                    plainBody.append(plainQuote);

                    const QString htmlQuote
                        = quotedHtmlText(htmlMessageText(shouldStripSignature(), SelectionAllowed));
                    htmlBody.append(htmlQuote);
                }
            } else if (cmd.startsWith(QStringLiteral("FORCEDPLAIN"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FORCEDPLAIN";
                mQuotes = ReplyAsPlain;
                i += strlen("FORCEDPLAIN");
            } else if (cmd.startsWith(QStringLiteral("FORCEDHTML"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FORCEDHTML";
                mQuotes = ReplyAsHtml;
                i += strlen("FORCEDHTML");
            } else if (cmd.startsWith(QStringLiteral("QHEADERS"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: QHEADERS";
                i += strlen("QHEADERS");
                if (mOrigMsg) {
                    QString plainQuote
                        = quotedPlainText(QString::fromLatin1(MessageCore::StringUtil::headerAsSendableString(mOrigMsg)));
                    if (plainQuote.endsWith(QLatin1Char('\n'))) {
                        plainQuote.chop(1);
                    }
                    plainBody.append(plainQuote);

                    const QString htmlQuote
                        = quotedHtmlText(QString::fromLatin1(MessageCore::StringUtil::headerAsSendableString(mOrigMsg)));
                    const QString str = plainToHtml(htmlQuote);
                    htmlBody.append(str);
                }
            } else if (cmd.startsWith(QStringLiteral("HEADERS"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: HEADERS";
                i += strlen("HEADERS");
                if (mOrigMsg) {
                    const QString str = QString::fromLatin1(MessageCore::StringUtil::headerAsSendableString(mOrigMsg));
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("TEXTPIPE="))) {
                // pipe message body through command and insert it as is
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TEXTPIPE=";
                QString q;
                const int len = parseQuotes(QStringLiteral("TEXTPIPE="), cmd, q);
                i += len;
                const QString pipe_cmd = q;
                if (mOrigMsg) {
                    const QString plainStr
                        = pipe(pipe_cmd, plainMessageText(shouldStripSignature(), NoSelectionAllowed));
                    plainBody.append(plainStr);

                    const QString htmlStr
                        = pipe(pipe_cmd, htmlMessageText(shouldStripSignature(), NoSelectionAllowed));
                    htmlBody.append(htmlStr);
                }
            } else if (cmd.startsWith(QStringLiteral("MSGPIPE="))) {
                // pipe full message through command and insert result as is
                qCDebug(TEMPLATEPARSER_LOG) << "Command: MSGPIPE=";
                QString q;
                const int len = parseQuotes(QStringLiteral("MSGPIPE="), cmd, q);
                i += len;
                if (mOrigMsg) {
                    QString pipe_cmd = q;
                    const QString str = pipe(pipe_cmd, QString::fromLatin1(mOrigMsg->encodedContent()));
                    plainBody.append(str);

                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("BODYPIPE="))) {
                // pipe message body generated so far through command and insert result as is
                qCDebug(TEMPLATEPARSER_LOG) << "Command: BODYPIPE=";
                QString q;
                const int len = parseQuotes(QStringLiteral("BODYPIPE="), cmd, q);
                i += len;
                const QString pipe_cmd = q;
                const QString plainStr = pipe(pipe_cmd, plainBody);
                plainBody.append(plainStr);

                const QString htmlStr = pipe(pipe_cmd, htmlBody);
                const QString body = plainToHtml(htmlStr);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("CLEARPIPE="))) {
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

                KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-CursorPos");
                header->fromUnicodeString(QString::number(0), "utf-8");
                mMsg->setHeader(header);
            } else if (cmd.startsWith(QStringLiteral("TEXT"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TEXT";
                i += strlen("TEXT");
                if (mOrigMsg) {
                    const QString plainStr = plainMessageText(shouldStripSignature(), NoSelectionAllowed);
                    plainBody.append(plainStr);

                    const QString htmlStr = htmlMessageText(shouldStripSignature(), NoSelectionAllowed);
                    htmlBody.append(htmlStr);
                }
            } else if (cmd.startsWith(QStringLiteral("OTEXTSIZE"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTEXTSIZE";
                i += strlen("OTEXTSIZE");
                if (mOrigMsg) {
                    const QString str = QStringLiteral("%1").arg(mOrigMsg->body().length());
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OTEXT"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTEXT";
                i += strlen("OTEXT");
                if (mOrigMsg) {
                    const QString plainStr = plainMessageText(shouldStripSignature(), NoSelectionAllowed);
                    plainBody.append(plainStr);

                    const QString htmlStr = htmlMessageText(shouldStripSignature(), NoSelectionAllowed);
                    htmlBody.append(htmlStr);
                }
            } else if (cmd.startsWith(QStringLiteral("OADDRESSEESADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OADDRESSEESADDR";
                i += strlen("OADDRESSEESADDR");
                if (mOrigMsg) {
                    const QString to = mOrigMsg->to()->asUnicodeString();
                    const QString cc = mOrigMsg->cc()->asUnicodeString();
                    if (!to.isEmpty()) {
                        const QString toLine = i18nc("@item:intext email To", "To:") + QLatin1Char(' ') + to;
                        plainBody.append(toLine);
                        const QString body = plainToHtml(toLine);
                        htmlBody.append(body);
                    }
                    if (!to.isEmpty() && !cc.isEmpty()) {
                        plainBody.append(QLatin1Char('\n'));
                        const QString str = plainToHtml(QString(QLatin1Char('\n')));
                        htmlBody.append(str);
                    }
                    if (!cc.isEmpty()) {
                        const QString ccLine = i18nc("@item:intext email CC", "CC:") + QLatin1Char(' ') +  cc;
                        plainBody.append(ccLine);
                        const QString str = plainToHtml(ccLine);
                        htmlBody.append(str);
                    }
                }
            } else if (cmd.startsWith(QStringLiteral("CCADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CCADDR";
                i += strlen("CCADDR");
                const QString str = mMsg->cc()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("CCNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CCNAME";
                i += strlen("CCNAME");
                const QString str = mMsg->cc()->displayString();
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("CCFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CCFNAME";
                i += strlen("CCFNAME");
                const QString str = mMsg->cc()->displayString();
                plainBody.append(getFirstName(str));
                const QString body = plainToHtml(getFirstName(str));
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("CCLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CCLNAME";
                i += strlen("CCLNAME");
                const QString str = mMsg->cc()->displayString();
                plainBody.append(getLastName(str));
                const QString body = plainToHtml(getLastName(str));
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("TOADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TOADDR";
                i += strlen("TOADDR");
                const QString str = mMsg->to()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("TONAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TONAME";
                i += strlen("TONAME");
                const QString str = (mMsg->to()->displayString());
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("TOFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TOFNAME";
                i += strlen("TOFNAME");
                const QString str = mMsg->to()->displayString();
                plainBody.append(getFirstName(str));
                const QString body = plainToHtml(getFirstName(str));
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("TOLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TOLNAME";
                i += strlen("TOLNAME");
                const QString str = mMsg->to()->displayString();
                plainBody.append(getLastName(str));
                const QString body = plainToHtml(getLastName(str));
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("TOLIST"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TOLIST";
                i += strlen("TOLIST");
                const QString str = mMsg->to()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("FROMADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FROMADDR";
                i += strlen("FROMADDR");
                const QString str = mMsg->from()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("FROMNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FROMNAME";
                i += strlen("FROMNAME");
                const QString str = mMsg->from()->displayString();
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("FROMFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FROMFNAME";
                i += strlen("FROMFNAME");
                const QString str = mMsg->from()->displayString();
                plainBody.append(getFirstName(str));
                const QString body = plainToHtml(getFirstName(str));
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("FROMLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: FROMLNAME";
                i += strlen("FROMLNAME");
                const QString str = mMsg->from()->displayString();
                plainBody.append(getLastName(str));
                const QString body = plainToHtml(getLastName(str));
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("FULLSUBJECT")) || cmd.startsWith(QStringLiteral("FULLSUBJ"))) {
                if (cmd.startsWith(QStringLiteral("FULLSUBJ"))) {
                    qCDebug(TEMPLATEPARSER_LOG) << "Command: FULLSUBJ";
                    i += strlen("FULLSUBJ");
                } else {
                    qCDebug(TEMPLATEPARSER_LOG) << "Command: FULLSUBJECT";
                    i += strlen("FULLSUBJECT");
                }
                const QString str = mMsg->subject()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("MSGID"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: MSGID";
                i += strlen("MSGID");
                const QString str = mMsg->messageID()->asUnicodeString();
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("OHEADER="))) {
                // insert specified content of header from original message
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OHEADER=";
                QString q;
                const int len = parseQuotes(QStringLiteral("OHEADER="), cmd, q);
                i += len;
                if (mOrigMsg) {
                    const QString hdr = q;
                    QString str;
                    if (auto hrdMsgOrigin = mOrigMsg->headerByType(hdr.toLocal8Bit().constData())) {
                        str = hrdMsgOrigin->asUnicodeString();
                    }
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("HEADER="))) {
                // insert specified content of header from current message
                qCDebug(TEMPLATEPARSER_LOG) << "Command: HEADER=";
                QString q;
                const int len = parseQuotes(QStringLiteral("HEADER="), cmd, q);
                i += len;
                const QString hdr = q;
                QString str;
                if (auto hrdMsgOrigin = mOrigMsg->headerByType(hdr.toLocal8Bit().constData())) {
                    str = hrdMsgOrigin->asUnicodeString();
                }
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("HEADER( "))) {
                // insert specified content of header from current message
                qCDebug(TEMPLATEPARSER_LOG) << "Command: HEADER(";
                QRegExp re = QRegExp(QLatin1String("^HEADER\\((.+)\\)"));
                re.setMinimal(true);
                int res = re.indexIn(cmd);
                if (res != 0) {
                    // something wrong
                    i += strlen("HEADER( ");
                } else {
                    i += re.matchedLength();
                    const QString hdr = re.cap(1);
                    QString str;
                    if (auto hrdMsgOrigin = mOrigMsg->headerByType(hdr.toLocal8Bit().constData())) {
                        str = hrdMsgOrigin->asUnicodeString();
                    }
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OCCADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OCCADDR";
                i += strlen("OCCADDR");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->cc()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OCCNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OCCNAME";
                i += strlen("OCCNAME");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->cc()->displayString();
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OCCFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OCCFNAME";
                i += strlen("OCCFNAME");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->cc()->displayString();
                    plainBody.append(getFirstName(str));
                    const QString body = plainToHtml(getFirstName(str));
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OCCLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OCCLNAME";
                i += strlen("OCCLNAME");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->cc()->displayString();
                    plainBody.append(getLastName(str));
                    const QString body = plainToHtml(getLastName(str));
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OTOADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTOADDR";
                i += strlen("OTOADDR");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->to()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OTONAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTONAME";
                i += strlen("OTONAME");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->to()->displayString();
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OTOFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTOFNAME";
                i += strlen("OTOFNAME");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->to()->displayString();
                    plainBody.append(getFirstName(str));
                    const QString body = plainToHtml(getFirstName(str));
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OTOLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTOLNAME";
                i += strlen("OTOLNAME");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->to()->displayString();
                    plainBody.append(getLastName(str));
                    const QString body = plainToHtml(getLastName(str));
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OTOLIST"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTOLIST";
                i += strlen("OTOLIST");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->to()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OTO"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTO";
                i += strlen("OTO");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->to()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OFROMADDR"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OFROMADDR";
                i += strlen("OFROMADDR");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->from()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OFROMNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OFROMNAME";
                i += strlen("OFROMNAME");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->from()->displayString();
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OFROMFNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OFROMFNAME";
                i += strlen("OFROMFNAME");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->from()->displayString();
                    plainBody.append(getFirstName(str));
                    const QString body = plainToHtml(getFirstName(str));
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OFROMLNAME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OFROMLNAME";
                i += strlen("OFROMLNAME");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->from()->displayString();
                    plainBody.append(getLastName(str));
                    const QString body = plainToHtml(getLastName(str));
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OFULLSUBJECT")) || cmd.startsWith(QStringLiteral("OFULLSUBJ"))) {
                if (cmd.startsWith(QStringLiteral("OFULLSUBJECT"))) {
                    qCDebug(TEMPLATEPARSER_LOG) << "Command: OFULLSUBJECT";
                    i += strlen("OFULLSUBJECT");
                } else {
                    qCDebug(TEMPLATEPARSER_LOG) << "Command: OFULLSUBJ";
                    i += strlen("OFULLSUBJ");
                }
                if (mOrigMsg) {
                    const QString str = mOrigMsg->subject()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OMSGID"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OMSGID";
                i += strlen("OMSGID");
                if (mOrigMsg) {
                    const QString str = mOrigMsg->messageID()->asUnicodeString();
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("DATEEN"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DATEEN";
                i += strlen("DATEEN");
                const QDateTime date = QDateTime::currentDateTime();
                QLocale locale(QLocale::C);
                const QString str = locale.toString(date.date(), QLocale::LongFormat);
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("DATESHORT"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DATESHORT";
                i += strlen("DATESHORT");
                const QDateTime date = QDateTime::currentDateTime();
                const QString str = definedLocale.toString(date.date(), QLocale::ShortFormat);
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("DATE"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DATE";
                i += strlen("DATE");
                const QDateTime date = QDateTime::currentDateTime();
                const QString str = definedLocale.toString(date.date(), QLocale::LongFormat);
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("DOW"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DOW";
                i += strlen("DOW");
                const QDateTime date = QDateTime::currentDateTime();
                const QString str = definedLocale.dayName(date.date().dayOfWeek(), QLocale::LongFormat);
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("TIMELONGEN"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TIMELONGEN";
                i += strlen("TIMELONGEN");
                const QDateTime date = QDateTime::currentDateTime();
                QLocale locale(QLocale::C);
                const QString str = locale.toString(date.time(), QLocale::LongFormat);
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("TIMELONG"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TIMELONG";
                i += strlen("TIMELONG");
                const QDateTime date = QDateTime::currentDateTime();
                const QString str = definedLocale.toString(date.time(), QLocale::LongFormat);
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("TIME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: TIME";
                i += strlen("TIME");
                const QDateTime date = QDateTime::currentDateTime();
                const QString str = definedLocale.toString(date.time(), QLocale::ShortFormat);
                plainBody.append(str);
                const QString body = plainToHtml(str);
                htmlBody.append(body);
            } else if (cmd.startsWith(QStringLiteral("ODATEEN"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: ODATEEN";
                i += strlen("ODATEEN");
                if (mOrigMsg) {
                    const QDateTime date = mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = QLocale::c().toString(date.date(), QLocale::LongFormat);
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("ODATESHORT"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: ODATESHORT";
                i += strlen("ODATESHORT");
                if (mOrigMsg) {
                    const QDateTime date = mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = definedLocale.toString(date.date(), QLocale::ShortFormat);
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("ODATE"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: ODATE";
                i += strlen("ODATE");
                if (mOrigMsg) {
                    const QDateTime date = mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = definedLocale.toString(date.date(), QLocale::LongFormat);
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("ODOW"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: ODOW";
                i += strlen("ODOW");
                if (mOrigMsg) {
                    const QDateTime date = mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = definedLocale.dayName(date.date().dayOfWeek(), QLocale::LongFormat);
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OTIMELONGEN"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTIMELONGEN";
                i += strlen("OTIMELONGEN");
                if (mOrigMsg) {
                    const QDateTime date = mOrigMsg->date()->dateTime().toLocalTime();
                    QLocale locale(QLocale::C);
                    const QString str = locale.toString(date.time(), QLocale::LongFormat);
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OTIMELONG"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTIMELONG";
                i += strlen("OTIMELONG");
                if (mOrigMsg) {
                    const QDateTime date = mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = definedLocale.toString(date.time(), QLocale::LongFormat);
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("OTIME"))) {
                qCDebug(TEMPLATEPARSER_LOG) << "Command: OTIME";
                i += strlen("OTIME");
                if (mOrigMsg) {
                    const QDateTime date = mOrigMsg->date()->dateTime().toLocalTime();
                    const QString str = definedLocale.toString(date.time(), QLocale::ShortFormat);
                    plainBody.append(str);
                    const QString body = plainToHtml(str);
                    htmlBody.append(body);
                }
            } else if (cmd.startsWith(QStringLiteral("BLANK"))) {
                // do nothing
                qCDebug(TEMPLATEPARSER_LOG) << "Command: BLANK";
                i += strlen("BLANK");
            } else if (cmd.startsWith(QStringLiteral("NOP"))) {
                // do nothing
                qCDebug(TEMPLATEPARSER_LOG) << "Command: NOP";
                i += strlen("NOP");
            } else if (cmd.startsWith(QStringLiteral("CLEAR"))) {
                // clear body buffer; not too useful yet
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CLEAR";
                i += strlen("CLEAR");
                plainBody.clear();
                htmlBody.clear();
                KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-CursorPos");
                header->fromUnicodeString(QString::number(0), "utf-8");
                mMsg->setHeader(header);
            } else if (cmd.startsWith(QStringLiteral("DEBUGOFF"))) {
                // turn off debug
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DEBUGOFF";
                i += strlen("DEBUGOFF");
                mDebug = false;
            } else if (cmd.startsWith(QStringLiteral("DEBUG"))) {
                // turn on debug
                qCDebug(TEMPLATEPARSER_LOG) << "Command: DEBUG";
                i += strlen("DEBUG");
                mDebug = true;
            } else if (cmd.startsWith(QStringLiteral("CURSOR"))) {
                // turn on debug
                qCDebug(TEMPLATEPARSER_LOG) << "Command: CURSOR";
                int oldI = i;
                i += strlen("CURSOR");
                KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-CursorPos");
                header->fromUnicodeString(QString::number(plainBody.length()), "utf-8");
                /* if template is:
                *  FOOBAR
                *  %CURSOR
                *
                * Make sure there is an empty line for the cursor otherwise it will be placed at the end of FOOBAR
                */
                if (oldI > 0 && tmpl[ oldI - 1 ] == QLatin1Char('\n') && i == tmpl_len - 1) {
                    plainBody.append(QLatin1Char('\n'));
                }
                mMsg->setHeader(header);
                mForceCursorPosition = true;
                //FIXME HTML part for header remaining
            } else if (cmd.startsWith(QStringLiteral("SIGNATURE"))) {
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
                && ((c == QLatin1Char('\n') && tmpl[i + 1] == QLatin1Char('\r'))
                    || (c == QLatin1Char('\r') && tmpl[i + 1] == QLatin1Char('\n')))) {
                // skip one more
                i += 1;
            }
            dnl = false;
        } else {
            plainBody.append(c);
            if (c == QLatin1Char('\n') || c == QLatin1Char('\r')) {
                htmlBody.append(QStringLiteral("<br />"));
                htmlBody.append(c);
                if (tmpl.size() > i + 1
                    && ((c == QLatin1Char('\n') && tmpl[i + 1] == QLatin1Char('\r'))
                        || (c == QLatin1Char('\r') && tmpl[i + 1] == QLatin1Char('\n')))) {
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
    const KMime::Content *content = mOrigMsg->mainBodyPart("text/html");
    if (mQuotes == ReplyAsPlain
        || (mQuotes != ReplyAsHtml && !TemplateParserSettings::self()->replyUsingHtml())
        || (!content || !content->hasContent())) {
        htmlBody.clear();
    } else {
        makeValidHtml(htmlBody);
    }
    addProcessedBodyToMessage(plainBody, htmlBody);
    Q_EMIT parsingDone(mForceCursorPosition);
}

QString TemplateParserJob::getPlainSignature() const
{
    const KIdentityManagement::Identity &identity
        = m_identityManager->identityForUoid(mIdentity);

    if (identity.isNull()) {
        return QString();
    }

    KIdentityManagement::Signature signature
        = const_cast<KIdentityManagement::Identity &>(identity).signature();

    if (signature.type() == KIdentityManagement::Signature::Inlined
        && signature.isInlinedHtml()) {
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
    const KIdentityManagement::Identity &identity
        = m_identityManager->identityForUoid(mIdentity);
    if (identity.isNull()) {
        return QString();
    }

    KIdentityManagement::Signature signature
        = const_cast<KIdentityManagement::Identity &>(identity).signature();

    if (!signature.isInlinedHtml()) {
        signature = signature.rawText().toHtmlEscaped();
        return signature.rawText().replace(QLatin1Char('\n'), QStringLiteral("<br />"));
    }
    return signature.rawText();
}

void TemplateParserJob::addProcessedBodyToMessage(const QString &plainBody, const QString &htmlBody) const
{
    MessageCore::ImageCollector ic;
    ic.collectImagesFrom(mOrigMsg.data());

    // Now, delete the old content and set the new content, which
    // is either only the new text or the new text with some attachments.
    const auto parts = mMsg->contents();
    for (KMime::Content *content : parts) {
        mMsg->removeContent(content, true /*delete*/);
    }

    // Set To and CC from the template
    if (!mTo.isEmpty()) {
        mMsg->to()->fromUnicodeString(mMsg->to()->asUnicodeString() + QLatin1Char(',') + mTo, "utf-8");
    }

    if (!mCC.isEmpty()) {
        mMsg->cc()->fromUnicodeString(mMsg->cc()->asUnicodeString() + QLatin1Char(',') + mCC, "utf-8");
    }

    mMsg->contentType()->clear(); // to get rid of old boundary

    //const QByteArray boundary = KMime::multiPartBoundary();
    KMime::Content *const mainTextPart
        = htmlBody.isEmpty()
          ? createPlainPartContent(plainBody)
          : createMultipartAlternativeContent(plainBody, htmlBody);
    mainTextPart->assemble();

    KMime::Content *textPart = mainTextPart;
    if (!ic.images().empty()) {
        textPart = createMultipartRelated(ic, mainTextPart);
        textPart->assemble();
    }

    // If we have some attachments, create a multipart/mixed mail and
    // add the normal body as well as the attachments
    KMime::Content *mainPart = textPart;
    if (mMode == Forward) {
        auto attachments = mOrigMsg->attachments();
        attachments += mOtp->nodeHelper()->attachmentsOfExtraContents();
        if (!attachments.isEmpty()) {
            mainPart = createMultipartMixed(attachments, textPart);
            mainPart->assemble();
        }
    }

    mMsg->setBody(mainPart->encodedBody());
    mMsg->setHeader(mainPart->contentType());
    mMsg->setHeader(mainPart->contentTransferEncoding());
    mMsg->assemble();
    mMsg->parse();
}

KMime::Content *TemplateParserJob::createMultipartMixed(const QVector<KMime::Content *> &attachments, KMime::Content *textPart) const
{
    KMime::Content *mixedPart = new KMime::Content(mMsg.data());
    const QByteArray boundary = KMime::multiPartBoundary();
    mixedPart->contentType()->setMimeType("multipart/mixed");
    mixedPart->contentType()->setBoundary(boundary);
    mixedPart->contentTransferEncoding()->setEncoding(KMime::Headers::CE7Bit);
    mixedPart->addContent(textPart);

    int attachmentNumber = 1;
    for (KMime::Content *attachment : attachments) {
        mixedPart->addContent(attachment);
        // If the content type has no name or filename parameter, add one, since otherwise the name
        // would be empty in the attachment view of the composer, which looks confusing
        if (attachment->contentType(false)) {
            if (!attachment->contentType()->hasParameter(QStringLiteral("name"))
                && !attachment->contentType()->hasParameter(QStringLiteral("filename"))) {
                attachment->contentType()->setParameter(
                    QStringLiteral("name"), i18nc("@item:intext", "Attachment %1", attachmentNumber));
            }
        }
        ++attachmentNumber;
    }
    return mixedPart;
}

KMime::Content *TemplateParserJob::createMultipartRelated(const MessageCore::ImageCollector &ic, KMime::Content *mainTextPart) const
{
    KMime::Content *relatedPart = new KMime::Content(mMsg.data());
    const QByteArray boundary = KMime::multiPartBoundary();
    relatedPart->contentType()->setMimeType("multipart/related");
    relatedPart->contentType()->setBoundary(boundary);
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
    KMime::Content *textPart = new KMime::Content(mMsg.data());
    textPart->contentType()->setMimeType("text/plain");
    QTextCodec *charset = selectCharset(m_charsets, plainBody);
    textPart->contentType()->setCharset(charset->name());
    textPart->contentTransferEncoding()->setEncoding(KMime::Headers::CE8Bit);
    textPart->fromUnicodeString(plainBody);
    return textPart;
}

KMime::Content *TemplateParserJob::createMultipartAlternativeContent(const QString &plainBody, const QString &htmlBody) const
{
    KMime::Content *multipartAlternative = new KMime::Content(mMsg.data());
    multipartAlternative->contentType()->setMimeType("multipart/alternative");
    const QByteArray boundary = KMime::multiPartBoundary();
    multipartAlternative->contentType()->setBoundary(boundary);

    KMime::Content *textPart = createPlainPartContent(plainBody);
    multipartAlternative->addContent(textPart);

    KMime::Content *htmlPart = new KMime::Content(mMsg.data());
    htmlPart->contentType()->setMimeType("text/html");
    QTextCodec *charset = selectCharset(m_charsets, htmlBody);
    htmlPart->contentType()->setCharset(charset->name());
    htmlPart->contentTransferEncoding()->setEncoding(KMime::Headers::CE8Bit);
    htmlPart->fromUnicodeString(htmlBody);
    multipartAlternative->addContent(htmlPart);

    return multipartAlternative;
}

QString TemplateParserJob::findCustomTemplate(const QString &tmplName)
{
    CTemplates t(tmplName);
    mTo = t.to();
    mCC = t.cC();
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

    qCDebug(TEMPLATEPARSER_LOG) << "Folder identify found:" << mFolder;
    if (mFolder >= 0) {   // only if a folder was found
        QString fid = QString::number(mFolder);
        Templates fconf(fid);
        if (fconf.useCustomTemplates()) {     // does folder use custom templates?
            switch (mMode) {
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
                qCDebug(TEMPLATEPARSER_LOG) << "Unknown message mode:" << mMode;
                return QString();
            }
            mQuoteString = fconf.quoteString();
            if (!tmpl.isEmpty()) {
                return tmpl;  // use folder-specific template
            }
        }
    }

    if (!mIdentity) {   // find identity message belongs to
        mIdentity = identityUoid(mMsg);
        if (!mIdentity && mOrigMsg) {
            mIdentity = identityUoid(mOrigMsg);
        }
        mIdentity = m_identityManager->identityForUoidOrDefault(mIdentity).uoid();
        if (!mIdentity) {
            qCDebug(TEMPLATEPARSER_LOG) << "Oops! No identity for message";
        }
    }
    qCDebug(TEMPLATEPARSER_LOG) << "Identity found:" << mIdentity;

    QString iid;
    if (mIdentity) {
        iid = TemplatesConfiguration::configIdString(mIdentity);          // templates ID for that identity
    } else {
        iid = QStringLiteral("IDENTITY_NO_IDENTITY"); // templates ID for no identity
    }

    Templates iconf(iid);
    if (iconf.useCustomTemplates()) {   // does identity use custom templates?
        switch (mMode) {
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
            qCDebug(TEMPLATEPARSER_LOG) << "Unknown message mode:" << mMode;
            return QString();
        }
        mQuoteString = iconf.quoteString();
        if (!tmpl.isEmpty()) {
            return tmpl;  // use identity-specific template
        }
    }

    switch (mMode) {  // use the global template
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
        qCDebug(TEMPLATEPARSER_LOG) << "Unknown message mode:" << mMode;
        return QString();
    }

    mQuoteString = TemplateParserSettings::self()->quoteString();
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

    if (!success && mDebug) {
        KMessageBox::error(
            nullptr,
            xi18nc("@info",
                   "Pipe command <command>%1</command> failed.", cmd));
    }

    if (success) {
        return QString::fromLatin1(process.readAllStandardOutput());
    } else {
        return QString();
    }
}

void TemplateParserJob::setWordWrap(bool wrap, int wrapColWidth)
{
    mWrap = wrap;
    mColWrap = wrapColWidth;
}

QString TemplateParserJob::plainMessageText(bool aStripSignature, AllowSelection isSelectionAllowed) const
{
    if (!mSelection.isEmpty() && (isSelectionAllowed == SelectionAllowed)) {
        return mSelection;
    }

    if (!mOrigMsg) {
        return QString();
    }
    QString result = mOtp->plainTextContent();
    if (result.isEmpty()) {
        result = mExtractHtmlInfoResult.mPlainText;
    }
    if (aStripSignature) {
        result = MessageCore::StringUtil::stripSignature(result);
    }

    return result;
}

QString TemplateParserJob::htmlMessageText(bool aStripSignature, AllowSelection isSelectionAllowed)
{
    if (!mSelection.isEmpty() && (isSelectionAllowed == SelectionAllowed)) {
        //TODO implement mSelection for HTML
        return mSelection;
    }
    mHeadElement = mExtractHtmlInfoResult.mHeaderElement;
    const QString bodyElement = mExtractHtmlInfoResult.mBodyElement;
    if (!bodyElement.isEmpty()) {
        if (aStripSignature) {
            //FIXME strip signature works partially for HTML mails
            return MessageCore::StringUtil::stripSignature(bodyElement);
        }
        return bodyElement;
    }

    if (aStripSignature) {
        //FIXME strip signature works partially for HTML mails
        return MessageCore::StringUtil::stripSignature(mExtractHtmlInfoResult.mHtmlElement);
    }
    return mExtractHtmlInfoResult.mHtmlElement;
}

QString TemplateParserJob::quotedPlainText(const QString &selection) const
{
    QString content = selection;
    // Remove blank lines at the beginning:
    const int firstNonWS = content.indexOf(QRegExp(QLatin1String("\\S")));
    const int lineStart = content.lastIndexOf(QLatin1Char('\n'), firstNonWS);
    if (lineStart >= 0) {
        content.remove(0, static_cast<unsigned int>(lineStart));
    }

    const QString indentStr
        = MessageCore::StringUtil::formatQuotePrefix(mQuoteString, mOrigMsg->from()->displayString());
    if (TemplateParserSettings::self()->smartQuote() && mWrap) {
        content = MessageCore::StringUtil::smartQuote(content, mColWrap - indentStr.length());
    }
    content.replace(QLatin1Char('\n'), QLatin1Char('\n') + indentStr);
    content.prepend(indentStr);
    content += QLatin1Char('\n');

    return content;
}

QString TemplateParserJob::quotedHtmlText(const QString &selection) const
{
    QString content = selection;
    //TODO 1) look for all the variations of <br>  and remove the blank lines
    //2) implement vertical bar for quoted HTML mail.
    //3) After vertical bar is implemented, If a user wants to edit quoted message,
    // then the <blockquote> tags below should open and close as when required.

    //Add blockquote tag, so that quoted message can be differentiated from normal message
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
    int id = idString.toUInt(&ok);

    if (!ok || id == 0) {
        id = m_identityManager->identityForAddress(
            msg->to()->asUnicodeString() + QLatin1String(", ") + msg->cc()->asUnicodeString()).uoid();
    }

    return id;
}

bool TemplateParserJob::isHtmlSignature() const
{
    const KIdentityManagement::Identity &identity
        = m_identityManager->identityForUoid(mIdentity);

    if (identity.isNull()) {
        return false;
    }

    const KIdentityManagement::Signature signature
        = const_cast<KIdentityManagement::Identity &>(identity).signature();

    return signature.isInlinedHtml();
}

QString TemplateParserJob::plainToHtml(const QString &body)
{
    QString str = body;
    str = str.toHtmlEscaped();
    str.replace(QLatin1Char('\n'), QStringLiteral("<br />\n"));
    return str;
}

//TODO implement this function using a DOM tree parser
void TemplateParserJob::makeValidHtml(QString &body)
{
    QRegExp regEx;
    regEx.setMinimal(true);
    regEx.setPattern(QStringLiteral("<html.*>"));

    if (!body.isEmpty() && !body.contains(regEx)) {
        regEx.setPattern(QStringLiteral("<body.*>"));
        if (!body.contains(regEx)) {
            body = QLatin1String("<body>") + body + QLatin1String("<br/></body>");
        }
        regEx.setPattern(QStringLiteral("<head.*>"));
        if (!body.contains(regEx)) {
            body = QLatin1String("<head>") + mHeadElement + QLatin1String("</head>") + body;
        }
        body = QLatin1String("<html>") + body + QLatin1String("</html>");
    }
}
