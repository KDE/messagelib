/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "utils/configurethemesdialog.h"
#include "utils/configurethemesdialog_p.h"

#include "core/theme.h"
#include "utils/themeeditor.h"

#include "core/manager.h"

#include <QFrame>
#include <QGridLayout>
#include <QMap>
#include <QPushButton>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QIcon>
#include <QListWidget>
#include <QVBoxLayout>

namespace MessageList
{
namespace Utils
{
class ThemeListWidgetItem : public QListWidgetItem
{
public:
    ThemeListWidgetItem(QListWidget *par, const Core::Theme &set)
        : QListWidgetItem(set.name(), par)
    {
        mTheme = new Core::Theme(set);
    }

    ~ThemeListWidgetItem() override
    {
        delete mTheme;
    }

    [[nodiscard]] Core::Theme *theme() const
    {
        return mTheme;
    }

    void forgetTheme()
    {
        mTheme = nullptr;
    }

private:
    Core::Theme *mTheme = nullptr;
};

class ThemeListWidget : public QListWidget
{
public:
    ThemeListWidget(QWidget *parent)
        : QListWidget(parent)
    {
    }

public:
    // need a larger but shorter QListWidget
    [[nodiscard]] QSize sizeHint() const override
    {
        return {450, 128};
    }
};
} // namespace Utils
} // namespace MessageList

using namespace MessageList::Core;
using namespace MessageList::Utils;

ConfigureThemesDialog::ConfigureThemesDialog(QWidget *parent)
    : QDialog(parent)
    , d(new ConfigureThemesDialogPrivate(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    auto mainLayout = new QVBoxLayout(this);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ConfigureThemesDialog::reject);
    setWindowTitle(i18nc("@title:window", "Customize Themes"));

    auto base = new QWidget(this);
    mainLayout->addWidget(base);
    mainLayout->addWidget(buttonBox);

    auto g = new QGridLayout(base);
    g->setContentsMargins({});

    d->mThemeList = new ThemeListWidget(base);
    d->mThemeList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->mThemeList->setSortingEnabled(true);
    g->addWidget(d->mThemeList, 0, 0, 7, 1);

    connect(d->mThemeList, &ThemeListWidget::currentItemChanged, this, [this](QListWidgetItem *item) {
        d->themeListItemClicked(item);
    });
    connect(d->mThemeList, &ThemeListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        d->themeListItemClicked(item);
    });

    d->mNewThemeButton = new QPushButton(i18nc("@action:button", "New Theme"), base);
    d->mNewThemeButton->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));
    g->addWidget(d->mNewThemeButton, 0, 1);

    connect(d->mNewThemeButton, &QPushButton::clicked, this, [this]() {
        d->newThemeButtonClicked();
    });

    d->mCloneThemeButton = new QPushButton(i18nc("@action:button", "Clone Theme"), base);
    d->mCloneThemeButton->setIcon(QIcon::fromTheme(QStringLiteral("edit-copy")));
    g->addWidget(d->mCloneThemeButton, 1, 1);

    connect(d->mCloneThemeButton, &QPushButton::clicked, this, [this]() {
        d->cloneThemeButtonClicked();
    });

    auto f = new QFrame(base);
    f->setFrameStyle(QFrame::Sunken | QFrame::HLine);
    f->setMinimumHeight(24);
    g->addWidget(f, 2, 1, Qt::AlignVCenter);

    d->mExportThemeButton = new QPushButton(i18nc("@action:button", "Export Theme..."), base);
    g->addWidget(d->mExportThemeButton, 3, 1);

    connect(d->mExportThemeButton, &QPushButton::clicked, this, [this]() {
        d->exportThemeButtonClicked();
    });

    d->mImportThemeButton = new QPushButton(i18nc("@action:button", "Import Theme..."), base);
    g->addWidget(d->mImportThemeButton, 4, 1);
    connect(d->mImportThemeButton, &QPushButton::clicked, this, [this]() {
        d->importThemeButtonClicked();
    });

    f = new QFrame(base);
    f->setFrameStyle(QFrame::Sunken | QFrame::HLine);
    f->setMinimumHeight(24);
    g->addWidget(f, 5, 1, Qt::AlignVCenter);

    d->mDeleteThemeButton = new QPushButton(i18nc("@action:button", "Delete Theme"), base);
    d->mDeleteThemeButton->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));
    g->addWidget(d->mDeleteThemeButton, 6, 1);

    connect(d->mDeleteThemeButton, &QPushButton::clicked, this, [this]() {
        d->deleteThemeButtonClicked();
    });

    d->mEditor = new ThemeEditor(base);
    g->addWidget(d->mEditor, 8, 0, 1, 2);

    connect(d->mEditor, &ThemeEditor::themeNameChanged, this, [this]() {
        d->editedThemeNameChanged();
    });

    g->setColumnStretch(0, 1);
    g->setRowStretch(4, 1);

    connect(okButton, &QPushButton::clicked, this, [this]() {
        d->okButtonClicked();
    });

    d->fillThemeList();
}

