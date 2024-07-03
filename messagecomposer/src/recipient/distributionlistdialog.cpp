/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    This file was part of KMail.
    SPDX-FileCopyrightText: 2005 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "distributionlistdialog.h"

#include <Akonadi/CollectionDialog>
#include <Akonadi/ContactGroupSearchJob>
#include <Akonadi/ContactSearchJob>
#include <Akonadi/ItemCreateJob>
#include <KEmailAddress>

#include "messagecomposer_debug.h"
#include <KLocalizedString>
#include <KMessageBox>
#include <QInputDialog>
#include <QLineEdit>

#include <KConfigGroup>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

using namespace MessageComposer;

namespace MessageComposer
{
class DistributionListItem : public QTreeWidgetItem
{
public:
    explicit DistributionListItem(QTreeWidget *tree)
        : QTreeWidgetItem(tree)
    {
        setFlags(flags() | Qt::ItemIsUserCheckable);
    }

    void setAddressee(const KContacts::Addressee &a, const QString &email)
    {
        init(a, email);
    }

    void init(const KContacts::Addressee &a, const QString &email)
    {
        mAddressee = a;
        mEmail = email;
        mId = -1;
        setText(0, mAddressee.realName());
        setText(1, mEmail);
    }

    [[nodiscard]] KContacts::Addressee addressee() const
    {
        return mAddressee;
    }

    [[nodiscard]] QString email() const
    {
        return mEmail;
    }

    [[nodiscard]] bool isTransient() const
    {
        return mId == -1;
    }

    void setId(Akonadi::Item::Id id)
    {
        mId = id;
    }

    [[nodiscard]] Akonadi::Item::Id id() const
    {
        return mId;
    }

private:
    KContacts::Addressee mAddressee;
    QString mEmail;
    Akonadi::Item::Id mId = -1;
};
}

DistributionListDialog::DistributionListDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Save Distribution List"));
    setModal(false);

    auto mainLayout = new QVBoxLayout(this);

    auto topFrame = new QWidget(this);
    mainLayout->addWidget(topFrame);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    mUser1Button = new QPushButton(this);
    buttonBox->addButton(mUser1Button, QDialogButtonBox::ActionRole);
    mUser1Button->setText(i18nc("@action:button", "Save List"));
    mUser1Button->setEnabled(false);
    mUser1Button->setDefault(true);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DistributionListDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DistributionListDialog::reject);

    mainLayout->addWidget(buttonBox);

    auto topLayout = new QVBoxLayout(topFrame);
    topLayout->setContentsMargins({});

    auto titleLayout = new QHBoxLayout;
    topLayout->addLayout(titleLayout);

    auto label = new QLabel(i18nc("@label:textbox Name of the distribution list.", "&Name:"), topFrame);
    titleLayout->addWidget(label);

    mTitleEdit = new QLineEdit(topFrame);
    mTitleEdit->setPlaceholderText(i18nc("@info:placeholder", "Name of Distribution List"));
    titleLayout->addWidget(mTitleEdit);
    mTitleEdit->setFocus();
    mTitleEdit->setClearButtonEnabled(true);
    label->setBuddy(mTitleEdit);

    mRecipientsList = new QTreeWidget(topFrame);
    mRecipientsList->setHeaderLabels(QStringList() << i18nc("@title:column Name of the recipient", "Name")
                                                   << i18nc("@title:column Email of the recipient", "Email"));
    mRecipientsList->setRootIsDecorated(false);
    mRecipientsList->header()->setSectionsMovable(false);
    topLayout->addWidget(mRecipientsList);
    connect(mUser1Button, &QPushButton::clicked, this, &DistributionListDialog::slotUser1);
    connect(mTitleEdit, &QLineEdit::textChanged, this, &DistributionListDialog::slotTitleChanged);
    readConfig();
}

DistributionListDialog::~DistributionListDialog()
{
    writeConfig();
}

