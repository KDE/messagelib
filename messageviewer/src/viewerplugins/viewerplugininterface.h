/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <Akonadi/Item>
#include <KMime/Message>
#include <QObject>

class QAction;
class KActionCollection;
namespace MessageViewer
{
class ViewerPluginInterfacePrivate;
/**
 * @brief The ViewerPluginInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ViewerPluginInterface : public QObject
{
    Q_OBJECT
public:
    explicit ViewerPluginInterface(QObject *parent = nullptr);
    ~ViewerPluginInterface() override;
    enum SpecificFeatureType { None = 0, NeedSelection = 2, NeedMessage = 4, NeedUrl = 8, All = 16 };
    Q_FLAGS(SpecificFeatureTypes)
    Q_DECLARE_FLAGS(SpecificFeatureTypes, SpecificFeatureType)

    virtual void execute();

    virtual void setText(const QString &text);
    virtual QList<QAction *> actions() const;
    virtual void setUrl(const QUrl &url);
    virtual void setMessage(const KMime::Message::Ptr &value);
    virtual void setMessageItem(const Akonadi::Item &item);
    virtual void setCurrentCollection(const Akonadi::Collection &col);
    virtual void closePlugin();
    virtual ViewerPluginInterface::SpecificFeatureTypes featureTypes() const = 0;
    virtual void updateAction(const Akonadi::Item &item);
    virtual void refreshActionList(KActionCollection *ac);

protected:
    virtual void showWidget();
    void addHelpTextAction(QAction *act, const QString &text);

protected Q_SLOTS:
    void slotActivatePlugin();

Q_SIGNALS:
    void activatePlugin(MessageViewer::ViewerPluginInterface *);

private:
    std::unique_ptr<ViewerPluginInterfacePrivate> const d;
};
}
