/*
 * SPDX-FileCopyrightText: 2005 Till Adam <adam@kde.org>
 * SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
 */

#include "messageviewer/messageviewerutil.h"
#include "MessageCore/MessageCoreSettings"
#include "MessageCore/StringUtil"
#include "messageviewer_debug.h"
#include "messageviewerutil_p.h"
#include <MimeTreeParser/NodeHelper>

#include <PimCommon/RenameFileDialog>

#include <Gravatar/GravatarCache>
#include <gravatar/gravatarsettings.h>

#include <KMbox/MBox>

#include <KFileWidget>
#include <KIO/FileCopyJob>
#include <KIO/StatJob>
#include <KJobWidgets>
#include <KLocalizedString>
#include <KMessageBox>
#include <KMime/Message>
#include <KRecentDirs>
#include <QAction>
#include <QActionGroup>
#include <QDBusConnectionInterface>
#include <QDesktopServices>
#include <QFileDialog>
#include <QIcon>
#include <QRegularExpression>
#include <QTemporaryFile>
#include <QWidget>

using namespace MessageViewer;
/** Checks whether @p str contains external references. To be precise,
    we only check whether @p str contains 'xxx="http[s]:' where xxx is
    not href. Obfuscated external references are ignored on purpose.
*/

bool Util::containsExternalReferences(const QString &str, const QString &extraHead)
{
    const bool hasBaseInHeader = extraHead.contains(QLatin1StringView("<base href=\""), Qt::CaseInsensitive);
    if (hasBaseInHeader
        && (str.contains(QLatin1StringView("href=\"/"), Qt::CaseInsensitive) || str.contains(QLatin1StringView("<img src=\"/"), Qt::CaseInsensitive))) {
        return true;
    }
    int httpPos = str.indexOf(QLatin1StringView("\"http:"), Qt::CaseInsensitive);
    int httpsPos = str.indexOf(QLatin1StringView("\"https:"), Qt::CaseInsensitive);
    while (httpPos >= 0 || httpsPos >= 0) {
        // pos = index of next occurrence of "http: or "https: whichever comes first
        const int pos = (httpPos < httpsPos) ? ((httpPos >= 0) ? httpPos : httpsPos) : ((httpsPos >= 0) ? httpsPos : httpPos);
        // look backwards for "href"
        if (pos > 5) {
            int hrefPos = str.lastIndexOf(QLatin1StringView("href"), pos - 5, Qt::CaseInsensitive);
            // if no 'href' is found or the distance between 'href' and '"http[s]:'
            // is larger than 7 (7 is the distance in 'href = "http[s]:') then
            // we assume that we have found an external reference
            if ((hrefPos == -1) || (pos - hrefPos > 7)) {
                // HTML messages created by KMail itself for now contain the following:
                // <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
                // Make sure not to show an external references warning for this string
                int dtdPos = str.indexOf(QLatin1StringView("http://www.w3.org/TR/html4/loose.dtd"), pos + 1);
                if (dtdPos != (pos + 1)) {
                    return true;
                }
            }
        }
        // find next occurrence of "http: or "https:
        if (pos == httpPos) {
            httpPos = str.indexOf(QLatin1StringView("\"http:"), httpPos + 6, Qt::CaseInsensitive);
        } else {
            httpsPos = str.indexOf(QLatin1StringView("\"https:"), httpsPos + 7, Qt::CaseInsensitive);
        }
    }
    QRegularExpressionMatch rmatch;

    const int startImgIndex = str.indexOf(QLatin1StringView("<img "));
    QString newStringImg;
    if (startImgIndex != -1) {
        for (int i = startImgIndex, total = str.length(); i < total; ++i) {
            const QChar charStr = str.at(i);
            if (charStr == QLatin1Char('>')) {
                newStringImg += charStr;
                break;
            } else {
                newStringImg += charStr;
            }
        }
        if (!newStringImg.isEmpty()) {
            static QRegularExpression image1RegularExpression =
                QRegularExpression(QStringLiteral("<img.*src=\"https?:/.*\".*>"), QRegularExpression::CaseInsensitiveOption);
            const bool containsReg2 = newStringImg.contains(image1RegularExpression, &rmatch);
            if (!containsReg2) {
                static QRegularExpression image2RegularExpression =
                    QRegularExpression(QStringLiteral("<img.*src=https?:/.*>"), QRegularExpression::CaseInsensitiveOption);
                const bool containsReg = newStringImg.contains(image2RegularExpression, &rmatch);
                return containsReg;
            } else {
                return true;
            }
        }
    }
    return false;
}

