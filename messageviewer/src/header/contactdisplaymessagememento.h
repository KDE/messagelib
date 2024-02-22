/* SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "interfaces/bodypart.h"
#include "messageviewer/viewer.h"

#include <KContacts/Addressee>
#include <KContacts/Picture>

#include <QObject>
#include <QPointer>

class KJob;
namespace Gravatar
{
class GravatarResolvUrlJob;
}

namespace Akonadi
{
class ContactSearchJob;
}
namespace MessageViewer
{
class ContactDisplayMessageMemento : public QObject, public MimeTreeParser::Interface::BodyPartMemento
{
    Q_OBJECT
public:
    explicit ContactDisplayMessageMemento(const QString &emailAddress);
    ~ContactDisplayMessageMemento() override;
    void processAddress(const KContacts::Addressee &addressee);
    [[nodiscard]] KContacts::Picture photo() const;

    [[nodiscard]] bool finished() const;

    void detach() override;

    [[nodiscard]] QPixmap gravatarPixmap() const;

    [[nodiscard]] QImage imageFromUrl() const;

Q_SIGNALS:
    // TODO: Factor our update and detach into base class
    void update(MimeTreeParser::UpdateMode);
    void changeDisplayMail(Viewer::DisplayFormatMessage displayAsHtml, bool remoteContent);

private:
    void slotGravatarResolvUrlFinished(Gravatar::GravatarResolvUrlJob *);
    void slotSearchJobFinished(KJob *job);
    [[nodiscard]] bool searchPhoto(const KContacts::AddresseeList &list);
    KContacts::Picture mPhoto;
    QPixmap mGravatarPixmap;
    QImage mImageFromUrl;
    const QString mEmailAddress;
    bool mFinished = false;
    QPointer<Akonadi::ContactSearchJob> mSearchJob;
};
}
