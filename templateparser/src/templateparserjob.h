/*
   SPDX-FileCopyrightText: 2017-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_export.h"

#include <KMime/Message>

#include <QObject>

#include <memory>

namespace KIdentityManagementCore
{
class IdentityManager;
}

namespace MessageCore
{
class ImageCollector;
}

struct TemplateParserExtractHtmlInfoResult;
class TemplateParserJobTest;

namespace TemplateParser
{
class TemplateParserJobPrivate;
/**
 * \brief The TemplateParser transforms a message with a given template.
 *
 * \par Introduction
 * The TemplateParser transforms a message with a given template.
 * A template contains text and commands, such as %QUOTE or %ODATE, which will be
 * replaced with the real values in process().
 *
 * \par Basics
 * The message given in the templateparser constructor amsg, is the message that
 * is being transformed.
 * aorig_msg is the original message to which actions are performed.
 * The message text in amsg will be replaced by the processed text of the template,
 * but other properties, such as the attachments or the subject, are preserved.
 *
 * There are two different kind of commands: Those that work on the message that is
 * to be transformed and those that work on an 'original message'.
 * Those that work on the message that is to be transformed have no special prefix, e.g.
 * '%DATE'. Those that work on the original message have an 'O' prefix, for example
 * '%ODATE'.
 * This means that the %DATE command will take the date of the message passed in the
 * constructor, the message which is to be transformed, whereas the %ODATE command will
 * take the date of the message that is being passed in process(), the original message.
 *
 * \par The process()
 * The process() function takes aorig_msg as parameter. This aorig_msg is the original
 * message from which various commands in templates with prefix 'O' extract the data and adds
 * to the processed message.
 * This function finds the template and passes them to processWithTemplate(), where templates
 * are processed and its value is added to the processed message.
 *
 * \par Reply To/Forward Plain Text Mails
 * Plain Text mails are the mails with only text part and no html part. While creating
 * reply/forward to mails, processWithTemplate() processes all the commands and then
 * appends its values to plainBody and htmlBody. This function then on the
 * basis of whether the user wants to use plain mails or HTML mails, clears the htmlBody,
 * or just passes both the plainBody and htmlBody unaltered.
 *
 * \par Reply To/Forward HTML Mails
 * By HTML mails here, we mean multipart/alternative mails. As mentioned above, all
 * commands in the TemplateParser appends text, i.e. plain text to plainBody
 * and html text to htmlBody in the function processWithTemplate().
 * This function also takes a decision of clearing the htmlBody on the basis of fact
 * whether the user wants to reply/forward using plain mails or multipart/alternative
 * mails.
 *
 * \par When "TO" and when "NOT TO" make multipart/alternative Mails
 * User is the master to decide when to and when not to make multipart/alternative mails.
 * <b>For user who <u>don't prefer</u> using HTML mails</b>
 * There is a TemplateParserSettings::self()->replyUsingHtml() (in GUI as Settings->Configure KMail->
 * Composer->General->"Reply using HTML if present"), which when not true (checkbox disabled
 * in UI), will clear the htmlBody.
 * An another option within the standard templates, %FORCEDPLAIN command raises the flag,
 * ReplyAsPlain. This flag when raised in processWithTemplate() takes care that the
 * processed message will contain text/plain part by clearing the htmlBody.
 *
 * Once the htmlBody is cleared, plainBody and an empty htmlBody is passed to
 * addProcessedBodyToMessage(). Here since the htmlBody is empty, text/plain messages are
 * assembled and thus user is not dealing with any kind of HTML part.
 *
 * <b>For user who <u>do prefer</u> using HTML mails</b>
 * The setting discussed above as "Reply using HTML if present" (when checked to true),
 * passes the htmlBody to addProcessedBodyToMessage() without doing any changes.
 * An another option %FORCEDHTML within standard templates command raises the flag ReplyAsHtml.
 * This flag when raised in processWithTemplate() takes care that the htmlBody is passed to
 * addProcessedBodyToMessage() unaltered.
 *
 * Since htmlBody received by addProcessedBodyToMessage() is not empty, multipart/alternative
 * messages are assembled.
 *
 * @NOTE Resolving conflict between TemplateParserSettings "replyUsingHtml" and FORCEDXXXX command.
 * The conflict is resolved by simply giving preference to the commands over TemplateParserSettings.
 *
 * \par Make plain part
 * mMsg is the reply message in which the message text will be replaced by the
 * processed value from templates.
 *
 * In case of no attachments, the message will be a single-part message.
 * A KMime::Content containing the plainBody from processWithTemplate() is
 * created.  Then the encodedBody(), contentType (text/plain) of this
 * KMime::Content is set in the body and the header of mMsg.
 *
 * \par Make multipart/alternative mails
 * First of all a KMime::Content (content) is created with a content-type of
 * multipart/alternative.  Then in the same way as plain-part is created in above
 * paragraph, a KMime::Content (sub-content) containing the plainBody is created
 * and added as child to the content.  Then a new KMime::Content (sub-content)
 * with htmlBody as the body is created.  The content-type is set as text/html.
 * This new sub-content is then added to the parent content.  Now, since the
 * parent content (multipart/alternative) has two sub-content (text/plain and
 * text/html) to it, it is added to the reply message (mMsg).
 *
 * TODO: What is the usecase of the commands that work on the message to be transformed?
 *       In general you only use the commands that work on the original message...
 */