bool Util::checkOverwrite(const QUrl &url, QWidget *w)
{
    bool fileExists = false;
    if (url.isLocalFile()) {
        fileExists = QFile::exists(url.toLocalFile());
    } else {
        auto job = KIO::stat(url, KIO::StatJob::DestinationSide, KIO::StatBasic);
        KJobWidgets::setWindow(job, w);
        fileExists = job->exec();
    }
    if (fileExists) {
        if (KMessageBox::Cancel
            == KMessageBox::warningContinueCancel(w,
                                                  i18n("A file named \"%1\" already exists. "
                                                       "Are you sure you want to overwrite it?",
                                                       url.toDisplayString()),
                                                  i18nc("@title:window", "Overwrite File?"),
                                                  KStandardGuiItem::overwrite())) {
            return false;
        }
    }
    return true;
}

bool Util::handleUrlWithQDesktopServices(const QUrl &url)
{
#if defined Q_OS_WIN || defined Q_OS_MACX
    QDesktopServices::openUrl(url);
    return true;
#else
    // Always handle help through khelpcenter or browser
    if (url.scheme() == QLatin1StringView("help")) {
        QDesktopServices::openUrl(url);
        return true;
    }
    return false;
#endif
}

bool Util::saveContents(QWidget *parent, const KMime::Content::List &contents, QList<QUrl> &urlList)
{
    QUrl url;
    QUrl dirUrl;
    QString recentDirClass;
    QUrl currentFolder;
    const bool multiple = (contents.count() > 1);
    if (multiple) {
        // get the dir
        dirUrl = QFileDialog::getExistingDirectoryUrl(parent,
                                                      i18n("Save Attachments To"),
                                                      KFileWidget::getStartUrl(QUrl(QStringLiteral("kfiledialog:///attachmentDir")), recentDirClass));
        if (!dirUrl.isValid()) {
            return false;
        }

        // we may not get a slash-terminated url out of KFileDialog
        if (!dirUrl.path().endsWith(QLatin1Char('/'))) {
            dirUrl.setPath(dirUrl.path() + QLatin1Char('/'));
        }
        currentFolder = dirUrl;
    } else {
        // only one item, get the desired filename
        KMime::Content *content = contents.first();
        QString fileName = MimeTreeParser::NodeHelper::fileName(content);
        fileName = MessageCore::StringUtil::cleanFileName(fileName);
        if (fileName.isEmpty()) {
            fileName = i18nc("filename for an unnamed attachment", "attachment.1");
        }

        QUrl localUrl = KFileWidget::getStartUrl(QUrl(QStringLiteral("kfiledialog:///attachmentDir")), recentDirClass);
        localUrl.setPath(localUrl.path() + QLatin1Char('/') + fileName);
        QFileDialog::Options options = QFileDialog::DontConfirmOverwrite;
        url = QFileDialog::getSaveFileUrl(parent, i18n("Save Attachment"), localUrl, QString(), nullptr, options);
        if (url.isEmpty()) {
            return false;
        }
        currentFolder = KIO::upUrl(url);
    }

    if (!recentDirClass.isEmpty()) {
        KRecentDirs::add(recentDirClass, currentFolder.path());
    }

    QMap<QString, int> renameNumbering;

    bool globalResult = true;
    int unnamedAtmCount = 0;
    PimCommon::RenameFileDialog::RenameFileDialogResult result = PimCommon::RenameFileDialog::RENAMEFILE_IGNORE;
    for (KMime::Content *content : std::as_const(contents)) {
        QUrl curUrl;
        if (!dirUrl.isEmpty()) {
            curUrl = dirUrl;
            QString fileName = MimeTreeParser::NodeHelper::fileName(content);
            fileName = MessageCore::StringUtil::cleanFileName(fileName);
            if (fileName.isEmpty()) {
                ++unnamedAtmCount;
                fileName = i18nc("filename for the %1-th unnamed attachment", "attachment.%1", unnamedAtmCount);
            }
            if (!curUrl.path().endsWith(QLatin1Char('/'))) {
                curUrl.setPath(curUrl.path() + QLatin1Char('/'));
            }
            curUrl.setPath(curUrl.path() + fileName);
        } else {
            curUrl = url;
        }
        if (!curUrl.isEmpty()) {
            // Bug #312954
            if (multiple && (curUrl.fileName() == QLatin1StringView("smime.p7s"))) {
                continue;
            }
            // Rename the file if we have already saved one with the same name:
            // try appending a number before extension (e.g. "pic.jpg" => "pic_2.jpg")
            const QString origFile = curUrl.fileName();
            QString file = origFile;

            while (renameNumbering.contains(file)) {
                file = origFile;
                int num = renameNumbering[file] + 1;
                int dotIdx = file.lastIndexOf(QLatin1Char('.'));
                file.insert((dotIdx >= 0) ? dotIdx : file.length(), QLatin1Char('_') + QString::number(num));
            }
            curUrl = curUrl.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash);
            curUrl.setPath(curUrl.path() + QLatin1Char('/') + file);

            // Increment the counter for both the old and the new filename
            if (!renameNumbering.contains(origFile)) {
                renameNumbering[origFile] = 1;
            } else {
                renameNumbering[origFile]++;
            }

            if (file != origFile) {
                if (!renameNumbering.contains(file)) {
                    renameNumbering[file] = 1;
                } else {
                    renameNumbering[file]++;
                }
            }

            if (!(result == PimCommon::RenameFileDialog::RENAMEFILE_OVERWRITEALL || result == PimCommon::RenameFileDialog::RENAMEFILE_IGNOREALL)) {
                bool fileExists = false;
                if (curUrl.isLocalFile()) {
                    fileExists = QFile::exists(curUrl.toLocalFile());
                } else {
                    auto job = KIO::stat(url, KIO::StatJob::DestinationSide, KIO::StatDetail::StatBasic);
                    KJobWidgets::setWindow(job, parent);
                    fileExists = job->exec();
                }
                if (fileExists) {
                    QPointer<PimCommon::RenameFileDialog> dlg = new PimCommon::RenameFileDialog(curUrl, multiple, parent);
                    result = static_cast<PimCommon::RenameFileDialog::RenameFileDialogResult>(dlg->exec());
                    if (result == PimCommon::RenameFileDialog::RENAMEFILE_IGNORE || result == PimCommon::RenameFileDialog::RENAMEFILE_IGNOREALL) {
                        delete dlg;
                        continue;
                    } else if (result == PimCommon::RenameFileDialog::RENAMEFILE_RENAME) {
                        if (dlg) {
                            curUrl = dlg->newName();
                        }
                    }
                    delete dlg;
                }
            }
            // save
            if (result != PimCommon::RenameFileDialog::RENAMEFILE_IGNOREALL) {
                const bool resultSave = saveContent(parent, content, curUrl);
                if (!resultSave) {
                    globalResult = resultSave;
                } else {
                    urlList.append(curUrl);
                }
            }
        }
    }

    return globalResult;
}

