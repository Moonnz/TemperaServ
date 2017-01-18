#ifndef FENCLIENT_H
#define FENCLIENT_H

#include <QObject>
#include <QWidget>

class FenClient : public QObject
{
    Q_OBJECT
public:
    explicit FenClient(QObject *parent = 0);

signals:

public slots:
};

#endif // FENCLIENT_H