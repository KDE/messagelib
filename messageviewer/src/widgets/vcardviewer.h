/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Daniel Molkentin <molkentin@kde.org>
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef MESSAGEVIEWER_VCARDVIEWER_H
#define MESSAGEVIEWER_VCARDVIEWER_H
#include <QDialog>
#include <kcontacts/addressee.h>

namespace KAddressBookGrantlee {
class GrantleeContactViewer;
}
class QPushButton;
namespace MessageViewer {
class VCardViewer : public QDialog
{
    Q_OBJECT
public:
    explicit VCardViewer(QWidget *parent, const QByteArray &vCard);
    ~VCardViewer() override;

private:
    void slotUser1();
    void slotUser2();
    void slotUser3();
    void readConfig();
    void writeConfig();

    KContacts::Addressee::List mAddresseeList;
    int mAddresseeListIndex = 0;
    KAddressBookGrantlee::GrantleeContactViewer *mContactViewer = nullptr;
    QPushButton *mUser2Button = nullptr;
    QPushButton *mUser3Button = nullptr;
};
}

#endif // VCARDVIEWER_H