// This starts one ContactSearchJob for each of the specified recipients.
void DistributionListDialog::setRecipients(const Recipient::List &recipients)
{
    Recipient::List::ConstIterator end(recipients.constEnd());
    for (Recipient::List::ConstIterator it = recipients.constBegin(); it != end; ++it) {
        const QStringList emails = KEmailAddress::splitAddressList((*it)->email());
        QStringList::ConstIterator end2(emails.constEnd());
        for (QStringList::ConstIterator it2 = emails.constBegin(); it2 != end2; ++it2) {
            QString name;
            QString email;
            KContacts::Addressee::parseEmailAddress(*it2, name, email);
            if (!email.isEmpty()) {
                auto job = new Akonadi::ContactSearchJob(this);
                job->setQuery(Akonadi::ContactSearchJob::Email, email.toLower(), Akonadi::ContactSearchJob::ExactMatch);
                job->setProperty("name", name);
                job->setProperty("email", email);
                connect(job, &Akonadi::ContactSearchJob::result, this, &DistributionListDialog::slotDelayedSetRecipients);
            }
        }
    }
}

// This result slot will be called once for each of the original recipients.
// There could potentially be more than one Akonadi item returned per
// recipient, in the case where email addresses are duplicated between contacts.
void DistributionListDialog::slotDelayedSetRecipients(KJob *job)
{
    const Akonadi::ContactSearchJob *searchJob = qobject_cast<Akonadi::ContactSearchJob *>(job);
    const Akonadi::Item::List akItems = searchJob->items();

    const QString email = searchJob->property("email").toString();
    QString name = searchJob->property("name").toString();
    if (name.isEmpty()) {
        const int index = email.indexOf(QLatin1Char('@'));
        if (index != -1) {
            name = email.left(index);
        } else {
            name = email;
        }
    }

    if (akItems.isEmpty()) {
        KContacts::Addressee contact;
        contact.setNameFromString(name);
        contact.addEmail(KContacts::Email(email));

        auto item = new DistributionListItem(mRecipientsList);
        item->setAddressee(contact, email);
        item->setCheckState(0, Qt::Checked);
    } else {
        bool isFirst = true;
        for (const Akonadi::Item &akItem : std::as_const(akItems)) {
            if (akItem.hasPayload<KContacts::Addressee>()) {
                const auto contact = akItem.payload<KContacts::Addressee>();

                auto item = new DistributionListItem(mRecipientsList);
                item->setAddressee(contact, email);

                // Need to record the Akonadi ID of the contact, so that
                // it can be added as a reference later.  Setting an ID
                // makes the item non-transient.
                item->setId(akItem.id());

                // If there were multiple contacts returned for an email address,
                // then check the first one and uncheck any subsequent ones.
                if (isFirst) {
                    item->setCheckState(0, Qt::Checked);
                    isFirst = false;
                } else {
                    // Need this to create an unchecked item, as otherwise the
                    // item will have no checkbox at all.
                    item->setCheckState(0, Qt::Unchecked);
                }
            }
        }
    }
}

void DistributionListDialog::slotUser1()
{
    bool isEmpty = true;
    const int numberOfTopLevel(mRecipientsList->topLevelItemCount());
    for (int i = 0; i < numberOfTopLevel; ++i) {
        auto item = static_cast<DistributionListItem *>(mRecipientsList->topLevelItem(i));
        if (item && item->checkState(0) == Qt::Checked) {
            isEmpty = false;
            break;
        }
    }

    if (isEmpty) {
        KMessageBox::information(this,
                                 i18nc("@info",
                                       "There are no recipients in your list. "
                                       "First select some recipients, "
                                       "then try again."));
        return;
    }

    QString name = mTitleEdit->text();

    if (name.isEmpty()) {
        bool ok = false;
        name = QInputDialog::getText(this,
                                     i18nc("@title:window", "New Distribution List"),
                                     i18nc("@label:textbox", "Please enter name:"),
                                     QLineEdit::Normal,
                                     QString(),
                                     &ok);
        if (!ok || name.isEmpty()) {
            return;
        }
    }

    auto job = new Akonadi::ContactGroupSearchJob();
    job->setQuery(Akonadi::ContactGroupSearchJob::Name, name);
    job->setProperty("name", name);
    connect(job, &Akonadi::ContactSearchJob::result, this, &DistributionListDialog::slotDelayedUser1);
}

