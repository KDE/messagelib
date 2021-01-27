/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  Based on KMail code by various authors (kmmsgpartdlg).

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentpropertiesdialog.h"

#include "attachmentfrommimecontentjob.h"
#include "ui_attachmentpropertiesdialog.h"
#include "ui_attachmentpropertiesdialog_readonly.h"

#include "messagecore_debug.h"
#include <KAboutData>
#include <PimCommon/PimUtil>

#include <kmime/kmime_content.h>
#include <kmime/kmime_headers.h>

#include <KFormat>
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QMimeDatabase>
#include <QMimeType>
#include <QPushButton>
#include <QVBoxLayout>

using namespace MessageCore;

class Q_DECL_HIDDEN MessageCore::AttachmentPropertiesDialog::Private
{
public:
    Private(AttachmentPropertiesDialog *qq)
        : q(qq)
    {
    }

    ~Private()
    {
        delete ui;
        delete uiReadOnly;
    }

    void init(const AttachmentPart::Ptr &part, bool readOnly);
    void polishUi();
    void mimeTypeChanged(const QString &type); // slot
    void populateEncodings();
    void populateMimeTypes();
    void populateWhatsThis();
    void loadFromPart();
    void saveToPart();

    AttachmentPropertiesDialog *const q;
    AttachmentPart::Ptr mPart;

    Ui::AttachmentPropertiesDialog *ui = nullptr;
    Ui::AttachmentPropertiesDialogReadOnly *uiReadOnly = nullptr;
    QVBoxLayout *mainLayout = nullptr;
    bool mReadOnly = false;
};

void AttachmentPropertiesDialog::Private::init(const AttachmentPart::Ptr &part, bool readOnly)
{
    mReadOnly = readOnly;
    mPart = part;

    auto widget = new QWidget(q);
    mainLayout = new QVBoxLayout;
    q->setLayout(mainLayout);

    mainLayout->addWidget(widget);
    if (mReadOnly) {
        uiReadOnly = new Ui::AttachmentPropertiesDialogReadOnly;
        uiReadOnly->setupUi(widget);
    } else {
        ui = new Ui::AttachmentPropertiesDialog;
        ui->setupUi(widget);
    }
    polishUi();
    q->setModal(true);

    loadFromPart();
}

void AttachmentPropertiesDialog::Private::polishUi()
{
    // Tweak the dialog, depending on whether it is read-only or not.
    QDialogButtonBox *buttonBox = nullptr;

    if (mReadOnly) {
        buttonBox = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Close, q);
    } else {
        // Update the icon when the selected mime type changes.

        connect(ui->mimeType, qOverload<const QString &>(&QComboBox::currentTextChanged), q, [this](const QString &str) {
            mimeTypeChanged(str);
        });
        populateMimeTypes();
        populateEncodings();
        buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help, q);
        QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
        okButton->setDefault(true);
        okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    }
    q->connect(buttonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, q, &AttachmentPropertiesDialog::slotHelp);
    q->connect(buttonBox, &QDialogButtonBox::accepted, q, &QDialog::accept);
    q->connect(buttonBox, &QDialogButtonBox::rejected, q, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
    populateWhatsThis();
}

void AttachmentPropertiesDialog::Private::mimeTypeChanged(const QString &type)
{
    QMimeDatabase db;
    const QMimeType mimeType = db.mimeTypeForName(type);
    QPixmap pix =
        QIcon::fromTheme(mimeType.iconName(), QIcon::fromTheme(QStringLiteral("unknown"))).pixmap(q->style()->pixelMetric(QStyle::PM_MessageBoxIconSize));

    if (mReadOnly) {
        uiReadOnly->mimeIcon->setPixmap(pix);
    } else {
        ui->mimeIcon->setPixmap(pix);
    }
}

