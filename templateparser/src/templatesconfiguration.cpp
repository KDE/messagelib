/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 * SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "templatesconfiguration.h"
using namespace Qt::Literals::StringLiterals;

#include "globalsettings_templateparser.h"
#include "templatesconfiguration_kfg.h"
#include <TextCustomEditor/PlainTextEditor>

#include "templateparser_debug.h"
#include <KLocalizedString>
#include <KMessageBox>

#include <QWhatsThis>

using namespace TemplateParser;
class TemplateParser::TemplatesConfigurationPrivate
{
public:
    TemplatesConfigurationPrivate() = default;

    QString mHelpString;
};

TemplatesConfiguration::TemplatesConfiguration(QWidget *parent, const QString &name)
    : QWidget(parent)
    , d(new TemplateParser::TemplatesConfigurationPrivate)
{
    setupUi(this);
    setObjectName(name);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizeHint();

    connect(textEdit_new->editor(), &QPlainTextEdit::textChanged, this, &TemplatesConfiguration::slotTextChanged);
    connect(textEdit_reply->editor(), &QPlainTextEdit::textChanged, this, &TemplatesConfiguration::slotTextChanged);
    connect(textEdit_reply_all->editor(), &QPlainTextEdit::textChanged, this, &TemplatesConfiguration::slotTextChanged);
    connect(textEdit_forward->editor(), &QPlainTextEdit::textChanged, this, &TemplatesConfiguration::slotTextChanged);
    connect(lineEdit_quote, &QLineEdit::textChanged, this, &TemplatesConfiguration::slotTextChanged);

    connect(mInsertCommand,
            qOverload<const QString &, int>(&TemplateParser::TemplatesInsertCommandPushButton::insertCommand),
            this,
            &TemplatesConfiguration::slotInsertCommand);

    d->mHelpString = i18n(
        "<p>Here you can create and manage templates to use when "
        "composing new messages, replies or forwarded messages.</p>"
        "<p>The message templates support substitution commands, "
        "either simply type them or select them from "
        "the <i>Insert command</i> menu.</p>");
    const QString templateConfigurationName(name);
    if (templateConfigurationName == QLatin1StringView("folder-templates")) {
        d->mHelpString += i18n(
            "<p>Templates specified here are folder-specific. "
            "They override both global templates and per-identity "
            "templates.</p>");
    } else if (templateConfigurationName == QLatin1StringView("identity-templates")) {
        d->mHelpString += i18n(
            "<p>Templates specified here are identity-specific. "
            "They override global templates, but can be overridden by "
            "per-folder templates if they are specified.</p>");
    } else {
        d->mHelpString += i18n(
            "<p>These are global (default) templates. They can be overridden "
            "by per-identity templates or per-folder templates "
            "if they are specified.</p>");
    }

    mHelp->setText(i18n("<a href=\"whatsthis\">How does this work?</a>"));
    connect(mHelp, &QLabel::linkActivated, this, &TemplatesConfiguration::slotHelpLinkClicked);
    mHelp->setContextMenuPolicy(Qt::NoContextMenu);
}

TemplatesConfiguration::~TemplatesConfiguration()
{
    disconnect(textEdit_new->editor(), &QPlainTextEdit::textChanged, this, &TemplatesConfiguration::slotTextChanged);
    disconnect(textEdit_reply->editor(), &QPlainTextEdit::textChanged, this, &TemplatesConfiguration::slotTextChanged);
    disconnect(textEdit_reply_all->editor(), &QPlainTextEdit::textChanged, this, &TemplatesConfiguration::slotTextChanged);
    disconnect(textEdit_forward->editor(), &QPlainTextEdit::textChanged, this, &TemplatesConfiguration::slotTextChanged);
    disconnect(lineEdit_quote, &QLineEdit::textChanged, this, &TemplatesConfiguration::slotTextChanged);
}

void TemplatesConfiguration::slotHelpLinkClicked(const QString &)
{
    QWhatsThis::showText(QCursor::pos(), d->mHelpString);
}

void TemplatesConfiguration::slotTextChanged()
{
    Q_EMIT changed();
}

