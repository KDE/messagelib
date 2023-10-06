/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@ubiz.ru>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "templateparser_export.h"

#include <QObject>
#include <memory>

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

    [[nodiscard]] KActionMenu *replyActionMenu() const;
    [[nodiscard]] KActionMenu *replyAllActionMenu() const;
    [[nodiscard]] KActionMenu *forwardActionMenu() const;

public Q_SLOTS:
    void update();

Q_SIGNALS:
    void replyTemplateSelected(const QString &tmpl);
    void replyAllTemplateSelected(const QString &tmpl);
    void forwardTemplateSelected(const QString &tmpl);

private:
    TEMPLATEPARSER_NO_EXPORT void slotReplySelected(int idx);
    TEMPLATEPARSER_NO_EXPORT void slotReplyAllSelected(int idx);
    TEMPLATEPARSER_NO_EXPORT void slotForwardSelected(int idx);
    TEMPLATEPARSER_NO_EXPORT void clear();
    std::unique_ptr<CustomTemplatesMenuPrivate> const d;
};
}
