#include "remotecontroller.h"

remoteController::remoteController(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(dataReceive()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));

    timerServer = new QTimer(this);
    connect(timerServer, SIGNAL(timeout()), this, SLOT(actualizeConnectionTime()));

    timerData = new QTimer(this);
    connect(timerData, SIGNAL(timeout()), this, SLOT(sendLastCommand()));

    connectionStatut = 0;
    connectionTime = 0;

    sizeMessage = 0;

    power = 0;

    degrees = 0;

    posX = 0;
    posY = 0;

    controlMode = 0;

    leftRightCalibrate = 0;
    frontBackCalibrate = 0;

    maxPower = 1;
    maxAngle = 1;
    sensibility = 0;
}

remoteController::~remoteController()
{

}

void remoteController::updatePositionJoystick(QPointF posJoystick)
{
    qDebug() << "update position";
    qDebug() << posJoystick;

    posX = posJoystick.x();
    posY = posJoystick.y();

    if(posX > 70) {
        posX = ((posX-70)/70)*45;
    }
    else
    {
        posX = ((posX-70)/70)*45;
    }

    if(posY > 70) {
        posY = ((posY-70)/70)*45;
    }
    else
    {
        posY = ((posY-70)/70)*45;
    }

    qDebug() << posX;
    qDebug() << posY;

    sendCommandMotor();

    emit updateText("JOYSTICK-X", (int)posX);
    emit updateText("JOYSTICK-Y", (int)posY);
}

void remoteController::updatePositionTrottle(int value)
{
    qDebug() << "update throttle";
    qDebug() << value;

    power = qCeil(maxPower * value);

    qDebug() << power;

    sendCommandMotor();
}

void remoteController::updateOrientationDegrees(int value)
{
    degrees = value;

    sendCommandMotor();

    emit updateText("DEGREES", (int)degrees);
}

void remoteController::updateLED(bool switchOn)
{
    qDebug() << "update LED";

    if(switchOn) {
        sendCommand("L Y");
    }
    else
    {
        sendCommand("L N");
    }
}

void remoteController::connectRemote(QString ip, int port)
{
    qDebug() << ip;
    qDebug() << port;

    connectionStatut = 1;

    serverIp = ip;
    serverPort = port;

    socket->abort();

    socket->connectToHost(serverIp, serverPort);
    timerServer->start(1000);
}

void remoteController::disconnectRemote()
{
    stopTimer();

    socket->abort();
}

void remoteController::dataReceive()
{

    QString message(socket->readAll());

    qDebug() << message;

    analyzeCommand(message);

    emit updateConsole("-DRONE- " + message);
}

void remoteController::connected()
{
    connectionStatut = 2;

    timerData->start(500);

    emit updateStatutConnection("CONNECT");
}

void remoteController::deconnected()
{
    connectionStatut = 0;

    stopTimer();

    emit updateStatutConnection("DISCONNECT");
}

