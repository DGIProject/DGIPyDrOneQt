#include "dgipydrone.h"
#include "ui_dgipydrone.h"

DGIpydrOne::DGIpydrOne(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DGIpydrOne)
{
    ui->setupUi(this);

    ui->connectBeforeWidget->show();

    ui->buttonCancelConnect->setEnabled(false);

    mCompassGauge = new QcGaugeWidget;
    mCompassGauge->addBackground(99);
    QcBackgroundItem *bkg1 = mCompassGauge->addBackground(92);
    bkg1->clearrColors();
    bkg1->addColor(0.1,Qt::black);
    bkg1->addColor(1.0,Qt::white);

    QcBackgroundItem *bkg2 = mCompassGauge->addBackground(88);
    bkg2->clearrColors();
    bkg2->addColor(0.1,Qt::white);
    bkg2->addColor(1.0,Qt::black);

    QcLabelItem *w = mCompassGauge->addLabel(80);
    w->setText("W");
    w->setAngle(270);
    w->setColor(Qt::white);

    QcLabelItem *n = mCompassGauge->addLabel(80);
    n->setText("N");
    n->setAngle(0);
    n->setColor(Qt::white);

    QcLabelItem *e = mCompassGauge->addLabel(80);
    e->setText("E");
    e->setAngle(90);
    e->setColor(Qt::white);

    QcLabelItem *s = mCompassGauge->addLabel(80);
    s->setText("S");
    s->setAngle(180);
    s->setColor(Qt::white);

    QcDegreesItem *deg = mCompassGauge->addDegrees(70);
    deg->setStep(5);
    deg->setMaxDegree(270);
    deg->setMinDegree(-75);
    deg->setColor(Qt::white);

    mCompassNeedle2 = mCompassGauge->addNeedle(60);
    mCompassNeedle2->setNeedle(QcNeedleItem::DiamonNeedle);
    mCompassNeedle2->setValueRange(0,360);
    mCompassNeedle2->setMaxDegree(360);
    mCompassNeedle2->setMinDegree(0);
    mCompassNeedle2->setCurrentValue(150);

    mCompassNeedle = mCompassGauge->addNeedle(60);
    mCompassNeedle->setNeedle(QcNeedleItem::CompassNeedle);
    mCompassNeedle->setValueRange(0,360);
    mCompassNeedle->setMaxDegree(360);
    mCompassNeedle->setMinDegree(0);
    mCompassGauge->addBackground(7);
    mCompassGauge->addGlass(88);
    ui->verticalLayout->addWidget(mCompassGauge);

    mAttitudeGauge = new QcGaugeWidget;
    mAttitudeGauge->addBackground(99);
    QcBackgroundItem *bkg = mAttitudeGauge->addBackground(92);
    bkg->clearrColors();
    bkg->addColor(0.1,Qt::black);
    bkg->addColor(1.0,Qt::white);
    mAttMeter = mAttitudeGauge->addAttitudeMeter(88);

    mAttitudeNeedle = mAttitudeGauge->addNeedle(70);
    mAttitudeNeedle->setMinDegree(0);
    mAttitudeNeedle->setMaxDegree(180);
    mAttitudeNeedle->setValueRange(0,180);
    mAttitudeNeedle->setCurrentValue(90);
    mAttitudeNeedle->setColor(Qt::white);
    mAttitudeNeedle->setNeedle(QcNeedleItem::AttitudeMeterNeedle);
    mAttitudeGauge->addGlass(80);
    ui->verticalLayout2->addWidget(mAttitudeGauge);

    controller = new remoteController();

    connect(controller, SIGNAL(updateStatutConnection(QString)), this, SLOT(statutConnection(QString)));
    connect(controller, SIGNAL(updateConsole(QString)), this, SLOT(updateConsole(QString)));
    connect(controller, SIGNAL(updateConnectionTime(int)), this, SLOT(updateConnectionTime(int)));
    connect(controller, SIGNAL(updateText(QString, int)), this, SLOT(updateTextInformation(QString, int)));

    ui->joystickView->setGeometry(10, 80, 250, 250);

    sceneJoystick = new QGraphicsScene();
    sceneJoystick->setSceneRect(0,0, ui->joystickView->width() - 10, ui->joystickView->height() - 10);
    ui->joystickView->setScene(sceneJoystick);

    joystick = new joystickRect(controller);

    sceneJoystick->addItem(joystick);

    vLeftSonar = vRightSonar = vFrontSonar = vBackSonar = vUpSonar = vBackSonar = vDegrees = vVerticalSpeed = vHorizontalSpeed = vPressure = vTemperature = vHumidity = 0;

    drawDroneInformations();
}

