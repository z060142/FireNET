// Copyright (C) 2014-2017 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/FireNET/blob/master/LICENSE

#include "tcpthread.h"
#include "global.h"

TcpThread::TcpThread(QObject *parent) : QObject(parent),
	m_loop(nullptr)
{
	Q_UNUSED(parent);
}

TcpThread::~TcpThread()
{
	qDebug() << "~TcpThread";
	SAFE_RELEASE(m_loop);
}

void TcpThread::run()
{
	//Make an event loop to keep this alive on the thread
	m_loop = new QEventLoop();
	connect(this, &TcpThread::quit, m_loop, &QEventLoop::quit);
	m_loop->exec();

	qDebug() << this << "finished on " << QThread::currentThread();
	emit finished();
}

int TcpThread::Count()
{
	QReadLocker locker(&m_lock);
	return m_connections.count();
}

void TcpThread::connecting(qintptr handle, TcpThread *runnable, TcpConnection* connection)
{
	if (runnable != this) 
		return;

	qDebug() << "Connecting: " << handle << " on " << runnable << " with " << connection;

	connection->moveToThread(QThread::currentThread());

	m_connections.append(connection);
	AddSignals(connection);
	connection->accept(handle);
}

void TcpThread::idle(int value)
{
	foreach(TcpConnection* connection, m_connections)
	{
		if (!connection) 
			continue;

		int idle = connection->IdleTime();
		qDebug() << this << connection << " idle for " << idle << " timeout is " << value;

		if (idle >= value)
		{
			qDebug() << this << "Closing idle connection" << connection;
			connection->quit();
		}
	}
}

void TcpThread::closing()
{
	emit quit();
}

void TcpThread::opened()
{
	TcpConnection *connection = static_cast<TcpConnection*>(sender());
	if (!connection)
		return;

	qDebug() << connection << "opened";
}

void TcpThread::closed()
{
	qDebug() << this << "Attempting closed";
	TcpConnection *connection = static_cast<TcpConnection*>(sender());
	if (!connection) return;

	qDebug() << connection << "closed";
	m_connections.removeAll(connection);

	qDebug() << this << "deleting" << connection;

	connection->deleteLater();
}

TcpConnection* TcpThread::CreateConnection()
{
	TcpConnection *connection = new TcpConnection();

	qDebug() << this << "created" << connection;

	return connection;
}

void TcpThread::AddSignals(TcpConnection * connection)
{
	connect(connection, &TcpConnection::opened, this, &TcpThread::opened, Qt::QueuedConnection);
	connect(connection, &TcpConnection::closed, this, &TcpThread::closed, Qt::QueuedConnection);
	connect(this, &TcpThread::quit, connection, &TcpConnection::quit, Qt::QueuedConnection);
}