/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QDir>
#include <QObject>

namespace KIdentityManagementWidgets
{
class IdentityCombo;
}
namespace KIdentityManagementCore
{
class IdentityManager;
}

class ComposerViewBaseTest : public QObject
{
    Q_OBJECT
public:
    explicit ComposerViewBaseTest(QObject *parent = nullptr);
    ~ComposerViewBaseTest() override;
private Q_SLOTS:
    void cleanupTestCase();
    void initTestCase();
    void init();
    void cleanup();
    void shouldHaveDefaultValue();
    void testGenerateCryptoMessagesAutocrypt_data();
    void testGenerateCryptoMessagesAutocrypt();
    void testGenerateCryptoMessagesAutocryptSMime();
    void testAutoSaveMessage();
    void testAutocryptKey();

private:
    KIdentityManagementCore::IdentityManager *mIdentMan = nullptr;
    KIdentityManagementWidgets::IdentityCombo *mIdentCombo = nullptr;
    QDir autocryptDir;
};
