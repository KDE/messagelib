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
/*!
 * \class TemplateParser::TemplatesConfiguration
 * \inmodule TemplateParser
 * \inheaderfile TemplateParser/TemplatesConfiguration
 * \brief The TemplatesConfiguration class
 */
class TEMPLATEPARSER_EXPORT TemplatesConfiguration : public QWidget, Ui::TemplatesConfigurationBase
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param parent parent widget
     * \param name configuration name
     */
    explicit TemplatesConfiguration(QWidget *parent = nullptr, const QString &name = QString());
    /*!
     * \brief Destructor
     */
    ~TemplatesConfiguration() override;

    /*!
     * \brief Loads templates from global settings
     */
    void loadFromGlobal();
    /*!
     * \brief Saves templates to global settings
     */
    void saveToGlobal();
    /*!
     * \brief Loads templates from identity
     * \param id identity ID
     */
    void loadFromIdentity(uint id);
    /*!
     * \brief Saves templates to identity
     * \param id identity ID
     */
    void saveToIdentity(uint id);
    /*!
     * \brief Loads templates from folder
     * \param id folder ID
     * \param identity identity ID (optional)
     */
    void loadFromFolder(const QString &id, uint identity = 0);
    /*!
     * \brief Saves templates to folder
     * \param id folder ID
     */
    void saveToFolder(const QString &id);
    /*!
     * \brief Resets templates to default
     */
    void resetToDefault();

    /*!
     * \brief Returns the help label
     * \return QLabel pointer
     */
    QLabel *helpLabel() const;

    /**
     * Returns the template configuration identifier string for a given identity.
     */
    [[nodiscard]] static QString configIdString(uint id);

public Q_SLOTS:
    /*!
     * \brief Inserts a command into the current template
     * \param cmd command string
     * \param adjustCursor cursor adjustment value
     */
    void slotInsertCommand(const QString &cmd, int adjustCursor = 0);
    /*!
     * \brief Slot called when text changes
     */
    void slotTextChanged();

Q_SIGNALS:
    /*!
     * \brief Emitted when templates are changed
     */
    void changed();

private:
    TEMPLATEPARSER_NO_EXPORT void slotHelpLinkClicked(const QString &);
    [[nodiscard]] TEMPLATEPARSER_NO_EXPORT QPlainTextEdit *currentTextEdit() const;
    [[nodiscard]] TEMPLATEPARSER_NO_EXPORT QString strOrBlank(const QString &str);
    std::unique_ptr<TemplatesConfigurationPrivate> const d;
};
}
