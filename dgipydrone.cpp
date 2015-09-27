#include "dgipydrone.h"
#include "ui_dgipydrone.h"

DGIpydrOne::DGIpydrOne(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DGIpydrOne)
{
    ui->setupUi(this);

    bool f_haveJoystick = input.initInput(0);

    if (f_haveJoystick)
    {
       ui->checkGamepad->setChecked(false);
       tmr.setInterval(15);
       connect(&tmr,SIGNAL(timeout()),this,SLOT(readJoystickState()));
       tmr.start();
    }

    useJoystick = false;
    calibrateJoystick = false;

    ui->sonarView->hide();

    ui->profileLoaded->setText(ui->profileName->text());

    ui->connectBeforeWidget->hide();
    ui->connectBefore_optionsWidget->show();

    ui->buttonCancelConnect->setEnabled(false);

    ui->buttonStartSession->setText("Start session");
    ui->buttonStartSession->setStyleSheet("background-color: rgb(11, 190, 68);");

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
    w->setAngle(0);
    w->setColor(Qt::white);

    QcLabelItem *n = mCompassGauge->addLabel(80);
    n->setText("N");
    n->setAngle(90);
    n->setColor(Qt::white);

    QcLabelItem *e = mCompassGauge->addLabel(80);
    e->setText("E");
    e->setAngle(180);
    e->setColor(Qt::white);

    QcLabelItem *s = mCompassGauge->addLabel(80);
    s->setText("S");
    s->setAngle(270);
    s->setColor(Qt::white);

    QcDegreesItem *deg = mCompassGauge->addDegrees(70);
    deg->setStep(5);
    deg->setMaxDegree(359);
    deg->setMinDegree(0);
    deg->setColor(Qt::white);

    mCompassNeedle2 = mCompassGauge->addNeedle(60);
    mCompassNeedle2->setNeedle(QcNeedleItem::DiamonNeedle);
    mCompassNeedle2->setValueRange(0,360);
    mCompassNeedle2->setMaxDegree(360);
    mCompassNeedle2->setMinDegree(0);
    mCompassNeedle2->setCurrentValue(90);


    mCompassNeedle = mCompassGauge->addNeedle(60);
    mCompassNeedle->setNeedle(QcNeedleItem::CompassNeedle);
    mCompassNeedle->setValueRange(0,360);
    mCompassNeedle->setMaxDegree(360);
    mCompassNeedle->setMinDegree(0);
    mCompassNeedle->setCurrentValue(90);
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
    connect(controller, SIGNAL(updateConsole(QString, QString)), this, SLOT(updateConsole(QString, QString)));
    connect(controller, SIGNAL(updateConnectionTime(int)), this, SLOT(updateConnectionTime(int)));
    connect(controller, SIGNAL(updateInformations(QString,int)), this, SLOT(updateInformationsInterface(QString, int)));

    ui->joystickView->setGeometry(10, 160, 250, 250);

    sceneJoystick = new QGraphicsScene();
    sceneJoystick->setSceneRect(0,0, ui->joystickView->width() - 10, ui->joystickView->height() - 10);
    ui->joystickView->setScene(sceneJoystick);

    joystick = new joystickRect(controller);

    sceneJoystick->addItem(joystick);

    firstTimeConnection = false;

    motorCalibrate = 0;

    vLeftSonar = vRightSonar = vFrontSonar = vBackSonar = vUpSonar = vBackSonar = vDegrees = vVerticalSpeed = vHorizontalSpeed = vPressure = 0;

    drawDroneInformations();
}

DGIpydrOne::~DGIpydrOne()
{
    controller->disconnectRemote();

    delete ui;
}

void DGIpydrOne::on_buttonConnect_clicked()
{
    updateConsole("RECEIVE", tr("<em>Connecting ...</em>"));

    ui->buttonFastConnect->setText("Connecting ...");

    ui->buttonConnect->setEnabled(false);
    ui->buttonCancelConnect->setEnabled(true);

    controller->connectRemote(ui->serverIp->text(), ui->serverPort->value());
}

