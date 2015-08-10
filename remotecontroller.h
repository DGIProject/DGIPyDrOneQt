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

    void connectRemote(QString, int);
    void disconnectRemote();
    void sendCommand(QString);

    void updateValues(float, float, int);
    void loadInformations();

    void sendCalibrate(int, int);

    bool startSession();
    bool stopSession();

private:
    bool playingSession;

    QTcpSocket *socket;

    QString serverIp;
    int serverPort;

    QTimer *timerServer;
    QTimer *timerData;

    int connectionStatut;
    int connectionTime;

    quint16 sizeMessage;

    int power;

    float degrees;

    float posX;
    float posY;

    int controlMode;

    int leftRightCalibrate;
    int frontBackCalibrate;

    float maxPower;
    float maxAngle;
    int sensibility;

    QString lastCommand;

    int rNumber;
    int lastRNumber;

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
    void sendLastCommand();

signals:
    void updateStatutConnection(QString);
    void updateConsole(QString);
    void updateConnectionTime(int);
    void updateInformations(QString, int);
};

#endif // REMOTECONTROLLER_H
