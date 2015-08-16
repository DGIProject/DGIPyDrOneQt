#include "profileeditor.h"

ProfileEditor::ProfileEditor(QObject *parent) : QObject(parent)
{

}

ProfileEditor::~ProfileEditor()
{

}

bool ProfileEditor::loadProfile(QString fileName)
{
    if(QFile::exists(fileName)) {
        QSettings settings(fileName, QSettings::IniFormat);

        vServerIp = settings.value("serverIp").toString();
        vServerPort = settings.value("serverPort").toInt();
        vMaxPower = settings.value("maxPower").toInt();
        vMaxAngle = settings.value("maxAngle").toInt();
        vControlMode = settings.value("controlMode").toInt();
        vLeftRightCalibration = settings.value("leftRightCalibration").toInt();
        vFrontBackCalibration = settings.value("frontBackCalibration").toInt();

        return true;
    }
    else {
        return false;
    }
}

void ProfileEditor::saveProfile(QString fileName, QString serverIp, int serverPort, int maxPower, int maxAngle, int controlMode, int leftRightCalibration, int frontBackCalibration)
{
    if(QFile::exists(fileName + ".DGIDrOne")) {
        QMessageBox msgBox;
        msgBox.setText(tr("Profile already exist."));
        msgBox.setInformativeText(tr("Do you want to overwrite your changes ?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);

        int answer = msgBox.exec();

        if(answer == QMessageBox::Cancel) {
            return;
        }
    }

    QSettings settings(fileName + ".DGIDrOne", QSettings::IniFormat);
    settings.setValue("serverIp", serverIp);
    settings.setValue("serverPort", serverPort);
    settings.setValue("maxPower", maxPower);
    settings.setValue("maxAngle", maxAngle);
    settings.setValue("controlMode", controlMode);
    settings.setValue("leftRightCalibration", leftRightCalibration);
    settings.setValue("frontBackCalibration", frontBackCalibration);

    QMessageBox msgBox;
    msgBox.setText(tr("Saved profile."));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    msgBox.exec();
}

QString ProfileEditor::serverIp()
{
    return vServerIp;
}

int ProfileEditor::serverPort()
{
    return vServerPort;
}

int ProfileEditor::maxPower()
{
    return vMaxPower;
}

int ProfileEditor::maxAngle()
{
    return vMaxAngle;
}

int ProfileEditor::controlMode()
{
    return vControlMode;
}

int ProfileEditor::leftRightCalibration()
{
    return vLeftRightCalibration;
}

int ProfileEditor::frontBackCalibration()
{
    return vFrontBackCalibration;
}

