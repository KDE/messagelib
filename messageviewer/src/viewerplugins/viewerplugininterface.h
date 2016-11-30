/*
   Copyright (C) 2015-2016 Laurent Montel <montel@kde.org>

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

#ifndef VIEWERPLUGININTERFACE_H
#define VIEWERPLUGININTERFACE_H

#include <QObject>
#include "messageviewer_export.h"
#include <KMime/Message>
#include <AkonadiCore/Item>

class QAction;
class KActionCollection;
namespace MessageViewer
{
class ViewerPluginInterfacePrivate;
class MESSAGEVIEWER_EXPORT ViewerPluginInterface : public QObject
{
    Q_OBJECT
public:
    explicit ViewerPluginInterface(QObject *parent = Q_NULLPTR);
    ~ViewerPluginInterface();
    enum SpecificFeatureType {
        None = 0,
        NeedSelection = 2,
        NeedMessage = 4,
        NeedUrl = 8,
        All = 16
    };
    Q_FLAGS(SpecificFeatureTypes)
    Q_DECLARE_FLAGS(SpecificFeatureTypes, SpecificFeatureType)

    virtual void execute();

    virtual void setText(const QString &text);
    virtual QList<QAction *> actions() const;
    virtual void setUrl(const QUrl &url);
    virtual void setMessage(const KMime::Message::Ptr &value);
    virtual void setMessageItem(const Akonadi::Item &item);
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
    ViewerPluginInterfacePrivate *const d;
};
}

#endif // VIEWERPLUGININTERFACE_H