DGIpydrOne::~DGIpydrOne()
{
    controller->disconnectRemote();

    delete ui;
}

void DGIpydrOne::on_buttonConnect_clicked()
{
    updateConsole(tr("<em>Connecting ...</em>"));

    ui->buttonConnect->setEnabled(false);
    ui->buttonCancelConnect->setEnabled(true);

    controller->connectRemote(ui->serverIp->text(), ui->serverPort->value());
}

void DGIpydrOne::on_buttonCancelConnect_clicked()
{
    ui->buttonConnect->setEnabled(true);
    controller->disconnectRemote();

    updateConsole(tr("<em>Canceled connection.</em>"));
}

void DGIpydrOne::on_buttonSendCommand_clicked()
{
    if(ui->commandText->text().length() > 0) {
        controller->sendCommand(ui->commandText->text());
        updateConsole("-ME- " + ui->commandText->text());

        ui->commandText->clear();
        ui->commandText->setFocus();
    }
}

void DGIpydrOne::statutConnection(QString statut)
{
    qDebug() << statut;

    if(statut == "CONNECT") {
        updateConsole(tr("<em>Connected</em>"));

        ui->connectBeforeWidget->hide();
    }
    else if(statut == "DISCONNECT") {
        updateConsole(tr("<em>Disconnected</em>"));

        ui->buttonConnect->setEnabled(true);
        ui->buttonCancelConnect->setEnabled(false);

        ui->connectBeforeWidget->show();
    }
    else
    {
        updateConsole(statut);

        ui->buttonConnect->setEnabled(true);
    }
}

