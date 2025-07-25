/*
  SPDX-FileCopyrightText: 2016 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QObject>
using namespace Qt::Literals::StringLiterals;

#include <QTest>

#include "recipient/recipientline.h"
#include "recipient/recipientseditor.h"

#include <KMime/Types>
#include <QClipboard>

class RecipientsLineTestFactory : public MessageComposer::RecipientLineFactory
{
    Q_OBJECT

public:
    explicit RecipientsLineTestFactory(QObject *parent = nullptr)
        : MessageComposer::RecipientLineFactory(parent)
    {
    }

    KPIM::MultiplyingLine *newLine(QWidget *parent) override
    {
        auto line = qobject_cast<MessageComposer::RecipientLineNG *>(MessageComposer::RecipientLineFactory::newLine(parent));
        line->setEnableAkonadiSearch(false);
        line->setEnableIndexSearch(false);
        return line;
    }
};

class RecipientsEditorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_addLineOnCommaPress();
    void test_splitStringInputToLines();
    void test_splitPastedListToLines();
};

void RecipientsEditorTest::test_addLineOnCommaPress()
{
    MessageComposer::RecipientsEditor editor(new RecipientsLineTestFactory());
    editor.show();
    editor.activateWindow();
    QVERIFY(QTest::qWaitForWindowActive(&editor));

    QVERIFY(editor.recipients().isEmpty());

    auto lineEdit = editor.lines().constFirst()->findChild<MessageComposer::RecipientLineEdit *>();
    lineEdit->setFocus();

    // Simulate typing email address
    QTest::keyClicks(lineEdit, u"\"Vratil, Daniel\" <dvratil@kde.org>"_s, Qt::NoModifier, 10);

    QCOMPARE(editor.recipients().size(), 1);
    QCOMPARE(editor.recipients().constFirst()->email(), u"\"Vratil, Daniel\" <dvratil@kde.org>"_s);

    QTest::keyClick(lineEdit, Qt::Key_Comma, Qt::NoModifier, 0);

    lineEdit = editor.lines().at(1)->findChild<MessageComposer::RecipientLineEdit *>();
    QVERIFY(lineEdit->hasFocus());

    QTest::keyClicks(lineEdit, u"test@example.test"_s, Qt::NoModifier, 10);
    QCOMPARE(editor.recipients().size(), 2);
    QCOMPARE(editor.recipients().at(1)->email(), u"test@example.test"_s);
    QCOMPARE(editor.recipients().at(1)->type(), editor.recipients().at(0)->type());
}

void RecipientsEditorTest::test_splitStringInputToLines()
{
    MessageComposer::RecipientsEditor editor(new RecipientsLineTestFactory());

    QCOMPARE(editor.recipients().size(), 0);

    const auto mboxes = KMime::Types::Mailbox::listFromUnicodeString(u"test@example.com, \"Vrátil, Daniel\" <dvratil@kde.org>"_s);
    editor.setRecipientString(mboxes, MessageComposer::Recipient::To);

    QCOMPARE(editor.recipients().size(), 2);
    QCOMPARE(editor.recipients().at(0)->email(), u"test@example.com"_s);
    QCOMPARE(editor.recipients().at(1)->email(), u"\"Vrátil, Daniel\" <dvratil@kde.org>"_s);
}

void RecipientsEditorTest::test_splitPastedListToLines()
{
    MessageComposer::RecipientsEditor editor(new RecipientsLineTestFactory());

    QCOMPARE(editor.recipients().size(), 0);

    const auto clipboard = QApplication::clipboard();
    const QString oldText = clipboard->text();

    clipboard->setText(u"test@example.com, \"Vrátil, Daniel\" <dvratil@kde.org>"_s);

    auto lineEdit = editor.lines().at(0)->findChild<MessageComposer::RecipientLineEdit *>();
    // paste() is protected in KPIM::AddresseeLineEdit
    QMetaObject::invokeMethod(lineEdit, "paste");

    // This will still fail the test, but will allow us to reset the clipboard
    [&editor]() {
        QCOMPARE(editor.recipients().size(), 2);
        QCOMPARE(editor.recipients().at(0)->email(), u"test@example.com"_s);
        QCOMPARE(editor.recipients().at(1)->email(), u"\"Vrátil, Daniel\" <dvratil@kde.org>"_s);
    }();

    clipboard->setText(oldText);
}

QTEST_MAIN(RecipientsEditorTest)

#include "recipientseditortest.moc"
