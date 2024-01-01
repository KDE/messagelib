/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanageruleswidget.h"
#include "dkimmanagerulescombobox.h"
#include "dkimruledialog.h"
#include "messageviewer_dkimcheckerdebug.h"
#include <KLineEditEventHandler>
#include <KLocalizedString>
#include <KMessageBox>
#include <KTreeWidgetSearchLine>
#include <MessageViewer/DKIMManagerRules>
#include <QHeaderView>
#include <QMenu>
#include <QPointer>
#include <QTreeWidget>
#include <QVBoxLayout>
using namespace MessageViewer;
DKIMManageRulesWidgetItem::DKIMManageRulesWidgetItem(QTreeWidget *parent)
    : QTreeWidgetItem(parent)
    , mRuleTypeCombobox(new DKIMManageRulesComboBox)
{
    treeWidget()->setItemWidget(this, ColumnType::RuleType, mRuleTypeCombobox);
}

DKIMManageRulesWidgetItem::~DKIMManageRulesWidgetItem() = default;

MessageViewer::DKIMRule DKIMManageRulesWidgetItem::rule() const
{
    MessageViewer::DKIMRule rule;
    rule.setEnabled(checkState(ColumnType::Enabled) == Qt::Checked);
    rule.setDomain(text(ColumnType::Domain));
    rule.setFrom(text(ColumnType::From));
    rule.setListId(text(ColumnType::ListId));
    rule.setPriority(text(ColumnType::Priority).toInt());
    rule.setRuleType(mRuleTypeCombobox->ruleType());
    rule.setSignedDomainIdentifier(text(ColumnType::SDid).split(QLatin1Char(' ')));
    return rule;
}

void DKIMManageRulesWidgetItem::setRule(const MessageViewer::DKIMRule &rule)
{
    if (mRule != rule) {
        mRule = rule;
        updateInfo();
    }
}

void DKIMManageRulesWidgetItem::updateInfo()
{
    setCheckState(ColumnType::Enabled, mRule.enabled() ? Qt::Checked : Qt::Unchecked);
    setText(ColumnType::Domain, mRule.domain());
    setText(ColumnType::ListId, mRule.listId());
    setText(ColumnType::From, mRule.from());
    setText(ColumnType::SDid, mRule.signedDomainIdentifier().join(QLatin1Char(' ')));
    setText(ColumnType::Priority, QString::number(mRule.priority()));
    mRuleTypeCombobox->setRuleType(mRule.ruleType());
}

DKIMManageRulesWidget::DKIMManageRulesWidget(QWidget *parent)
    : QWidget(parent)
    , mTreeWidget(new QTreeWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));
    mainLayout->setContentsMargins({});

    mTreeWidget->setObjectName(QLatin1StringView("treewidget"));
    mTreeWidget->setRootIsDecorated(false);
    mTreeWidget->setHeaderLabels({i18n("Active"), i18n("Domain"), i18n("List-ID"), i18n("From"), i18n("SDID"), i18n("Rule type"), i18n("Priority")});
    mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mTreeWidget->setAlternatingRowColors(true);

    auto searchLineEdit = new KTreeWidgetSearchLine(this, mTreeWidget);
    searchLineEdit->setObjectName(QLatin1StringView("searchlineedit"));
    searchLineEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(searchLineEdit);
    KLineEditEventHandler::catchReturnKey(searchLineEdit);

    mainLayout->addWidget(mTreeWidget);
    connect(mTreeWidget, &QTreeWidget::customContextMenuRequested, this, &DKIMManageRulesWidget::slotCustomContextMenuRequested);
    connect(mTreeWidget, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item) {
        if (item) {
            auto rulesItem = static_cast<DKIMManageRulesWidgetItem *>(item);
            modifyRule(rulesItem);
        }
    });
}

DKIMManageRulesWidget::~DKIMManageRulesWidget() = default;

void DKIMManageRulesWidget::updateRules()
{
    const QList<MessageViewer::DKIMRule> rules = MessageViewer::DKIMManagerRules::self()->rules();

    for (const MessageViewer::DKIMRule &rule : rules) {
        auto item = new DKIMManageRulesWidgetItem(mTreeWidget);
        item->setRule(rule);
    }
    emitUpdateExportButton();
}

void DKIMManageRulesWidget::loadSettings()
{
    mTreeWidget->setSortingEnabled(true);
    mTreeWidget->header()->setSortIndicatorShown(true);
    mTreeWidget->header()->setSectionsClickable(true);
    mTreeWidget->sortByColumn(0, Qt::AscendingOrder);
    updateRules();
}

