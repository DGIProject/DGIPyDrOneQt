#ifndef ANALYZENETWORK_H
#define ANALYZENETWORK_H

#include <QThread>
#include <QProcess>
#include <QDebug>

class AnalyzeNetwork : public QThread
{
    Q_OBJECT
public:
    AnalyzeNetwork(QString beginIp);
    ~AnalyzeNetwork();

    void run();

    void sendInformation(QString);

private:
    QString searchIp;
    int lastNumberIp;

signals:
    void answerIp(QString);
};

#endif // ANALYZENETWORK_H
