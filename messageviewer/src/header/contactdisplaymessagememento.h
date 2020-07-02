/* SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CONTACTDISPLAYMESSAGEMEMENTO_H
#define CONTACTDISPLAYMESSAGEMEMENTO_H

#include "interfaces/bodypart.h"
#include "messageviewer/viewer.h"

#include <KContacts/Picture>
#include <KContacts/Addressee>

#include <QObject>
#include <QPointer>

class KJob;
namespace Gravatar {
class GravatarResolvUrlJob;
}

namespace Akonadi {
class ContactSearchJob;
}
namespace MessageViewer {
class ContactDisplayMessageMemento : public QObject, public MimeTreeParser::Interface::BodyPartMemento
{
    Q_OBJECT
public:
    explicit ContactDisplayMessageMemento(const QString &emailAddress);
    ~ContactDisplayMessageMemento() override;
    void processAddress(const KContacts::Addressee &addressee);
    Q_REQUIRED_RESULT KContacts::Picture photo() const;

    Q_REQUIRED_RESULT bool finished() const;

    void detach() override;

    Q_REQUIRED_RESULT QPixmap gravatarPixmap() const;

    Q_REQUIRED_RESULT QImage imageFromUrl() const;

Q_SIGNALS:
    // TODO: Factor our update and detach into base class
    void update(MimeTreeParser::UpdateMode);
    void changeDisplayMail(Viewer::DisplayFormatMessage displayAsHtml, bool remoteContent);

private Q_SLOTS:
    void slotSearchJobFinished(KJob *job);

    void slotGravatarResolvUrlFinished(Gravatar::GravatarResolvUrlJob *);
private:
    Q_REQUIRED_RESULT bool searchPhoto(const KContacts::AddresseeList &list);
    KContacts::Picture mPhoto;
    QPixmap mGravatarPixmap;
    QImage mImageFromUrl;
    QString mEmailAddress;
    bool mFinished = false;
    QPointer<Akonadi::ContactSearchJob> mSearchJob;
};
}

#endif /* CONTACTDISPLAYMESSAGE_H */
