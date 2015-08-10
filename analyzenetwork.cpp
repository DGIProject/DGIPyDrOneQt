#include "analyzenetwork.h"

AnalyzeNetwork::AnalyzeNetwork(QString beginIp)
{
    searchIp = beginIp;
    lastNumberIp = 1;
}

AnalyzeNetwork::~AnalyzeNetwork()
{

}

void AnalyzeNetwork::run()
{
    while(lastNumberIp <= 255) {
        int exitCode = QProcess::execute("ping", QStringList() << "-n" << "1" << "-w" << "1" << searchIp + QString::number(lastNumberIp));

        if(exitCode == 0) {
            qDebug() << "ok" + searchIp + QString::number(lastNumberIp);

            emit answerIp(searchIp + QString::number(lastNumberIp));
        }
        else {
            qDebug() << "no" + searchIp + QString::number(lastNumberIp);
        }

        lastNumberIp++;
    }

    qDebug() << "finish";
}

