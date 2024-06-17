/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "utils/configureaggregationsdialog.h"
#include "utils/configureaggregationsdialog_p.h"

#include "core/aggregation.h"
#include "utils/aggregationeditor.h"

#include "core/manager.h"

#include <QFrame>
#include <QGridLayout>
#include <QMap>
#include <QPushButton>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QIcon>
#include <QVBoxLayout>

namespace MessageList
{
namespace Utils
{
class AggregationListWidgetItem : public QListWidgetItem
{
private:
    Core::Aggregation *mAggregation = nullptr;

public:
    AggregationListWidgetItem(QListWidget *par, const Core::Aggregation &set)
        : QListWidgetItem(set.name(), par)
    {
        mAggregation = new Core::Aggregation(set);
    }

    ~AggregationListWidgetItem() override
    {
        delete mAggregation;
    }

public:
    [[nodiscard]] Core::Aggregation *aggregation() const
    {
        return mAggregation;
    }

    void forgetAggregation()
    {
        mAggregation = nullptr;
    }
};

/**
 * The widget that lists the available Aggregations.
 *
 * At the moment of writing, derived from QListWidget only to override sizeHint().
 */
class AggregationListWidget : public QListWidget
{
public:
    AggregationListWidget(QWidget *parent)
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

ConfigureAggregationsDialog::ConfigureAggregationsDialog(QWidget *parent)
    : QDialog(parent)
    , d(new ConfigureAggregationsDialogPrivate(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    auto mainLayout = new QVBoxLayout(this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ConfigureAggregationsDialog::reject);
    setWindowTitle(i18nc("@title:window", "Customize Message Aggregation Modes"));

    auto base = new QWidget(this);
    mainLayout->addWidget(base);
    mainLayout->addWidget(buttonBox);

    auto g = new QGridLayout(base);
    g->setContentsMargins({});

    d->mAggregationList = new AggregationListWidget(base);
    d->mAggregationList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->mAggregationList->setSortingEnabled(true);
    g->addWidget(d->mAggregationList, 0, 0, 7, 1);

    connect(d->mAggregationList, &AggregationListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        d->aggregationListItemClicked(item);
    });
    connect(d->mAggregationList, &AggregationListWidget::currentItemChanged, this, [this](QListWidgetItem *item) {
        d->aggregationListItemClicked(item);
    });

    d->mNewAggregationButton = new QPushButton(i18nc("@action:button", "New Aggregation"), base);
    d->mNewAggregationButton->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));
    g->addWidget(d->mNewAggregationButton, 0, 1);

    connect(d->mNewAggregationButton, &QPushButton::clicked, this, [this]() {
        d->newAggregationButtonClicked();
    });

    d->mCloneAggregationButton = new QPushButton(i18nc("@action:button", "Clone Aggregation"), base);
    d->mCloneAggregationButton->setIcon(QIcon::fromTheme(QStringLiteral("edit-copy")));
    g->addWidget(d->mCloneAggregationButton, 1, 1);

    connect(d->mCloneAggregationButton, &QPushButton::clicked, this, [this]() {
        d->cloneAggregationButtonClicked();
    });

    auto f = new QFrame(base);
    f->setFrameStyle(QFrame::Sunken | QFrame::HLine);
    f->setMinimumHeight(24);
    g->addWidget(f, 2, 1, Qt::AlignVCenter);

    d->mExportAggregationButton = new QPushButton(i18nc("@action:button", "Export Aggregation..."), base);
    g->addWidget(d->mExportAggregationButton, 3, 1);

    connect(d->mExportAggregationButton, &QPushButton::clicked, this, [this]() {
        d->exportAggregationButtonClicked();
    });

    d->mImportAggregationButton = new QPushButton(i18nc("@action:button", "Import Aggregation..."), base);
    g->addWidget(d->mImportAggregationButton, 4, 1);
    connect(d->mImportAggregationButton, &QPushButton::clicked, this, [this]() {
        d->importAggregationButtonClicked();
    });

    f = new QFrame(base);
    f->setFrameStyle(QFrame::Sunken | QFrame::HLine);
    f->setMinimumHeight(24);
    g->addWidget(f, 5, 1, Qt::AlignVCenter);

    d->mDeleteAggregationButton = new QPushButton(i18nc("@action:button", "Delete Aggregation"), base);
    d->mDeleteAggregationButton->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));
    g->addWidget(d->mDeleteAggregationButton, 6, 1);

    connect(d->mDeleteAggregationButton, &QPushButton::clicked, this, [this]() {
        d->deleteAggregationButtonClicked();
    });

    d->mEditor = new AggregationEditor(base);
    g->addWidget(d->mEditor, 8, 0, 1, 2);

    connect(d->mEditor, &AggregationEditor::aggregationNameChanged, this, [this]() {
        d->editedAggregationNameChanged();
    });

    g->setColumnStretch(0, 1);
    g->setRowStretch(7, 1);

    connect(okButton, &QPushButton::clicked, this, [this]() {
        d->okButtonClicked();
    });

    d->fillAggregationList();
}

