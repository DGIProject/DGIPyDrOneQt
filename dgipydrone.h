#ifndef DGIPYDRONE_H
#define DGIPYDRONE_H

#include "remotecontroller.h"
#include "profileeditor.h"
#include "joystickrect.h"
#include "xinputGamepad.h"
#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QScrollBar>
#include <QtSensors/QAccelerometer>
#include <QtSensors/QGyroscope>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsView>

namespace Ui {
class DGIpydrOne;
}

class DGIpydrOne : public QMainWindow
{
    Q_OBJECT

public:
    explicit DGIpydrOne(QWidget *parent = 0);
    ~DGIpydrOne();

private slots:
    void on_throttleSlider_valueChanged(int);

    void on_buttonConnect_clicked();
    void on_buttonCancelConnect_clicked();
    void on_buttonSendCommand_clicked();

    void statutConnection(QString);
    void updateConsole(QString);
    void updateConnectionTime(int);
    void updateTextInformation(QString, int);

    void on_buttonSaveProfile_clicked();

    void on_checkLED_clicked();

    void on_leftRightCalibrate_valueChanged(int value);

    void on_frontBackCalibrate_valueChanged(int value);

    void on_checkCollision_clicked();

    void on_manualMode_clicked();

    void on_automaticMode_clicked();

    void on_buttonCalibrateDrone_clicked();

    void on_buttonLessCompass_clicked();

    void on_buttonMoreCompass_clicked();

private:
    Ui::DGIpydrOne *ui;

    void drawDroneInformations();

    int vLeftSonar,
    vRightSonar,
    vFrontSonar,
    vBackSonar,
    vUpSonar,
    vDownSonar,
    vDegrees,
    vVerticalSpeed,
    vHorizontalSpeed,
    vPressure,
    vTemperature,
    vHumidity;

    QcGaugeWidget *mCompassGauge;
    QcNeedleItem *mCompassNeedle, *mCompassNeedle2;

    QcGaugeWidget * mAttitudeGauge;
    QcNeedleItem * mAttitudeNeedle;
    QcAttitudeMeter *mAttMeter;

    remoteController *controller;

    QGraphicsScene *sceneJoystick;
    joystickRect *joystick;

    QGraphicsScene *sceneSonar;

    QGraphicsItem *leftSonar;
    QGraphicsItem *rightSonar;
    QGraphicsItem *frontSonar;
    QGraphicsItem *backSonar;
    QGraphicsItem *upSonar;
    QGraphicsItem *downSonar;

    XInput input;
    QTimer tmr;

    bool f_haveJoystick, cancelJoystick = false;


public slots:
       void readJoystickState();
};

#endif // DGIPYDRONE_H