ConfigureThemesDialog::~ConfigureThemesDialog() = default;

void ConfigureThemesDialog::selectTheme(const QString &themeId)
{
    ThemeListWidgetItem *item = d->findThemeItemById(themeId);
    if (item) {
        d->mThemeList->setCurrentItem(item);
        d->themeListItemClicked(item);
    }
}

void ConfigureThemesDialog::ConfigureThemesDialogPrivate::okButtonClicked()
{
    commitEditor();

    Manager::instance()->removeAllThemes();

    const int c = mThemeList->count();
    int i = 0;
    while (i < c) {
        auto item = dynamic_cast<ThemeListWidgetItem *>(mThemeList->item(i));
        if (item) {
            Manager::instance()->addTheme(item->theme());
            item->forgetTheme();
        }
        ++i;
    }

    Manager::instance()->themesConfigurationCompleted();
    Q_EMIT q->okClicked();
    q->accept(); // this will delete too
}

void ConfigureThemesDialog::ConfigureThemesDialogPrivate::commitEditor()
{
    Theme *editedTheme = mEditor->editedTheme();
    if (!editedTheme) {
        return;
    }

    mEditor->commit();

    ThemeListWidgetItem *editedItem = findThemeItemByTheme(editedTheme);
    if (!editedItem) {
        return;
    }

    // We must reset the runtime column state as the columns might have
    // totally changed in the editor
    editedTheme->resetColumnState();

    QString goodName = uniqueNameForTheme(editedTheme->name(), editedTheme);
    editedTheme->setName(goodName);
    editedItem->setText(goodName);
}

void ConfigureThemesDialog::ConfigureThemesDialogPrivate::editedThemeNameChanged()
{
    Theme *set = mEditor->editedTheme();
    if (!set) {
        return;
    }

    ThemeListWidgetItem *it = findThemeItemByTheme(set);
    if (!it) {
        return;
    }

    QString goodName = uniqueNameForTheme(set->name(), set);

    it->setText(goodName);
}

void ConfigureThemesDialog::ConfigureThemesDialogPrivate::fillThemeList()
{
    const QMap<QString, Theme *> &sets = Manager::instance()->themes();

    QMap<QString, Theme *>::ConstIterator end(sets.constEnd());
    for (QMap<QString, Theme *>::ConstIterator it = sets.constBegin(); it != end; ++it) {
        (void)new ThemeListWidgetItem(mThemeList, *(*it));
    }
}

void ConfigureThemesDialog::ConfigureThemesDialogPrivate::themeListItemClicked(QListWidgetItem *cur)
{
    commitEditor();

    const int numberOfSelectedItem(mThemeList->selectedItems().count());

    ThemeListWidgetItem *item = cur ? dynamic_cast<ThemeListWidgetItem *>(cur) : nullptr;
    mDeleteThemeButton->setEnabled(item && !item->theme()->readOnly());
    mCloneThemeButton->setEnabled(numberOfSelectedItem == 1);
    mEditor->editTheme(item ? item->theme() : nullptr);
    mExportThemeButton->setEnabled(numberOfSelectedItem > 0);

    if (item && !item->isSelected()) {
        item->setSelected(true); // make sure it's true
    }
}

