/* Copyright (c) 2010 Volker Krause <vkrause@kde.org>
   Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __MESSAGEVIEWER_TEST_RENDERTEST_H__
#define __MESSAGEVIEWER_TEST_RENDERTEST_H__

#include <QObject>
#include <MimeTreeParser/MessagePart>

class RenderTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();

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

#endif