bool Util::saveContent(QWidget *parent, KMime::Content *content, const QUrl &url)
{
    // FIXME: This is all horribly broken. First of all, creating a NodeHelper and then immediately
    //        reading out the encryption/signature state will not work at all.
    //        Then, topLevel() will not work for attachments that are inside encrypted parts.
    //        What should actually be done is either passing in an ObjectTreeParser that has already
    //        parsed the message, or creating an OTP here (which would have the downside that the
    //        password dialog for decrypting messages is shown twice)
#if 0 // totally broken
    KMime::Content *topContent = content->topLevel();
    MimeTreeParser::NodeHelper *mNodeHelper = new MimeTreeParser::NodeHelper;
    bool bSaveEncrypted = false;
    bool bEncryptedParts = mNodeHelper->encryptionState(content)
                           != MimeTreeParser::KMMsgNotEncrypted;
    if (bEncryptedParts) {
        if (KMessageBox::questionTwoActions(parent,
                                       i18n(
                                           "The part %1 of the message is encrypted. Do you want to keep the encryption when saving?",
                                           url.fileName()),
                                       i18n("KMail Question"), KGuiItem(i18nc("@action:button", "Keep Encryption")),
                                       KGuiItem(i18nc("@action:button", "Do Not Keep")))
            == KMessageBox::ButtonCode::PrimaryAction) {
            bSaveEncrypted = true;
        }
    }

    bool bSaveWithSig = true;
    if (mNodeHelper->signatureState(content) != MessageViewer::MimeTreeParser::KMMsgNotSigned) {
        if (KMessageBox::questionTwoActions(parent,
                                       i18n(
                                           "The part %1 of the message is signed. Do you want to keep the signature when saving?",
                                           url.fileName()),
                                       i18n("KMail Question"), KGuiItem(i18nc("@action:button", "Keep Signature")),
                                       KGuiItem(i18nc("@action:button", "Do Not Keep")))
            != KMessageBox::Yes) {
            bSaveWithSig = false;
        }
    }

    QByteArray data;
    if (bSaveEncrypted || !bEncryptedParts) {
        KMime::Content *dataNode = content;
        QByteArray rawDecryptedBody;
        bool gotRawDecryptedBody = false;
        if (!bSaveWithSig) {
            if (topContent->contentType()->mimeType() == "multipart/signed") {
                // carefully look for the part that is *not* the signature part:
                if (MimeTreeParser::ObjectTreeParser::findType(topContent,
                                                               "application/pgp-signature", true,
                                                               false)) {
                    dataNode = MimeTreeParser::ObjectTreeParser ::findTypeNot(topContent,
                                                                              "application",
                                                                              "pgp-signature", true,
                                                                              false);
                } else if (MimeTreeParser::ObjectTreeParser::findType(topContent,
                                                                      "application/pkcs7-mime",
                                                                      true, false)) {
                    dataNode = MimeTreeParser::ObjectTreeParser ::findTypeNot(topContent,
                                                                              "application",
                                                                              "pkcs7-mime", true,
                                                                              false);
                } else {
                    dataNode = MimeTreeParser::ObjectTreeParser ::findTypeNot(topContent,
                                                                              "multipart", "", true,
                                                                              false);
                }
            } else {
                EmptySource emptySource;
                MimeTreeParser::ObjectTreeParser otp(&emptySource, 0, 0, false, false);

                // process this node and all it's siblings and descendants
                mNodeHelper->setNodeUnprocessed(dataNode, true);
                otp.parseObjectTree(dataNode);

                rawDecryptedBody = otp.rawDecryptedBody();
                gotRawDecryptedBody = true;
            }
        }
        QByteArray cstr = gotRawDecryptedBody
                          ? rawDecryptedBody
                          : dataNode->decodedContent();
        data = KMime::CRLFtoLF(cstr);
    }
#else
    const QByteArray data = content->decodedContent();
    qCWarning(MESSAGEVIEWER_LOG) << "Port the encryption/signature handling when saving a KMime::Content.";
#endif
    QDataStream ds;
    QFile file;
    QTemporaryFile tf;
    if (url.isLocalFile()) {
        // save directly
        file.setFileName(url.toLocalFile());
        if (!file.open(QIODevice::WriteOnly)) {
            KMessageBox::error(parent,
                               xi18nc("1 = file name, 2 = error string",
                                      "<qt>Could not write to the file<br /><filename>%1</filename><br /><br />%2</qt>",
                                      file.fileName(),
                                      file.errorString()),
                               i18nc("@title:window", "Error saving attachment"));
            return false;
        }
        ds.setDevice(&file);
    } else {
        // tmp file for upload
        tf.open();
        ds.setDevice(&tf);
    }

    const int bytesWritten = ds.writeRawData(data.data(), data.size());
    if (bytesWritten != data.size()) {
        auto f = static_cast<QFile *>(ds.device());
        KMessageBox::error(parent,
                           xi18nc("1 = file name, 2 = error string",
                                  "<qt>Could not write to the file<br /><filename>%1</filename><br /><br />%2</qt>",
                                  f->fileName(),
                                  f->errorString()),
                           i18n("Error saving attachment"));
        // Remove the newly created empty or partial file
        f->remove();
        return false;
    }

    if (!url.isLocalFile()) {
        // QTemporaryFile::fileName() is only defined while the file is open
        QString tfName = tf.fileName();
        tf.close();
        auto job = KIO::file_copy(QUrl::fromLocalFile(tfName), url);
        KJobWidgets::setWindow(job, parent);
        if (!job->exec()) {
            KMessageBox::error(parent,
                               xi18nc("1 = file name, 2 = error string",
                                      "<qt>Could not write to the file<br /><filename>%1</filename><br /><br />%2</qt>",
                                      url.toDisplayString(),
                                      job->errorString()),
                               i18n("Error saving attachment"));
            return false;
        }
    } else {
        file.close();
    }

    return true;
}

