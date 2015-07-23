#include "profileeditor.h"

ProfileEditor::ProfileEditor(QObject *parent) : QObject(parent)
{

}

ProfileEditor::~ProfileEditor()
{

}

void ProfileEditor::loadProfile(QString fileName)
{

}

void ProfileEditor::saveProfile(QString fileName, int sensibility, int maxPower, int maxAngle)
{
    qDebug() << "saveProfile";

    if(QFile::exists(fileName + ".ini")) {
        qDebug() << "alreadyExist";

        QMessageBox msgBox;
        msgBox.setText("Profile already exist.");
        msgBox.setInformativeText("Do you want to overwrite your changes ?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);

        int answer = msgBox.exec();

        if(answer == QMessageBox::Cancel) {
            qDebug() << "cancel answer";
            return;
        }
    }

    QSettings settings(fileName + ".ini", QSettings::IniFormat);
    settings.setValue("sensibility", sensibility);
    settings.setValue("maxPower", maxPower);
    settings.setValue("maxAngle", maxAngle);
}

