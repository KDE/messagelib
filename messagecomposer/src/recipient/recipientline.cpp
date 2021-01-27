/*
    SPDX-FileCopyrightText: 2010 Casey Link <unnamedrambler@gmail.com>
    SPDX-FileCopyrightText: 2009-2010 Klaralvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "recipientline.h"

#include <KEmailAddress>
#include <KLocalizedString>

#include <QHBoxLayout>
#include <QKeyEvent>

using namespace MessageComposer;
using namespace KPIM;

RecipientComboBox::RecipientComboBox(QWidget *parent)
    : QComboBox(parent)
{
}

void RecipientComboBox::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Right) {
        Q_EMIT rightPressed();
    } else {
        QComboBox::keyPressEvent(ev);
    }
}

RecipientLineEdit::RecipientLineEdit(QWidget *parent)
    : ComposerLineEdit(parent)
{
    setExpandIntern(false);
}

void RecipientLineEdit::keyPressEvent(QKeyEvent *ev)
{
    // Laurent Bug:280153
    /*if ( ev->key() == Qt::Key_Backspace  &&  text().isEmpty() ) {
    ev->accept();
    Q_EMIT deleteMe();
    } else */
    if (ev->key() == Qt::Key_Left && cursorPosition() == 0 && !ev->modifiers().testFlag(Qt::ShiftModifier)) { // Shift would be pressed during selection
        Q_EMIT leftPressed();
    } else if (ev->key() == Qt::Key_Right && cursorPosition() == text().length()
               && !ev->modifiers().testFlag(Qt::ShiftModifier)) { // Shift would be pressed during selection
        Q_EMIT rightPressed();
    } else {
        MessageComposer::ComposerLineEdit::keyPressEvent(ev);
    }
}

RecipientLineNG::RecipientLineNG(QWidget *parent)
    : MultiplyingLine(parent)
    , mData(new Recipient)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QBoxLayout *topLayout = new QHBoxLayout(this);
    topLayout->setContentsMargins({});

    const QStringList recipientTypes = Recipient::allTypeLabels();

    mCombo = new RecipientComboBox(this);
    mCombo->addItems(recipientTypes);
    topLayout->addWidget(mCombo);
    mCombo->setToolTip(i18nc("@label:listbox", "Select type of recipient"));
    mEdit = new RecipientLineEdit(this);
    mEdit->setToolTip(i18n("Set the list of email addresses to receive this message"));
    mEdit->setClearButtonEnabled(true);
    topLayout->addWidget(mEdit);
    mEdit->installEventFilter(this);

    connect(mEdit, &RecipientLineEdit::returnPressed, this, &RecipientLineNG::slotReturnPressed);
    connect(mEdit, &RecipientLineEdit::deleteMe, this, &RecipientLineNG::slotPropagateDeletion);
    connect(mEdit, &QLineEdit::textChanged, this, &RecipientLineNG::analyzeLine);
    connect(mEdit, &RecipientLineEdit::focusUp, this, &RecipientLineNG::slotFocusUp);
    connect(mEdit, &RecipientLineEdit::focusDown, this, &RecipientLineNG::slotFocusDown);
    connect(mEdit, &RecipientLineEdit::rightPressed, this, &RecipientLineNG::rightPressed);
    connect(mEdit, &RecipientLineEdit::iconClicked, this, &RecipientLineNG::iconClicked);

    connect(mEdit, &RecipientLineEdit::leftPressed, mCombo, qOverload<>(&QWidget::setFocus));
    connect(mEdit, &RecipientLineEdit::editingFinished, this, &RecipientLineNG::slotEditingFinished);
    connect(mEdit, &RecipientLineEdit::clearButtonClicked, this, &RecipientLineNG::slotPropagateDeletion);
    connect(mCombo, &RecipientComboBox::rightPressed, mEdit, qOverload<>(&QWidget::setFocus));

    connect(mCombo, qOverload<int>(&RecipientComboBox::activated), this, &RecipientLineNG::slotTypeModified);

    connect(mEdit, &RecipientLineEdit::addAddress, this, &RecipientLineNG::slotAddRecipient);
}

void RecipientLineNG::slotEditingFinished()
{
    if (mEdit->text().isEmpty()) {
        Q_EMIT deleteLine(this);
    }
}

void RecipientLineNG::slotAddRecipient(const QString &email)
{
    Q_EMIT addRecipient(this, email);
    slotReturnPressed();
}