bool Util::saveAttachments(const KMime::Content::List &contents, QWidget *parent, QList<QUrl> &urlList)
{
    if (contents.isEmpty()) {
        KMessageBox::information(parent, i18n("Found no attachments to save."));
        return false;
    }

    return Util::saveContents(parent, contents, urlList);
}

QString Util::generateFileNameForExtension(const Akonadi::Item &msgBase, const QString &extension)
{
    QString fileName;

    if (msgBase.hasPayload<KMime::Message::Ptr>()) {
        fileName = MessageCore::StringUtil::cleanFileName(MessageCore::StringUtil::cleanSubject(msgBase.payload<KMime::Message::Ptr>().data()).trimmed());
        fileName.remove(QLatin1Char('\"'));
    } else {
        fileName = i18n("message");
    }

    if (!fileName.endsWith(extension)) {
        fileName += extension;
    }
    return fileName;
}

QString Util::generateMboxFileName(const Akonadi::Item &msgBase)
{
    return Util::generateFileNameForExtension(msgBase, QStringLiteral(".mbox"));
}

bool Util::saveMessageInMboxAndGetUrl(QUrl &url, const Akonadi::Item::List &retrievedMsgs, QWidget *parent, bool appendMessages)
{
    if (retrievedMsgs.isEmpty()) {
        return false;
    }
    const Akonadi::Item msgBase = retrievedMsgs.first();
    QString fileName = generateMboxFileName(msgBase);

    const QString filter = i18n("email messages (*.mbox);;all files (*)");

    QString fileClass;
    const QUrl startUrl = KFileWidget::getStartUrl(QUrl(QStringLiteral("kfiledialog:///savemessage")), fileClass);
    QUrl localUrl;
    localUrl.setPath(startUrl.path() + QLatin1Char('/') + fileName);
    QFileDialog::Options opt;
    if (appendMessages) {
        opt |= QFileDialog::DontConfirmOverwrite;
    }
    QUrl dirUrl = QFileDialog::getSaveFileUrl(parent,
                                              i18np("Save Message", "Save Messages", retrievedMsgs.count()),
                                              QUrl::fromLocalFile(localUrl.toString()),
                                              filter,
                                              nullptr,
                                              opt);
    if (!dirUrl.isEmpty()) {
        QFile file;
        QTemporaryFile tf;
        QString localFileName;
        if (dirUrl.isLocalFile()) {
            // save directly
            file.setFileName(dirUrl.toLocalFile());
            localFileName = file.fileName();
            if (!appendMessages) {
                QFile::remove(localFileName);
            }
        } else {
            // tmp file for upload
            tf.open();
            localFileName = tf.fileName();
        }

        KMBox::MBox mbox;
        if (!mbox.load(localFileName)) {
            if (appendMessages) {
                KMessageBox::error(parent, i18n("File %1 could not be loaded.", localFileName), i18nc("@title:window", "Error loading message"));
            } else {
                KMessageBox::error(parent, i18n("File %1 could not be created.", localFileName), i18nc("@title:window", "Error saving message"));
            }
            return false;
        }
        for (const Akonadi::Item &item : std::as_const(retrievedMsgs)) {
            if (item.hasPayload<KMime::Message::Ptr>()) {
                mbox.appendMessage(item.payload<KMime::Message::Ptr>());
            }
        }

        if (!mbox.save()) {
            KMessageBox::error(parent, i18n("We cannot save message."), i18n("Error saving message"));
            return false;
        }
        localUrl = QUrl::fromLocalFile(localFileName);
        if (localUrl.isLocalFile()) {
            KRecentDirs::add(fileClass, localUrl.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash).path());
        }

        if (!dirUrl.isLocalFile()) {
            // QTemporaryFile::fileName() is only defined while the file is open
            QString tfName = tf.fileName();
            tf.close();
            auto job = KIO::file_copy(QUrl::fromLocalFile(tfName), dirUrl);
            KJobWidgets::setWindow(job, parent);
            if (!job->exec()) {
                KMessageBox::error(parent,
                                   xi18nc("1 = file name, 2 = error string",
                                          "<qt>Could not write to the file<br /><filename>%1</filename><br /><br />%2</qt>",
                                          url.toDisplayString(),
                                          job->errorString()),
                                   i18nc("@title:window", "Error saving message"));
                return false;
            }
        } else {
            file.close();
        }
        url = localUrl;
    }
    return true;
}

