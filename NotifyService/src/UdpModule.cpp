/*
 * UdpModule.cpp
 *
 *  Created on: 16.01.2014
 *      Author: benjaminsliwa
 */

/*
 *
 * UdpModule object code have been greatfully given by Benjamin Sliwa, developer of
 * Talk2Watch app. It's based on BlackBerry's X And O's sample app.
 *
 * It's uncommented, and I'll not add comment to it in respect of author
 *
 * Authorization was given by the author to use and modify this code.
 *
 */

#include "UdpModule.h"

UdpModule::UdpModule(QObject *_parent)
{
	// Avoid console warning as this variable is not used
	Q_UNUSED(_parent);

	m_server = new QUdpSocket(this);
	m_socket = new QUdpSocket(this);
}

void UdpModule::listenOnPort(int _port)
{
	m_server->bind(QHostAddress::Any, _port);
	bool ok = connect(m_server, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

	ok==true ? qDebug() << "binding ok" : qDebug() << "binding failed";
}

void UdpModule::sendMessage(QString _data)
{
	qDebug() << "UdpModule::sendMessage " << _data;
	m_socket->writeDatagram(_data.toStdString().c_str(),QHostAddress("127.0.0.1"), 7146);
}

void UdpModule::onReadyRead()
{
	qDebug() << "UdpServer::dataReceived";
    while (m_server->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_server->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_server->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);
        QString data = QString(datagram);

        qDebug() << data;
        emit receivedData(data);

    }
}
