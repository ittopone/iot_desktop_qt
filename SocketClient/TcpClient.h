#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = 0);

private:
    //定义SOCKET对象
    QTcpSocket * m_tcpsocket;
    //是否与服务器连接
    bool m_isConnected;
public:
    bool sendMessage(QByteArray bytes);
    bool readMessage(QByteArray & bytes);
    void test();
public slots:
    void on_connected();
    void on_disconnected();
};

#endif // TCPCLIENT_H