void AttachmentPropertiesDialog::Private::populateWhatsThis()
{
    // FIXME These are such a mess... Make them straightforward and pretty.

    const QString msgMimeType = i18n(
        "<p>The <em>MIME type</em> of the file:</p>"
        "<p>Normally, you do not need to touch this setting, since the "
        "type of the file is automatically checked; but, sometimes, %1 "
        "may not detect the type correctly -- here is where you can fix "
        "that.</p>",
        KAboutData::applicationData().componentName());

    const QString msgSize = i18n(
        "<p>The estimated size of the attachment:</p>"
        "<p>Note that, in an email message, a binary file encoded with "
        "base64 will take up four thirds the actual size of the file.</p>");

    const QString msgName = i18n(
        "<p>The file name of the part:</p>"
        "<p>Although this defaults to the name of the attached file, "
        "it does not specify the file to be attached; rather, it "
        "suggests a file name to be used by the recipient's mail agent "
        "when saving the part to disk.</p>");

    const QString msgDescription = i18n(
        "<p>A description of the part:</p>"
        "<p>This is just an informational description of the part, "
        "much like the Subject is for the whole message; most "
        "mail agents will show this information in their message "
        "previews alongside the attachment's icon.</p>");

    const QString msgEncoding = i18n(
        "<p>The transport encoding of this part:</p>"
        "<p>Normally, you do not need to change this, since %1 will use "
        "a decent default encoding, depending on the MIME type; yet, "
        "sometimes, you can significantly reduce the size of the "
        "resulting message, e.g. if a PostScript file does not contain "
        "binary data, but consists of pure text -- in this case, choosing "
        "\"quoted-printable\" over the default \"base64\" will save up "
        "to 25% in resulting message size.</p>",
        KAboutData::applicationData().componentName());

    const QString msgAutoDisplay = i18n(
        "<p>Check this option if you want to suggest to the "
        "recipient the automatic (inline) display of this part in the "
        "message preview, instead of the default icon view;</p>"
        "<p>Technically, this is carried out by setting this part's "
        "<em>Content-Disposition</em> header field to \"inline\" "
        "instead of the default \"attachment\".</p>");

    const QString msgSign = i18n(
        "<p>Check this option if you want this message part to be "
        "signed.</p>"
        "<p>The signature will be made with the key that you associated "
        "with the currently-selected identity.</p>");

    const QString msgEncrypt = i18n(
        "<p>Check this option if you want this message part to be "
        "encrypted.</p>"
        "<p>The part will be encrypted for the recipients of this "
        "message.</p>");

    if (mReadOnly) {
        uiReadOnly->size->setWhatsThis(msgSize);
        uiReadOnly->name->setWhatsThis(msgName);
        uiReadOnly->encoding->setWhatsThis(msgEncoding);
    } else {
        ui->mimeType->setWhatsThis(msgMimeType);
        ui->size->setWhatsThis(msgSize);
        ui->name->setWhatsThis(msgName);
        ui->encrypt->setWhatsThis(msgEncrypt);
        ui->sign->setWhatsThis(msgSign);
        ui->autoDisplay->setWhatsThis(msgAutoDisplay);
        ui->encoding->setWhatsThis(msgEncoding);
        ui->description->setWhatsThis(msgDescription);
    }
}

void AttachmentPropertiesDialog::Private::populateEncodings()
{
    using namespace KMime;
    using namespace KMime::Headers;

    ui->encoding->clear();
    ui->encoding->addItem(nameForEncoding(CE7Bit), int(CE7Bit));
    ui->encoding->addItem(nameForEncoding(CE8Bit), int(CE8Bit));
    ui->encoding->addItem(nameForEncoding(CEquPr), int(CEquPr));
    ui->encoding->addItem(nameForEncoding(CEbase64), int(CEbase64));

    // TODO 8bit should be disabled if it is disabled in Settings.
    // Also, if it's a message/* part, base64 and qp should be disabled.
    // But since this is a dialog for power users anyway, let them shoot
    // themselves in the foot.  (The AttachmentJob will fail when they
    // try to compose the message.)
}

void AttachmentPropertiesDialog::Private::populateMimeTypes()
{
    const QStringList list = QStringList() << QStringLiteral("text/html") << QStringLiteral("text/plain") << QStringLiteral("image/gif")
                                           << QStringLiteral("image/jpeg") << QStringLiteral("image/png") << QStringLiteral("application/octet-stream")
                                           << QStringLiteral("application/x-gunzip") << QStringLiteral("application/zip");

    ui->mimeType->addItems(list);
}