void DGIpydrOne::updateConsole(QString text)
{
    ui->console->append(text);

    QScrollBar *sb = ui->console->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void DGIpydrOne::updateConnectionTime(int time)
{
    ui->connectionTime->setText(QString::number(time) + "s");
}

void DGIpydrOne::updateTextInformation(QString type, int value)
{
    qDebug() << type;

    QStringList list;
    list << "JOYSTICK-X" << "JOYSTICK-Y" << "THROTTLE" << "LSONAR" << "RSONAR" << "FSONAR" << "BSONAR" << "USONAR" << "DSONAR" << "DEGREES" << "VSPEED" << "HSPEED" << "BATTERY" << "PRESSURE" << "TEMPERATURE" << "HUMIDITY";

    switch (list.indexOf(type)) {
    case 0:
        ui->joystickXInformation->setText("Joystick X : " + QString::number(value) + "°");
        break;
    case 1:
        ui->joystickYInformation->setText("Joystick Y : " + QString::number(value) + "°");
        break;
    case 2:
        ui->throttleLabel->setText(QString::number(value) + "%");
        break;
    case 6:
        vLeftSonar = value;
        break;
    case 7:
        vRightSonar = value;
        break;
    case 8:
        vFrontSonar = value;
        break;
    case 9:
        vBackSonar = value;
        break;
    case 10:
        vUpSonar = value;
        break;
    case 11:
        vDownSonar = value;
        break;
    case 12:
        vDegrees = value;
        ui->degreesLabel->setText(QString::number(value) + "°");
        mCompassNeedle->setCurrentValue(vDegrees);
        break;
    case 13:
        vVerticalSpeed = value;
        break;
    case 14:
        vHorizontalSpeed = value;
        break;
    case 15:
        ui->batteryLevelBar->setValue((int)(((float)value / 12) * 100));
        break;
    case 16:
        vHumidity = value;
        break;
    case 17:
        vTemperature = value;
        break;
    case 18:
        vHumidity = value;
        break;
    default:
        qDebug() << "error type";
        break;
    }

    //drawDroneInformations();
}

void DGIpydrOne::on_throttleSlider_valueChanged(int value)
{
    controller->updatePositionTrottle(value);
}

void DGIpydrOne::on_buttonSaveProfile_clicked()
{
    ProfileEditor *profile = new ProfileEditor();
    profile->saveProfile("test", 0, 0, 0);
}

void DGIpydrOne::on_checkLED_clicked()
{
    if(ui->checkLED->isChecked()) {
        controller->updateLED(true);
    }
    else
    {
        controller->updateLED(false);
    }
}

void DGIpydrOne::drawDroneInformations()
{
    sceneSonar = new QGraphicsScene();
    sceneSonar->setSceneRect(0, 0, ui->sonarView->width() - 10, ui->sonarView->height() - 10);

    QPixmap droneImage(":/drone.jpg");

    QGraphicsItem *droneItem = sceneSonar->addPixmap(droneImage);
    droneItem->setPos((sceneSonar->width() / 2) - (droneImage.width() / 2), (sceneSonar->height() / 2) - (droneImage.height() / 2));

    leftSonar = sceneSonar->addPixmap(QPixmap(":/line.jpg"));
    leftSonar->setRotation(90);
    leftSonar->setPos((vLeftSonar / 30) * 82, 5);

    rightSonar = sceneSonar->addPixmap(QPixmap(":/line.jpg"));
    rightSonar->setRotation(90);
    rightSonar->setPos(274 - ((vRightSonar / 30) * 82), 5);

    frontSonar = sceneSonar->addPixmap(QPixmap(":/line.jpg"));
    frontSonar->setPos(5, -10 + ((vFrontSonar / 30) * 82));

    backSonar = sceneSonar->addPixmap(QPixmap(":/line.jpg"));
    backSonar->setPos(5, 262 - ((vBackSonar / 30) * 82));

    //upSonar = sceneSonar->addPixmap(QPixmap(":/line.jpg"));
    //upSonar->setPos(5, 0);

    //downSonar = sceneSonar->addPixmap(QPixmap(":/line.jpg"));
    //downSonar->setPos(5, 200);

    QFont fontTextItem;
    fontTextItem.setPixelSize(15);
    fontTextItem.setBold(false);
    fontTextItem.setFamily("Calibri");

    QGraphicsTextItem *leftSonarText = new QGraphicsTextItem();
    leftSonarText->setPos(15, sceneSonar->height() / 2);
    leftSonarText->setFont(fontTextItem);
    leftSonarText->setPlainText(QString::number(vLeftSonar) + "cm");

    QGraphicsTextItem *rightSonarText = new QGraphicsTextItem();
    rightSonarText->setPos(sceneSonar->width() - 45, sceneSonar->height() / 2);
    rightSonarText->setFont(fontTextItem);
    rightSonarText->setPlainText(QString::number(vRightSonar) + "cm");

    QGraphicsTextItem *frontSonarText = new QGraphicsTextItem();
    frontSonarText->setPos((sceneSonar->width() / 2) - 10, 45);
    frontSonarText->setFont(fontTextItem);
    frontSonarText->setPlainText(QString::number(vFrontSonar) + "cm");

    QGraphicsTextItem *backSonarText = new QGraphicsTextItem();
    backSonarText->setPos((sceneSonar->width() / 2) - 10, sceneSonar->height() - 45);
    backSonarText->setFont(fontTextItem);
    backSonarText->setPlainText(((vBackSonar == 0) ? "∞" : QString::number(vBackSonar)) + "cm");

    QGraphicsTextItem *pressureText = new QGraphicsTextItem();
    pressureText->setPos(0, 15);
    pressureText->setFont(fontTextItem);
    pressureText->setPlainText("P : " + QString::number(vPressure) + "Pa");

    sceneSonar->addItem(leftSonarText);
    sceneSonar->addItem(rightSonarText);
    sceneSonar->addItem(frontSonarText);
    sceneSonar->addItem(backSonarText);
    sceneSonar->addItem(pressureText);

    ui->sonarView->setScene(sceneSonar);
}

void DGIpydrOne::on_leftRightCalibrate_valueChanged(int value)
{
    qDebug() << value;

    controller->sendCalibrate(value, ui->frontBackCalibrate->value());
}

void DGIpydrOne::on_frontBackCalibrate_valueChanged(int value)
{
    qDebug() << value;

    controller->sendCalibrate(ui->leftRightCalibrate->value(), value);
}

void DGIpydrOne::on_checkCollision_clicked()
{
    if(ui->checkCollision->isChecked()) {
        controller->sendCommand("S Y");
    }
    else {
        controller->sendCommand("S N");
    }
}

void DGIpydrOne::on_manualMode_clicked()
{
    if(ui->manualMode->isChecked()) {
        controller->sendCommand("M 0");
    }
}

void DGIpydrOne::on_automaticMode_clicked()
{
    if(ui->automaticMode->isChecked()) {
        controller->sendCommand("M 1");
    }
}

void DGIpydrOne::on_buttonCalibrateDrone_clicked()
{
    controller->sendCommand("A Y");
}

void DGIpydrOne::on_buttonLessCompass_clicked()
{
    mCompassNeedle2->setCurrentValue(mCompassNeedle2->currentValue()-1);
    controller->updateOrientationDegrees(mCompassNeedle2->currentValue());
    qDebug() << mCompassNeedle2->position();
}

void DGIpydrOne::on_buttonMoreCompass_clicked()
{
    mCompassNeedle2->setCurrentValue(mCompassNeedle2->currentValue()+1);
    controller->updateOrientationDegrees(mCompassNeedle2->currentValue());
}
