#ifndef REMOTECONTROLLER_H
#define REMOTECONTROLLER_H

#include <QObject>
#include <QPointF>
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>
#include <QtMath>
#include <QByteArray>

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

    void sendCalibrate(int, int);

private:
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

    void analyzeCommand(QString);
    void stopTimer();

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
    void updateText(QString, int);
};

#endif // REMOTECONTROLLER_H
