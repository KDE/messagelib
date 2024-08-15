/*
    SPDX-FileCopyrightText: 2010 Casey Link <unnamedrambler@gmail.com>
    SPDX-FileCopyrightText: 2009-2010 Klaralvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>

    Refactored from earlier code by:
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2004 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "recipientseditorsidewidget.h"

#include "kwindowpositioner.h"
#include "recipientspicker.h"

#include <KLocalizedString>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

using namespace MessageComposer;

RecipientsEditorSideWidget::RecipientsEditorSideWidget(RecipientsEditor *view, QWidget *parent)
    : QWidget(parent)
    , mEditor(view)
    , mTotalLabel(new QLabel(this))
    , mDistributionListButton(new QPushButton(i18nc("@action:button", "Save List..."), this))
    , mSelectButton(new QPushButton(i18nc("@action:button Open recipient selection dialog.", "Se&lect..."), this))
{
    auto topLayout = new QVBoxLayout(this);

    topLayout->setContentsMargins({});
    topLayout->addStretch(1);

    mTotalLabel->setAlignment(Qt::AlignCenter);
    mTotalLabel->setTextFormat(Qt::PlainText);
    topLayout->addWidget(mTotalLabel);
    mTotalLabel->hide();

    topLayout->addStretch(1);

    topLayout->addWidget(mDistributionListButton);
    mDistributionListButton->hide();
    connect(mDistributionListButton, &QAbstractButton::clicked, this, &RecipientsEditorSideWidget::saveDistributionList);
    mDistributionListButton->setToolTip(i18nc("@info:tooltip", "Save recipients as distribution list"));

    topLayout->addWidget(mSelectButton);
    connect(mSelectButton, &QPushButton::clicked, this, &RecipientsEditorSideWidget::pickRecipient);
    mSelectButton->setToolTip(i18nc("@info:tooltip", "Select recipients from address book"));
    updateTotalToolTip();
}

RecipientsEditorSideWidget::~RecipientsEditorSideWidget() = default;

RecipientsPicker *RecipientsEditorSideWidget::picker() const
{
    if (!mRecipientPicker) {
        // hacks to allow picker() to be const in the presence of lazy loading
        auto non_const_this = const_cast<RecipientsEditorSideWidget *>(this);
        mRecipientPicker = new RecipientsPicker(non_const_this);
        connect(mRecipientPicker, &RecipientsPicker::pickedRecipient, non_const_this, &RecipientsEditorSideWidget::pickedRecipient);
        mPickerPositioner = new KWindowPositioner(mSelectButton, mRecipientPicker);
    }
    return mRecipientPicker;
}

void RecipientsEditorSideWidget::setFocus()
{
    mSelectButton->setFocus();
}

void RecipientsEditorSideWidget::setTotal(int recipients, int lines)
{
    QString labelText;
    if (recipients == 0) {
        labelText = i18nc("@info:status No recipients selected", "No recipients");
    } else {
        labelText = i18ncp("@info:status Number of recipients selected", "1 recipient", "%1 recipients", recipients);
    }

    if (lines > 3) {
        mTotalLabel->setText(labelText);
        mTotalLabel->show();
        updateTotalToolTip();
    } else {
        mTotalLabel->hide();
    }

    if (lines > 2) {
        mDistributionListButton->show();
    } else {
        mDistributionListButton->hide();
    }
}

void RecipientsEditorSideWidget::updateTotalToolTip()
{
    QString text;

    QString to;
    QString cc;
    QString bcc;
    QString replyTo;

    Recipient::List recipients = mEditor->recipients();
    Recipient::List::ConstIterator it;
    Recipient::List::ConstIterator end(recipients.constEnd());
    for (it = recipients.constBegin(); it != end; ++it) {
        const QString emailLine = QLatin1StringView("&nbsp;&nbsp;") + (*it)->email().toHtmlEscaped() + QLatin1StringView("<br/>");
        switch ((*it)->type()) {
        case Recipient::To:
            to += emailLine;
            break;
        case Recipient::Cc:
            cc += emailLine;
            break;
        case Recipient::Bcc:
            bcc += emailLine;
            break;
        case Recipient::ReplyTo:
            replyTo += emailLine;
            break;
        default:
            break;
        }
    }

    text += i18nc("@info:tooltip %1 list of emails", "To:%1", to);
    if (!cc.isEmpty()) {
        text += i18nc("@info:tooltip %1 list of emails", "CC:%1", cc);
    }
    if (!bcc.isEmpty()) {
        text += i18nc("@info:tooltip %1 list of emails", "BCC:%1", bcc);
    }
    if (!replyTo.isEmpty()) {
        text += i18nc("@info:tooltip %1 list of emails", "Reply-To:%1", replyTo);
    }

    mTotalLabel->setToolTip(QStringLiteral("<html><head><body>%1</body></head></html>").arg(text));
}

void RecipientsEditorSideWidget::pickRecipient()
{
    MessageComposer::RecipientsPicker *p = picker();
    Recipient::Ptr rec = mEditor->activeRecipient();
    if (rec) {
        p->setDefaultType(rec->type());
        p->setRecipients(mEditor->recipients());
        mPickerPositioner->reposition();
        p->show();
    }
}

#include "moc_recipientseditorsidewidget.cpp"
