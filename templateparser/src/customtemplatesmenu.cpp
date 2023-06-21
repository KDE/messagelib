/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@ubiz.ru>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "customtemplatesmenu.h"
#include "customtemplates.h"
#include "customtemplates_kfg.h"
#include "globalsettings_templateparser.h"

#include <KActionCollection>
#include <KActionMenu>
#include <KLocalizedString>
#include <QIcon>
#include <QList>
#include <QMenu>

using namespace TemplateParser;
class TemplateParser::CustomTemplatesMenuPrivate
{
public:
    CustomTemplatesMenuPrivate() = default;

    ~CustomTemplatesMenuPrivate()
    {
        delete mCustomReplyActionMenu;
        delete mCustomReplyAllActionMenu;
        delete mCustomForwardActionMenu;
    }

    KActionCollection *mOwnerActionCollection = nullptr;

    QStringList mCustomTemplates;
    QList<QAction *> mCustomTemplateActions;

    // Custom template actions menu
    KActionMenu *mCustomReplyActionMenu = nullptr;
    KActionMenu *mCustomReplyAllActionMenu = nullptr;
    KActionMenu *mCustomForwardActionMenu = nullptr;
};

CustomTemplatesMenu::CustomTemplatesMenu(QWidget *owner, KActionCollection *ac)
    : d(new TemplateParser::CustomTemplatesMenuPrivate)
{
    d->mOwnerActionCollection = ac;

    d->mCustomForwardActionMenu = new KActionMenu(QIcon::fromTheme(QStringLiteral("mail-forward-custom")), i18n("With Custom Template"), owner);
    d->mOwnerActionCollection->addAction(QStringLiteral("custom_forward"), d->mCustomForwardActionMenu);

    d->mCustomReplyActionMenu = new KActionMenu(QIcon::fromTheme(QStringLiteral("mail-reply-custom")), i18n("Reply With Custom Template"), owner);
    d->mOwnerActionCollection->addAction(QStringLiteral("custom_reply"), d->mCustomReplyActionMenu);

    d->mCustomReplyAllActionMenu = new KActionMenu(QIcon::fromTheme(QStringLiteral("mail-reply-all-custom")), i18n("Reply to All With Custom Template"), owner);
    d->mOwnerActionCollection->addAction(QStringLiteral("custom_reply_all"), d->mCustomReplyAllActionMenu);

    update();
}

CustomTemplatesMenu::~CustomTemplatesMenu()
{
    clear();
}

KActionMenu *CustomTemplatesMenu::replyActionMenu() const
{
    return d->mCustomReplyActionMenu;
}

KActionMenu *CustomTemplatesMenu::replyAllActionMenu() const
{
    return d->mCustomReplyAllActionMenu;
}

KActionMenu *CustomTemplatesMenu::forwardActionMenu() const
{
    return d->mCustomForwardActionMenu;
}

void CustomTemplatesMenu::clear()
{
    qDeleteAll(d->mCustomTemplateActions);
    d->mCustomTemplateActions.clear();

    d->mCustomReplyActionMenu->menu()->clear();
    d->mCustomReplyAllActionMenu->menu()->clear();
    d->mCustomForwardActionMenu->menu()->clear();
    d->mCustomTemplates.clear();
}

