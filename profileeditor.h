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

    void loadProfile(QString);
    void saveProfile(QString, int, int, int);

signals:

public slots:
};

#endif // PROFILEEDITOR_H
