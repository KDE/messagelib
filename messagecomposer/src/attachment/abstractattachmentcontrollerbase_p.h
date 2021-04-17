/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 * SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
 *
 * Parts based on KMail code by:
 * Various authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "abstractattachmentcontrollerbase.h"

#include "MessageComposer/AttachmentClipBoardJob"
#include "MessageComposer/AttachmentFromPublicKeyJob"
#include "MessageComposer/AttachmentJob"
#include "MessageComposer/AttachmentVcardFromAddressBookJob"
#include "MessageComposer/Composer"
#include "MessageComposer/GlobalPart"
#include <MessageComposer/AttachmentModel>

#include <MessageViewer/MessageViewerUtil>

#include <MimeTreeParser/NodeHelper>

#include <MessageCore/StringUtil>

#include <AkonadiCore/itemfetchjob.h>
#include <QIcon>
#include <kio/jobuidelegate.h>

#include <QMenu>
#include <QPointer>
#include <QTreeView>

#include "messagecomposer_debug.h"
#include <KEncodingFileDialog>
#include <KFileItemActions>
#include <KIO/ApplicationLauncherJob>
#include <KLocalizedString>
#include <KMessageBox>
#include <QAction>
#include <QMimeDatabase>
#include <QPushButton>
#include <QTemporaryFile>

#include <Libkleo/KeySelectionDialog>
#include <QGpgME/Protocol>

#include "attachment/attachmentfrommimecontentjob.h"
#include "attachment/attachmentfromurljob.h"
#include "messagecore/attachmentupdatejob.h"
#include <Akonadi/Contact/EmailAddressSelectionDialog>
#include <Akonadi/Contact/EmailAddressSelectionWidget>
#include <KIO/Job>
#include <KIO/JobUiDelegate>
#include <KIO/OpenUrlJob>
#include <MessageCore/AttachmentCompressJob>
#include <MessageCore/AttachmentFromUrlUtils>
#include <MessageCore/AttachmentPropertiesDialog>
#include <settings/messagecomposersettings.h>

#include <KMime/Content>
#include <QFileDialog>

using namespace MessageComposer;
using namespace MessageCore;

/// Internal class for the AbstractAttachmentControllerBase used for keeping binary
/// compatibility between versions.
/// \internal
class Q_DECL_HIDDEN MessageComposer::AbstractAttachmentControllerBase::Private
{
public:
    Private(AbstractAttachmentControllerBase *qq);
    ~Private();

    void attachmentRemoved(const AttachmentPart::Ptr &part); // slot
    void compressJobResult(KJob *job); // slot
    void compressJobOverwriteResult(KJob *job); // slot
    void loadJobResult(KJob *job); // slot
    void openSelectedAttachments(); // slot
    void viewSelectedAttachments(); // slot
    void removeSelectedAttachments(); // slot
    void selectedAttachmentProperties(); // slot
    void attachPublicKeyJobResult(KJob *job); // slot
    void slotAttachmentContentCreated(KJob *job); // slot
    void addAttachmentPart(AttachmentPart::Ptr part);
    void attachVcardFromAddressBook(KJob *job); // slot
    void attachClipBoardElement(KJob *job);
    void selectedAllAttachment();
    void createOpenWithMenu(QMenu *topMenu, const AttachmentPart::Ptr &part);
    void reloadAttachment();
    void updateJobResult(KJob *);

    AttachmentPart::List selectedParts;
    AbstractAttachmentControllerBase *const q;
    MessageComposer::AttachmentModel *model = nullptr;

    // If part p is compressed, uncompressedParts[p] is the uncompressed part.
    QHash<AttachmentPart::Ptr, AttachmentPart::Ptr> uncompressedParts;
    bool encryptEnabled = false;
    bool signEnabled = false;
    bool canAttachPublicKey = false;
    bool canAttachOwnPublicKey = false;
    bool attachOwnVCard;
    bool identityHasOwnVcard = false;
};
