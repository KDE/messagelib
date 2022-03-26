/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#pragma once

#include "messagelist_export.h"
#include <Akonadi/Collection>
#include <QComboBox>
#include <memory>
namespace MessageList
{
namespace Core
{
class StorageModel;
class Theme;
} // namespace Core

namespace Utils
{
class ThemeComboBoxPrivate;

/**
 * A specialized QComboBox that lists all message list themes.
 */
class MESSAGELIST_EXPORT ThemeComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit ThemeComboBox(QWidget *parent);
    ~ThemeComboBox() override;

    QString currentTheme() const;

    void writeDefaultConfig() const;

    void writeStorageModelConfig(MessageList::Core::StorageModel *storageModel, bool isPrivateSetting) const;
    void writeStorageModelConfig(const QString &id, bool isPrivateSetting) const;

    void readStorageModelConfig(const Akonadi::Collection &col, bool &isPrivateSetting);
    void readStorageModelConfig(MessageList::Core::StorageModel *storageModel, bool &isPrivateSetting);

public Q_SLOTS:
    void slotLoadThemes();
    void selectDefault();

private:
    std::unique_ptr<ThemeComboBoxPrivate> const d;
};
} // namespace Utils
} // namespace MessageList
