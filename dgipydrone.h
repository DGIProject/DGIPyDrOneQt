#ifndef DGIPYDRONE_H
#define DGIPYDRONE_H

#include "analyzenetwork.h"
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
#include <QProcess>
#include <QFileDialog>

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
    void updateConsole(QString, QString);
    void updateConnectionTime(int);
    void updateInformationsInterface(QString, int);

    void on_buttonSaveProfile_clicked();

    void on_checkLED_stateChanged();

    void on_leftRightCalibrate_valueChanged(int value);

    void on_frontBackCalibrate_valueChanged(int value);

    void on_checkCollision_stateChanged();

    void on_manualMode_clicked();

    void on_automaticMode_clicked();

    void on_buttonCalibrateDrone_clicked();

    void on_buttonLessCompass_clicked();

    void on_buttonMoreCompass_clicked();

    void on_buttonStartSession_clicked();

    void on_buttonPing_clicked();

    void on_buttonStopPing_clicked();

    void on_listIp_doubleClicked(const QModelIndex &index);

    void on_axisSensibilitySlider_valueChanged(int value);

    void on_rotationSensibilitySlider_valueChanged(int value);

    void on_maximalPowerSlider_valueChanged(int value);

    void on_maximalAngleSlider_valueChanged(int value);

    void on_buttonFastConnect_clicked();

    void on_profileName_textChanged(const QString &arg1);

    void on_buttonLoadProfileFile_clicked();

    void on_calibrateMotor1Slider_valueChanged(int value);

    void on_calibrateMotor2Slider_valueChanged(int value);

    void on_calibrateMotor3Slider_valueChanged(int value);

    void on_calibrateMotor4Slider_valueChanged(int value);

    void on_buttonResetCalibration_clicked();

private:
    Ui::DGIpydrOne *ui;

    void drawDroneInformations();

    AnalyzeNetwork *network;

    bool firstTimeConnection;

    int vLeftSonar,
    vRightSonar,
    vFrontSonar,
    vBackSonar,
    vUpSonar,
    vDownSonar,
    vDegrees,
    vVerticalSpeed,
    vHorizontalSpeed,
    vPressure;

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

    bool useJoystick;
    bool calibrateJoystick;

    bool pressedButtons[12] = {false, false, false, false, false, false, false, false, false, false, false, false};

    float calibrateMotor1, calibrateMotor2, calibrateMotor3, calibrateMotor4;


public slots:
       void readJoystickState();

       void connectedDevices(QString);
};

#endif // DGIPYDRONE_H
