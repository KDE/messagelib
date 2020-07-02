/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@ubiz.ru>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef TEMPLATEPARSER_CUSTOMTEMPLATESMENU_H
#define TEMPLATEPARSER_CUSTOMTEMPLATESMENU_H

#include "templateparser_export.h"

#include <QObject>

class KActionCollection;
class KActionMenu;

namespace TemplateParser {
class CustomTemplatesMenuPrivate;
/**
 * @brief The CustomTemplatesMenu class
 */
class TEMPLATEPARSER_EXPORT CustomTemplatesMenu : public QObject
{
    Q_OBJECT

public:
    explicit CustomTemplatesMenu(QWidget *parent, KActionCollection *ac);
    ~CustomTemplatesMenu();

    KActionMenu *replyActionMenu() const;
    KActionMenu *replyAllActionMenu() const;
    KActionMenu *forwardActionMenu() const;

public Q_SLOTS:
    void update();

Q_SIGNALS:
    void replyTemplateSelected(const QString &tmpl);
    void replyAllTemplateSelected(const QString &tmpl);
    void forwardTemplateSelected(const QString &tmpl);

private Q_SLOTS:
    void slotReplySelected(int idx);
    void slotReplyAllSelected(int idx);
    void slotForwardSelected(int idx);

private:
    void clear();
    CustomTemplatesMenuPrivate *const d;
};
}

#endif