void RecipientLineNG::slotTypeModified()
{
    mModified = true;

    Q_EMIT typeModified(this);
}

void RecipientLineNG::analyzeLine(const QString &text)
{
    const QStringList r = KEmailAddress::splitAddressList(text);
    mRecipientsCount = r.count();
    mModified = true;
    Q_EMIT countChanged();
}

int RecipientLineNG::recipientsCount() const
{
    return mRecipientsCount;
}

void RecipientLineNG::setData(const MultiplyingLineData::Ptr &data)
{
    Recipient::Ptr rec = qSharedPointerDynamicCast<Recipient>(data);
    if (!rec) {
        return;
    }
    mData = rec;
    fieldsFromData();
}

MultiplyingLineData::Ptr RecipientLineNG::data() const
{
    if (isModified()) {
        const_cast<RecipientLineNG *>(this)->dataFromFields();
    }
    return mData;
}

void RecipientLineNG::dataFromFields()
{
    if (!mData) {
        return;
    }
    const QString editStr(mEdit->text());
    QString displayName, addrSpec, comment;
    if (KEmailAddress::splitAddress(editStr, displayName, addrSpec, comment) == KEmailAddress::AddressOk) {
        mData->setName(displayName);
    }
    mData->setEmail(editStr);
    mData->setType(Recipient::idToType(mCombo->currentIndex()));
    mModified = false;
}

void RecipientLineNG::fieldsFromData()
{
    if (!mData) {
        return;
    }
    mCombo->setCurrentIndex(Recipient::typeToId(mData->type()));
    mEdit->setText(mData->email());
}

void RecipientLineNG::activate()
{
    mEdit->setFocus();
}

bool RecipientLineNG::isActive() const
{
    return mEdit->hasFocus();
}

bool RecipientLineNG::isEmpty() const
{
    return mEdit->text().isEmpty();
}

bool RecipientLineNG::isModified() const
{
    return mModified || mEdit->isModified();
}

void RecipientLineNG::clearModified()
{
    mModified = false;
    mEdit->setModified(false);
}

int RecipientLineNG::setColumnWidth(int w)
{
    w = qMax(w, mCombo->sizeHint().width());
    mCombo->setFixedWidth(w);
    mCombo->updateGeometry();
    parentWidget()->updateGeometry();
    return w;
}

void RecipientLineNG::fixTabOrder(QWidget *previous)
{
    setTabOrder(previous, mCombo);
    setTabOrder(mCombo, mEdit);
}

QWidget *RecipientLineNG::tabOut() const
{
    return mEdit;
}

void RecipientLineNG::clear()
{
    mRecipientsCount = 0;
    mEdit->clear();
}

bool RecipientLineNG::canDeleteLineEdit() const
{
    return mEdit->canDeleteLineEdit();
}

void RecipientLineNG::setCompletionMode(KCompletion::CompletionMode mode)
{
    mEdit->setCompletionMode(mode);
}

Recipient::Type RecipientLineNG::recipientType() const
{
    return Recipient::idToType(mCombo->currentIndex());
}

void RecipientLineNG::setRecipientType(Recipient::Type type)
{
    mCombo->setCurrentIndex(Recipient::typeToId(type));
    slotTypeModified();
}

void RecipientLineNG::setRecentAddressConfig(KConfig *config)
{
    mEdit->setRecentAddressConfig(config);
}

Recipient::Ptr RecipientLineNG::recipient() const
{
    return qSharedPointerDynamicCast<Recipient>(data());
}

void RecipientLineNG::setIcon(const QIcon &icon, const QString &tooltip)
{
    mEdit->setIcon(icon, tooltip);
}

void RecipientLineNG::setEnableIndexSearch(bool enableIndexSearch)
{
    mEdit->setEnableBalooSearch(enableIndexSearch);
}

bool RecipientLineNG::enableIndexSearch() const
{
    return mEdit->enableBalooSearch();
}

void RecipientLineNG::setEnableAkonadiSearch(bool enableAkonadiSearch)
{
    mEdit->setEnableAkonadiSearch(enableAkonadiSearch);
}

bool RecipientLineNG::enableAkonadiSearch() const
{
    return mEdit->enableAkonadiSearch();
}

QString RecipientLineNG::rawData() const
{
    return mEdit->text();
}

bool RecipientLineNG::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == mEdit) {
        if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
            Q_EMIT activeChanged();
        }
    }

    return false;
}