void CustomTemplatesMenu::update()
{
    clear();

    const QStringList list = TemplateParserSettings::self()->customTemplates();
    QStringList::const_iterator it = list.constBegin();
    QStringList::const_iterator end = list.constEnd();
    int idx = 0;
    int replyc = 0;
    int replyallc = 0;
    int forwardc = 0;
    for (; it != end; ++it) {
        CTemplates t(*it);
        d->mCustomTemplates.append(*it);
        QString nameAction(*it);
        nameAction.replace(QLatin1Char('&'), QStringLiteral("&&"));

        const QString nameActionName = nameAction.replace(QLatin1Char(' '), QLatin1Char('_'));

        QAction *action = nullptr;
        switch (t.type()) {
        case CustomTemplates::TReply:
            action = new QAction(nameAction, d->mOwnerActionCollection); // krazy:exclude=tipsandthis
            d->mOwnerActionCollection->setDefaultShortcut(action, t.shortcut());
            d->mOwnerActionCollection->addAction(nameActionName, action);
            connect(action, &QAction::triggered, this, [this, idx] {
                slotReplySelected(idx);
            });
            d->mCustomReplyActionMenu->addAction(action);
            d->mCustomTemplateActions.append(action);
            ++replyc;
            break;

        case CustomTemplates::TReplyAll:
            action = new QAction(nameAction, d->mOwnerActionCollection); // krazy:exclude=tipsandthis
            d->mOwnerActionCollection->setDefaultShortcut(action, t.shortcut());
            d->mOwnerActionCollection->addAction(nameActionName, action);
            connect(action, &QAction::triggered, this, [this, idx] {
                slotReplyAllSelected(idx);
            });
            d->mCustomReplyAllActionMenu->addAction(action);
            d->mCustomTemplateActions.append(action);
            ++replyallc;
            break;

        case CustomTemplates::TForward:
            action = new QAction(nameAction, d->mOwnerActionCollection); // krazy:exclude=tipsandthis
            d->mOwnerActionCollection->addAction(nameActionName, action);
            d->mOwnerActionCollection->setDefaultShortcut(action, t.shortcut());
            connect(action, &QAction::triggered, this, [this, idx] {
                slotForwardSelected(idx);
            });
            d->mCustomForwardActionMenu->addAction(action);
            d->mCustomTemplateActions.append(action);
            ++forwardc;
            break;

        case CustomTemplates::TUniversal:
            action = new QAction(nameAction, d->mOwnerActionCollection); // krazy:exclude=tipsandthis
            d->mOwnerActionCollection->addAction(nameActionName, action);

            connect(action, &QAction::triggered, this, [this, idx] {
                slotReplySelected(idx);
            });
            d->mCustomReplyActionMenu->addAction(action);
            d->mCustomTemplateActions.append(action);
            ++replyc;
            action = new QAction(nameAction, d->mOwnerActionCollection); // krazy:exclude=tipsandthis
            connect(action, &QAction::triggered, this, [this, idx] {
                slotReplyAllSelected(idx);
            });
            d->mCustomReplyAllActionMenu->addAction(action);
            d->mCustomTemplateActions.append(action);
            ++replyallc;
            action = new QAction(nameAction, d->mOwnerActionCollection); // krazy:exclude=tipsandthis
            connect(action, &QAction::triggered, this, [this, idx] {
                slotForwardSelected(idx);
            });
            d->mCustomForwardActionMenu->addAction(action);
            d->mCustomTemplateActions.append(action);
            ++forwardc;
            break;
        }

        ++idx;
    }

    if (!replyc) {
        QAction *noAction = d->mCustomReplyActionMenu->menu()->addAction(i18n("(no custom templates)"));
        noAction->setEnabled(false);
        d->mCustomReplyActionMenu->setEnabled(false);
    }
    if (!replyallc) {
        QAction *noAction = d->mCustomReplyAllActionMenu->menu()->addAction(i18n("(no custom templates)"));
        noAction->setEnabled(false);
        d->mCustomReplyAllActionMenu->setEnabled(false);
    }
    if (!forwardc) {
        QAction *noAction = d->mCustomForwardActionMenu->menu()->addAction(i18n("(no custom templates)"));
        noAction->setEnabled(false);
        d->mCustomForwardActionMenu->setEnabled(false);
    }
}

void CustomTemplatesMenu::slotReplySelected(int idx)
{
    Q_EMIT replyTemplateSelected(d->mCustomTemplates.at(idx));
}

void CustomTemplatesMenu::slotReplyAllSelected(int idx)
{
    Q_EMIT replyAllTemplateSelected(d->mCustomTemplates.at(idx));
}

void CustomTemplatesMenu::slotForwardSelected(int idx)
{
    Q_EMIT forwardTemplateSelected(d->mCustomTemplates.at(idx));
}

#include "moc_customtemplatesmenu.cpp"