void AttachmentPropertiesDialog::Private::loadFromPart()
{
    Q_ASSERT(mPart);

    if (mReadOnly) {
        uiReadOnly->mimeType->setText(QString::fromLatin1(mPart->mimeType()));
        mimeTypeChanged(QString::fromLatin1(mPart->mimeType()));
        uiReadOnly->size->setText(KFormat().formatByteSize(mPart->size()));
        uiReadOnly->name->setText(mPart->name().isEmpty() ? mPart->fileName() : mPart->name());
        if (mPart->description().isEmpty()) {
            uiReadOnly->description->hide();
            uiReadOnly->descriptionLabel->hide();
        } else {
            uiReadOnly->description->setText(mPart->description());
        }
        uiReadOnly->encoding->setText(KMime::nameForEncoding(mPart->encoding()));
    } else {
        const QString mimeType = QString::fromLatin1(mPart->mimeType());
        const int index = ui->mimeType->findText(mimeType);
        if (index == -1) {
            ui->mimeType->insertItem(0, mimeType);
            ui->mimeType->setCurrentIndex(0);
        } else {
            ui->mimeType->setCurrentIndex(index);
        }
        ui->size->setText(KFormat().formatByteSize(mPart->size()));
        ui->name->setText(mPart->name().isEmpty() ? mPart->fileName() : mPart->name());
        ui->description->setText(mPart->description());
        ui->encoding->setCurrentIndex(int(mPart->encoding()));
        ui->autoDisplay->setChecked(mPart->isInline());
        ui->encrypt->setChecked(mPart->isEncrypted());
        ui->sign->setChecked(mPart->isSigned());
    }
}

static QString removeNewlines(const QString &input)
{
    QString ret(input);
    ret.replace(QLatin1Char('\n'), QLatin1Char(' '));
    return ret;
}

void AttachmentPropertiesDialog::Private::saveToPart()
{
    Q_ASSERT(mPart);
    Q_ASSERT(!mReadOnly);

    if (mReadOnly) {
        return;
    }
    mPart->setMimeType(ui->mimeType->currentText().toLatin1());
    const QString name = removeNewlines(ui->name->text());
    mPart->setName(name);
    mPart->setFileName(name);
    mPart->setDescription(removeNewlines(ui->description->text()));
    mPart->setInline(ui->autoDisplay->isChecked());
    mPart->setSigned(ui->sign->isChecked());
    mPart->setEncrypted(ui->encrypt->isChecked());
    mPart->setInline(ui->autoDisplay->isChecked());

    if (ui->mimeType->currentText().startsWith(QLatin1String("message")) && ui->encoding->itemData(ui->encoding->currentIndex()) != KMime::Headers::CE7Bit
        && ui->encoding->itemData(ui->encoding->currentIndex()) != KMime::Headers::CE8Bit) {
        qCWarning(MESSAGECORE_LOG) << R"(Encoding on message/rfc822 must be "7bit" or "8bit".)";
    }

    mPart->setEncoding(KMime::Headers::contentEncoding(ui->encoding->itemData(ui->encoding->currentIndex()).toInt()));
}

AttachmentPropertiesDialog::AttachmentPropertiesDialog(const AttachmentPart::Ptr &part, bool readOnly, QWidget *parent)
    : QDialog(parent)
    , d(new Private(this))
{
    d->init(part, readOnly);
    setWindowTitle(i18nc("@title:window", "Attachment Properties"));
}

AttachmentPropertiesDialog::AttachmentPropertiesDialog(const KMime::Content *content, QWidget *parent)
    : QDialog(parent)
    , d(new Private(this))
{
    auto job = new AttachmentFromMimeContentJob(content, this);
    job->exec();
    if (job->error()) {
        qCCritical(MESSAGECORE_LOG) << "AttachmentFromMimeContentJob failed." << job->errorString();
    }

    const AttachmentPart::Ptr part = job->attachmentPart();
    d->init(part, true);
    setWindowTitle(i18nc("@title:window", "Attachment Properties"));
}

AttachmentPropertiesDialog::~AttachmentPropertiesDialog()
{
    delete d;
}

AttachmentPart::Ptr AttachmentPropertiesDialog::attachmentPart() const
{
    return d->mPart;
}

bool AttachmentPropertiesDialog::isEncryptEnabled() const
{
    if (d->ui) {
        return d->ui->encrypt->isEnabled();
    }
    return false;
}

void AttachmentPropertiesDialog::setEncryptEnabled(bool enabled)
{
    if (d->ui) {
        d->ui->encrypt->setEnabled(enabled);
    }
}

bool AttachmentPropertiesDialog::isSignEnabled() const
{
    if (d->ui) {
        return d->ui->sign->isEnabled();
    }
    return false;
}

void AttachmentPropertiesDialog::setSignEnabled(bool enabled)
{
    if (d->ui) {
        d->ui->sign->setEnabled(enabled);
    }
}

void AttachmentPropertiesDialog::accept()
{
    if (!d->mReadOnly) {
        d->saveToPart();
    }

    QDialog::accept();
}

void AttachmentPropertiesDialog::slotHelp()
{
    PimCommon::Util::invokeHelp(QStringLiteral("kmail2/the-composer-window.html"), QStringLiteral("attachments"));
}

#include "moc_attachmentpropertiesdialog.cpp"