void DGIpydrOne::on_buttonCancelConnect_clicked()
{
    ui->buttonConnect->setEnabled(true);
    controller->disconnectRemote();

    updateConsole("RECEIVE", tr("<em>Canceled connection.</em>"));
}

void DGIpydrOne::on_buttonSendCommand_clicked()
{
    if(ui->commandText->text().length() > 0) {
        controller->sendCommand(ui->commandText->text());
        updateConsole("SEND", ("-ME- " + ui->commandText->text()));

        ui->commandText->clear();
        ui->commandText->setFocus();
    }
}

void DGIpydrOne::readJoystickState()
{
    sf::Joystick::update();

    //gamepad button
    if (sf::Joystick::isButtonPressed(0, 1))
    {
        if(!pressedButtons[1]) {
            ui->checkGamepad->setChecked(ui->checkGamepad->isChecked() ? false : true);
            useJoystick = !useJoystick;
        }

        pressedButtons[1] = true;
    }
    else {
        pressedButtons[1] = false;
    }

    if(useJoystick) {
        //still connected joystick
        if(sf::Joystick::getAxisPosition(0, sf::Joystick::X) == 0) {
            useJoystick = false;

            ui->checkGamepad->setChecked(false);

            return;
        }

        //start session button
        if (sf::Joystick::isButtonPressed(0, 0))
        {
            if(!pressedButtons[0]) {
                ui->buttonStartSession->click();
            }

            pressedButtons[0] = true;
        }
        else {
            pressedButtons[0] = false;
        }

        //control mode button
        if (sf::Joystick::isButtonPressed(0, 2))
        {
            if(!pressedButtons[2]) {
                if(controller->gControlMode() == 1) {
                    ui->manualMode->setChecked(true);
                    controller->updateControlMode(0);
                }
                else {
                    ui->automaticMode->setChecked(true);
                    controller->updateControlMode(1);
                }
            }

            pressedButtons[2] = true;
        }
        else {
            pressedButtons[2] = false;
        }

        //calibrate or degrees button
        if (sf::Joystick::isButtonPressed(0, 3))
        {
            if(!pressedButtons[3]) {
                calibrateJoystick = !calibrateJoystick;
            }

            pressedButtons[3] = true;
        }
        else {
            pressedButtons[3] = false;
        }

        //x, y axis
        int xJoystick = (((sf::Joystick::getAxisPosition(0, sf::Joystick::X) / 100) * 140) + 140) / 2;
        int yJoystick = (((sf::Joystick::getAxisPosition(0, sf::Joystick::Y) / 100) * 140) + 140) / 2;

        joystick->setPosDirect(xJoystick, yJoystick);

        //throttle
        float zT = sf::Joystick::getAxisPosition(0, sf::Joystick::Z);

        if(zT == 0) {
            zT = 50;
        }
        else if(zT < 0) {
            zT = (int)(50 -((zT / 100) * 50));
        }
        else {
            zT = (int)((1 - (zT / 100)) * 50);
        }

        if(zT != ui->throttleSlider->value()) {
            controller->updatePositionTrottle(zT);

            ui->throttleSlider->setValue(zT);
        }

        //calibration or degrees
        int povXJoystick = sf::Joystick::getAxisPosition(0, sf::Joystick::PovX);
        int povYJoystick = sf::Joystick::getAxisPosition(0, sf::Joystick::PovY);

        if(calibrateJoystick) {
            /*
            if(povXJoystick == 100 || povXJoystick == 70) {
                ui->leftRightCalibrate->setValue(ui->leftRightCalibrate->value() + 1);
            }
            else if(povXJoystick == -100 || povXJoystick == -70) {
                ui->leftRightCalibrate->setValue(ui->leftRightCalibrate->value() - 1);
            }

            if(povYJoystick == 100 || povYJoystick == 70) {
                ui->frontBackCalibrate->setValue(ui->frontBackCalibrate->value() + 1);
            }
            else if(povYJoystick == -100 || povYJoystick == -70) {
                ui->frontBackCalibrate->setValue(ui->frontBackCalibrate->value() - 1);
            }
            */

            if(povXJoystick == 100) {
                if(!pressedButtons[4]) {
                    if(motorCalibrate == 3) {
                        motorCalibrate = 0;
                    }
                    else {
                        motorCalibrate++;
                    }

                    ui->motorCalibrateLabel->setText(QString::number(motorCalibrate));
                }

                pressedButtons[4] = true;
            }
            else {
                pressedButtons[4] = false;
            }

            if(povXJoystick == -100) {
                if(!pressedButtons[5]) {
                    if(motorCalibrate == 0) {
                        motorCalibrate = 3;
                    }
                    else {
                        motorCalibrate--;
                    }

                    ui->motorCalibrateLabel->setText(QString::number(motorCalibrate));
                }

                pressedButtons[5] = true;
            }
            else {
                pressedButtons[5] = false;
            }

            if(povYJoystick == 100) {
                switch (motorCalibrate) {
                case 0:
                    ui->calibrateMotor1Slider->setValue(ui->calibrateMotor1Slider->value() + 1);
                    break;
                case 1:
                    ui->calibrateMotor2Slider->setValue(ui->calibrateMotor2Slider->value() + 1);
                    break;
                case 2:
                    ui->calibrateMotor3Slider->setValue(ui->calibrateMotor3Slider->value() + 1);
                    break;
                case 3:
                    ui->calibrateMotor4Slider->setValue(ui->calibrateMotor4Slider->value() + 1);
                    break;
                default:
                    qDebug() << "error motor calibrate";
                    break;
                }
            }
            else if(povYJoystick == -100) {
                switch (motorCalibrate) {
                case 0:
                    ui->calibrateMotor1Slider->setValue(ui->calibrateMotor1Slider->value() - 1);
                    break;
                case 1:
                    ui->calibrateMotor2Slider->setValue(ui->calibrateMotor2Slider->value() - 1);
                    break;
                case 2:
                    ui->calibrateMotor3Slider->setValue(ui->calibrateMotor3Slider->value() - 1);
                    break;
                case 3:
                    ui->calibrateMotor4Slider->setValue(ui->calibrateMotor4Slider->value() - 1);
                    break;
                default:
                    qDebug() << "error motor calibrate";
                    break;
                }
            }
        }
        else {
            if(povXJoystick == 100 || povXJoystick == 70) {
                //degrees +

                int tempDegrees = mCompassNeedle2->currentValue() + 1;

                if(tempDegrees < 0)
                {
                    tempDegrees = 359 + tempDegrees;
                }
                else if(tempDegrees > 359)
                {
                    tempDegrees = tempDegrees - 359;
                }

                int degrees = tempDegrees - 90;

                if(degrees < 0)
                {
                    degrees = 359 + tempDegrees - 90;
                }

                ui->degreesLabel->setText(QString::number(degrees) + "°");
                mCompassNeedle2->setCurrentValue(tempDegrees);

                controller->updateOrientationDegrees(degrees);
            }
            else if(povXJoystick == -100 || povXJoystick == -70) {
                //degrees -

                int tempDegrees = mCompassNeedle2->currentValue() - 1;

                if(tempDegrees < 0)
                {
                    tempDegrees = 359 + tempDegrees;
                }
                else if(tempDegrees > 359)
                {
                    tempDegrees = tempDegrees - 359;
                }

                int degrees = tempDegrees - 90;

                if(degrees < 0)
                {
                    degrees = 359 + tempDegrees - 90;
                }

                ui->degreesLabel->setText(QString::number(degrees) + "°");
                mCompassNeedle2->setCurrentValue(tempDegrees);

                controller->updateOrientationDegrees(degrees);
            }
        }
    }

    /*
    if (!input.updateState()) return;

    // Output buttns' state
     for (int r = 0; r < 24; r++)
    {
        bool isPressed = input.isKeyPressed(r);
        if (isPressed)
        {
            qDebug() << r << "is pressend";

            switch (r) {
            case 0:
                ui->checkGamepad->setChecked((ui->checkGamepad->isChecked())?false:true);
                break;
            case 1 :
                ui->throttleSlider->setValue(0);
                controller->updatePositionTrottle(0);
                ui->checkGamepad->setChecked(false);
                break;
            case 3 :
                on_buttonCalibrateDrone_clicked();
                break;
            default:
                break;
            }
        }
      //  tblButtons.setText( r,1, stateString );
    }

    if (!ui->checkGamepad->isChecked()) return;


    // Update main axes
    /*  ui->sliderVertical->setValue((input.getVertical()+1.0f)*50.0f);
    ui->sliderHorizontal->setValue((input.getHorizontal()+1.0f)*50.0f);
    ui->dialRotation->setValue((input.getRotationZ()+1.0f)*50.0f);
    ui->dialThrottle->setValue((input.getThrottle()+1.0f)*50.0f);*/


    //qDebug() << input.getVertical() << " " << input.getHorizontal() << " " << input.getRotationZ() << input.getThrottle();

    /*
    ui->leftRightCalibrate->setValue(ui->leftRightCalibrate->value()+roundf(input.getRawAxis(6)));
    ui->frontBackCalibrate->setValue(ui->frontBackCalibrate->value()+roundf(input.getRawAxis(7)));
    */

   // int newThrottleValue = (int)(((-input.getVertical()*10)+10)/2);

    /*
    int newThrottleValue = (int)(ui->throttleSlider->value()+((((int)input.getVertical()*10000==0)?0:(-input.getVertical()))*2));

    qDebug() << newThrottleValue;

    if (ui->throttleSlider->value() != newThrottleValue )
    {

        if (newThrottleValue<=0)
        {
            newThrottleValue = 0;
        }
        else if (newThrottleValue>=100)
        {
            newThrottleValue = 100;
        }

        ui->throttleSlider->setValue(newThrottleValue);

        controller->updatePositionTrottle(ui->throttleSlider->value());
    }


    int x = (int) (((input.getThrottle()*140)+140)/2), y= (int) (((input.getRotationZ()*140)+140)/2);
    if (joystick->_location.x() != x || joystick->_location.y() != y )
    {
        joystick->setPosDirect( x , y );

    }

    int temporyValue = mCompassNeedle2->currentValue()+(int)(input.getHorizontal()*5);
    if ( mCompassNeedle2->currentValue() != temporyValue)
    {
        if (temporyValue<0)
        {
            temporyValue=359+temporyValue;
        }
        else if (temporyValue>359)
        {
            temporyValue = temporyValue-359;
        }

        int degrees = temporyValue-90;
        if (degrees<0)
        {
             degrees = 359+temporyValue-90;
        }


        ui->degreesLabel->setText(QString::number(degrees)+"°");
        mCompassNeedle2->setCurrentValue(temporyValue);
        //controller->updateOrientationDegrees(degrees);
    }
    */
}

