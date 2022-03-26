/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QTabWidget>

class QLineEdit;
class KTextEdit;

namespace MessageList
{
namespace Core
{
class OptionSet;
} // namespace Core

namespace Utils
{
/**
 * The base class for the OptionSet editors. Provides common functionality.
 */
class OptionSetEditor : public QTabWidget
{
    Q_OBJECT

public:
    explicit OptionSetEditor(QWidget *parent);
    ~OptionSetEditor() override;
    void setReadOnly(bool readOnly);

protected:
    /**
     * Returns the editor for the name of the OptionSet.
     * Derived classes are responsible of filling this UI element and reading back data from it.
     */
    QLineEdit *nameEdit() const;

    /**
     * Returns the editor for the description of the OptionSet.
     * Derived classes are responsible of filling this UI element and reading back data from it.
     */
    KTextEdit *descriptionEdit() const;

protected Q_SLOTS:
    /**
     * Handles editing of the name field.
     * Pure virtual slot. Derived classes must provide an implementation.
     */
    virtual void slotNameEditTextEdited(const QString &newName) = 0;

private:
    QLineEdit *mNameEdit = nullptr; ///< The editor for the OptionSet name
    KTextEdit *mDescriptionEdit = nullptr; ///< The editor for the OptionSet description
};
} // namespace Utils
} // namespace MessageList
