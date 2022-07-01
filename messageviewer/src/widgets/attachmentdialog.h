/*
  This file is part of KMail, the KDE mail client.
  SPDX-FileCopyrightText: 2009 Martin Koller <kollix@aon.at>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KService>
#include <QObject>

class QDialog;
class QDialogButtonBox;
namespace MessageViewer
{
/**
 * A class which handles the dialog used to present the user a choice what to do
 * with an attachment.
 */
class AttachmentDialog : public QObject
{
    Q_OBJECT

public:
    /// returncodes for exec()
    enum {
        Save = 2,
        Open,
        OpenWith,
        Cancel,
    };

    // if @offer is non-null, the "open with <application>" button will also be shown,
    // otherwise only save, open with, cancel
    explicit AttachmentDialog(QWidget *parent, const QString &filenameText, const KService::Ptr &offer, const QString &dontAskAgainName);

    // executes the modal dialog
    int exec();

private:
    void saveClicked();
    void openClicked();
    void openWithClicked();
    QString text, dontAskName;
    QDialog *const dialog;
    QDialogButtonBox *mButtonBox = nullptr;
};
}