void DGIpydrOne::statutConnection(QString statut)
{
    qDebug() << statut;

    if(statut == "CONNECT") {
        updateConsole("RECEIVE", tr("<em>Connected</em>"));

        ui->buttonFastConnect->setText("Connected !");

        ui->connectBeforeWidget->hide();
        ui->connectBefore_optionsWidget->hide();
    }
    else if(statut == "DISCONNECT") {
        updateConsole("RECEIVE", tr("<em>Disconnected</em>"));

        ui->buttonFastConnect->setText("Disconnected. Drone !");

        ui->buttonConnect->setEnabled(true);
        ui->buttonCancelConnect->setEnabled(false);

        ui->connectBeforeWidget->show();
        ui->connectBefore_optionsWidget->show();

        QMessageBox::warning(this, "Switch-off motors", "Think to switch-off motors of your drone.", QMessageBox::Ok);

        firstTimeConnection = false;
    }
    else
    {
        updateConsole("RECEIVE", statut);

        ui->buttonFastConnect->setText("Disconnected. Drone !");

        ui->buttonConnect->setEnabled(true);
    }
}

void DGIpydrOne::updateConsole(QString type, QString text)
{
    if(type == "RECEIVE") {
        ui->consoleReceive->append(text);

        QScrollBar *sb = ui->consoleReceive->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
    else if(type == "SEND") {
        ui->consoleSend->append(text);

        QScrollBar *sb = ui->consoleSend->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

void DGIpydrOne::updateConnectionTime(int time)
{
    int minutes = qFloor((float)time / 60);
    int seconds = time - (60 * minutes);
    int hours = qFloor((float)minutes / 60);

    QString allTime = ((hours > 0) ? (QString::number(hours) + "h") : "") + ((minutes > 0) ? (QString::number(minutes) + "m") : "") + QString::number(seconds) + "s";

    ui->connectionTime->setText(allTime);
    ui->buttonFastConnect->setText("Connected ! " + allTime);
}

void DGIpydrOne::updateInformationsInterface(QString type, int value)
{
    //qDebug() << type;

    QStringList list;
    list << "JOYSTICK-X" << "JOYSTICK-Y" << "THROTTLE" << "LSONAR" << "RSONAR" << "FSONAR" << "BSONAR" << "PITCH" << "ROLL" << "DEGREES" << "VSPEED" << "HSPEED" << "BATTERY" << "PRESSURE" << "TEMPERATURE" << "HUMIDITY" << "MODE" << "USESONARS" << "AXISSENSIBILITY" << "ROTATIONSENSIBILITY" << "FLASHINGLED";

    switch (list.indexOf(type)) {
    case 0:
        ui->joystickXInformation->setText(QString::number(value) + "°");
        break;
    case 1:
        ui->joystickYInformation->setText(QString::number(value) + "°");
        break;
    case 2:
        ui->throttleLabel->setText(QString::number(value) + "%");
        break;
    case 3:
        vLeftSonar = value;
        break;
    case 4:
        vRightSonar = value;
        break;
    case 5:
        vFrontSonar = value;
        break;
    case 6:
        vBackSonar = value;
        break;
    case 7:
        //vUpSonar = value;
        mAttMeter->setCurrentPitch(value);
        break;
    case 8:
        //vDownSonar = value;
        mAttitudeNeedle->setCurrentValue(90 - value);
        mAttMeter->setCurrentRoll(value);
        break;
    case 9:
        vDegrees = value;
        mCompassNeedle->setCurrentValue(value + 90);

        if(!firstTimeConnection) {
            mCompassNeedle2->setCurrentValue(value + 90);
            controller->updateOrientationDegrees(value);
            ui->degreesLabel->setText(QString::number(value) + "°");

            firstTimeConnection = true;
        }
        break;
    case 10:
        vVerticalSpeed = value;
        ui->VSLabel->setText(QString::number(value) + " M/S");
        break;
    case 11:
        vHorizontalSpeed = value;
        ui->HSLabel->setText(QString::number(value) +  " M/S");
        break;
    case 12:
        ui->batteryLevelBar->setValue((int)((((float)value - 1000) / 300) * 100));
        break;
    case 13:
        vPressure = value;
        ui->pressureLabel->setText(QString::number(value) + " Pa");
        break;
    case 14:
        ui->temperatureLabel->setText(QString::number(value) + " °C");
        break;
    case 15:
        ui->humidityLabel->setText(QString::number(value) + " %");
        break;
    case 16:
        ui->automaticMode->setChecked((value == 1) ? true : false);
        ui->manualMode->setChecked((value == 1) ? false : true);
        break;
    case 17:
        ui->checkCollision->setChecked((value == 1) ? true : false);
        break;
    case 18:
        ui->axisSensibilitySlider->setValue(value);
        break;
    case 19:
        ui->rotationSensibilitySlider->setValue(value);
        break;
    case 20:
        ui->checkLED->setChecked((value == 1) ? true : false);
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
    profile->saveProfile(ui->profileName->text(),
                         ui->serverIp->text(),
                         ui->serverPort->value(),
                         ui->maximalPowerSlider->value(),
                         ui->maximalAngleSlider->value(),
                         controller->gControlMode(),
                         ui->leftRightCalibrate->value(),
                         ui->frontBackCalibrate->value());
}

void DGIpydrOne::on_checkLED_stateChanged()
{
    if(ui->checkLED->isChecked()) {
        controller->sendCommand("L Y");
    }
    else
    {
        controller->sendCommand("L N");
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
    controller->sendCalibrate(value, ui->frontBackCalibrate->value());
}

void DGIpydrOne::on_frontBackCalibrate_valueChanged(int value)
{
    controller->sendCalibrate(ui->leftRightCalibrate->value(), value);
}

void DGIpydrOne::on_checkCollision_stateChanged()
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
        controller->updateControlMode(0);
    }
}

void DGIpydrOne::on_automaticMode_clicked()
{
    if(ui->automaticMode->isChecked()) {
        controller->updateControlMode(1);
    }
}

void DGIpydrOne::on_buttonCalibrateDrone_clicked()
{
    controller->sendCommand("A Y");
}

void DGIpydrOne::on_buttonLessCompass_clicked()
{
    int temporyValue = mCompassNeedle2->currentValue()-1;
    if (temporyValue<0)
    {
        temporyValue=359+temporyValue;
    }
    else if (temporyValue>359)
    {
        temporyValue = temporyValue-359;
    }

    int degrees = temporyValue-90;
    if (degrees<0)
    {
         degrees = 359+temporyValue-90;
    }


    ui->degreesLabel->setText(QString::number(degrees)+"°");
    mCompassNeedle2->setCurrentValue(temporyValue);
    controller->updateOrientationDegrees(degrees);
}

void DGIpydrOne::on_buttonMoreCompass_clicked()
{
    int temporyValue = mCompassNeedle2->currentValue()+1;
    if (temporyValue<0)
    {
        temporyValue=359+temporyValue;
    }
    else if (temporyValue>359)
    {
        temporyValue = temporyValue-359;
    }

    int degrees = temporyValue-90;
    if (degrees<0)
    {
         degrees = 359+temporyValue-90;
    }


    ui->degreesLabel->setText(QString::number(degrees)+"°");
    mCompassNeedle2->setCurrentValue(temporyValue);
    controller->updateOrientationDegrees(degrees);
}

void DGIpydrOne::on_buttonStartSession_clicked()
{
    if(ui->throttleSlider->value() == 0) {
        if(!controller->isPlayingSession()) {
            if(controller->startSession()) {
                ui->buttonStartSession->setText("Stop session");
                ui->buttonStartSession->setStyleSheet("background-color: rgb(255, 74, 77);");

                QMessageBox::information(this, tr("Information"), tr("You can turn-on motors."), QMessageBox::Ok);
            }
        }
        else {
            if(controller->stopSession()) {
                ui->buttonStartSession->setText("Start session");
                ui->buttonStartSession->setStyleSheet("background-color: rgb(11, 190, 68);");
            }
        }
    }
    else {
        QMessageBox::warning(this, tr("Warning"), tr("Throttle not at the lowest position."), QMessageBox::Ok);
    }
}

void DGIpydrOne::on_buttonPing_clicked()
{
    ui->listIp->clear();

    QStringList pieceIp = ui->serverIp->text().split(".");
    QString searchIp = pieceIp[0] + "." + pieceIp[1] + "." + pieceIp[2] + ".";

    network = new AnalyzeNetwork(searchIp);
    connect(network, SIGNAL(answerIp(QString)), this, SLOT(connectedDevices(QString)));

    network->start();
}

void DGIpydrOne::on_buttonStopPing_clicked() {
    network->terminate();
}

void DGIpydrOne::connectedDevices(QString ip) {
    ui->listIp->addItem(ip);
}

void DGIpydrOne::on_listIp_doubleClicked(const QModelIndex &index)
{
    qDebug() << ui->listIp->currentItem()->text();

    ui->serverIp->setText(ui->listIp->currentItem()->text());
}

void DGIpydrOne::on_axisSensibilitySlider_valueChanged(int value)
{
    controller->sendCommand("O " + QString::number(value) + "|" + QString::number(ui->rotationSensibilitySlider->value()));
}

void DGIpydrOne::on_rotationSensibilitySlider_valueChanged(int value)
{
    controller->sendCommand("O " + QString::number(ui->axisSensibilitySlider->value()) + "|" + QString::number(value));
}

void DGIpydrOne::on_maximalPowerSlider_valueChanged(int value)
{
    controller->updateProperties(value, ui->maximalAngleSlider->value());
}

void DGIpydrOne::on_maximalAngleSlider_valueChanged(int value)
{
    controller->updateProperties(ui->maximalPowerSlider->value(), value);
}

void DGIpydrOne::on_buttonFastConnect_clicked()
{
    if(controller->connectionStatut == 0) {
        on_buttonConnect_clicked();
    }
}

void DGIpydrOne::on_profileName_textChanged(const QString &arg1)
{
    ui->profileLoaded->setText(arg1);
}

void DGIpydrOne::on_buttonLoadProfileFile_clicked()
{
    if(!controller->isPlayingSession()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open profile"), "", tr("Profile file (*.DGIDrOne)"));

        if(fileName != "") {
            ProfileEditor *profile = new ProfileEditor(this);

            if(profile->loadProfile(fileName)) {
                QFileInfo file(QFile(fileName).fileName());

                ui->profileLoaded->setText(file.fileName().split(".")[0]);
                ui->serverIp->setText(profile->serverIp());
                ui->serverPort->setValue(profile->serverPort());
                ui->maximalPowerSlider->setValue(profile->maxPower());
                ui->maximalAngleSlider->setValue(profile->maxAngle());

                if(profile->controlMode() == 1) {
                    controller->updateControlMode(1);
                    ui->automaticMode->setChecked(true);
                }
                else {
                    controller->updateControlMode(0);
                    ui->manualMode->setChecked(true);
                }

                ui->leftRightCalibrate->setValue(profile->leftRightCalibration());
                ui->frontBackCalibrate->setValue(profile->frontBackCalibration());

                QMessageBox::information(this, tr("Information"), tr("Loaded profile."), QMessageBox::Ok);
            }
            else {
                QMessageBox::critical(this, tr("Error"), tr("File doesn't exist."), QMessageBox::Ok);
            }
        }
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Stop session before."));
    }
}

void DGIpydrOne::on_calibrateMotor1Slider_valueChanged(int value)
{
    controller->sendCommand("B 0|" + QString::number(value));

    ui->calibrateMotor1Label->setText(QString::number(value) + "%");
}

void DGIpydrOne::on_calibrateMotor2Slider_valueChanged(int value)
{
    controller->sendCommand("B 1|" + QString::number(value));

    ui->calibrateMotor2Label->setText(QString::number(value) + "%");
}

void DGIpydrOne::on_calibrateMotor3Slider_valueChanged(int value)
{
    controller->sendCommand("B 2|" + QString::number(value));

    ui->calibrateMotor3Label->setText(QString::number(value) + "%");
}

void DGIpydrOne::on_calibrateMotor4Slider_valueChanged(int value)
{
    controller->sendCommand("B 3|" + QString::number(value));

    ui->calibrateMotor4Label->setText(QString::number(value) + "%");
}
