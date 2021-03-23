/* SPDX-FileCopyrightText: 2015 Sandro Knauß <bugs@sandroknauss.de>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#pragma once

#include <QObject>

#include <KMime/Message>

namespace MimeTreeParser
{
class NodeHelperTest : public QObject
{
    Q_OBJECT

public:
    NodeHelperTest();

private Q_SLOTS:
    void testPersistentIndex();
    void testLocalFiles();
    void testHREF();
    void testCreateTempDir();
    void testFromAsString();
    void shouldTestExtractAttachmentIndex_data();
    void shouldTestExtractAttachmentIndex();
};
}
