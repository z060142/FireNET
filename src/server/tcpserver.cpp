// Copyright � 2016 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: http://opensource.org/licenses/MIT

#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
    m_pool = QThreadPool::globalInstance();
    setMaxThreads(m_pool->maxThreadCount());
}

TcpServer::~TcpServer()
{
    close();
}

void TcpServer::setMaxThreads(int maximum)
{
    if(isListening())
    {
        qWarning() << "[TcpServer] WARNING Max threads not changed, call setMaxThreads() before listen()";
        return;
    }

    m_pool->setMaxThreadCount(maximum);
}

bool TcpServer::listen(const QHostAddress &address, quint16 port)
{
	qInfo() << "[TcpServer] Start listening on port " << port;
    startThreads();
    return QTcpServer::listen(address,port);
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "[TcpServer] Incomming connection...";

    TcpThread *pThread = freeThread();

    if(!pThread)
    {
        qCritical() << "[TcpServer] No free thread!";
        return;
    }

	pThread->accept(socketDescriptor, pThread->runnableThread());
}

void TcpServer::close()
{
	qInfo() << "[TcpServer] Closing server...";
    emit stop();

    m_threads.clear();
    m_pool->clear();

    QTcpServer::close();
}

void TcpServer::sendMessageToClient(QSslSocket * socket, QByteArray data)
{
	qDebug() << "[TcpServer] Send message to client. Original size = " << data.size();
	socket->write(data);
	socket->waitForBytesWritten(3);
}

void TcpServer::sendGlobalMessage(QByteArray data)
{
	qDebug() << "[TcpServer] Send message to all clients. Original size = " << data.size();

	QVector<SClient>::iterator it;
	for (it = vClients.begin(); it != vClients.end(); ++it)
	{
		it->socket->write(data);
		it->socket->waitForBytesWritten(3);
	}
}

void TcpServer::startThreads()
{
	qInfo() << "[TcpServer] Starting threads :" << m_pool->maxThreadCount();

    for(int i = 0; i < m_pool->maxThreadCount(); i++)
    {
        TcpThread *cThread = new TcpThread(this);
        startThread(cThread);
    }
}

void TcpServer::startThread(TcpThread *cThread)
{
    connect(this,&TcpServer::stop, cThread, &TcpThread::stop);
    m_threads.append(cThread);
    cThread->setAutoDelete(true);
    m_pool->start(cThread);
}

TcpThread *TcpServer::freeThread()
{
    if(m_threads.count() < 1)
    {
        qCritical() << "[TcpServer] No threads to run connection on!";
        return nullptr;
    }

	TcpThread* pThread;

	int threadsCount = 0;
	int maxClientsInThread = 0;

    // Detect maximum clients in thread
	for (threadsCount; threadsCount != m_pool->maxThreadCount(); threadsCount++)
	{
		pThread = m_threads.at(threadsCount);
		int clientsInThread = pThread->m_clients;

		if (clientsInThread > maxClientsInThread)
			maxClientsInThread = clientsInThread;
	}

	threadsCount = 0;

    // Return thread with minimal clients count
	for (threadsCount; threadsCount != m_pool->maxThreadCount(); threadsCount++)
	{
		pThread = m_threads.at(threadsCount);
		int clientsInThread = pThread->m_clients;
		if (clientsInThread < maxClientsInThread)
		{
			return pThread;
		}
	}


    return pThread;
}
