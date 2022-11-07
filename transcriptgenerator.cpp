#include "transcriptgenerator.h"
#include <QFileDialog>
#include<QStandardPaths>
#include<QDir>
#include<QDebug>
#include <QProgressBar>
#include<QApplication>
#include<QWidget>
#include<QMessageBox>
#include <QProcess>
#include <QProcessEnvironment>
TranscriptGenerator::TranscriptGenerator(QObject *parent,    QUrl *fileUr)
    : QObject{parent}
{

    fileUrl=fileUr;
    qInfo()<<*fileUrl;
}

TranscriptGenerator::TranscriptGenerator(QUrl *fileUr)
{

    fileUrl=fileUr;
    qInfo()<<*fileUrl;
}

void TranscriptGenerator::Upload_and_generate_Transcript()
{
    QProgressBar progressBar;
            progressBar.setMinimum(0);
            progressBar.setMaximum(100);
            progressBar.setValue(10);
            progressBar.show();
            progressBar.raise();
            progressBar.activateWindow();

    QFile myfile(fileUrl->toLocalFile());
    QFileInfo fileInfo(myfile);
    QString filename(fileInfo.fileName());
    QString filepaths=fileInfo.dir().path();


    if(!QFile::exists("client.py")){
        QFile mapper("client.py");
        QFileInfo mapperFileInfo(mapper);
        QFile aligner(":/client.py");
        if(!aligner.open(QIODevice::OpenModeFlag::ReadOnly)){
            return;
        }
        aligner.seek(0);
        QString cp=aligner.readAll();
        aligner.close();

        if(!mapper.open(QIODevice::OpenModeFlag::WriteOnly|QIODevice::Truncate)){

            return;
        }
        mapper.write(QByteArray(cp.toUtf8()));
        mapper.close();
        std::string makingexec="chmod +x "+mapperFileInfo.absoluteFilePath().replace(" ", "\\ ").toStdString();
        int result = system(makingexec.c_str());
        qInfo()<<result;
    }
    QFile client_script("client.exe");
    QFileInfo client_script_info(client_script);

//    std::string alignmentstr='\"'  +mapperFileInfo.absoluteFilePath().toStdString()+ '\"'
//        +" -cae "+ '\"'+initialDictFileInfo.absoluteFilePath().toStdString()+ '\"'
//        +" "+ '\"'+finalFileInfo.absoluteFilePath().toStdString()+ '\"'
//        +" "+ '\"'+repDictFileInfo.absoluteFilePath().toStdString()+ '\"';


    std::string client='\"'
        +client_script_info.absoluteFilePath().toStdString()+'\"'
        +" "+'\"'
        +fileInfo.absoluteFilePath().toStdString()+'\"'
        +" "+'\"'
        +filepaths.toStdString()
        +"/transcript.xml"+'\"';


    qInfo()<<client_script_info;

    QProcess cmd;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QProcessEnvironment envUpdate;

    envUpdate.insert("PATH", env.value("PATH"));
    cmd.setProcessEnvironment(envUpdate);
    cmd.start(client.c_str());
    cmd.waitForFinished();
    qInfo()<<"got transcript";


    bool fileExists = QFileInfo::exists(filepaths+"/transcript.xml") && QFileInfo(filepaths+"/transcript.xml").isFile();
    while(!fileExists){
        fileExists = QFileInfo::exists(filepaths+"/transcript.xml") && QFileInfo(filepaths+"/transcript.xml").isFile();
    }
            progressBar.setValue(100);
            progressBar.hide();



}