ThemeListWidgetItem *ConfigureThemesDialog::ConfigureThemesDialogPrivate::findThemeItemById(const QString &themeId)
{
    const int c = mThemeList->count();
    int i = 0;
    while (i < c) {
        auto item = dynamic_cast<ThemeListWidgetItem *>(mThemeList->item(i));
        if (item) {
            if (item->theme()->id() == themeId) {
                return item;
            }
        }
        ++i;
    }
    return nullptr;
}

ThemeListWidgetItem *ConfigureThemesDialog::ConfigureThemesDialogPrivate::findThemeItemByName(const QString &name, Theme *skipTheme)
{
    const int c = mThemeList->count();
    int i = 0;
    while (i < c) {
        auto item = dynamic_cast<ThemeListWidgetItem *>(mThemeList->item(i));
        if (item) {
            if (item->theme() != skipTheme) {
                if (item->theme()->name() == name) {
                    return item;
                }
            }
        }
        ++i;
    }
    return nullptr;
}

ThemeListWidgetItem *ConfigureThemesDialog::ConfigureThemesDialogPrivate::findThemeItemByTheme(Theme *set)
{
    const int c = mThemeList->count();
    int i = 0;
    while (i < c) {
        auto item = dynamic_cast<ThemeListWidgetItem *>(mThemeList->item(i));
        if (item) {
            if (item->theme() == set) {
                return item;
            }
        }
        ++i;
    }
    return nullptr;
}

QString ConfigureThemesDialog::ConfigureThemesDialogPrivate::uniqueNameForTheme(const QString &baseName, Theme *skipTheme)
{
    QString ret = baseName;
    if (ret.isEmpty()) {
        ret = i18n("Unnamed Theme");
    }

    int idx = 1;

    ThemeListWidgetItem *item = findThemeItemByName(ret, skipTheme);
    while (item) {
        idx++;
        ret = QStringLiteral("%1 %2").arg(baseName, QString::number(idx));
        item = findThemeItemByName(ret, skipTheme);
    }
    return ret;
}

void ConfigureThemesDialog::ConfigureThemesDialogPrivate::newThemeButtonClicked()
{
    const int numberOfSelectedItem(mThemeList->selectedItems().count());
    Theme emptyTheme;
    emptyTheme.setName(uniqueNameForTheme(i18n("New Theme")));
    auto col = new Theme::Column();
    col->setLabel(i18n("New Column"));
    col->setVisibleByDefault(true);
    col->addMessageRow(new Theme::Row());
    col->addGroupHeaderRow(new Theme::Row());
    emptyTheme.addColumn(col);
    auto item = new ThemeListWidgetItem(mThemeList, emptyTheme);

    mThemeList->clearSelection();
    mThemeList->setCurrentItem(item);
    Core::Theme *theme = item->theme();
    if (theme) {
        mEditor->editTheme(theme);

        mDeleteThemeButton->setEnabled(!theme->readOnly());
        mExportThemeButton->setEnabled(item);
        mCloneThemeButton->setEnabled(numberOfSelectedItem == 1);
    } else {
        mDeleteThemeButton->setEnabled(false);
        mExportThemeButton->setEnabled(false);
        mCloneThemeButton->setEnabled(false);
    }
}

void ConfigureThemesDialog::ConfigureThemesDialogPrivate::cloneThemeButtonClicked()
{
    auto item = dynamic_cast<ThemeListWidgetItem *>(mThemeList->currentItem());
    if (!item) {
        return;
    }
    commitEditor();
    item->setSelected(false);
    Theme copyTheme(*(item->theme()));
    copyTheme.setReadOnly(false);
    copyTheme.detach(); // detach shared data
    copyTheme.generateUniqueId(); // regenerate id so it becomes different
    copyTheme.setName(uniqueNameForTheme(item->theme()->name()));
    item = new ThemeListWidgetItem(mThemeList, copyTheme);

    mThemeList->setCurrentItem(item);
    mEditor->editTheme(item->theme());

    const int numberOfSelectedItem(mThemeList->selectedItems().count());
    mDeleteThemeButton->setEnabled(!item->theme()->readOnly());
    mExportThemeButton->setEnabled(true);
    mCloneThemeButton->setEnabled(numberOfSelectedItem == 1);
}

