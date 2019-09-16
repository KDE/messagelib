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

#include "composerattachmentinterface.h"
using namespace MessageComposer;
ComposerAttachmentInterface::ComposerAttachmentInterface()
{

}

ComposerAttachmentInterface::~ComposerAttachmentInterface()
{

}

QStringList ComposerAttachmentInterface::fileNames() const
{
    return mFileNames;
}

void ComposerAttachmentInterface::setFileNames(const QStringList &fileName)
{
    mFileNames = fileName;
}

int ComposerAttachmentInterface::count() const
{
    return mCount;
}

void ComposerAttachmentInterface::setCount(int count)
{
    mCount = count;
}

QStringList ComposerAttachmentInterface::namesAndSize() const
{
    return mNamesAndSize;
}

void ComposerAttachmentInterface::setNamesAndSize(const QStringList &nameAndSize)
{
    mNamesAndSize = nameAndSize;
}

QStringList ComposerAttachmentInterface::names() const
{
    return mNames;
}

void ComposerAttachmentInterface::setNames(const QStringList &name)
{
    mNames = name;
}
