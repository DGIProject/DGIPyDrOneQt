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

    maxPower = 1;
    maxAngle = 1;
    sensibility = 0;

    lastCommand = "P 0|0|0|0";
}

remoteController::~remoteController()
{

}

void remoteController::updatePositionJoystick(int xJoystick, int yJoystick)
{
    //qDebug() << "update position";
    //qDebug() << posJoystick;

    float tempPosX = xJoystick;
    float tempPosY = yJoystick;

    if(tempPosX > 70) {
        tempPosX = (int)(((tempPosX-70)/70)*45);
    }
    else
    {
        tempPosX = (int)(((tempPosX-70)/70)*45);
    }

    if(tempPosY > 70) {
        tempPosY = (int)(((tempPosY-70)/70)*45);
    }
    else
    {
        tempPosY = (int)(((tempPosY-70)/70)*45);
    }

    qDebug() << tempPosX;
    qDebug() << tempPosY;

    //qDebug() << posX;
    //qDebug() << posY;

    if(tempPosX != posX || tempPosY != posY) {
        posX = tempPosX;
        posY = tempPosY;

        sendCommandMotor();

        emit updateInformations("JOYSTICK-X", (int)posX);
        emit updateInformations("JOYSTICK-Y", (int)posY);
    }
}

void remoteController::updatePositionTrottle(int value)
{
    //qDebug() << "update throttle";
    //qDebug() << value;

    power = qCeil(maxPower * value);

    //qDebug() << power;

    sendCommandMotor();
}

void remoteController::updateOrientationDegrees(int value)
{
    degrees = value;

    sendCommandMotor();
}

void remoteController::connectRemote(QString ip, int port)
{
    //qDebug() << ip;
    //qDebug() << port;

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

    timerData->start(500);

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

    //qDebug() << command;

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

        qDebug() << QString("C " + command + " " + QString::number(rNumber)).toStdString().c_str();

        socket->write(QByteArray(QString("C " + command + " " + QString::number(rNumber)).toStdString().c_str()));
    }

    if(command.split(" ")[0] == "P") {
        lastCommand = command;
    }

    //qDebug() << "ok";
    //qDebug() << command.split(" ")[0];

    //lastCommand = command;
}

void remoteController::sendCommandMotor()
{
    //qDebug() << "sendCommandMotor";
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

        return true;
    }
    else {
        return false;
    }
}

bool remoteController::stopSession()
{
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
        emit updateInformations("ISSLEEPING", dataMessage[2].toInt());
        emit updateInformations("ISSTABILIZING", dataMessage[3].toInt());
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

void remoteController::updateValues(float nMaxPower, float nMaxAngle, int nSensibility)
{
    maxPower = nMaxPower;
    maxAngle = nMaxAngle;
    sensibility = nSensibility;
}

void remoteController::loadInformations()
{
    sendCommand("I Y");
}

int remoteController::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}
