/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "pluginattachmentinterface.h"
using namespace MessageComposer;
PluginAttachmentInterface::PluginAttachmentInterface()
{

}

PluginAttachmentInterface::~PluginAttachmentInterface()
{

}

QStringList PluginAttachmentInterface::fileNames() const
{
    return mFileNames;
}

void PluginAttachmentInterface::setFileNames(const QStringList &fileName)
{
    mFileNames = fileName;
}

int PluginAttachmentInterface::count() const
{
    return mCount;
}

void PluginAttachmentInterface::setCount(int count)
{
    mCount = count;
}

QStringList PluginAttachmentInterface::namesAndSize() const
{
    return mNamesAndSize;
}

void PluginAttachmentInterface::setNamesAndSize(const QStringList &nameAndSize)
{
    mNamesAndSize = nameAndSize;
}

QStringList PluginAttachmentInterface::names() const
{
    return mNames;
}

void PluginAttachmentInterface::setNames(const QStringList &name)
{
    mNames = name;
}
