#include "remotecontroller.h"

remoteController::remoteController(QObject *parent) : QObject(parent)
{
    playingSession = false;

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

    maxPower = 100;
    maxAngle = 45;

    lastCommand = "P 0|0|0|0";
}

remoteController::~remoteController()
{

}

void remoteController::updatePositionJoystick(int xJoystick, int yJoystick)
{
    float tempPosX = xJoystick;
    float tempPosY = yJoystick;

    tempPosX = (int)(((tempPosX-70)/70)*45);
    tempPosY = (int)(((tempPosY-70)/70)*45);

    if(controlMode == 1) {
        tempPosX = (int)((xJoystick > 70) ? (((float)maxAngle/45) * tempPosX) : -(((float)maxAngle/45) * tempPosX));
        tempPosY = (int)((yJoystick > 70) ? (((float)maxAngle/45) * tempPosY) : -(((float)maxAngle/45) * tempPosY));
    }

    if(tempPosX != posX || tempPosY != posY) {
        posX = tempPosX;
        posY = tempPosY;

        sendCommandMotor();

        emit updateInformations("JOYSTICK-X", posX);
        emit updateInformations("JOYSTICK-Y", posY);
    }
}

void remoteController::updatePositionTrottle(int value)
{
    power = qCeil(((float)maxPower / 100) * value);

    sendCommandMotor();
}

void remoteController::updateOrientationDegrees(int value)
{
    degrees = value;

    sendCommandMotor();
}

void remoteController::connectRemote(QString ip, int port)
{
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

    //qDebug() << message;

    analyzeCommand(message);

    emit updateConsole("-DRONE- " + message);
}

void remoteController::connected()
{
    connectionStatut = 2;

    timerData->start(250);

    emit updateStatutConnection("CONNECT");
}

void remoteController::deconnected()
{
    connectionStatut = 0;

    //stopSession();
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
    if(!playingSession) {
        command = "P 0|0|0|0";
    }

    qDebug() << command;

    QString commandType = command.split(" ")[0];

    if(connectionStatut == 2) {
        qDebug() << QString("C " + command + " " + QString::number(randomNumber())).toStdString().c_str();

        if(commandType == "P") {
            socket->write(QByteArray(QString("C " + command + " " + QString::number(randomNumber())).toStdString().c_str()));
        }
        else {
            for(int i = 0; i < 5; i++) {
                socket->write(QByteArray(QString("C " + command + " " + QString::number(randomNumber())).toStdString().c_str()));
            }
        }

        lastCommand = command;
    }
}

int remoteController::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

int remoteController::randomNumber() {
    int rNumber = randInt(0, 9);

    if(rNumber == lastRNumber) {
        if(rNumber == 9) {
            rNumber = 0;
        }
        else {
            rNumber ++;
        }
    }

    lastRNumber = rNumber;

    return rNumber;
}

void remoteController::sendCommandMotor()
{
    sendCommand("P " + QString::number(power) + "|" + QString::number((int)degrees) + "|" + QString::number((int)posX) + "|" + QString::number((int)posY));

    emit updateInformations("THROTTLE", power);
}

void remoteController::sendCalibrate(int lrCalibrate, int fbCalibrate)
{
    leftRightCalibrate = lrCalibrate;
    frontBackCalibrate = fbCalibrate;

    sendCommand("C " + QString::number(leftRightCalibrate) + "|" + QString::number(frontBackCalibrate));
}

bool remoteController::startSession()
{
    if(connectionStatut == 2) {
        playingSession = true;

        loadInformations();
        sendCommand("H N");

        return true;
    }
    else {
        return false;
    }
}

bool remoteController::stopSession()
{
    sendCommand("H Y");

    playingSession = false;

    return true;
}

void remoteController::analyzeCommand(QString command)
{
    //qDebug() << "analyzeCommand";

    QStringList commandSplit = command.split(" ");

    if(commandSplit[0] == "D") {
        QStringList dataMessage = commandSplit[1].split("|");

        //left sonar, right sonar, front sonar, back sonar, up sonar, down sonar, vertical speed, horizontal speed, degrees, battery, pressure, temperature, humidity

        emit updateInformations("LSONAR", dataMessage[0].toInt());
        emit updateInformations("RSONAR", dataMessage[1].toInt());
        emit updateInformations("FSONAR", dataMessage[2].toInt());
        emit updateInformations("BSONAR", dataMessage[3].toInt());
        emit updateInformations("ROLL", (int)dataMessage[4].toFloat());
        emit updateInformations("PITCH", (int)dataMessage[5].toFloat());
        emit updateInformations("VSPEED", (int)dataMessage[6].toFloat());
        emit updateInformations("HSPEED", (int)dataMessage[7].toFloat());
        emit updateInformations("DEGREES", dataMessage[8].toInt());
        emit updateInformations("BATTERY", dataMessage[9].toInt());
        emit updateInformations("PRESSURE", dataMessage[10].toInt());
        emit updateInformations("TEMPERATURE", dataMessage[11].toInt());
        emit updateInformations("HUMIDITY", dataMessage[12].toInt());
    }
    else if(commandSplit[0] == "I") {
        QStringList dataMessage = commandSplit[1].split("|");

        emit updateInformations("MODE", dataMessage[0].toInt());
        emit updateInformations("USESONARS", dataMessage[1].toInt());
        emit updateInformations("AXISSENSIBILITY", dataMessage[2].toInt());
        emit updateInformations("ROTATIONSENSIBILITY", dataMessage[3].toInt());
        emit updateInformations("FLASHINGLED", dataMessage[4].toInt());

        qDebug() << dataMessage;
    }
    else if(commandSplit[0] == "C") {
        qDebug() << "calibrate receive " + commandSplit[1];
    }
}

void remoteController::sendLastCommand()
{
    sendCommand(lastCommand);
}

void remoteController::updateProperties(int nMaxPower, int nMaxAngle)
{
    maxPower = nMaxPower;
    maxAngle = nMaxAngle;
}

void remoteController::loadInformations()
{
    for(int i = 0; i < 5; i++) {
        sendCommand("I Y");
    }
}
