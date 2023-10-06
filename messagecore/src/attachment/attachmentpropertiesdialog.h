/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  Based on KMail code by various authors (kmmsgpartdlg).

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"

#include "attachmentpart.h"

#include <QDialog>

namespace MessageCore
{
/**
 * @short A dialog for editing attachment properties.
 *
 * @author Constantin Berzan <exit3219@gmail.com>
 */
class MESSAGECORE_EXPORT AttachmentPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Creates a new attachment properties dialog.
     *
     * @param part The attachment part which properties to change.
     * @param readOnly Whether the dialog should be in read-only mode.
     * @param parent The parent object.
     */
    explicit AttachmentPropertiesDialog(const AttachmentPart::Ptr &part, bool readOnly = false, QWidget *parent = nullptr);

    /**
     * Creates a new attachment properties dialog.
     *
     * @param content The mime content that represents the attachment which properties to change.
     * @param parent The parent object.
     *
     * @note This converts the KMime::Content to an AttachmentPart internally.
     *       Therefore, saving the changes to the KMime::Content is not supported,
     *       and the dialog is in readOnly mode.
     */
    explicit AttachmentPropertiesDialog(const KMime::Content *content, QWidget *parent = nullptr);

    /**
     * Destroys the attachment properties dialog.
     */
    ~AttachmentPropertiesDialog() override;

    /**
     * Returns the modified attachment.
     */
    [[nodiscard]] AttachmentPart::Ptr attachmentPart() const;

    /**
     * Sets whether the encryption status of the attachment can be changed.
     */
    void setEncryptEnabled(bool enabled);

    /**
     * Returns whether the encryption status of the attachment can be changed.
     */
    [[nodiscard]] bool isEncryptEnabled() const;

    /**
     * Sets whether the signature status of the attachment can be changed.
     */
    void setSignEnabled(bool enabled);

    /**
     * Returns whether the signature status of the attachment can be changed.
     */
    [[nodiscard]] bool isSignEnabled() const;

public Q_SLOTS:
    void accept() override;
    void slotHelp();

private:
    //@cond PRIVATE
    class AttachmentPropertiesDialogPrivate;
    std::unique_ptr<AttachmentPropertiesDialogPrivate> const d;
    //@endcond
};
}
