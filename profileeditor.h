#ifndef PROFILEEDITOR_H
#define PROFILEEDITOR_H

#include <QObject>
#include <QFile>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>

class ProfileEditor : public QObject
{
    Q_OBJECT
public:
    explicit ProfileEditor(QObject *parent = 0);
    ~ProfileEditor();

    bool loadProfile(QString);
    void saveProfile(QString, QString, int, int, int, int, int, int);

    QString serverIp();
    int serverPort();
    int maxPower();
    int maxAngle();
    int controlMode();
    int leftRightCalibration();
    int frontBackCalibration();

private:
    QString vServerIp;
    int vServerPort;
    int vMaxPower;
    int vMaxAngle;
    int vControlMode;
    int vLeftRightCalibration;
    int vFrontBackCalibration;

signals:

public slots:
};

#endif // PROFILEEDITOR_H
