/*
  SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QDir>
#include <QObject>

namespace KIdentityManagement
{
class IdentityManager;
class IdentityCombo;
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
    KIdentityManagement::IdentityManager *mIdentMan = nullptr;
    KIdentityManagement::IdentityCombo *mIdentCombo = nullptr;
    QDir autocryptDir;
};