bool Util::saveMessageInMbox(const Akonadi::Item::List &retrievedMsgs, QWidget *parent, bool appendMessages)
{
    QUrl url;
    return saveMessageInMboxAndGetUrl(url, retrievedMsgs, parent, appendMessages);
}

bool Util::deleteAttachment(KMime::Content *node)
{
    Q_ASSERT(node);

    auto parentNode = node->parent();
    if (!parentNode) {
        return false;
    }

    QString filename;
    QString name;
    QByteArray mimetype;
    if (auto cd = node->contentDisposition(false)) {
        filename = cd->filename();
    }

    if (auto ct = node->contentType(false)) {
        name = ct->name();
        mimetype = ct->mimeType();
    }

    if (mimetype == "text/x-moz-deleted") {
        // The attachment has already been deleted, no need to delete the deletion attachment
        return false;
    }

    // text/plain part:
    const auto newName = i18nc("Argument is the original name of the deleted attachment", "Deleted: %1", name);
    auto deletePart = new KMime::Content(parentNode);
    auto deleteCt = deletePart->contentType(true);
    deleteCt->setMimeType("text/x-moz-deleted");
    deleteCt->setName(newName);
    deletePart->contentDisposition(true)->setDisposition(KMime::Headers::CDattachment);
    deletePart->contentDisposition(false)->setFilename(newName);

    deleteCt->setCharset("utf-8");
    deletePart->contentTransferEncoding()->setEncoding(KMime::Headers::CEquPr);
    QByteArray bodyMessage = QByteArrayLiteral("\n");
    bodyMessage += i18n("You deleted an attachment from this message. The original MIME headers for the attachment were:").toUtf8() + ("\n");
    bodyMessage += ("\nContent-Type: ") + mimetype;
    bodyMessage += ("\nname=\"") + name.toUtf8() + "\"";
    bodyMessage += ("\nfilename=\"") + filename.toUtf8() + "\"";
    deletePart->setBody(bodyMessage);
    parentNode->replaceContent(node, deletePart);

    parentNode->assemble();

    return true;
}