class TEMPLATEPARSER_EXPORT TemplateParserJob : public QObject
{
    Q_OBJECT
    friend class TemplateParserJobTest;

public:
    enum Mode : uint8_t {
        NewMessage,
        Reply,
        ReplyAll,
        Forward,
    };

    enum AllowSelection : uint8_t {
        SelectionAllowed,
        NoSelectionAllowed,
    };

    enum Quotes : uint8_t {
        ReplyAsOriginalMessage,
        ReplyAsPlain,
        ReplyAsHtml,
    };

public:
    explicit TemplateParserJob(const KMime::Message::Ptr &amsg, const Mode amode, QObject *parent = nullptr);
    ~TemplateParserJob() override;

    /**
     * Sets the selection. If this is set, only the selection will be added to
     * commands such as %QUOTE. Otherwise, the whole message is quoted.
     * If this is not called at all, the whole message is quoted as well.
     * Call this before calling process().
     */
    void setSelection(const QString &selection);

    /**
     * Sets whether the template parser is allowed to decrypt the original
     * message when needing its message text, for example for the %QUOTE command.
     * If true, it will tell the ObjectTreeParser it uses internally to decrypt the
     * message, and that will possibly show a password request dialog to the user.
     *
     * The default is false.
     */
    void setAllowDecryption(const bool allowDecryption);

    /**
     * Tell template parser whether or not to wrap words, and at what column
     * to wrap at.
     *
     * Default is true, wrapping at 80chars.
     */
    void setWordWrap(bool wrap, int wrapColWidth = 80);

    /**
     * Set the identity manager to be used when creating the template.
     */
    void setIdentityManager(KIdentityManagementCore::IdentityManager *ident);

    void process(const KMime::Message::Ptr &aorig_msg, qint64 afolder = -1);
    void process(const QString &tmplName, const KMime::Message::Ptr &aorig_msg, qint64 afolder = -1);
    void processWithIdentity(uint uoid, const KMime::Message::Ptr &aorig_msg, qint64 afolder = -1);

    void processWithTemplate(const QString &tmpl);

    void setReplyAsHtml(bool replyAsHtml);

Q_SIGNALS:
    void parsingDone(bool cursorPositionWasSet);
    void parsingFailed();

private:
    void slotExtractInfoDone(const TemplateParserExtractHtmlInfoResult &result);
    /// This finds the template to use. Either the one from the folder, identity or
    /// finally the global template.
    /// This also reads the To and CC address of the template
    /// @return the contents of the template
    [[nodiscard]] QString findTemplate();

    /// Finds the template with the given name.
    /// This also reads the To and CC address of the template
    /// @return the contents of the template
    [[nodiscard]] QString findCustomTemplate(const QString &tmpl);

    [[nodiscard]] QString pipe(const QString &cmd, const QString &buf);

    /**
     * Called by processWithTemplate(). This adds the completely processed body to
     * the message.
     *
     * This function creates plain text message or multipart/alternative message,
     * depending on whether the processed body has @p htmlBody or not.
     *
     * In append mode, this will simply append the text to the body.
     *
     * Otherwise, the content of the old message is deleted and replaced with @p plainBody
     * and @p htmlBody.
     * Attachments of the original message are also added back to the new message.
     */
    void addProcessedBodyToMessage(const QString &plainBody, const QString &htmlBody) const;