void TemplatesConfiguration::resetToDefault()
{
    const int choice = KMessageBox::questionTwoActionsCancel(nullptr,
                                                             i18n("Do you want to reset current template or all templates to default?"),
                                                             i18nc("@title:window", "Reset to default"),
                                                             KGuiItem(i18nc("@action:button", "Reset Current Template")),
                                                             KGuiItem(i18nc("@action:button", "Reset All Templates")),
                                                             KStandardGuiItem::cancel());

    if (choice == KMessageBox::Cancel) {
        return;
    } else if (choice == KMessageBox::ButtonCode::PrimaryAction) {
        const int toolboxCurrentIndex(toolBox1->currentIndex());
        if (toolBox1->widget(toolboxCurrentIndex) == page_new) {
            textEdit_new->setPlainText(DefaultTemplates::defaultNewMessage());
        } else if (toolBox1->widget(toolboxCurrentIndex) == page_reply) {
            textEdit_reply->setPlainText(DefaultTemplates::defaultReply());
        } else if (toolBox1->widget(toolboxCurrentIndex) == page_reply_all) {
            textEdit_reply_all->setPlainText(DefaultTemplates::defaultReplyAll());
        } else if (toolBox1->widget(toolboxCurrentIndex) == page_forward) {
            textEdit_forward->setPlainText(DefaultTemplates::defaultForward());
        } else {
            qCDebug(TEMPLATEPARSER_LOG) << "Unknown current page in TemplatesConfiguration!";
        }
    } else {
        textEdit_new->setPlainText(DefaultTemplates::defaultNewMessage());
        textEdit_reply->setPlainText(DefaultTemplates::defaultReply());
        textEdit_reply_all->setPlainText(DefaultTemplates::defaultReplyAll());
        textEdit_forward->setPlainText(DefaultTemplates::defaultForward());
    }
    lineEdit_quote->setText(DefaultTemplates::defaultQuoteString());
}

QLabel *TemplatesConfiguration::helpLabel() const
{
    return mHelp;
}

void TemplatesConfiguration::loadFromGlobal()
{
    QString str;
    str = TemplateParserSettings::self()->templateNewMessage();
    if (str.isEmpty()) {
        textEdit_new->setPlainText(DefaultTemplates::defaultNewMessage());
    } else {
        textEdit_new->setPlainText(str);
    }
    str = TemplateParserSettings::self()->templateReply();
    if (str.isEmpty()) {
        textEdit_reply->setPlainText(DefaultTemplates::defaultReply());
    } else {
        textEdit_reply->setPlainText(str);
    }
    str = TemplateParserSettings::self()->templateReplyAll();
    if (str.isEmpty()) {
        textEdit_reply_all->setPlainText(DefaultTemplates::defaultReplyAll());
    } else {
        textEdit_reply_all->setPlainText(str);
    }
    str = TemplateParserSettings::self()->templateForward();
    if (str.isEmpty()) {
        textEdit_forward->setPlainText(DefaultTemplates::defaultForward());
    } else {
        textEdit_forward->setPlainText(str);
    }
    str = TemplateParserSettings::self()->quoteString();
    if (str.isEmpty()) {
        lineEdit_quote->setText(DefaultTemplates::defaultQuoteString());
    } else {
        lineEdit_quote->setText(str);
    }
}

void TemplatesConfiguration::saveToGlobal()
{
    TemplateParserSettings::self()->setTemplateNewMessage(strOrBlank(textEdit_new->toPlainText()));
    TemplateParserSettings::self()->setTemplateReply(strOrBlank(textEdit_reply->toPlainText()));
    TemplateParserSettings::self()->setTemplateReplyAll(strOrBlank(textEdit_reply_all->toPlainText()));
    TemplateParserSettings::self()->setTemplateForward(strOrBlank(textEdit_forward->toPlainText()));
    TemplateParserSettings::self()->setQuoteString(lineEdit_quote->text());
    TemplateParserSettings::self()->save();
}

void TemplatesConfiguration::loadFromIdentity(uint id)
{
    Templates t(configIdString(id));

    QString str;

    str = t.templateNewMessage();
    if (str.isEmpty()) {
        str = TemplateParserSettings::self()->templateNewMessage();
    }
    if (str.isEmpty()) {
        str = DefaultTemplates::defaultNewMessage();
    }
    textEdit_new->setPlainText(str);

    str = t.templateReply();
    if (str.isEmpty()) {
        str = TemplateParserSettings::self()->templateReply();
    }
    if (str.isEmpty()) {
        str = DefaultTemplates::defaultReply();
    }
    textEdit_reply->setPlainText(str);

    str = t.templateReplyAll();
    if (str.isEmpty()) {
        str = TemplateParserSettings::self()->templateReplyAll();
    }
    if (str.isEmpty()) {
        str = DefaultTemplates::defaultReplyAll();
    }
    textEdit_reply_all->setPlainText(str);

    str = t.templateForward();
    if (str.isEmpty()) {
        str = TemplateParserSettings::self()->templateForward();
    }
    if (str.isEmpty()) {
        str = DefaultTemplates::defaultForward();
    }
    textEdit_forward->setPlainText(str);

    str = t.quoteString();
    if (str.isEmpty()) {
        str = TemplateParserSettings::self()->quoteString();
    }
    if (str.isEmpty()) {
        str = DefaultTemplates::defaultQuoteString();
    }
    lineEdit_quote->setText(str);
}

void TemplatesConfiguration::saveToIdentity(uint id)
{
    Templates t(configIdString(id));
    t.setTemplateNewMessage(strOrBlank(textEdit_new->toPlainText()));
    t.setTemplateReply(strOrBlank(textEdit_reply->toPlainText()));
    t.setTemplateReplyAll(strOrBlank(textEdit_reply_all->toPlainText()));
    t.setTemplateForward(strOrBlank(textEdit_forward->toPlainText()));
    t.setQuoteString(lineEdit_quote->text());
    t.save();
}

