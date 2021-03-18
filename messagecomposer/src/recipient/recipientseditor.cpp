/*
    SPDX-FileCopyrightText: 2010 Casey Link <unnamedrambler@gmail.com>
    SPDX-FileCopyrightText: 2009-2010 Klaralvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>

    Refactored from earlier code by:
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2004 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "recipientseditor.h"

#include "recipient.h"
#include "recipientseditorsidewidget.h"

#include "distributionlistdialog.h"
#include "settings/messagecomposersettings.h"

#include "messagecomposer_debug.h"

#include <KEmailAddress>
#include <KLocalizedString>
#include <KMessageBox>
#include <KMime/Headers>

#include <QKeyEvent>
#include <QLayout>

using namespace MessageComposer;
using namespace KPIM;

RecipientLineFactory::RecipientLineFactory(QObject *parent)
    : KPIM::MultiplyingLineFactory(parent)
{
}

KPIM::MultiplyingLine *RecipientLineFactory::newLine(QWidget *p)
{
    auto line = new RecipientLineNG(p);
    if (qobject_cast<RecipientsEditor *>(parent())) {
        connect(line,
                SIGNAL(addRecipient(RecipientLineNG *, QString)),
                qobject_cast<RecipientsEditor *>(parent()),
                SLOT(addRecipient(RecipientLineNG *, QString)));
    } else {
        qCWarning(MESSAGECOMPOSER_LOG) << "RecipientLineFactory::newLine: We can't connect to new line" << parent();
    }
    return line;
}

int RecipientLineFactory::maximumRecipients()
{
    return MessageComposer::MessageComposerSettings::self()->maximumRecipients();
}

class MessageComposer::RecipientsEditorPrivate
{
public:
    RecipientsEditorPrivate()
    {
    }

    KConfig *mRecentAddressConfig = nullptr;
    RecipientsEditorSideWidget *mSideWidget = nullptr;
    bool mSkipTotal = false;
};

RecipientsEditor::RecipientsEditor(QWidget *parent)
    : RecipientsEditor(new RecipientLineFactory(nullptr), parent)
{
}

RecipientsEditor::RecipientsEditor(RecipientLineFactory *lineFactory, QWidget *parent)
    : MultiplyingLineEditor(lineFactory, parent)
    , d(new MessageComposer::RecipientsEditorPrivate)
{
    factory()->setParent(this); // HACK: can't use 'this' above since it's not yet constructed at that point
    d->mSideWidget = new RecipientsEditorSideWidget(this, this);

    layout()->addWidget(d->mSideWidget);

    // Install global event filter and listen for keypress events for RecipientLineEdits.
    // Unfortunately we can't install ourselves directly as event filter for the edits,
    // because the RecipientLineEdit has its own event filter installed into QApplication
    // and so it would eat the event before it would reach us.
    qApp->installEventFilter(this);

    connect(d->mSideWidget, &RecipientsEditorSideWidget::pickedRecipient, this, &RecipientsEditor::slotPickedRecipient);
    connect(d->mSideWidget, &RecipientsEditorSideWidget::saveDistributionList, this, &RecipientsEditor::saveDistributionList);

    connect(this, &RecipientsEditor::lineAdded, this, &RecipientsEditor::slotLineAdded);
    connect(this, &RecipientsEditor::lineDeleted, this, &RecipientsEditor::slotLineDeleted);

    addData(); // one default line
}

RecipientsEditor::~RecipientsEditor()
{
    delete d;
}

bool RecipientsEditor::addRecipient(const QString &recipient, Recipient::Type type)
{
    return addData(Recipient::Ptr(new Recipient(recipient, type)));
}

void RecipientsEditor::addRecipient(RecipientLineNG *line, const QString &recipient)
{
    addRecipient(recipient, line->recipientType());
}

void RecipientsEditor::setRecipientString(const QVector<KMime::Types::Mailbox> &mailboxes, Recipient::Type type)
{
    int count = 1;

    for (const KMime::Types::Mailbox &mailbox : mailboxes) {
        if (count++ > MessageComposer::MessageComposerSettings::self()->maximumRecipients()) {
            KMessageBox::sorry(this,
                               i18ncp("@info:status",
                                      "Truncating recipients list to %2 of %1 entry.",
                                      "Truncating recipients list to %2 of %1 entries.",
                                      mailboxes.count(),
                                      MessageComposer::MessageComposerSettings::self()->maximumRecipients()));
            break;
        }
        addRecipient(mailbox.prettyAddress(KMime::Types::Mailbox::QuoteWhenNecessary), type);
    }
}

Recipient::List RecipientsEditor::recipients() const
{
    const QVector<MultiplyingLineData::Ptr> dataList = allData();
    Recipient::List recList;
    for (const MultiplyingLineData::Ptr &datum : dataList) {
        Recipient::Ptr rec = qSharedPointerDynamicCast<Recipient>(datum);
        if (!rec) {
            continue;
        }
        recList << rec;
    }
    return recList;
}

Recipient::Ptr RecipientsEditor::activeRecipient() const
{
    return qSharedPointerDynamicCast<Recipient>(activeData());
}

QString RecipientsEditor::recipientString(Recipient::Type type) const
{
    return recipientStringList(type).join(QLatin1String(", "));
}

QStringList RecipientsEditor::recipientStringList(Recipient::Type type) const
{
    QStringList selectedRecipients;
    for (const Recipient::Ptr &r : recipients()) {
        if (r->type() == type) {
            selectedRecipients << r->email();
        }
    }
    return selectedRecipients;
}

void RecipientsEditor::removeRecipient(const QString &recipient, Recipient::Type type)
{
    // search a line which matches recipient and type
    QListIterator<MultiplyingLine *> it(lines());
    MultiplyingLine *line = nullptr;
    while (it.hasNext()) {
        line = it.next();
        auto rec = qobject_cast<RecipientLineNG *>(line);
        if (rec) {
            if ((rec->recipient()->email() == recipient) && (rec->recipientType() == type)) {
                break;
            }
        }
    }
    if (line) {
        line->slotPropagateDeletion();
    }
}

void RecipientsEditor::saveDistributionList()
{
    std::unique_ptr<MessageComposer::DistributionListDialog> dlg(new MessageComposer::DistributionListDialog(this));
    dlg->setRecipients(recipients());
    dlg->exec();
}

void RecipientsEditor::selectRecipients()
{
    d->mSideWidget->pickRecipient();
}

void MessageComposer::RecipientsEditor::setRecentAddressConfig(KConfig *config)
{
    d->mRecentAddressConfig = config;
    if (config) {
        MultiplyingLine *line;
        foreach (line, lines()) {
            auto rec = qobject_cast<RecipientLineNG *>(line);
            if (rec) {
                rec->setRecentAddressConfig(config);
            }
        }
    }
}

void MessageComposer::RecipientsEditor::slotPickedRecipient(const Recipient &rec, bool &tooManyAddress)
{
    const Recipient::Type t = rec.type();
    tooManyAddress = addRecipient(rec.email(), t == Recipient::Undefined ? Recipient::To : t);
    mModified = true;
}

RecipientsPicker *RecipientsEditor::picker() const
{
    return d->mSideWidget->picker();
}

void RecipientsEditor::slotLineAdded(MultiplyingLine *line)
{
    // subtract 1 here, because we want the number of lines
    // before this line was added.
    const int count = lines().size() - 1;
    auto rec = qobject_cast<RecipientLineNG *>(line);
    if (!rec) {
        return;
    }

    if (d->mRecentAddressConfig) {
        rec->setRecentAddressConfig(d->mRecentAddressConfig);
    }

    if (count > 0) {
        if (count == 1) {
            auto last_rec = qobject_cast<RecipientLineNG *>(lines().constFirst());
            if (last_rec && (last_rec->recipientType() == Recipient::Bcc || last_rec->recipientType() == Recipient::ReplyTo)) {
                rec->setRecipientType(Recipient::To);
            } else {
                rec->setRecipientType(Recipient::Cc);
            }
        } else {
            auto last_rec = qobject_cast<RecipientLineNG *>(lines().at(lines().count() - 2));
            if (last_rec) {
                if (last_rec->recipientType() == Recipient::ReplyTo) {
                    rec->setRecipientType(Recipient::To);
                } else {
                    rec->setRecipientType(last_rec->recipientType());
                }
            }
        }
        line->fixTabOrder(lines().constLast()->tabOut());
    }
    connect(rec, &RecipientLineNG::countChanged, this, &RecipientsEditor::slotCalculateTotal);
}

void RecipientsEditor::slotLineDeleted(int pos)
{
    bool atLeastOneToLine = false;
    int firstCC = -1;
    for (int i = pos, total = lines().count(); i < total; ++i) {
        MultiplyingLine *line = lines().at(i);
        auto rec = qobject_cast<RecipientLineNG *>(line);
        if (rec) {
            if (rec->recipientType() == Recipient::To) {
                atLeastOneToLine = true;
            } else if ((rec->recipientType() == Recipient::Cc) && (firstCC < 0)) {
                firstCC = i;
            }
        }
    }

    if (!atLeastOneToLine && (firstCC >= 0)) {
        auto firstCCLine = qobject_cast<RecipientLineNG *>(lines().at(firstCC));
        if (firstCCLine) {
            firstCCLine->setRecipientType(Recipient::To);
        }
    }

    slotCalculateTotal();
}

bool RecipientsEditor::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress && qobject_cast<RecipientLineEdit *>(object)) {
        auto ke = static_cast<QKeyEvent *>(event);
        // Treats comma or semicolon as email separator, will automatically move focus
        // to a new line, basically preventing user from inputting more than one
        // email address per line, which breaks our opportunistic crypto in composer
        if (ke->key() == Qt::Key_Comma || (ke->key() == Qt::Key_Semicolon && MessageComposerSettings::self()->allowSemicolonAsAddressSeparator())) {
            auto line = qobject_cast<RecipientLineNG *>(object->parent());
            const auto split = KEmailAddress::splitAddressList(line->rawData() + QLatin1String(", "));
            if (split.size() > 1) {
                addRecipient(QString(), line->recipientType());
                setFocusBottom();
                return true;
            }
        }
    } else if (event->type() == QEvent::FocusIn && qobject_cast<RecipientLineEdit *>(object)) {
        Q_EMIT focusInRecipientLineEdit();
    }

    return false;
}

void RecipientsEditor::slotCalculateTotal()
{
    // Prevent endless recursion when splitting recipient
    if (d->mSkipTotal) {
        return;
    }
    int empty = 0;
    MultiplyingLine *line = nullptr;
    foreach (line, lines()) {
        auto rec = qobject_cast<RecipientLineNG *>(line);
        if (rec) {
            if (rec->isEmpty()) {
                ++empty;
            } else {
                const int recipientsCount = rec->recipientsCount();
                if (recipientsCount > 1) {
                    // Ensure we always have only one recipient per line
                    d->mSkipTotal = true;
                    Recipient::Ptr recipient = rec->recipient();
                    const auto split = KEmailAddress::splitAddressList(recipient->email());
                    bool maximumElementFound = false;
                    for (int i = 1 /* sic! */; i < split.count(); ++i) {
                        maximumElementFound = addRecipient(split[i], rec->recipientType());
                        if (maximumElementFound) {
                            break;
                        }
                    }
                    recipient->setEmail(split[0]);
                    rec->setData(recipient);
                    setFocusBottom(); // focus next empty entry
                    d->mSkipTotal = false;
                    if (maximumElementFound) {
                        return;
                    }
                }
            }
        }
    }
    // We always want at least one empty line
    if (empty == 0) {
        addData();
    }
    int count = 0;
    foreach (line, lines()) {
        auto rec = qobject_cast<RecipientLineNG *>(line);
        if (rec) {
            if (!rec->isEmpty()) {
                count++;
            }
        }
    }
    // update the side widget
    d->mSideWidget->setTotal(count, lines().count());
}

RecipientLineNG *RecipientsEditor::activeLine() const
{
    MultiplyingLine *line = MultiplyingLineEditor::activeLine();
    return qobject_cast<RecipientLineNG *>(line);
}
