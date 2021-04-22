/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@ubiz.ru>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "templateparser_export.h"

#include <QObject>

class KActionCollection;
class KActionMenu;

namespace TemplateParser
{
class CustomTemplatesMenuPrivate;
/**
 * @brief The CustomTemplatesMenu class
 */
class TEMPLATEPARSER_EXPORT CustomTemplatesMenu : public QObject
{
    Q_OBJECT

public:
    explicit CustomTemplatesMenu(QWidget *parent, KActionCollection *ac);
    ~CustomTemplatesMenu() override;

    Q_REQUIRED_RESULT KActionMenu *replyActionMenu() const;
    Q_REQUIRED_RESULT KActionMenu *replyAllActionMenu() const;
    Q_REQUIRED_RESULT KActionMenu *forwardActionMenu() const;

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

