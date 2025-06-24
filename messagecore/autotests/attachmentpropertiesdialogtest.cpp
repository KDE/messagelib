/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentpropertiesdialogtest.h"
using namespace Qt::Literals::StringLiterals;

#include "qtest_messagecore.h"

#include <QCheckBox>

#include "messagecore_debug.h"
#include <QComboBox>
#include <QLineEdit>
#include <QTest>

#include <KMime/Content>
using namespace KMime;

#include <MessageCore/AttachmentPart>
#include <MessageCore/AttachmentPropertiesDialog>
using namespace MessageCore;

QTEST_MAIN(AttachmentPropertiesDialogTest)

AttachmentPropertiesDialogTest::AttachmentPropertiesDialogTest(QObject *parent)
    : QObject(parent)
{
}

void AttachmentPropertiesDialogTest::testAttachmentPartReadWrite()
{
    // Sample data.
    const QString name = u"old name"_s;
    const QString newName = u"new name"_s;
    const QString description = u"old description"_s;
    const QString newDescription = u"new description"_s;
    const QByteArray data("12345");
    const QByteArray mimeType("text/plain");
    const QByteArray newMimeType("x-weird/x-type");
    const Headers::contentEncoding encoding = Headers::CEquPr;
    const Headers::contentEncoding newEncoding = Headers::CE7Bit;
    const bool autoDisplay = true;
    const bool encrypt = true;
    const bool sign = true;

    // Create the part.
    AttachmentPart::Ptr part = AttachmentPart::Ptr(new AttachmentPart);
    part->setName(name);
    part->setDescription(description);
    part->setData(data);
    part->setMimeType(mimeType);
    part->setEncoding(encoding);
    part->setInline(autoDisplay);
    part->setEncrypted(encrypt);
    part->setSigned(sign);

    // Show the dialog and verify that it is accurate.
    auto dialog = new AttachmentPropertiesDialog(part);
    dialog->show();
    auto nameEdit = dialog->findChild<QLineEdit *>(u"name"_s);
    Q_ASSERT(nameEdit);
    QCOMPARE(nameEdit->text(), name);
    auto descriptionEdit = dialog->findChild<QLineEdit *>(u"description"_s);
    Q_ASSERT(descriptionEdit);
    QCOMPARE(descriptionEdit->text(), description);
    auto mimeTypeCombo = dialog->findChild<QComboBox *>(u"mimeType"_s);
    Q_ASSERT(mimeTypeCombo);
    QCOMPARE(mimeTypeCombo->currentText().toLatin1(), mimeType);
    auto encodingCombo = dialog->findChild<QComboBox *>(u"encoding"_s);
    Q_ASSERT(encodingCombo);
    QCOMPARE(encodingCombo->currentIndex(), int(encoding));
    auto autoDisplayCheck = dialog->findChild<QCheckBox *>(u"autoDisplay"_s);
    Q_ASSERT(autoDisplayCheck);
    QCOMPARE(autoDisplayCheck->isChecked(), autoDisplay);
    auto encryptCheck = dialog->findChild<QCheckBox *>(u"encrypt"_s);
    Q_ASSERT(encryptCheck);
    QCOMPARE(encryptCheck->isChecked(), encrypt);
    auto signCheck = dialog->findChild<QCheckBox *>(u"sign"_s);
    Q_ASSERT(signCheck);
    QCOMPARE(signCheck->isChecked(), sign);
    // QTest::qWait( 5000 );

    // Make some changes in the dialog.
    nameEdit->setText(newName);
    descriptionEdit->setText(newDescription);
    const QString comboBoxMimeType = QString::fromLatin1(newMimeType);
    int index = mimeTypeCombo->findText(comboBoxMimeType);
    if (index == -1) {
        mimeTypeCombo->insertItem(0, comboBoxMimeType);
        mimeTypeCombo->setCurrentIndex(0);
    } else {
        mimeTypeCombo->setCurrentIndex(index);
    }
    encodingCombo->setCurrentIndex(int(newEncoding));
    autoDisplayCheck->setChecked(!autoDisplay);
    encryptCheck->setChecked(!encrypt);
    signCheck->setChecked(!sign);

    // Click on 'OK' and verify changes.
    dialog->accept();
    delete dialog;
    QCOMPARE(part->name(), newName);
    QCOMPARE(part->description(), newDescription);
    QCOMPARE(part->data(), data); // Unchanged.
    QCOMPARE(part->mimeType(), newMimeType);
    QCOMPARE(int(part->encoding()), int(newEncoding));
    QCOMPARE(part->isInline(), !autoDisplay);
    QCOMPARE(part->isEncrypted(), !encrypt);
    QCOMPARE(part->isSigned(), !sign);
}

void AttachmentPropertiesDialogTest::testAttachmentPartReadOnly()
{
    // Sample data.
    const QString name = u"old name"_s;

    // Create the part.
    AttachmentPart::Ptr part = AttachmentPart::Ptr(new AttachmentPart);
    part->setName(name);

    // Show the (read-only) dialog and do some changes.
    auto dialog = new AttachmentPropertiesDialog(part, true, nullptr);
    dialog->show();
    // Click on 'OK'.  No changes should have been made.
    dialog->accept();
    delete dialog;
    QCOMPARE(part->name(), name); // No change.
}

void AttachmentPropertiesDialogTest::testAttachmentPartCancel()
{
    // Sample data.
    const QString name = u"old name"_s;
    const QString newName = u"new name"_s;

    // Create the part.
    AttachmentPart::Ptr part = AttachmentPart::Ptr(new AttachmentPart);
    part->setName(name);

    // Show the (read-write) dialog and do some changes.
    auto dialog = new AttachmentPropertiesDialog(part);
    dialog->show();
    auto nameEdit = dialog->findChild<QLineEdit *>(u"name"_s);
    Q_ASSERT(nameEdit);
    nameEdit->setText(newName);

    // Click on 'Cancel'.  No changes should have been made.
    dialog->reject();
    delete dialog;
    QCOMPARE(part->name(), name); // No change.
}

void AttachmentPropertiesDialogTest::testMimeContentReadOnly()
{
    // Sample data.
    const QString name = u"old name"_s;
    const QByteArray charset("us-ascii");

    // Create the MIME Content.
    auto content = new Content;
    content->contentType()->setName(name);
    content->contentType()->setRFC2047Charset(charset);
    const Content *constContent = content;

    // Show the (read-only) dialog and do some changes.
    auto dialog = new AttachmentPropertiesDialog(constContent);
    dialog->show();

    // Click on 'OK'.  The MIME Content should be untouched.
    dialog->accept();
    delete dialog;
    QCOMPARE(content->contentType()->name(), name); // No change.
    delete content;
}

#include "moc_attachmentpropertiesdialogtest.cpp"