void remoteController::erreurSocket(QAbstractSocket::SocketError error)
{
    switch(error)
    {
        case QAbstractSocket::HostNotFoundError:
            stopTimer();
            emit updateStatutConnection(tr("<em>ERROR : not found server.</em>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            connectionStatut = 3;
            emit updateStatutConnection(tr("<em>ERROR : connection refused.</em>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            connectionStatut = 3;
            emit updateStatutConnection(tr("<em>ERROR : cuted connection.</em>"));
            break;
        default:
            stopTimer();
            emit updateStatutConnection(tr("<em>ERROR : ") + socket->errorString() + tr("</em>"));
    }
}

void remoteController::actualizeConnectionTime()
{
    connectionTime++;

    if(connectionStatut == 3) {
        connectRemote(serverIp, serverPort);
    }

    emit updateConnectionTime(connectionTime);
}

void remoteController::stopTimer()
{
    timerServer->stop();
    timerData->stop();

    connectionTime = 0;

    emit updateConnectionTime(connectionTime);
}

void remoteController::sendCommand(QString command)
{
    qDebug() << command;

    //analyzeCommand(command);

    if(connectionStatut == 2) {
        rNumber = randInt(0, 9);

        if(rNumber == lastRNumber) {
            if(rNumber == 9) {
                rNumber = 0;
            }
            else {
                rNumber ++;
            }
        }

        lastRNumber = rNumber;

        socket->write(QByteArray(QString("C " + command + " " + QString::number(rNumber)).toStdString().c_str()));
        
        //socket->write(QByteArray(QString("C " + command).toStdString().c_str()));
    }

    lastCommand = command;
}

void remoteController::sendCommandMotor()
{
    qDebug() << "sendCommandMotor";

    /*
    int motor1, motor2, motor3, motor4;
    motor1 = motor2 = motor3 = motor4 = power;

    if(posX < 0) {
        motor1 -= maxAngle * ((-posX/45) * 10);
        motor3 = motor1;
    }

    if(posX > 0) {
        motor2 -= maxAngle * ((posX/45) * 10);
        motor4 = motor2;
    }

    if(posY < 0) {
        motor1 -= maxAngle * ((-posY/45) * 10);
        motor2 = motor1;
    }

    if(posY > 0) {
        motor3 -= maxAngle * ((posY/45) * 10);
        motor4 = motor3;
    }

    motor1 = (motor1 < 0) ? 0 : motor1;
    motor2 = (motor2 < 0) ? 0 : motor2;
    motor3 = (motor3 < 0) ? 0 : motor3;
    motor4 = (motor4 < 0) ? 0 : motor4;
    */

    //sendCommand("P " + QString::number(power) + "|" + QString::number(power) + "|" + QString::number(power) + "|" + QString::number(power) +  "|" + QString::number((int)degrees) + "|" + QString::number((int)posX) + "|" + QString::number((int)posY));

    sendCommand("P " + QString::number(power) + "|" + QString::number((int)degrees) + "|" + QString::number((int)posX) + "|" + QString::number((int)posY));

    emit updateText("THROTTLE", power);
}

void remoteController::sendCalibrate(int lrCalibrate, int fbCalibrate)
{
    leftRightCalibrate = lrCalibrate;
    frontBackCalibrate = fbCalibrate;

    sendCommand("C " + QString::number(leftRightCalibrate) + "|" + QString::number(frontBackCalibrate));
}

void remoteController::analyzeCommand(QString command)
{
    //qDebug() << "analyzeCommand";

    QStringList commandSplit = command.split(" ");

    if(commandSplit[0] == "D") {
        QStringList dataMessage = commandSplit[1].split("|");

        //left sonar, right sonar, front sonar, back sonar, up sonar, down sonar, vertical speed, horizontal speed, degrees, battery, pressure, temperature, humidity

        emit updateText("LSONAR", dataMessage[0].toInt());
        emit updateText("RSONAR", dataMessage[1].toInt());
        emit updateText("FSONAR", dataMessage[2].toInt());
        emit updateText("BSONAR", dataMessage[3].toInt());
        emit updateText("ROLL", (int)dataMessage[4].toFloat());
        emit updateText("PITCH", (int)dataMessage[5].toFloat());
        emit updateText("VSPEED", (int)dataMessage[6].toFloat());
        emit updateText("HSPEED", (int)dataMessage[7].toFloat());
        emit updateText("DEGREES", dataMessage[8].toInt());
        emit updateText("BATTERY", dataMessage[9].toInt());
        emit updateText("PRESSURE", dataMessage[10].toInt());
        emit updateText("TEMPERATURE", dataMessage[11].toInt());
        emit updateText("HUMIDITY", dataMessage[12].toInt());
    }
    else if(commandSplit[0] == "C") {
        qDebug() << "calibrate receive " + commandSplit[1];
    }
}

void remoteController::sendLastCommand()
{
    sendCommand(lastCommand);
}

void remoteController::updateValues(float nMaxPower, float nMaxAngle, int nSensibility)
{
    maxPower = nMaxPower;
    maxAngle = nMaxAngle;
    sensibility = nSensibility;
}

int remoteController::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}
