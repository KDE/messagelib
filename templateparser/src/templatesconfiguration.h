/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "templateparser_export.h"
#include "ui_templatesconfiguration_base.h"
class QPlainTextEdit;
namespace TemplateParser
{
class TemplatesConfigurationPrivate;
/**
 * @brief The TemplatesConfiguration class
 */
class TEMPLATEPARSER_EXPORT TemplatesConfiguration : public QWidget, Ui::TemplatesConfigurationBase
{
    Q_OBJECT

public:
    explicit TemplatesConfiguration(QWidget *parent = nullptr, const QString &name = QString());
    ~TemplatesConfiguration() override;

    void loadFromGlobal();
    void saveToGlobal();
    void loadFromIdentity(uint id);
    void saveToIdentity(uint id);
    void loadFromFolder(const QString &id, uint identity = 0);
    void saveToFolder(const QString &id);
    void resetToDefault();

    QLabel *helpLabel() const;

    /**
     * Returns the template configuration identifier string for a given identity.
     */
    static Q_REQUIRED_RESULT QString configIdString(uint id);

public Q_SLOTS:
    void slotInsertCommand(const QString &cmd, int adjustCursor = 0);
    void slotTextChanged();

private Q_SLOTS:
    void slotHelpLinkClicked(const QString &);

Q_SIGNALS:
    void changed();

private:
    Q_REQUIRED_RESULT QPlainTextEdit *currentTextEdit() const;
    Q_REQUIRED_RESULT QString strOrBlank(const QString &str);
    TemplatesConfigurationPrivate *const d;
};
}