int Util::deleteAttachments(const KMime::Content::List &nodes)
{
    int updatedCount = 0;
    for (const auto node : nodes) {
        if (deleteAttachment(node)) {
            ++updatedCount;
        }
    }
    return updatedCount;
}

QAction *Util::createAppAction(const KService::Ptr &service, bool singleOffer, QActionGroup *actionGroup, QObject *parent)
{
    QString actionName(service->name().replace(QLatin1Char('&'), QStringLiteral("&&")));
    if (singleOffer) {
        actionName = i18n("Open &with %1", actionName);
    } else {
        actionName = i18nc("@item:inmenu Open With, %1 is application name", "%1", actionName);
    }

    auto act = new QAction(parent);
    act->setIcon(QIcon::fromTheme(service->icon()));
    act->setText(actionName);
    actionGroup->addAction(act);
    act->setData(QVariant::fromValue(service));
    return act;
}

bool Util::excludeExtraHeader(const QString &s)
{
    static QRegularExpression divRef(QStringLiteral("</div>"), QRegularExpression::CaseInsensitiveOption);
    if (s.contains(divRef)) {
        return true;
    }
    static QRegularExpression bodyRef(QStringLiteral("body.s*>.s*div"), QRegularExpression::CaseInsensitiveOption);
    if (s.contains(bodyRef)) {
        return true;
    }
    return false;
}

