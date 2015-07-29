#ifndef REMOTECONTROLLER_H
#define REMOTECONTROLLER_H

#include <QObject>
#include <QPointF>
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>
#include <QtMath>
#include <QByteArray>
#include "qcgaugewidget.h"

class remoteController : public QObject
{
    Q_OBJECT
public:
    explicit remoteController(QObject *parent = 0);
    ~remoteController();

    void updatePositionJoystick(QPointF);
    void updatePositionTrottle(int);
    void updateOrientationDegrees(int);
    void updateLED(bool);

    void connectRemote(QString, int);
    void disconnectRemote();
    void sendCommand(QString);

    void updateValues(float, float, int);

private:
    QTcpSocket *socket;

    QString serverIp;
    int serverPort;

    QTimer *timerServer;
    QTimer *timerData;

    int connectionStatut;
    int connectionTime;

    quint16 sizeMessage;

    float posX;
    float posY;

    int power;

    float degrees;

    float maxPower;
    float maxAngle;
    int sensibility;

<<<<<<< HEAD
    QString lastCommand;

    int rNumber;
    int lastRNumber;

=======
>>>>>>> origin/master
    void analyzeCommand(QString);

    void stopTimer();

    int randInt(int, int);

private slots:
    void dataReceive();
    void connected();
    void deconnected();
    void erreurSocket(QAbstractSocket::SocketError error);

    void actualizeConnectionTime();
    void sendCommandMotor();

signals:
    void updateStatutConnection(QString);
    void updateConsole(QString);
    void updateConnectionTime(int);
    void updateText(QString, int);
};

#endif // REMOTECONTROLLER_H
