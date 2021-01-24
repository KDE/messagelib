/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/modelinvariantindex.h"
#include "core/modelinvariantindex_p.h"
#include "core/modelinvariantrowmapper.h"
#include "core/modelinvariantrowmapper_p.h"

using namespace MessageList::Core;

ModelInvariantIndex::ModelInvariantIndex()
    : d(new Private)
{
}

ModelInvariantIndex::~ModelInvariantIndex()
{
    if (d->mRowMapper) {
        d->mRowMapper->d->indexDead(this);
    }

    delete d;
}

bool ModelInvariantIndex::isValid() const
{
    return d->mRowMapper != nullptr;
}

int ModelInvariantIndex::currentModelIndexRow()
{
    if (d->mRowMapper) {
        return d->mRowMapper->modelInvariantIndexToModelIndexRow(this);
    }
    return -1;
}
