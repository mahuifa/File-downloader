#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    Init();

}

void Widget::Init()
{

    manager = new QNetworkAccessManager(this);
    myfile = new QFile(this);
}


void Widget::on_pushButton_clicked()
{
    QNetworkRequest request;
    QString url = ui->lineEdit->text();
    request.setUrl(QUrl(url));

    reply = manager->get(request);              //发送请求

    connect(reply,&QNetworkReply::readyRead,this,&Widget::doProcessReadyRead);                //可读
    connect(reply,&QNetworkReply::finished,this,&Widget::doProcessFinished);                  //结束
    connect(reply,&QNetworkReply::downloadProgress,this,&Widget::doProcessDownloadProgress);  //大小
    connect(reply,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),          //异常
            this,&Widget::doProcessError);

    QStringList list = url.split("/");
    QString filename = list.at(list.length()-1);
    QString file = "E:/tp/" + filename;
    myfile->setFileName(file);
   bool ret = myfile->open(QIODevice::WriteOnly|QIODevice::Truncate);    //创建文件
    if(!ret)
    {
        QMessageBox::warning(this,"warning","打开失败");
        return;
    }

    ui->progressBar->setValue(0);             //设置进度条初始化为0
    ui->progressBar->setMinimum(0);
}

void Widget::doProcessReadyRead()             //读取并写入
{
    while(!reply->atEnd())
    {
        QByteArray ba = reply->readAll();
        myfile->write(ba);
    }
}

void Widget::doProcessFinished()
{
    myfile->close();
}

void Widget::doProcessDownloadProgress(qint64 recv_total, qint64 all_total)      //显示
{
    ui->progressBar->setMaximum(all_total);
    ui->progressBar->setValue(recv_total);

    QStringList list = ui->lineEdit->text().split("/");
    QString filename = list.at(list.length()-1);
    QString data = "下载成功--" + filename;
    ui->textEdit->setText(data);
}

void Widget::doProcessError(QNetworkReply::NetworkError code)
{
    qDebug() << code;
}

Widget::~Widget()
{
    delete ui;
}

