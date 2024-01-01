/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMessageWidget>
#include <QUrl>
class QTimer;
namespace MessageViewer
{
class OpenSavedFileFolderWidget : public KMessageWidget
{
    Q_OBJECT
public:
    enum class FileType {
        Attachment,
        Pdf,
    };

    explicit OpenSavedFileFolderWidget(QWidget *parent = nullptr);
    ~OpenSavedFileFolderWidget() override;

    void setUrls(const QList<QUrl> &urls, MessageViewer::OpenSavedFileFolderWidget::FileType fileType);

public Q_SLOTS:
    void slotShowWarning();
    void slotHideWarning();

private:
    void slotOpenSavedFileFolder();
    void slotTimeOut();
    void slotOpenFile();
    void slotExplicitlyClosed();
    QList<QUrl> mUrls;
    QTimer *const mTimer;
    QAction *const mShowFolderAction;
    QAction *const mOpenFileAction;
};
}
