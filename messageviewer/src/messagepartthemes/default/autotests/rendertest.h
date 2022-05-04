/*
   SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#pragma once

#include <MimeTreeParser/MessagePart>
#include <QObject>

class RenderTest : public QObject
{
    Q_OBJECT

public:
    static void initMain();

private Q_SLOTS:
    void testRenderSmart_data();
    void testRenderSmart();

    void testRenderSmartAsync_data();
    void testRenderSmartAsync();

    void testRenderSmartDetails_data();
    void testRenderSmartDetails();

    void testRenderInlined_data();
    void testRenderInlined();

    void testRenderIconic_data();
    void testRenderIconic();

    void testRenderHidden_data();
    void testRenderHidden();

    void testRenderHeaderOnly_data();
    void testRenderHeaderOnly();

private:
    void testRender();
    void testRenderTree(const MimeTreeParser::MessagePart::Ptr &messagePart);
};
