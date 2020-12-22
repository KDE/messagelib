/*
   SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef OPENATTACHMENTFOLDERWIDGET_H
#define OPENATTACHMENTFOLDERWIDGET_H

#include <KMessageWidget>
#include <QUrl>
class QTimer;
namespace MessageViewer {
class OpenSavedFileFolderWidget : public KMessageWidget
{
    Q_OBJECT
public:
    enum class FileType {
        Attachment,
        Pdf
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
    void slotExplicitlyClosed();
    QList<QUrl> mUrls;
    QTimer *mTimer = nullptr;
    QAction *mShowFolderAction = nullptr;
};
}

#endif // OPENATTACHMENTFOLDERWIDGET_H
