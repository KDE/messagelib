/*
   Copyright (C) 2014-2019 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "followupreminderselectdatedialog.h"

#include <KLocalizedString>
#include <KSharedConfig>
#include <KMessageBox>
#include <KDateComboBox>

#include <AkonadiWidgets/CollectionComboBox>

#include <KCalendarCore/Todo>

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
using namespace MessageComposer;
class MessageComposer::FollowUpReminderSelectDateDialogPrivate
{
public:
    FollowUpReminderSelectDateDialogPrivate()
    {
    }

    KDateComboBox *mDateComboBox = nullptr;
    Akonadi::CollectionComboBox *mCollectionCombobox = nullptr;
    QPushButton *mOkButton = nullptr;
};

FollowUpReminderSelectDateDialog::FollowUpReminderSelectDateDialog(QWidget *parent, QAbstractItemModel *model)
    : QDialog(parent)
    , d(new MessageComposer::FollowUpReminderSelectDateDialogPrivate)
{
    setWindowTitle(i18n("Select Date"));
    QVBoxLayout *topLayout = new QVBoxLayout(this);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    d->mOkButton->setObjectName(QStringLiteral("ok_button"));
    d->mOkButton->setDefault(true);
    d->mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &FollowUpReminderSelectDateDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &FollowUpReminderSelectDateDialog::reject);
    setModal(true);

    QWidget *mainWidget = new QWidget(this);
    topLayout->addWidget(mainWidget);
    topLayout->addWidget(buttonBox);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    QFormLayout *formLayout = new QFormLayout;
    formLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(formLayout);

    d->mDateComboBox = new KDateComboBox;
    QDate currentDate = QDate::currentDate().addDays(1);
    d->mDateComboBox->setMinimumDate(QDate::currentDate());
    d->mDateComboBox->setObjectName(QStringLiteral("datecombobox"));

    d->mDateComboBox->setDate(currentDate);

    formLayout->addRow(i18n("Date:"), d->mDateComboBox);

    d->mCollectionCombobox = new Akonadi::CollectionComboBox(model);
    d->mCollectionCombobox->setMinimumWidth(250);
    d->mCollectionCombobox->setAccessRightsFilter(Akonadi::Collection::CanCreateItem);
    d->mCollectionCombobox->setMimeTypeFilter(QStringList() << KCalendarCore::Todo::todoMimeType());
    d->mCollectionCombobox->setObjectName(QStringLiteral("collectioncombobox"));

    formLayout->addRow(i18n("Store ToDo in:"), d->mCollectionCombobox);

    connect(d->mDateComboBox->lineEdit(), &QLineEdit::textChanged, this, &FollowUpReminderSelectDateDialog::slotDateChanged);
    connect(d->mCollectionCombobox, qOverload<int>(&Akonadi::CollectionComboBox::currentIndexChanged), this, &FollowUpReminderSelectDateDialog::updateOkButton);
    updateOkButton();
}

FollowUpReminderSelectDateDialog::~FollowUpReminderSelectDateDialog()
{
    delete d;
}

void FollowUpReminderSelectDateDialog::updateOkButton()
{
    d->mOkButton->setEnabled(!d->mDateComboBox->lineEdit()->text().isEmpty()
                             && d->mDateComboBox->date().isValid()
                             && (d->mCollectionCombobox->count() > 0)
                             && d->mCollectionCombobox->currentCollection().isValid());
}

void FollowUpReminderSelectDateDialog::slotDateChanged()
{
    updateOkButton();
}

QDate FollowUpReminderSelectDateDialog::selectedDate() const
{
    return d->mDateComboBox->date();
}

Akonadi::Collection FollowUpReminderSelectDateDialog::collection() const
{
    return d->mCollectionCombobox->currentCollection();
}

void FollowUpReminderSelectDateDialog::accept()
{
    const QDate date = selectedDate();
    if (date < QDate::currentDate()) {
        KMessageBox::error(this, i18n("The selected date must be greater than the current date."), i18n("Invalid date"));
        return;
    }
    if (!d->mCollectionCombobox->currentCollection().isValid()) {
        KMessageBox::error(this, i18n("The selected folder is not valid."), i18n("Invalid folder"));
        return;
    }
    QDialog::accept();
}