ConfigureAggregationsDialog::~ConfigureAggregationsDialog() = default;

void ConfigureAggregationsDialog::selectAggregation(const QString &aggregationId)
{
    AggregationListWidgetItem *item = d->findAggregationItemById(aggregationId);
    if (item) {
        d->mAggregationList->setCurrentItem(item);
        d->aggregationListItemClicked(item);
    }
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::okButtonClicked()
{
    if (Manager::instance()) {
        commitEditor();

        Manager::instance()->removeAllAggregations();

        const int c = mAggregationList->count();
        int i = 0;
        while (i < c) {
            auto item = dynamic_cast<AggregationListWidgetItem *>(mAggregationList->item(i));
            if (item) {
                Manager::instance()->addAggregation(item->aggregation());
                item->forgetAggregation();
            }
            ++i;
        }

        Manager::instance()->aggregationsConfigurationCompleted();
    }
    Q_EMIT q->okClicked();
    q->accept(); // this will delete too
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::commitEditor()
{
    Aggregation *editedAggregation = mEditor->editedAggregation();
    if (!editedAggregation) {
        return;
    }

    mEditor->commit();

    AggregationListWidgetItem *editedItem = findAggregationItemByAggregation(editedAggregation);
    if (!editedItem) {
        return;
    }
    const QString goodName = uniqueNameForAggregation(editedAggregation->name(), editedAggregation);
    editedAggregation->setName(goodName);
    editedItem->setText(goodName);
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::editedAggregationNameChanged()
{
    Aggregation *set = mEditor->editedAggregation();
    if (!set) {
        return;
    }

    AggregationListWidgetItem *it = findAggregationItemByAggregation(set);
    if (!it) {
        return;
    }

    const QString goodName = uniqueNameForAggregation(set->name(), set);

    it->setText(goodName);
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::fillAggregationList()
{
    if (!Manager::instance()) {
        return;
    }
    const QMap<QString, Aggregation *> &sets = Manager::instance()->aggregations();
    QMap<QString, Aggregation *>::ConstIterator end(sets.constEnd());
    for (QMap<QString, Aggregation *>::ConstIterator it = sets.constBegin(); it != end; ++it) {
        (void)new AggregationListWidgetItem(mAggregationList, *(*it));
    }
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::aggregationListItemClicked(QListWidgetItem *cur)
{
    commitEditor();
    updateButton(cur);
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::updateButton(QListWidgetItem *cur)
{
    const int numberOfSelectedItem(mAggregationList->selectedItems().count());

    AggregationListWidgetItem *item = cur ? dynamic_cast<AggregationListWidgetItem *>(cur) : nullptr;
    mDeleteAggregationButton->setEnabled(item && !item->aggregation()->readOnly() && (mAggregationList->count() > 1));

    mCloneAggregationButton->setEnabled(numberOfSelectedItem == 1);
    mExportAggregationButton->setEnabled(numberOfSelectedItem > 0);
    mEditor->editAggregation(item ? item->aggregation() : nullptr);
    if (item && !item->isSelected()) {
        item->setSelected(true); // make sure it's true
    }
}

AggregationListWidgetItem *ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::findAggregationItemByName(const QString &name,
                                                                                                                      Aggregation *skipAggregation)
{
    const int c = mAggregationList->count();
    int i = 0;
    while (i < c) {
        auto item = dynamic_cast<AggregationListWidgetItem *>(mAggregationList->item(i));
        if (item) {
            if (item->aggregation() != skipAggregation) {
                if (item->aggregation()->name() == name) {
                    return item;
                }
            }
        }
        ++i;
    }
    return nullptr;
}

AggregationListWidgetItem *ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::findAggregationItemById(const QString &aggregationId)
{
    const int c = mAggregationList->count();
    int i = 0;
    while (i < c) {
        auto item = dynamic_cast<AggregationListWidgetItem *>(mAggregationList->item(i));
        if (item) {
            if (item->aggregation()->id() == aggregationId) {
                return item;
            }
        }
        ++i;
    }
    return nullptr;
}

AggregationListWidgetItem *ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::findAggregationItemByAggregation(Aggregation *set)
{
    const int c = mAggregationList->count();
    int i = 0;
    while (i < c) {
        auto item = dynamic_cast<AggregationListWidgetItem *>(mAggregationList->item(i));
        if (item) {
            if (item->aggregation() == set) {
                return item;
            }
        }
        ++i;
    }
    return nullptr;
}

QString ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::uniqueNameForAggregation(const QString &baseName, Aggregation *skipAggregation)
{
    QString ret = baseName;
    if (ret.isEmpty()) {
        ret = i18n("Unnamed Aggregation");
    }

    int idx = 1;

    AggregationListWidgetItem *item = findAggregationItemByName(ret, skipAggregation);
    while (item) {
        idx++;
        ret = QStringLiteral("%1 %2").arg(baseName).arg(idx);
        item = findAggregationItemByName(ret, skipAggregation);
    }
    return ret;
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::newAggregationButtonClicked()
{
    Aggregation emptyAggregation;
    emptyAggregation.setName(uniqueNameForAggregation(i18n("New Aggregation")));
    auto item = new AggregationListWidgetItem(mAggregationList, emptyAggregation);

    mAggregationList->clearSelection();
    mAggregationList->setCurrentItem(item);
    mDeleteAggregationButton->setEnabled(item && !item->aggregation()->readOnly());
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::cloneAggregationButtonClicked()
{
    auto item = dynamic_cast<AggregationListWidgetItem *>(mAggregationList->currentItem());
    if (!item) {
        return;
    }
    commitEditor();
    item->setSelected(false);
    Aggregation copyAggregation(*(item->aggregation()));
    copyAggregation.setReadOnly(false);
    copyAggregation.generateUniqueId(); // regenerate id so it becomes different
    copyAggregation.setName(uniqueNameForAggregation(item->aggregation()->name()));
    item = new AggregationListWidgetItem(mAggregationList, copyAggregation);

    mAggregationList->setCurrentItem(item);
    aggregationListItemClicked(item);
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::deleteAggregationButtonClicked()
{
    const QList<QListWidgetItem *> list = mAggregationList->selectedItems();
    if (list.isEmpty()) {
        return;
    }

    mEditor->editAggregation(nullptr); // forget it
    for (QListWidgetItem *it : list) {
        auto item = dynamic_cast<AggregationListWidgetItem *>(it);
        if (!item) {
            return;
        }
        if (!item->aggregation()->readOnly()) {
            delete item; // this will trigger aggregationListCurrentItemChanged()
        }
        if (mAggregationList->count() < 2) {
            break; // no way: desperately try to keep at least one option set alive :)
        }
    }

    auto newItem = dynamic_cast<AggregationListWidgetItem *>(mAggregationList->currentItem());
    updateButton(newItem);
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::importAggregationButtonClicked()
{
    const QString filename = QFileDialog::getOpenFileName(q, i18n("Import Aggregation"));
    if (!filename.isEmpty()) {
        KConfig config(filename);

        if (config.hasGroup(QStringLiteral("MessageListView::Aggregations"))) {
            KConfigGroup grp(&config, QStringLiteral("MessageListView::Aggregations"));
            const int cnt = grp.readEntry("Count", 0);
            int idx = 0;
            while (idx < cnt) {
                const QString data = grp.readEntry(QStringLiteral("Set%1").arg(idx), QString());
                if (!data.isEmpty()) {
                    auto set = new Aggregation();
                    if (set->loadFromString(data)) {
                        set->setReadOnly(false);
                        set->generateUniqueId(); // regenerate id so it becomes different
                        set->setName(uniqueNameForAggregation(set->name()));
                        (void)new AggregationListWidgetItem(mAggregationList, *set);
                    } else {
                        delete set; // b0rken
                    }
                }
                ++idx;
            }
        }
    }
}

void ConfigureAggregationsDialog::ConfigureAggregationsDialogPrivate::exportAggregationButtonClicked()
{
    const QList<QListWidgetItem *> list = mAggregationList->selectedItems();
    if (list.isEmpty()) {
        return;
    }
    const QString filename = QFileDialog::getSaveFileName(q, i18n("Export Aggregation"), QString(), i18n("All Files (*)"));
    if (!filename.isEmpty()) {
        KConfig config(filename);

        KConfigGroup grp(&config, QStringLiteral("MessageListView::Aggregations"));
        grp.writeEntry("Count", list.count());

        int idx = 0;
        for (QListWidgetItem *item : list) {
            auto themeItem = static_cast<AggregationListWidgetItem *>(item);
            grp.writeEntry(QStringLiteral("Set%1").arg(idx), themeItem->aggregation()->saveToString());
            ++idx;
        }
    }
}

#include "moc_configureaggregationsdialog.cpp"
