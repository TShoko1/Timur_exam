#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <algorithm>

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();
public slots:
    void heapSort(QVector<int>& data, int step);
    void heapify(QVector<int>& data, int n, int i, int step);
    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();
    void slotServerSend();
private:
    QTcpServer* mTcpServer;
    QList<QTcpSocket*> mClients;
    int server_status;
    bool mIsDataSent;
};

#endif // MYTCPSERVER_H
