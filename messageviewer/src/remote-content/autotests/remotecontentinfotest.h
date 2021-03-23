#pragma once

#include <QObject>

class RemoteContentInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteContentInfoTest(QObject *parent = nullptr);
    ~RemoteContentInfoTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldVerifyIsValid();
};

