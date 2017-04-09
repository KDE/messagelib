/*
  Copyright (C) 2016 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include <QObject>
#include <QTest>

#include "recipient/recipientseditor.h"
#include "recipient/recipientline.h"

#include <KMime/Types>
#include <QClipboard>
#include <QSignalSpy>

class RecipientsLineTestFactory : public MessageComposer::RecipientLineFactory
{
    Q_OBJECT

public:
    explicit RecipientsLineTestFactory(QObject *parent = nullptr)
        : MessageComposer::RecipientLineFactory(parent)
    {
    }

    KPIM::MultiplyingLine *newLine(QWidget *parent) Q_DECL_OVERRIDE {
        auto line = qobject_cast<MessageComposer::RecipientLineNG *>(
            MessageComposer::RecipientLineFactory::newLine(parent));
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
    QApplication::setActiveWindow(&editor);
    QTest::qWaitForWindowActive(&editor);

    QCOMPARE(editor.recipients().size(), 0);

    auto lineEdit = editor.lines().first()->findChild<MessageComposer::RecipientLineEdit *>();
    lineEdit->setFocus();

    // Simulate typing email address
    QTest::keyClicks(lineEdit, QStringLiteral("\"Vratil, Daniel\" <dvratil@kde.org>"), Qt::NoModifier, 10);

    QCOMPARE(editor.recipients().size(), 1);
    QCOMPARE(editor.recipients().first()->email(), QStringLiteral("\"Vratil, Daniel\" <dvratil@kde.org>"));

    QTest::keyClick(lineEdit, Qt::Key_Comma, Qt::NoModifier, 0);

    lineEdit = editor.lines().at(1)->findChild<MessageComposer::RecipientLineEdit *>();
    QVERIFY(lineEdit->hasFocus());

    QTest::keyClicks(lineEdit, QStringLiteral("test@example.test"), Qt::NoModifier, 10);
    QCOMPARE(editor.recipients().size(), 2);
    QCOMPARE(editor.recipients().at(1)->email(), QStringLiteral("test@example.test"));
    QCOMPARE(editor.recipients().at(1)->type(), editor.recipients().at(0)->type());
}

void RecipientsEditorTest::test_splitStringInputToLines()
{
    MessageComposer::RecipientsEditor editor(new RecipientsLineTestFactory());

    QCOMPARE(editor.recipients().size(), 0);

    const auto mboxes = KMime::Types::Mailbox::listFromUnicodeString(QStringLiteral("test@example.com, \"Vrátil, Daniel\" <dvratil@kde.org>"));
    editor.setRecipientString(mboxes, MessageComposer::Recipient::To);

    QCOMPARE(editor.recipients().size(), 2);
    QCOMPARE(editor.recipients().at(0)->email(), QStringLiteral("test@example.com"));
    QCOMPARE(editor.recipients().at(1)->email(), QStringLiteral("\"Vrátil, Daniel\" <dvratil@kde.org>"));
}

void RecipientsEditorTest::test_splitPastedListToLines()
{
    MessageComposer::RecipientsEditor editor(new RecipientsLineTestFactory());

    QCOMPARE(editor.recipients().size(), 0);

    const auto clipboard = QApplication::clipboard();
    const QString oldText = clipboard->text();

    clipboard->setText(QStringLiteral("test@example.com, \"Vrátil, Daniel\" <dvratil@kde.org>"));

    auto lineEdit = editor.lines().at(0)->findChild<MessageComposer::RecipientLineEdit *>();
    // paste() is protected in KPIM::AddresseeLineEdit
    QMetaObject::invokeMethod(lineEdit, "paste");

    // This will still fail the test, but will allow us to reset the clipboard
    [&editor]() {
        QCOMPARE(editor.recipients().size(), 2);
        QCOMPARE(editor.recipients().at(0)->email(), QStringLiteral("test@example.com"));
        QCOMPARE(editor.recipients().at(1)->email(), QStringLiteral("\"Vrátil, Daniel\" <dvratil@kde.org>"));
    }();

    clipboard->setText(oldText);
}

QTEST_MAIN(RecipientsEditorTest)

#include "recipientseditortest.moc"
