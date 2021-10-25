/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

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
    void shouldHaveDefaultValue();
    void testGenerateCryptoMessages();
    void testAutoSaveMessage();
private:
    KIdentityManagement::IdentityManager *mIdentMan = nullptr;
    KIdentityManagement::IdentityCombo *mIdentCombo = nullptr;
};

