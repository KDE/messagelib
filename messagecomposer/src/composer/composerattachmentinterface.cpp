/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