    /**
     * Determines whether the signature should be stripped when getting the text
     * of the original message, e.g. for commands such as %QUOTE
     */
    bool shouldStripSignature() const;

    static int parseQuotes(const QString &prefix, const QString &str, QString &quote);

    /**
     * Return the text signature used the by current identity.
     */
    [[nodiscard]] QString getPlainSignature() const;

    /**
     * Return the HTML signature used the by current identity.
     */
    [[nodiscard]] QString getHtmlSignature() const;

    /**
     * Returns message body indented by the
     * given indentation string. This is suitable for including the message
     * in another message of for replies, forwards.
     *
     * No attachments are handled if includeAttach is false.
     * The signature is stripped if aStripSignature is true and
     * smart quoting is turned on. Signed or encrypted texts
     * get converted to plain text when allowDecryption is true.
     */
    [[nodiscard]] QString quotedPlainText(const QString &selection = QString()) const;

    /**
     * Returns HTML message body.
     * This is suitable for including the message
     * in another message of for replies, forwards.
     *
     * No attachments are handled if includeAttach is false.
     * The signature is stripped if aStripSignature is true and
     * smart quoting is turned on. Signed or encrypted texts
     * get converted to plain text when allowDecryption is true.
     */
    [[nodiscard]] QString quotedHtmlText(const QString &selection) const;

    /**
     * This function return the plain text part from the OTP.
     * For HTML only mails. It returns the converted plain text
     * from the OTP.
     * @param allowSelectionOnly takes care that if a reply/forward
     * is made to a selected part of message, then the selection is
     * returned as it is without going through th OTP
     * @param aStripSignature strips the signature out of the message
     *
     */
    [[nodiscard]] QString plainMessageText(bool aStripSignature, AllowSelection isSelectionAllowed) const;

    /**
     * Returns the HTML content of the message as plain text
     */
    [[nodiscard]] QString htmlMessageText(bool aStripSignature, AllowSelection isSelectionAllowed);

    /** @return the UOID of the identity for this message.
     * Searches the "x-kmail-identity" header and if that fails,
     * searches with KIdentityManagementCore::IdentityManager::identityForAddress()
     */
    [[nodiscard]] uint identityUoid(const KMime::Message::Ptr &msg) const;

    /**
     * Returns KMime content of the plain text part of the message after setting
     * its mime type, charset and CTE.
     * This function is called by:-
     * 1) TemplateParser::addProcessedBodyToMessage(), which uses this content
     *    to simply create the text/plain message
     *
     * 2) TemplateParser::createMultipartAlternativeContent() which adds this content
     *    to create the multipart/alternative message.
     */
    KMime::Content *createPlainPartContent(const QString &plainBody) const;

    /**
     * Returns KMime content of the multipart/alternative part of the message
     * after setting the mime type, charset and CTE of its respective text/plain
     * part and text/html part.
     */
    KMime::Content *createMultipartAlternativeContent(const QString &plainBody, const QString &htmlBody) const;

    /**
     * Returns a multipart/mixed KMime::Content that has textPart and all
     * attachments as children.
     * @param attachments the list of attachments to add
     * @param textPart a KMime::Content that is to be added as a child.
     * @since 4.8
     */
    KMime::Content *createMultipartMixed(const QList<KMime::Content *> &attachments, KMime::Content *textPart) const;

    /**
     * Returnsa multipart/related KMime::Content that has mainTextPart and all
     * embedded images as children.
     * @param ac a reference to an MessageCore::ImageCollector that has
     *           collected all attachments.
     * @param mainTextPart a KMime::Content that is to be added as a child.
     * @since 4.8
     */
    KMime::Content *createMultipartRelated(const MessageCore::ImageCollector &ic, KMime::Content *mainTextPart) const;

    /**
     * Checks if the signature is HTML or not.
     */
    [[nodiscard]] bool isHtmlSignature() const;

    /**
     * Does the necessary conversions like escaping characters, changing "\n" to
     * breakline tag before appending text to htmlBody.
     */
    [[nodiscard]] static QString plainTextToHtml(const QString &body);

    /**
     * Make a HTML content valid by adding missing html/head/body tag.
     */
    void makeValidHtml(QString &body);

    friend class ::TemplateParserJobTest;
    std::unique_ptr<TemplateParserJobPrivate> d;
};
}