void DistributionListDialog::slotDelayedUser1(KJob *job)
{
    const Akonadi::ContactGroupSearchJob *searchJob = qobject_cast<Akonadi::ContactGroupSearchJob *>(job);
    const QString name = searchJob->property("name").toString();

    if (!searchJob->contactGroups().isEmpty()) {
        qDebug() << " searchJob->contactGroups()" << searchJob->contactGroups().count();
        KMessageBox::information(this,
                                 xi18nc("@info",
                                        "<para>Distribution list with the given name <resource>%1</resource> "
                                        "already exists. Please select a different name.</para>",
                                        name));
        return;
    }

    QPointer<Akonadi::CollectionDialog> dlg = new Akonadi::CollectionDialog(Akonadi::CollectionDialog::KeepTreeExpanded, nullptr, this);
    dlg->setMimeTypeFilter(QStringList() << KContacts::Addressee::mimeType() << KContacts::ContactGroup::mimeType());
    dlg->setAccessRightsFilter(Akonadi::Collection::CanCreateItem);
    dlg->setWindowTitle(i18nc("@title:window", "Select Address Book"));
    dlg->setDescription(i18n("Select the address book folder to store the contact group in:"));
    if (dlg->exec()) {
        const Akonadi::Collection targetCollection = dlg->selectedCollection();
        delete dlg;

        KContacts::ContactGroup group(name);
        const int numberOfTopLevel(mRecipientsList->topLevelItemCount());
        for (int i = 0; i < numberOfTopLevel; ++i) {
            auto item = static_cast<DistributionListItem *>(mRecipientsList->topLevelItem(i));
            if (item && item->checkState(0) == Qt::Checked) {
                qCDebug(MESSAGECOMPOSER_LOG) << item->addressee().fullEmail() << item->addressee().uid();
                if (item->isTransient()) {
                    group.append(KContacts::ContactGroup::Data(item->addressee().realName(), item->email()));
                } else {
                    KContacts::ContactGroup::ContactReference reference(QString::number(item->id()));
                    if (item->email() != item->addressee().preferredEmail()) {
                        reference.setPreferredEmail(item->email());
                    }
                    group.append(reference);
                }
            }
        }

        Akonadi::Item groupItem(KContacts::ContactGroup::mimeType());
        groupItem.setPayload<KContacts::ContactGroup>(group);

        Akonadi::Job *createJob = new Akonadi::ItemCreateJob(groupItem, targetCollection);
        connect(createJob, &Akonadi::ItemCreateJob::result, this, &DistributionListDialog::slotContactGroupCreateJobResult);
    }

    delete dlg;
}

void DistributionListDialog::slotContactGroupCreateJobResult(KJob *job)
{
    if (job->error()) {
        KMessageBox::information(this, i18n("Unable to create distribution list: %1", job->errorString()));
        qCWarning(MESSAGECOMPOSER_LOG) << "Unable to create distribution list:" << job->errorText();
    } else {
        accept();
    }
}

void DistributionListDialog::slotTitleChanged(const QString &text)
{
    mUser1Button->setEnabled(!text.trimmed().isEmpty());
}

namespace
{
static const char myDistributionListDialogGroupName[] = "DistributionListDialog";
}

void DistributionListDialog::readConfig()
{
    KSharedConfig::Ptr cfg = KSharedConfig::openStateConfig();
    KConfigGroup group(cfg, QLatin1StringView(myDistributionListDialogGroupName));
    const QSize size = group.readEntry("Size", QSize());
    if (!size.isEmpty()) {
        resize(size);
    }
    mRecipientsList->header()->restoreState(group.readEntry("Header", QByteArray()));
}

void DistributionListDialog::writeConfig()
{
    KSharedConfig::Ptr cfg = KSharedConfig::openStateConfig();
    KConfigGroup group(cfg, QLatin1StringView(myDistributionListDialogGroupName));
    group.writeEntry("Size", size());
    group.writeEntry("Header", mRecipientsList->header()->saveState());
}

#include "moc_distributionlistdialog.cpp"