void Util::addHelpTextAction(QAction *act, const QString &text)
{
    act->setStatusTip(text);
    act->setToolTip(text);
    if (act->whatsThis().isEmpty()) {
        act->setWhatsThis(text);
    }
}

void Util::readGravatarConfig()
{
    Gravatar::GravatarCache::self()->setMaximumSize(Gravatar::GravatarSettings::self()->gravatarCacheSize());
    if (!Gravatar::GravatarSettings::self()->gravatarSupportEnabled()) {
        Gravatar::GravatarCache::self()->clear();
    }
}

QString Util::parseBodyStyle(const QString &style)
{
    const int indexStyle = style.indexOf(QLatin1StringView("style=\""));
    if (indexStyle != -1) {
        // qDebug() << " style " << style;
        const int indexEnd = style.indexOf(QLatin1Char('"'), indexStyle + 7);
        if (indexEnd != -1) {
            const QStringView styleStr = QStringView(style).mid(indexStyle + 7, indexEnd - (indexStyle + 7));
            const auto lstStyle = styleStr.split(QLatin1Char(';'), Qt::SkipEmptyParts);
            QStringList lst;
            for (const auto &style : lstStyle) {
                // qDebug() << " style : " << style;
                if (!style.trimmed().contains(QLatin1StringView("white-space")) && !style.trimmed().contains(QLatin1StringView("text-align"))) {
                    lst.append(style.toString().trimmed());
                }
            }
            if (!lst.isEmpty()) {
                // qDebug() << " lst " << lst;
                return QStringLiteral(" style=\"%1").arg(lst.join(QLatin1Char(';'))) + QStringLiteral(";\"");
            }
        }
    }
    return {};
}

