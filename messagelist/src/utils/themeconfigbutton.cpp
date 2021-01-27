/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "utils/themeconfigbutton.h"

#include "core/manager.h"
#include "core/theme.h"
#include "utils/configurethemesdialog.h"
#include "utils/themecombobox.h"
#include "utils/themecombobox_p.h"

#include <KLocalizedString>

using namespace MessageList::Core;
using namespace MessageList::Utils;

class MessageList::Utils::ThemeConfigButtonPrivate
{
public:
    ThemeConfigButtonPrivate(ThemeConfigButton *owner)
        : q(owner)
    {
    }

    ThemeConfigButton *const q;

    const ThemeComboBox *mThemeComboBox = nullptr;

    void slotConfigureThemes();
};

void ThemeConfigButtonPrivate::slotConfigureThemes()
{
    QString currentThemeID;
    if (mThemeComboBox != nullptr) {
        currentThemeID = mThemeComboBox->currentTheme();
    }

    auto dialog = new ConfigureThemesDialog(q->window());
    dialog->selectTheme(currentThemeID);

    QObject::connect(dialog, &ConfigureThemesDialog::okClicked, q, &ThemeConfigButton::configureDialogCompleted);

    dialog->show();
}

ThemeConfigButton::ThemeConfigButton(QWidget *parent, const ThemeComboBox *themeComboBox)
    : QPushButton(i18n("Configure..."), parent)
    , d(new ThemeConfigButtonPrivate(this))
{
    d->mThemeComboBox = themeComboBox;
    connect(this, &ThemeConfigButton::pressed, this, [this]() {
        d->slotConfigureThemes();
    });

    // Keep theme combo up-to-date with any changes made in the configure dialog.
    if (d->mThemeComboBox != nullptr) {
        connect(this, &ThemeConfigButton::configureDialogCompleted, d->mThemeComboBox, &ThemeComboBox::slotLoadThemes);
    }
    setEnabled(Manager::instance());
}

ThemeConfigButton::~ThemeConfigButton()
{
    delete d;
}

#include "moc_themeconfigbutton.cpp"
