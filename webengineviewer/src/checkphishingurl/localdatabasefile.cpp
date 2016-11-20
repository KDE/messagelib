/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "localdatabasefile.h"

using namespace WebEngineViewer;

LocalDataBaseFile::LocalDataBaseFile(const QString &filename)
    : mFile(filename),
      mValid(false)
{
    load();
}

LocalDataBaseFile::~LocalDataBaseFile()
{

}

bool LocalDataBaseFile::load()
{
    if (!mFile.open(QIODevice::ReadOnly))
        return false;
    mData = mFile.map(0, mFile.size());
    if (mData) {
        const int major = getUint16(0);
        const int minor = getUint16(2);
        mValid = (major == 1 && minor >= 1 && minor <= 2);
    }
    mMtime = QFileInfo(mFile).lastModified();
    return mValid;
}

bool LocalDataBaseFile::reload()
{
    mValid = false;
    if (mFile.isOpen()) {
        mFile.close();
    }
    mData = 0;
    return load();
}
