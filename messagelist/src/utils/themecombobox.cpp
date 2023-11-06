/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "themecombobox.h"

#include "utils/themecombobox.h"
#include "utils/themecombobox_p.h"

#include "core/manager.h"
#include "core/theme.h"
#include "messagelistsettings.h"
#include "storagemodel.h"

using namespace MessageList::Core;
using namespace MessageList::Utils;

ThemeComboBox::ThemeComboBox(QWidget *parent)
    : QComboBox(parent)
    , d(new ThemeComboBoxPrivate(this))
{
    if (Manager::instance()) {
        d->slotLoadThemes();
    } else {
        setEnabled(false);
    }
}

ThemeComboBox::~ThemeComboBox() = default;

QString ThemeComboBox::currentTheme() const
{
    return itemData(currentIndex()).toString();
}

void ThemeComboBox::writeDefaultConfig() const
{
    KConfigGroup group(MessageListSettings::self()->config(), QStringLiteral("MessageListView::StorageModelThemes"));

    const QString themeID = currentTheme();
    group.writeEntry(QStringLiteral("DefaultSet"), themeID);
    if (Manager::instance()) {
        Manager::instance()->themesConfigurationCompleted();
    }
}

void ThemeComboBox::writeStorageModelConfig(MessageList::Core::StorageModel *storageModel, bool isPrivateSetting) const
{
    writeStorageModelConfig(storageModel->id(), isPrivateSetting);
}

void ThemeComboBox::writeStorageModelConfig(const QString &id, bool isPrivateSetting) const
{
    if (Manager::instance()) {
        QString themeID;
        if (isPrivateSetting) {
            themeID = currentTheme();
        } else { // explicitly use default theme id when using default theme.
            themeID = Manager::instance()->defaultTheme()->id();
        }
        Manager::instance()->saveThemeForStorageModel(id, themeID, isPrivateSetting);
        Manager::instance()->themesConfigurationCompleted();
    }
}

void ThemeComboBox::readStorageModelConfig(const Akonadi::Collection &col, bool &isPrivateSetting)
{
    if (Manager::instance()) {
        const Theme *theme = Manager::instance()->themeForStorageModel(col, &isPrivateSetting);
        d->setCurrentTheme(theme);
    }
}

void ThemeComboBox::readStorageModelConfig(MessageList::Core::StorageModel *storageModel, bool &isPrivateSetting)
{
    if (Manager::instance()) {
        const Theme *theme = Manager::instance()->themeForStorageModel(storageModel, &isPrivateSetting);
        d->setCurrentTheme(theme);
    }
}

void ThemeComboBox::selectDefault()
{
    if (Manager::instance()) {
        const Theme *defaultTheme = Manager::instance()->defaultTheme();
        d->setCurrentTheme(defaultTheme);
    }
}

void ThemeComboBox::slotLoadThemes()
{
    d->slotLoadThemes();
}

void ThemeComboBoxPrivate::slotLoadThemes()
{
    if (!Manager::instance()) {
        return;
    }
    q->clear();

    // Get all message list themes and sort them into alphabetical order.
    QList<Theme *> themes = Manager::instance()->themes().values();
    std::sort(themes.begin(), themes.end(), MessageList::Core::Theme::compareName);

    for (const Theme *theme : std::as_const(themes)) {
        q->addItem(theme->name(), QVariant(theme->id()));
    }
}

void ThemeComboBoxPrivate::setCurrentTheme(const Theme *theme)
{
    Q_ASSERT(theme != nullptr);

    const QString themeID = theme->id();
    const int themeIndex = q->findData(QVariant(themeID));
    q->setCurrentIndex(themeIndex);
}

#include "moc_themecombobox.cpp"
