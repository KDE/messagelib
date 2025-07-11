/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2009 Thomas McGuire <mcguire@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "stringutil.h"

#include "MessageCore/MessageCoreSettings"

#include <KEmailAddress>
#include <KLocalizedString>
#include <KMime/Headers>

#include "messagecore_debug.h"
#include <KUser>

#include <KCodecs>
#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <KPIMTextEdit/TextUtils>
#include <QHostInfo>
#include <QRegularExpression>
#include <QUrlQuery>

using namespace KMime;
using namespace KMime::Types;
using namespace KMime::HeaderParsing;
using namespace Qt::Literals::StringLiterals;
namespace MessageCore
{
namespace StringUtil
{
// Removes trailing spaces and tabs at the end of the line
static void removeTrailingSpace(QString &line)
{
    int i = line.length() - 1;
    while ((i >= 0) && ((line[i] == u' ') || (line[i] == u'\t'))) {
        i--;
    }
    line.truncate(i + 1);
}

// Splits the line off in two parts: The quote prefixes and the actual text of the line.
// For example, for the string "> > > Hello", it would be split up in "> > > " as the quote
// prefix, and "Hello" as the actual text.
// The actual text is written back to the "line" parameter, and the quote prefix is returned.
static QString splitLine(QString &line)
{
    removeTrailingSpace(line);
    int i = 0;
    int startOfActualText = -1;

    // TODO: Replace tabs with spaces first.

    // Loop through the chars in the line to find the place where the quote prefix stops
    const int lineLength(line.length());
    while (i < lineLength) {
        const QChar c = line[i];
        const bool isAllowedQuoteChar = (c == u'>') || (c == u':') || (c == u'|') || (c == u' ') || (c == u'\t');
        if (isAllowedQuoteChar) {
            startOfActualText = i + 1;
        } else {
            break;
        }
        ++i;
    }

    // If the quote prefix only consists of whitespace, don't consider it as a quote prefix at all
    if (line.left(startOfActualText).trimmed().isEmpty()) {
        startOfActualText = 0;
    }

    // No quote prefix there -> nothing to do
    if (startOfActualText <= 0) {
        return {};
    }

    // Entire line consists of only the quote prefix
    if (i == line.length()) {
        const QString quotePrefix = line.left(startOfActualText);
        line.clear();
        return quotePrefix;
    }

    // Line contains both the quote prefix and the actual text, really split it up now
    const QString quotePrefix = line.left(startOfActualText);
    line = line.mid(startOfActualText);

    return quotePrefix;
}

// Writes all lines/text parts contained in the "textParts" list to the output text, "msg".
// Quote characters are added in front of each line, and no line is longer than
// maxLength.
//
// Although the lines in textParts are considered separate lines, they can actually be run
// together into a single line in some cases. This is basically the main difference to flowText().
//
// Example:
//   textParts = "Hello World, this is a test.", "Really"
//   indent = ">"
//   maxLength = 20
//   Result: "> Hello World, this\n
//            > is a test. Really"
// Notice how in this example, the text line "Really" is no longer a separate line, it was run
// together with a previously broken line.
//
// "textParts" is cleared upon return.
static bool flushPart(QString &msg, QStringList &textParts, const QString &indent, int maxLength)
{
    if (maxLength < 20) {
        maxLength = 20;
    }

    // Remove empty lines at end of quote
    while (!textParts.isEmpty() && textParts.last().isEmpty()) {
        textParts.removeLast();
    }

    QString text;

    for (const QString &line : textParts) {
        // An empty line in the input means that an empty line should be in the output as well.
        // Therefore, we write all of our text so far to the msg.
        if (line.isEmpty()) {
            if (!text.isEmpty()) {
                msg += KPIMTextEdit::TextUtils::flowText(text, indent, maxLength) + u'\n';
            }
            msg += indent + u'\n';
        } else {
            if (text.isEmpty()) {
                text = line;
            } else {
                text += u' ' + line.trimmed();
            }
            // If the line doesn't need to be wrapped at all, just write it out as-is.
            // When a line exceeds the maximum length and therefore needs to be broken, this statement
            // if false, and therefore we keep adding lines to our text, so they get ran together in the
            // next flowText call, as "text" contains several text parts/lines then.
            if ((text.length() < maxLength) || (line.length() < (maxLength - 10))) {
                msg += KPIMTextEdit::TextUtils::flowText(text, indent, maxLength) + u'\n';
            }
        }
    }

    // Write out pending text to the msg
    if (!text.isEmpty()) {
        msg += KPIMTextEdit::TextUtils::flowText(text, indent, maxLength);
    }

    const bool appendEmptyLine = !textParts.isEmpty();
    textParts.clear();

    return appendEmptyLine;
}

QList<QPair<QString, QString>> parseMailtoUrl(const QUrl &url)
{
    QList<QPair<QString, QString>> values;
    if (url.scheme() != QLatin1StringView("mailto")) {
        return values;
    }
    QString str = url.toString();
    QStringList toStr;
    int i = 0;

    // String can be encoded.
    str = KCodecs::decodeRFC2047String(str);
    // Bug 427697
    str.replace(u"&#38;"_s, u"&"_s);
    const QUrl newUrl = QUrl::fromUserInput(str);

    int indexTo = -1;
    // Workaround line with # see bug 406208
    const int indexOf = str.indexOf(u'?');
    if (indexOf != -1) {
        str.remove(0, indexOf + 1);
        QUrlQuery query(str);
        const auto listQuery = query.queryItems(QUrl::FullyDecoded);
        for (const auto &queryItem : listQuery) {
            if (queryItem.first == QLatin1StringView("to")) {
                toStr << queryItem.second;
                indexTo = i;
            } else {
                if (queryItem.second.isEmpty()) {
                    // Bug 206269 => A%26B => encoded '&'
                    if (i >= 1) {
                        values[i - 1].second = values[i - 1].second + u"&"_s + queryItem.first;
                    }
                } else {
                    QPair<QString, QString> pairElement;
                    pairElement.first = queryItem.first.toLower();
                    pairElement.second = queryItem.second;
                    values.append(pairElement);
                    i++;
                }
            }
        }
    }
    QStringList to = {KEmailAddress::decodeMailtoUrl(newUrl)};
    if (!toStr.isEmpty()) {
        to << toStr;
    }
    const QString fullTo = to.join(QLatin1StringView(", "));
    if (!fullTo.isEmpty()) {
        QPair<QString, QString> pairElement;
        pairElement.first = u"to"_s;
        pairElement.second = fullTo;
        if (indexTo != -1) {
            values.insert(indexTo, pairElement);
        } else {
            values.prepend(pairElement);
        }
    }
    return values;
}

QString stripSignature(const QString &msg)
{
    // Following RFC 3676, only > before --
    // I prefer to not delete a SB instead of delete good mail content.
    static const QRegularExpression sbDelimiterSearch(u"(^|\n)[> ]*-- \n"_s);
    // The regular expression to look for prefix change
    static const QRegularExpression commonReplySearch(u"^[ ]*>"_s);

    QString res = msg;
    int posDeletingStart = 1; // to start looking at 0

    // While there are SB delimiters (start looking just before the deleted SB)
    while ((posDeletingStart = res.indexOf(sbDelimiterSearch, posDeletingStart - 1)) >= 0) {
        QString prefix; // the current prefix
        QString line; // the line to check if is part of the SB
        int posNewLine = -1;

        // Look for the SB beginning
        const int posSignatureBlock = res.indexOf(u'-', posDeletingStart);
        // The prefix before "-- "$
        if (res.at(posDeletingStart) == u'\n') {
            ++posDeletingStart;
        }

        prefix = res.mid(posDeletingStart, posSignatureBlock - posDeletingStart);
        posNewLine = res.indexOf(u'\n', posSignatureBlock) + 1;

        // now go to the end of the SB
        while (posNewLine < res.size() && posNewLine > 0) {
            // handle the undefined case for mid ( x , -n ) where n>1
            int nextPosNewLine = res.indexOf(u'\n', posNewLine);

            if (nextPosNewLine < 0) {
                nextPosNewLine = posNewLine - 1;
            }

            line = res.mid(posNewLine, nextPosNewLine - posNewLine);

            // check when the SB ends:
            // * does not starts with prefix or
            // * starts with prefix+(any substring of prefix)
            if ((prefix.isEmpty() && line.indexOf(commonReplySearch) < 0)
                || (!prefix.isEmpty() && line.startsWith(prefix) && line.mid(prefix.size()).indexOf(commonReplySearch) < 0)) {
                posNewLine = res.indexOf(u'\n', posNewLine) + 1;
            } else {
                break; // end of the SB
            }
        }

        // remove the SB or truncate when is the last SB
        if (posNewLine > 0) {
            res.remove(posDeletingStart, posNewLine - posDeletingStart);
        } else {
            res.truncate(posDeletingStart);
        }
    }

    return res;
}

AddressList splitAddressField(const QByteArray &text)
{
    AddressList result;
    const char *begin = text.begin();
    if (!begin) {
        return result;
    }

    const char *const end = text.begin() + text.length();

    if (!parseAddressList(begin, end, result)) {
        qCWarning(MESSAGECORE_LOG) << "Error in address splitting: parseAddressList returned false!" << text;
    }

    return result;
}

QString generateMessageId(const QString &address, const QString &suffix)
{
    const QDateTime dateTime = QDateTime::currentDateTime();

    QString msgIdStr = u'<' + dateTime.toString(u"yyyyMMddhhmm.sszzz"_s);

    if (!suffix.isEmpty()) {
        msgIdStr += u'@' + suffix;
    } else {
        msgIdStr += u'.' + KEmailAddress::toIdn(address);
    }

    msgIdStr += u'>';

    return msgIdStr;
}

QString quoteHtmlChars(const QString &str, bool removeLineBreaks)
{
    QString result;

    int strLength(str.length());
    result.reserve(6 * strLength); // maximal possible length
    for (int i = 0; i < strLength; ++i) {
        switch (str[i].toLatin1()) {
        case '<':
            result += QLatin1StringView("&lt;");
            break;
        case '>':
            result += QLatin1StringView("&gt;");
            break;
        case '&':
            result += QLatin1StringView("&amp;");
            break;
        case '"':
            result += QLatin1StringView("&quot;");
            break;
        case '\n':
            if (!removeLineBreaks) {
                result += QLatin1StringView("<br>");
            }
            break;
        case '\r':
            // ignore CR
            break;
        default:
            result += str[i];
        }
    }

    result.squeeze();
    return result;
}

void removePrivateHeaderFields(const KMime::Message::Ptr &message, bool cleanUpHeader)
{
    message->removeHeader("Status");
    message->removeHeader("X-Status");
    message->removeHeader("X-KMail-EncryptionState");
    message->removeHeader("X-KMail-SignatureState");
    message->removeHeader("X-KMail-Redirect-From");
    message->removeHeader("X-KMail-Link-Message");
    message->removeHeader("X-KMail-Link-Type");
    message->removeHeader("X-KMail-QuotePrefix");
    message->removeHeader("X-KMail-CursorPos");
    message->removeHeader("X-KMail-Templates");
    message->removeHeader("X-KMail-Drafts");
    message->removeHeader("X-KMail-UnExpanded-To");
    message->removeHeader("X-KMail-UnExpanded-CC");
    message->removeHeader("X-KMail-UnExpanded-BCC");
    message->removeHeader("X-KMail-UnExpanded-Reply-To");
    message->removeHeader("X-KMail-FccDisabled");

    if (cleanUpHeader) {
        message->removeHeader("X-KMail-Fcc");
        message->removeHeader("X-KMail-Transport");
        message->removeHeader("X-KMail-Identity");
        message->removeHeader("X-KMail-Transport-Name");
        message->removeHeader("X-KMail-Identity-Name");
        message->removeHeader("X-KMail-Dictionary");
    }
}

QByteArray asSendableString(const KMime::Message::Ptr &originalMessage)
{
    KMime::Message::Ptr message(new KMime::Message);
    message->setContent(originalMessage->encodedContent());

    removePrivateHeaderFields(message);
    message->removeHeader<KMime::Headers::Bcc>();

    return message->encodedContent();
}

QByteArray headerAsSendableString(const KMime::Message::Ptr &originalMessage)
{
    KMime::Message::Ptr message(new KMime::Message);
    message->setContent(originalMessage->encodedContent());

    removePrivateHeaderFields(message);
    message->removeHeader<KMime::Headers::Bcc>();

    return message->head();
}

QString emailAddrAsAnchor(const KMime::Types::Mailbox::List &mailboxList,
                          Display display,
                          const QString &cssStyle,
                          Link link,
                          AddressMode expandable,
                          const QString &fieldName,
                          int collapseNumber)
{
    QString result;
    int numberAddresses = 0;
    bool expandableInserted = false;
    KIdentityManagementCore::IdentityManager *im = KIdentityManagementCore::IdentityManager::self();

    const QString i18nMe = i18nc("signal that this email is defined in my identity", "Me");
    const bool onlyOneIdentity = (im->identities().count() == 1);
    for (const KMime::Types::Mailbox &mailbox : mailboxList) {
        const QString prettyAddressStr = mailbox.prettyAddress();
        if (!prettyAddressStr.isEmpty()) {
            numberAddresses++;
            if (expandable == ExpandableAddresses && !expandableInserted && numberAddresses > collapseNumber) {
                const QString actualListAddress = result;
                QString shortListAddress = actualListAddress;
                if (link == ShowLink) {
                    shortListAddress.truncate(result.length() - 2);
                }
                result =
                    u"<span><input type=\"checkbox\" class=\"addresslist_checkbox\" id=\"%1\" checked=\"checked\"/><span class=\"short%1\">"_s.arg(fieldName)
                    + shortListAddress;
                result += u"<label class=\"addresslist_label_short\" for=\"%1\"></label></span>"_s.arg(fieldName);
                expandableInserted = true;
                result += u"<span class=\"full%1\">"_s.arg(fieldName) + actualListAddress;
            }

            if (link == ShowLink) {
                result += QLatin1StringView("<a href=\"mailto:")
                    + QString::fromLatin1(
                              QUrl::toPercentEncoding(KEmailAddress::encodeMailtoUrl(mailbox.prettyAddress(KMime::Types::Mailbox::QuoteWhenNecessary)).path()))
                    + QLatin1StringView("\" ") + cssStyle + u'>';
            }
            const bool foundMe = !MessageCore::MessageCoreSettings::self()->displayOwnIdentity() && onlyOneIdentity
                && (im->identityForAddress(prettyAddressStr) != KIdentityManagementCore::Identity::null());

            if (display == DisplayNameOnly) {
                if (!mailbox.name().isEmpty()) { // Fallback to the email address when the name is not set.
                    result += foundMe ? i18nMe : quoteHtmlChars(mailbox.name(), true);
                } else {
                    result += foundMe ? i18nMe : quoteHtmlChars(prettyAddressStr, true);
                }
            } else {
                result += foundMe ? i18nMe : quoteHtmlChars(mailbox.prettyAddress(KMime::Types::Mailbox::QuoteWhenNecessary), true);
            }
            if (link == ShowLink) {
                result += QLatin1StringView("</a>, ");
            }
        }
    }

    if (link == ShowLink) {
        result.chop(2);
    }

    if (expandableInserted) {
        result += u"<label class=\"addresslist_label_full\" for=\"%1\"></label></span></span>"_s.arg(fieldName);
    }
    return result;
}

QString emailAddrAsAnchor(const KMime::Headers::Generics::MailboxList *mailboxList,
                          Display display,
                          const QString &cssStyle,
                          Link link,
                          AddressMode expandable,
                          const QString &fieldName,
                          int collapseNumber)
{
    Q_ASSERT(mailboxList);
    return emailAddrAsAnchor(mailboxList->mailboxes(), display, cssStyle, link, expandable, fieldName, collapseNumber);
}

QString emailAddrAsAnchor(const KMime::Headers::Generics::AddressList *addressList,
                          Display display,
                          const QString &cssStyle,
                          Link link,
                          AddressMode expandable,
                          const QString &fieldName,
                          int collapseNumber)
{
    Q_ASSERT(addressList);
    return emailAddrAsAnchor(addressList->mailboxes(), display, cssStyle, link, expandable, fieldName, collapseNumber);
}

bool addressIsInAddressList(const QString &address, const QStringList &addresses)
{
    const QString addrSpec = KEmailAddress::extractEmailAddress(address);

    QStringList::ConstIterator end(addresses.constEnd());
    for (QStringList::ConstIterator it = addresses.constBegin(); it != end; ++it) {
        if (qstricmp(addrSpec.toUtf8().data(), KEmailAddress::extractEmailAddress(*it).toUtf8().data()) == 0) {
            return true;
        }
    }

    return false;
}

QString guessEmailAddressFromLoginName(const QString &loginName)
{
    if (loginName.isEmpty()) {
        return {};
    }

    QString address = loginName;
    address += u'@';
    address += QHostInfo::localHostName();

    // try to determine the real name
    const KUser user(loginName);
    if (user.isValid()) {
        const QString fullName = user.property(KUser::FullName).toString();
        address = KEmailAddress::quoteNameIfNecessary(fullName) + QLatin1StringView(" <") + address + u'>';
    }

    return address;
}

QString smartQuote(const QString &msg, int maxLineLength)
{
    // The algorithm here is as follows:
    // We split up the incoming msg into lines, and then iterate over each line.
    // We keep adding lines with the same indent ( = quote prefix, e.g. "> " ) to a
    // "textParts" list. So the textParts list contains only lines with the same quote
    // prefix.
    //
    // When all lines with the same indent are collected in "textParts", we write those out
    // to the result by calling flushPart(), which does all the nice formatting for us.

    QStringList textParts;
    QString oldIndent;
    bool firstPart = true;
    QString result;

    int lineStart = 0;
    int lineEnd = msg.indexOf(u'\n');
    bool needToContinue = true;
    for (; needToContinue; lineStart = lineEnd + 1, lineEnd = msg.indexOf(u'\n', lineStart)) {
        QString line;
        if (lineEnd == -1) {
            if (lineStart == 0) {
                line = msg;
                needToContinue = false;
            } else if (lineStart != msg.length()) {
                line = msg.mid(lineStart, msg.length() - lineStart);
                needToContinue = false;
            } else {
                needToContinue = false;
            }
        } else {
            line = msg.mid(lineStart, lineEnd - lineStart);
        }
        // Split off the indent from the line
        const QString indent = splitLine(line);

        if (line.isEmpty()) {
            if (!firstPart) {
                textParts.append(QString());
            }
            continue;
        }

        if (firstPart) {
            oldIndent = indent;
            firstPart = false;
        }

        // The indent changed, that means we have to write everything contained in textParts to the
        // result, which we do by calling flushPart().
        if (oldIndent != indent) {
            // Check if the last non-blank line is a "From" line. A from line is the line containing the
            // attribution to a quote, e.g. "Yesterday, you wrote:". We'll just check for the last colon
            // here, to simply things.
            // If there is a From line, remove it from the textParts to that flushPart won't break it.
            // We'll manually add it to the result afterwards.
            QString fromLine;
            if (!textParts.isEmpty()) {
                for (int i = textParts.count() - 1; i >= 0; i--) {
                    // Check if we have found the From line
                    const QString textPartElement(textParts[i]);
                    if (textPartElement.endsWith(u':')) {
                        fromLine = oldIndent + textPartElement + u'\n';
                        textParts.removeAt(i);
                        break;
                    }

                    // Abort on first non-empty line
                    if (!textPartElement.trimmed().isEmpty()) {
                        break;
                    }
                }
            }

            // Write out all lines with the same indent using flushPart(). The textParts list
            // is cleared for us.
            if (flushPart(result, textParts, oldIndent, maxLineLength)) {
                if (oldIndent.length() > indent.length()) {
                    result += indent + u'\n';
                } else {
                    result += oldIndent + u'\n';
                }
            }

            if (!fromLine.isEmpty()) {
                result += fromLine;
            }

            oldIndent = indent;
        }

        textParts.append(line);
    }

    // Write out anything still pending
    flushPart(result, textParts, oldIndent, maxLineLength);

    // Remove superfluous newline which was appended in flowText
    if (!result.isEmpty() && result.endsWith(u'\n')) {
        result.chop(1);
    }

    return result;
}

QString formatQuotePrefix(const QString &wildString, const QString &fromDisplayString)
{
    QString result;

    if (wildString.isEmpty()) {
        return wildString;
    }

    int strLength(wildString.length());
    for (int i = 0; i < strLength;) {
        QChar ch = wildString[i++];
        if (ch == u'%' && i < strLength) {
            ch = wildString[i++];
            switch (ch.toLatin1()) {
            case 'f': { // sender's initials
                if (fromDisplayString.isEmpty()) {
                    break;
                }

                int j = 0;
                const int strLengthFromDisplayString(fromDisplayString.length());
                for (; j < strLengthFromDisplayString && fromDisplayString[j] > u' '; ++j) { }
                for (; j < strLengthFromDisplayString && fromDisplayString[j] <= u' '; ++j) { }
                result += fromDisplayString[0];
                if (j < strLengthFromDisplayString && fromDisplayString[j] > u' ') {
                    result += fromDisplayString[j];
                } else if (strLengthFromDisplayString > 1) {
                    if (fromDisplayString[1] > u' ') {
                        result += fromDisplayString[1];
                    }
                }
                break;
            }
            case '_':
                result += u' ';
                break;
            case '%':
                result += u'%';
                break;
            default:
                result += u'%';
                result += ch;
                break;
            }
        } else {
            result += ch;
        }
    }
    return result;
}

QString cleanFileName(const QString &name)
{
    QString fileName = name.trimmed();

    // We need to replace colons with underscores since those cause problems with
    // KFileDialog (bug in KFileDialog though) and also on Windows filesystems.
    // We also look at the special case of ": ", since converting that to "_ "
    // would look strange, simply "_" looks better.
    // https://issues.kolab.org/issue3805
    fileName.replace(QLatin1StringView(": "), u"_"_s);
    // replace all ':' with '_' because ':' isn't allowed on FAT volumes
    fileName.replace(u':', u'_');
    // better not use a dir-delimiter in a filename
    fileName.replace(u'/', u'_');
    fileName.replace(u'\\', u'_');

#ifdef Q_OS_WINDOWS
    // replace all '.' with '_', not just at the start of the filename
    // but don't replace the last '.' before the file extension.
    int i = fileName.lastIndexOf(u'.');
    if (i != -1) {
        i = fileName.lastIndexOf(u'.', i - 1);
    }

    while (i != -1) {
        fileName.replace(i, 1, u'_');
        i = fileName.lastIndexOf(u'.', i - 1);
    }
#endif

    // replace all '~' with '_', not just leading '~' either.
    fileName.replace(u'~', u'_');

    return fileName;
}

QString cleanSubject(KMime::Message *msg)
{
    return cleanSubject(msg,
                        MessageCore::MessageCoreSettings::self()->replyPrefixes() + MessageCore::MessageCoreSettings::self()->forwardPrefixes(),
                        true,
                        QString())
        .trimmed();
}

QString cleanSubject(KMime::Message *msg, const QStringList &prefixRegExps, bool replace, const QString &newPrefix)
{
    if (auto subject = msg->subject(false)) {
        return replacePrefixes(subject->asUnicodeString(), prefixRegExps, replace, newPrefix);
    } else {
        return {};
    }
}

QString forwardSubject(KMime::Message *msg)
{
    return cleanSubject(msg,
                        MessageCore::MessageCoreSettings::self()->forwardPrefixes(),
                        MessageCore::MessageCoreSettings::self()->replaceForwardPrefix(),
                        u"Fwd:"_s);
}

QString replySubject(KMime::Message *msg)
{
    return cleanSubject(msg,
                        MessageCore::MessageCoreSettings::self()->replyPrefixes(),
                        MessageCore::MessageCoreSettings::self()->replaceReplyPrefix(),
                        u"Re:"_s);
}

QString replacePrefixes(const QString &str, const QStringList &prefixRegExps, bool replace, const QString &newPrefix)
{
    bool recognized = false;
    // construct a big regexp that
    // 1. is anchored to the beginning of str (sans whitespace)
    // 2. matches at least one of the part regexps in prefixRegExps
    const QString bigRegExp = u"^(?:\\s+|(?:%1))+\\s*"_s.arg(prefixRegExps.join(u")|(?:"_s));
    const QRegularExpression rx(bigRegExp, QRegularExpression::CaseInsensitiveOption);
    if (rx.isValid()) {
        QString tmp = str;
        const QRegularExpressionMatch match = rx.match(tmp);
        if (match.hasMatch()) {
            recognized = true;
            if (replace) {
                return tmp.replace(0, match.capturedLength(0), newPrefix + u' ');
            }
        }
    } else {
        qCWarning(MESSAGECORE_LOG) << "bigRegExp = \"" << bigRegExp << "\"\n"
                                   << "prefix regexp is invalid!";
        // try good ole Re/Fwd:
        recognized = str.startsWith(newPrefix);
    }

    if (!recognized) {
        return newPrefix + u' ' + str;
    } else {
        return str;
    }
}

QString stripOffPrefixes(const QString &subject)
{
    const QStringList replyPrefixes = MessageCoreSettings::self()->replyPrefixes();

    const QStringList forwardPrefixes = MessageCoreSettings::self()->forwardPrefixes();

    const QStringList prefixRegExps = replyPrefixes + forwardPrefixes;

    // construct a big regexp that
    // 1. is anchored to the beginning of str (sans whitespace)
    // 2. matches at least one of the part regexps in prefixRegExps
    const QString bigRegExp = u"^(?:\\s+|(?:%1))+\\s*"_s.arg(prefixRegExps.join(u")|(?:"_s));

    static QRegularExpression regex;

    if (regex.pattern() != bigRegExp) {
        // the prefixes have changed, so update the regexp
        regex.setPattern(bigRegExp);
        regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    if (regex.isValid()) {
        QRegularExpressionMatch match = regex.match(subject);
        if (match.hasMatch()) {
            return subject.mid(match.capturedEnd(0));
        }
    } else {
        qCWarning(MESSAGECORE_LOG) << "bigRegExp = \"" << bigRegExp << "\"\n"
                                   << "prefix regexp is invalid!";
    }

    return subject;
}

void setEncodingFile(QUrl &url, const QString &encoding)
{
    QUrlQuery query;
    query.addQueryItem(u"charset"_s, encoding);
    url.setQuery(query);
}
}
}