void TemplatesConfiguration::loadFromFolder(const QString &id, uint identity)
{
    Templates t(id);
    Templates *tid = nullptr;

    if (identity) {
        tid = new Templates(configIdString(identity));
    }

    QString str;

    str = t.templateNewMessage();
    if (str.isEmpty() && tid) {
        str = tid->templateNewMessage();
    }
    if (str.isEmpty()) {
        str = TemplateParserSettings::self()->templateNewMessage();
        if (str.isEmpty()) {
            str = DefaultTemplates::defaultNewMessage();
        }
    }
    textEdit_new->setPlainText(str);

    str = t.templateReply();
    if (str.isEmpty() && tid) {
        str = tid->templateReply();
    }
    if (str.isEmpty()) {
        str = TemplateParserSettings::self()->templateReply();
        if (str.isEmpty()) {
            str = DefaultTemplates::defaultReply();
        }
    }
    textEdit_reply->setPlainText(str);

    str = t.templateReplyAll();
    if (str.isEmpty() && tid) {
        str = tid->templateReplyAll();
        if (str.isEmpty()) {
            str = TemplateParserSettings::self()->templateReplyAll();
        }
    }
    if (str.isEmpty()) {
        str = DefaultTemplates::defaultReplyAll();
    }
    textEdit_reply_all->setPlainText(str);

    str = t.templateForward();
    if (str.isEmpty() && tid) {
        str = tid->templateForward();
        if (str.isEmpty()) {
            str = TemplateParserSettings::self()->templateForward();
        }
    }
    if (str.isEmpty()) {
        str = DefaultTemplates::defaultForward();
    }
    textEdit_forward->setPlainText(str);

    str = t.quoteString();
    if (str.isEmpty() && tid) {
        str = tid->quoteString();
    }
    if (str.isEmpty()) {
        str = TemplateParserSettings::self()->quoteString();
        if (str.isEmpty()) {
            str = DefaultTemplates::defaultQuoteString();
        }
    }
    lineEdit_quote->setText(str);

    delete (tid);
}

void TemplatesConfiguration::saveToFolder(const QString &id)
{
    Templates t(id);

    t.setTemplateNewMessage(strOrBlank(textEdit_new->toPlainText()));
    t.setTemplateReply(strOrBlank(textEdit_reply->toPlainText()));
    t.setTemplateReplyAll(strOrBlank(textEdit_reply_all->toPlainText()));
    t.setTemplateForward(strOrBlank(textEdit_forward->toPlainText()));
    t.setQuoteString(lineEdit_quote->text());
    t.save();
}

QPlainTextEdit *TemplatesConfiguration::currentTextEdit() const
{
    QPlainTextEdit *edit = nullptr;

    const int toolboxCurrentIndex(toolBox1->currentIndex());
    if (toolBox1->widget(toolboxCurrentIndex) == page_new) {
        edit = textEdit_new->editor();
    } else if (toolBox1->widget(toolboxCurrentIndex) == page_reply) {
        edit = textEdit_reply->editor();
    } else if (toolBox1->widget(toolboxCurrentIndex) == page_reply_all) {
        edit = textEdit_reply_all->editor();
    } else if (toolBox1->widget(toolboxCurrentIndex) == page_forward) {
        edit = textEdit_forward->editor();
    } else {
        qCDebug(TEMPLATEPARSER_LOG) << "Unknown current page in TemplatesConfiguration!";
        edit = nullptr;
    }
    return edit;
}

void TemplatesConfiguration::slotInsertCommand(const QString &cmd, int adjustCursor)
{
    QPlainTextEdit *edit = currentTextEdit();
    if (!edit) {
        return;
    }

    // qCDebug(TEMPLATEPARSER_LOG) << "Insert command:" << cmd;
    const QString editText(edit->toPlainText());
    if ((editText.contains(QLatin1StringView("%FORCEDPLAIN")) && (cmd == QLatin1StringView("%FORCEDHTML")))
        || (editText.contains(QLatin1StringView("%FORCEDHTML")) && (cmd == QLatin1StringView("%FORCEDPLAIN")))) {
        KMessageBox::error(this,
                           i18n("Use of \"Reply using plain text\" and \"Reply using HTML text\" in pairs"
                                " is not correct. Use only one of the aforementioned commands with \" Reply as"
                                " Quoted Message command\" as per your need\n"
                                "(a)Reply using plain text for quotes to be strictly in plain text\n"
                                "(b)Reply using HTML text for quotes being in HTML format if present"));
    } else {
        QTextCursor cursor = edit->textCursor();
        cursor.insertText(cmd);
        cursor.setPosition(cursor.position() + adjustCursor);
        edit->setTextCursor(cursor);
        edit->setFocus();
    }
}

QString TemplatesConfiguration::strOrBlank(const QString &str)
{
    if (str.trimmed().isEmpty()) {
        return u"%BLANK"_s;
    }
    return str;
}

QString TemplatesConfiguration::configIdString(uint id)
{
    return u"IDENTITY_%1"_s.arg(id);
}

#include "moc_templatesconfiguration.cpp"