void ConfigureThemesDialog::ConfigureThemesDialogPrivate::deleteThemeButtonClicked()
{
    const QList<QListWidgetItem *> list = mThemeList->selectedItems();
    if (list.isEmpty()) {
        return;
    }
    const QString question = list.count() > 1 ? i18n("Do you want to delete selected themes?") : i18n("Do you want to delete \"%1\"?", list.first()->text());
    const int answer =
        KMessageBox::questionTwoActions(q, question, i18nc("@title:window", "Delete Theme"), KStandardGuiItem::del(), KStandardGuiItem::cancel());
    if (answer == KMessageBox::ButtonCode::PrimaryAction) {
        mEditor->editTheme(nullptr); // forget it
        for (QListWidgetItem *it : list) {
            auto item = dynamic_cast<ThemeListWidgetItem *>(it);
            if (!item) {
                return;
            }
            if (!item->theme()->readOnly()) {
                delete item; // this will trigger themeListCurrentItemChanged()
            }
            if (mThemeList->count() < 2) {
                break; // no way: desperately try to keep at least one option set alive :)
            }
        }

        auto newItem = dynamic_cast<ThemeListWidgetItem *>(mThemeList->currentItem());
        mDeleteThemeButton->setEnabled(newItem && !newItem->theme()->readOnly());
        mExportThemeButton->setEnabled(newItem);
        const int numberOfSelectedItem(mThemeList->selectedItems().count());
        mCloneThemeButton->setEnabled(numberOfSelectedItem == 1);
    }
}

void ConfigureThemesDialog::ConfigureThemesDialogPrivate::importThemeButtonClicked()
{
    const QString filename = QFileDialog::getOpenFileName(q, i18n("Import Theme"));
    if (!filename.isEmpty()) {
        KConfig config(filename);

        if (config.hasGroup(QStringLiteral("MessageListView::Themes"))) {
            KConfigGroup grp(&config, QStringLiteral("MessageListView::Themes"));
            const int cnt = grp.readEntry("Count", 0);
            int idx = 0;
            while (idx < cnt) {
                const QString data = grp.readEntry(QStringLiteral("Set%1").arg(idx), QString());
                if (!data.isEmpty()) {
                    auto set = new Theme();
                    if (set->loadFromString(data)) {
                        set->setReadOnly(false);
                        set->detach(); // detach shared data
                        set->generateUniqueId(); // regenerate id so it becomes different
                        set->setName(uniqueNameForTheme(set->name()));
                        (void)new ThemeListWidgetItem(mThemeList, *set);
                    } else {
                        delete set;
                    }
                }
                ++idx;
            }
        }
    }
}

void ConfigureThemesDialog::ConfigureThemesDialogPrivate::exportThemeButtonClicked()
{
    const QList<QListWidgetItem *> list = mThemeList->selectedItems();
    if (list.isEmpty()) {
        return;
    }
    const QString filename = QFileDialog::getSaveFileName(q, i18n("Export Theme"), QString(), i18n("All Files (*)"));
    if (!filename.isEmpty()) {
        KConfig config(filename);

        KConfigGroup grp(&config, QStringLiteral("MessageListView::Themes"));
        grp.writeEntry("Count", list.count());

        int idx = 0;
        for (QListWidgetItem *item : list) {
            auto themeItem = static_cast<ThemeListWidgetItem *>(item);
            grp.writeEntry(QStringLiteral("Set%1").arg(idx), themeItem->theme()->saveToString());
            ++idx;
        }
    }
}

#include "moc_configurethemesdialog.cpp"