QList<MessageViewer::DKIMRule> DKIMManageRulesWidget::rules() const
{
    QList<MessageViewer::DKIMRule> rules;
    const int total = mTreeWidget->topLevelItemCount();
    rules.reserve(total);
    for (int i = 0; i < total; ++i) {
        QTreeWidgetItem *item = mTreeWidget->topLevelItem(i);
        auto ruleItem = static_cast<DKIMManageRulesWidgetItem *>(item);
        rules.append(ruleItem->rule());
    }
    return rules;
}

void DKIMManageRulesWidget::saveSettings()
{
    MessageViewer::DKIMManagerRules::self()->saveRules(rules());
}

QByteArray DKIMManageRulesWidget::saveHeaders() const
{
    return mTreeWidget->header()->saveState();
}

void DKIMManageRulesWidget::restoreHeaders(const QByteArray &header)
{
    mTreeWidget->header()->restoreState(header);
}

void DKIMManageRulesWidget::addRule()
{
    QPointer<DKIMRuleDialog> dlg = new DKIMRuleDialog(this);
    if (dlg->exec()) {
        const MessageViewer::DKIMRule rule = dlg->rule();
        if (rule.isValid()) {
            auto item = new DKIMManageRulesWidgetItem(mTreeWidget);
            item->setRule(rule);
            emitUpdateExportButton();
        } else {
            qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Rule is not valid";
        }
    }
    delete dlg;
}

void DKIMManageRulesWidget::duplicateRule(DKIMManageRulesWidgetItem *rulesItem)
{
    QPointer<DKIMRuleDialog> dlg = new DKIMRuleDialog(this);
    dlg->loadRule(rulesItem->rule());
    if (dlg->exec()) {
        const MessageViewer::DKIMRule rule = dlg->rule();
        if (rule.isValid()) {
            auto item = new DKIMManageRulesWidgetItem(mTreeWidget);
            item->setRule(rule);
            emitUpdateExportButton();
        }
    }
    delete dlg;
}

void DKIMManageRulesWidget::emitUpdateExportButton()
{
    Q_EMIT updateExportButton(mTreeWidget->topLevelItemCount() > 0);
}

void DKIMManageRulesWidget::modifyRule(DKIMManageRulesWidgetItem *rulesItem)
{
    QPointer<DKIMRuleDialog> dlg = new DKIMRuleDialog(this);
    dlg->loadRule(rulesItem->rule());
    if (dlg->exec()) {
        const MessageViewer::DKIMRule rule = dlg->rule();
        if (rule.isValid()) {
            rulesItem->setRule(rule);
        }
    }
    delete dlg;
}

void DKIMManageRulesWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = mTreeWidget->itemAt(pos);
    QMenu menu(this);
    menu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add..."), this, [this]() {
        addRule();
    });
    auto rulesItem = dynamic_cast<DKIMManageRulesWidgetItem *>(item);
    if (rulesItem) {
        menu.addAction(QIcon::fromTheme(QStringLiteral("document-edit")), i18n("Modify..."), this, [this, rulesItem]() {
            modifyRule(rulesItem);
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-duplicate")), i18n("Duplicate Rule"), this, [this, rulesItem]() {
            duplicateRule(rulesItem);
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove Rule"), this, [this, item]() {
            const int answer = KMessageBox::warningTwoActions(this,
                                                              i18n("Do you want to delete this rule?"),
                                                              i18nc("@title:window", "Delete Rule"),
                                                              KStandardGuiItem::del(),
                                                              KStandardGuiItem::cancel());
            if (answer == KMessageBox::ButtonCode::PrimaryAction) {
                delete item;
                emitUpdateExportButton();
            }
        });
    }
    if (mTreeWidget->topLevelItemCount() > 0) {
        menu.addSeparator();
        menu.addAction(i18n("Delete All"), this, [this]() {
            const int answer = KMessageBox::warningTwoActions(this,
                                                              i18n("Do you want to delete all the rules?"),
                                                              i18nc("@title:window", "Delete Rules"),
                                                              KStandardGuiItem::del(),
                                                              KStandardGuiItem::cancel());
            if (answer == KMessageBox::ButtonCode::PrimaryAction) {
                mTreeWidget->clear();
                emitUpdateExportButton();
            }
        });
    }
    menu.exec(QCursor::pos());
}

#include "moc_dkimmanageruleswidget.cpp"
