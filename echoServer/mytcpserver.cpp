#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>

MyTcpServer::~MyTcpServer()
{
    mTcpServer->close();
    server_status = 0;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        server_status = 1;
        qDebug() << "server is started";
    }
}

void MyTcpServer::slotNewConnection(){
    if(server_status == 1){
        QTcpSocket* mTcpSocket = mTcpServer->nextPendingConnection();
        mTcpSocket->write("Hello, World!!! I am echo server!\r\n");
        connect(mTcpSocket, &QTcpSocket::readyRead,
                this,&MyTcpServer::slotServerRead);
        connect(mTcpSocket,&QTcpSocket::disconnected,
                this,&MyTcpServer::slotClientDisconnected);
        mClients.append(mTcpSocket);
    }
}



void MyTcpServer::slotServerRead()
{
    QTcpSocket* mTcpSocket = qobject_cast<QTcpSocket*>(sender());
    if (mTcpSocket == nullptr)
    {
        return;
    }

    QByteArray array = mTcpSocket->readAll();
    QString str = QString::fromUtf8(array).trimmed();
    QStringList parts = str.split("&");
    if (parts.size() == 3 && parts[0] == "sort")
    {
        int step = parts[1].toInt();
        QStringList values = parts[2].split(" ");
        values.removeAll("");
        QVector<int> data;
        for (int i = 0; i < values.size(); i++)
        {
            data.append(values[i].toInt());
        }
        heapSort(data, step);

        QTextStream stream(mTcpSocket);
        stream << "step " << step << ": ";
        for (int k = 0; k < data.size(); k++)
        {
            stream << data[k] << " ";
        }
        stream << "\r\n";

        // устанавливаем флаг после отправки данных клиенту
        mIsDataSent = true;
    }
}

void MyTcpServer::slotServerSend()
{
    QTcpSocket* mTcpSocket = qobject_cast<QTcpSocket*>(sender());
    if (mTcpSocket == nullptr)
    {
        return;
    }

    // если данные были отправлены клиенту, то закрываем соединение
    if (mIsDataSent)
    {
        mTcpSocket->close();
        mIsDataSent = false;
    }
}

void MyTcpServer::heapSort(QVector<int>& data, int step)
{
    int n = data.size();
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapify(data, n, i, step);
    }
    for (int i = n - 1; i >= 0; i--)
    {
        std::swap(data[0], data[i]);
        heapify(data, i, 0, step);
    }
}

void MyTcpServer::heapify(QVector<int>& data, int n, int i, int step)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < n && data[left] > data[largest])
    {
        largest = left;
    }
    if (right < n && data[right] > data[largest])
    {
        largest = right;
    }
    if (largest != i)
    {
        std::swap(data[i], data[largest]);
        if (step == i)
        {
            QString result = "step " + QString::number(step) + ": ";
            for (int k = 0; k < data.size(); k++)
            {
                result += QString::number(data[k]) + " ";
            }
            result += "\r\n";
            QTcpSocket* mTcpSocket = mClients.at(0);
            mTcpSocket->write(result.toUtf8());
        }
        heapify(data, n, largest, step);
    }
}

void MyTcpServer::slotClientDisconnected(){
    QTcpSocket* mTcpSocket = qobject_cast<QTcpSocket*>(sender());
    if(mTcpSocket != nullptr)
    {
        mClients.removeOne(mTcpSocket);
        mTcpSocket->close();
        mTcpSocket->deleteLater();
    }
}