// FIXME this used to go through the full webkit parser to extract the body and head blocks
// until we have that back, at least attempt to fix some of the damage
// yes, "parsing" HTML with regexps is very very wrong, but it's still better than not filtering
// this at all...
Util::HtmlMessageInfo Util::processHtml(const QString &htmlSource)
{
    Util::HtmlMessageInfo messageInfo;
    QString s = htmlSource.trimmed();
    static QRegularExpression docTypeRegularExpression = QRegularExpression(QStringLiteral("<!DOCTYPE[^>]*>"), QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch matchDocType;
    const int indexDoctype = s.indexOf(docTypeRegularExpression, 0, &matchDocType);
    QString textBeforeDoctype;
    if (indexDoctype > 0) {
        textBeforeDoctype = s.left(indexDoctype);
        s.remove(textBeforeDoctype);
    }
    const QString capturedString = matchDocType.captured();
    if (!capturedString.isEmpty()) {
        s = s.remove(capturedString).trimmed();
    }
    static QRegularExpression htmlRegularExpression = QRegularExpression(QStringLiteral("<html[^>]*>"), QRegularExpression::CaseInsensitiveOption);
    s = s.remove(htmlRegularExpression).trimmed();
    // head
    static QRegularExpression headEndRegularExpression = QRegularExpression(QStringLiteral("^<head/>"), QRegularExpression::CaseInsensitiveOption);
    s = s.remove(headEndRegularExpression).trimmed();
    const int startIndex = s.indexOf(QLatin1StringView("<head>"), Qt::CaseInsensitive);
    if (startIndex >= 0) {
        const auto endIndex = s.indexOf(QLatin1StringView("</head>"), Qt::CaseInsensitive);

        if (endIndex < 0) {
            messageInfo.htmlSource = htmlSource;
            return messageInfo;
        }
        const int index = startIndex + 6;
        messageInfo.extraHead = s.mid(index, endIndex - index);
        if (MessageViewer::Util::excludeExtraHeader(messageInfo.extraHead)) {
            messageInfo.extraHead.clear();
        }
        s = s.remove(startIndex, endIndex - startIndex + 7).trimmed();
        // qDebug() << "BEFORE messageInfo.extraHead**********" << messageInfo.extraHead;
        static QRegularExpression styleBodyRegularExpression =
            QRegularExpression(QStringLiteral("<style>\\s*body\\s*{"), QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption);
        QRegularExpressionMatch matchBodyStyle;
        const int bodyStyleStartIndex = messageInfo.extraHead.indexOf(styleBodyRegularExpression, 0, &matchBodyStyle);
        if (bodyStyleStartIndex > 0) {
            const auto endIndex = messageInfo.extraHead.indexOf(QLatin1StringView("</style>"), bodyStyleStartIndex, Qt::CaseInsensitive);
            // qDebug() << " endIndex " << endIndex;
            messageInfo.extraHead = messageInfo.extraHead.remove(bodyStyleStartIndex, endIndex - bodyStyleStartIndex + 8);
        }
        // qDebug() << "AFTER messageInfo.extraHead**********" << messageInfo.extraHead;
    }
    // body
    static QRegularExpression body = QRegularExpression(QStringLiteral("<body[^>]*>"), QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch matchBody;
    const int bodyStartIndex = s.indexOf(body, 0, &matchBody);
    if (bodyStartIndex >= 0) {
        // qDebug() << "matchBody  " << matchBody.capturedTexts();
        s = s.remove(bodyStartIndex, matchBody.capturedLength()).trimmed();
        // Parse style
        messageInfo.bodyStyle = matchBody.captured();
    }
    // Some mail has </div>$ at end
    static QRegularExpression htmlDivRegularExpression =
        QRegularExpression(QStringLiteral("(</html></div>|</html>)$"), QRegularExpression::CaseInsensitiveOption);
    s = s.remove(htmlDivRegularExpression).trimmed();
    // s = s.remove(QRegularExpression(QStringLiteral("</html>$"), QRegularExpression::CaseInsensitiveOption)).trimmed();
    static QRegularExpression bodyEndRegularExpression = QRegularExpression(QStringLiteral("</body>$"), QRegularExpression::CaseInsensitiveOption);
    s = s.remove(bodyEndRegularExpression).trimmed();
    messageInfo.htmlSource = textBeforeDoctype + s;
    return messageInfo;
}

QDebug operator<<(QDebug d, const Util::HtmlMessageInfo &t)
{
    d << " htmlSource " << t.htmlSource;
    d << " extraHead " << t.extraHead;
    d << " bodyStyle " << t.bodyStyle;
    return d;
}
