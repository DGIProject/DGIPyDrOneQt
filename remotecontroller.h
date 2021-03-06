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

    void updatePositionJoystick(int, int);
    void updatePositionTrottle(int);
    void updateOrientationDegrees(int);
    void updateControlMode(int);
    int gControlMode();

    void connectRemote(QString, int);
    void disconnectRemote();

    void sendCommand(QString);
    void sendDirectCommand(QString);

    void updateProperties(int, int);

    void sendCalibrate(int, int, int, int);

    bool startSession();
    bool stopSession();
    bool isPlayingSession();

    int connectionStatut;

private:
    bool playingSession;

    QTcpSocket *socket;

    QString serverIp;
    int serverPort;

    QTimer *timerServer;
    QTimer *timerData;

    int connectionTime;

    quint16 sizeMessage;

    int controlMode;

    int power;

    int degrees;

    int posX;
    int posY;

    int leftRightCalibrate;
    int frontBackCalibrate;

    int maxPower;
    int maxAngle;

    QString lastCommand;

    int lastRNumber;

    void analyzeCommand(QString);
    void stopTimer();

    int randInt(int, int);
    int randomNumber();

private slots:
    void dataReceive();
    void connected();
    void deconnected();
    void erreurSocket(QAbstractSocket::SocketError error);

    void actualizeConnectionTime();
    void sendCommandMotor();
    void sendCommandDrone();

signals:
    void updateStatutConnection(QString);
    void updateConsole(QString, QString);
    void updateConnectionTime(int);
    void updateInformations(QString, int);
};

#endif // REMOTECONTROLLER_H
