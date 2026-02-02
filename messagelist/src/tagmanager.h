/*
    SPDX-FileCopyrightText: 2025-2026 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_export.h"
#include <Akonadi/Tag>
#include <QMap>
#include <QObject>
namespace Akonadi
{
class Monitor;
}
class KJob;
namespace MessageList
{
namespace Core
{
/*!
 * \class MessageList::Core::TagManager
 * \inmodule MessageList
 * \inheaderfile MessageList/TagManager
 *
 * \brief The TagManager class
 */
class MESSAGELIST_EXPORT TagManager : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor.
     * \param parent The parent object.
     */
    explicit TagManager(QObject *parent = nullptr);
    /*!
     * \brief Destructor.
     */
    ~TagManager() override;

    /*!
     * \brief Gets the singleton instance of TagManager.
     * \return Pointer to the TagManager singleton.
     */
    static TagManager *self();

    /*!
     * \brief Gets the tag map.
     * \return Map of tag identifiers to tag names.
     */
    [[nodiscard]] QMap<QString, QString> mapTag() const;
    /*!
     * \brief Sets the tag map.
     * \param newMapTag The new tag map to set.
     */
    void setMapTag(const QMap<QString, QString> &newMapTag);

    /*!
     */
    [[nodiscard]] QString tagFromName(const QString &name) const;

    /*!
     */
    void slotTagsChanged();
Q_SIGNALS:
    /*!
     */
    void tagsFetched(const Akonadi::Tag::List &lst);

private:
    void init();
    void slotTagsFetched(KJob *job);
    Akonadi::Monitor *const mMonitor;
    QMap<QString, QString> mMapTag;
};
}
}
