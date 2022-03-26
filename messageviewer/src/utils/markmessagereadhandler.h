/*
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    SPDX-FileContributor: Tobias Koenig <tokoe@kdab.com>


    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include "messageviewer/viewer.h"

#include <QObject>

namespace MessageViewer
{
/**
 * @short A viewer handler to mark viewed messages as read.
 */
class MESSAGEVIEWER_EXPORT MarkMessageReadHandler : public QObject, public AbstractMessageLoadedHandler
{
    Q_OBJECT

public:
    /**
     * Creates a new mark message read handler.
     *
     * @param parent The parent object.
     */
    explicit MarkMessageReadHandler(QObject *parent = nullptr);

    /**
     * Destroys the mark message read handler.
     */
    ~MarkMessageReadHandler() override;

    /**
     * @copydoc AbstractMessageLoadedHandler::setItem()
     */
    void setItem(const Akonadi::Item &item) override;

private:
    //@cond PRIVATE
    class MarkMessageReadHandlerPrivate;
    friend class MarkMessageReadHandlerPrivate;
    std::unique_ptr<MarkMessageReadHandlerPrivate> const d;
    //@endcond
};
}
