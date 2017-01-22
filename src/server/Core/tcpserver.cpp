// Copyright (C) 2014-2017 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/FireNET/blob/master/LICENSE

#include "global.h"
#include "tcpserver.h"
#include "tcpthread.h"

#include "Workers/Databases/dbworker.h"
#include "Tools/settings.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
	m_maxThreads = 0;
	m_maxConnections = 0;
	m_connectionTimeout = 0;
	m_Status = EServer_Offline;
}

TcpServer::~TcpServer()
{
	qDebug() << "~TcpServer";
	QThreadPool::globalInstance()->clear();
}

void TcpServer::Clear()
{
	emit stop();
}

void TcpServer::Update()
{
	if (m_connectionTimeout > 0)
	{
		emit idle(m_connectionTimeout);
	}
}

void TcpServer::SetMaxThreads(int maximum)
{
	qDebug() << "Setting max threads to: " << maximum;
	m_maxThreads = maximum;
}

void TcpServer::SetMaxConnections(int value)
{
	qDebug() << "Setting max connections to: " << value;
	m_maxConnections = value;
}

void TcpServer::SetConnectionTimeout(int value)
{
	qDebug() << "Setting the connection timeout to: " << value;
	m_connectionTimeout = value;
}

bool TcpServer::Listen(const QHostAddress & address, quint16 port)
{
	if (m_maxThreads <= 0)
	{
		qCritical() << "Execute SetMaxThreads function before listen!";
		m_Status = EServer_Offline;
		return false;
	}

	if (!QTcpServer::listen(address, port))
	{
		qCritical() << errorString();
		m_Status = EServer_Offline;
		return false;
	}

	qInfo() << "Start listing on port :" << port;

	Start();

	m_Status = EServer_Online;

	return true;
}

void TcpServer::Start()
{
	for (int i = 0; i < m_maxThreads; i++)
	{
		TcpThread *runnable = CreateRunnable();
		if (!runnable)
		{
			qWarning() << "Could not find runable!";
			return;
		}

		StartRunnable(runnable);
	}
}

TcpThread * TcpServer::CreateRunnable()
{
	qDebug() << "Creating runnable...";

	TcpThread *runnable = new TcpThread();
	runnable->setAutoDelete(false);

	return runnable;
}

void TcpServer::StartRunnable(TcpThread * runnable)
{
	if (!runnable)
	{
		qWarning() << this << "Runnable is null!";
		return;
	}

	qDebug() << this << "Starting " << runnable;

	runnable->setAutoDelete(false);

	m_threads.append(runnable);

	connect(this, &TcpServer::closing, runnable, &TcpThread::closing, Qt::QueuedConnection);
	connect(runnable, &TcpThread::started, this, &TcpServer::started, Qt::QueuedConnection);
	connect(runnable, &TcpThread::finished, this, &TcpServer::finished, Qt::QueuedConnection);
	connect(this, &TcpServer::connecting, runnable, &TcpThread::connecting, Qt::QueuedConnection);
	connect(this, &TcpServer::idle, runnable, &TcpThread::idle, Qt::QueuedConnection);

	QThreadPool::globalInstance()->start(runnable);
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
	qDebug() << "Accepting " << socketDescriptor;

	if (GetClientCount() >= m_maxConnections)
	{
		qCritical() << "Can't accept new client, because server have limit" << m_maxConnections;
		Reject(socketDescriptor);
	}

	int previous = 0;
	TcpThread *runnable = m_threads.at(0);

	foreach(TcpThread *item, m_threads)
	{
		int count = item->Count();

		if (count == 0 || count < previous)
		{
			runnable = item;
			break;
		}

		previous = count;
	}

	if (!runnable)
	{
		qWarning() << "Could not find runable!";
		return;
	}

	Accept(socketDescriptor, runnable);
}

void TcpServer::Accept(qintptr handle, TcpThread * runnable)
{
	qDebug() << "Accepting" << handle << "on" << runnable;

	TcpConnection *connection = new TcpConnection;
	if (!connection)
	{
		qCritical() << this << "could not find connection to accept connection: " << handle;
		return;
	}

	emit connecting(handle, runnable, connection);
}

void TcpServer::Reject(qintptr handle)
{
	qDebug() << "Rejecting connection: " << handle;

	QTcpSocket *socket = new QTcpSocket(this);
	socket->setSocketDescriptor(handle);
	socket->close();
	socket->deleteLater();
}

void TcpServer::finished()
{
	qDebug() << this << "finished" << sender();
	TcpThread *runnable = static_cast<TcpThread*>(sender());

	if (!runnable)
		return;

	qDebug() << runnable << "has finished, removing from list";

	m_threads.removeAll(runnable);
	runnable->deleteLater();

	if (m_threads.size() <= 0)
		gEnv->isReadyToClose = true;
}

void TcpServer::stop()
{
	qDebug() << "Closing TcpServer and all connections...";
	emit closing();
	QTcpServer::close();
}

void TcpServer::started()
{
	TcpThread *runnable = static_cast<TcpThread*>(sender());
	if (!runnable) 
		return;
	qDebug() << runnable << "has started";
}

void TcpServer::AddNewClient(SClient client)
{
	if (client.socket == nullptr)
	{
		qWarning() << "Can't add client. Client socket = nullptr";
		return;
	}

	for (auto it = m_Clients.begin(); it != m_Clients.end(); ++it)
	{
		if (it->socket == client.socket)
		{
			qWarning() << "Can't add client" << client.socket << ". Client alredy added";
			return;
		}
	}

	qDebug() << "Adding new client" << client.socket;
	m_Clients.push_back(client);
}

void TcpServer::RemoveClient(SClient client)
{
	if (!client.socket)
	{
		qWarning() << "Can't remove client. Client socket = nullptr";
		return;
	}

	if (m_Clients.size() > 0)
	{
		for (auto it = m_Clients.begin(); it != m_Clients.end(); ++it)
		{
			if (it->socket == client.socket)
			{
				qDebug() << "Removing client" << client.socket;

				m_Clients.erase(it);
				return;
			}
		}
	}

	qWarning() << "Can't remove client. Client not found";
}

void TcpServer::UpdateClient(SClient* client)
{
	if (client->socket == nullptr)
	{
		qWarning() << "Can't update client. Client socket = nullptr";
		return;
	}

	for (auto it = m_Clients.begin(); it != m_Clients.end(); ++it)
	{
		if (it->socket == client->socket)
		{
			if (client->profile != nullptr)
			{
				it->profile = client->profile;
				it->status = client->status;

				qDebug() << "Client" << it->socket << "updated.";
				return;
			}
			else
			{
				qWarning() << "Can't update client" << it->socket << ". Profile = nullptr";
				return;
			}
		}
	}

	qWarning() << "Can't update client. Client" << client->socket << "not found";
}

bool TcpServer::UpdateProfile(SProfile * profile)
{
	for (auto it = m_Clients.begin(); it != m_Clients.end(); ++it)
	{
		if (it->profile != nullptr)
		{
			if (it->profile->uid == profile->uid)
			{
				// First update profile in DB
				if (gEnv->pDBWorker->UpdateProfile(profile))
				{
					it->profile = profile;
					qDebug() << "Profile" << profile->nickname << "updated";
					return true;
				}
				else
				{
					qWarning() << "Failed update" << profile->nickname << "profile in DB!";
					return false;
				}
			}
		}
	}

	qDebug() << "Profile" << profile->nickname << "not found.";
	return false;
}

QStringList TcpServer::GetPlayersList()
{
	QStringList playerList;

	for (auto it = m_Clients.begin(); it != m_Clients.end(); ++it)
	{
		if (it->profile != nullptr)
		{
			if (!it->profile->nickname.isEmpty())
			{
				playerList.push_back("Uid: " + QString::number(it->profile->uid) +
					" Nickname: " + it->profile->nickname +
					" Level: " + QString::number(it->profile->lvl) +
					" XP: " + QString::number(it->profile->xp));
			}
		}
	}

	return playerList;
}

int TcpServer::GetClientCount()
{
	return m_Clients.size();
}

QSslSocket * TcpServer::GetSocketByUid(int uid)
{
	if (uid <= 0)
		return nullptr;

	for (auto it = m_Clients.begin(); it != m_Clients.end(); ++it)
	{
		if (it->profile != nullptr)
		{
			if (it->profile->uid == uid)
			{
				qDebug() << "Socket finded. Return";
				return it->socket;
			}
		}
	}

	qDebug() << "Socket not finded.";
	return nullptr;
}

SProfile * TcpServer::GetProfileByUid(int uid)
{
	if (uid <= 0)
		return nullptr;

	for (auto it = m_Clients.begin(); it != m_Clients.end(); ++it)
	{
		if (it->profile != nullptr)
		{
			if (it->profile->uid == uid)
			{
				qDebug() << "Profile finded. Return";
				return it->profile;
			}
		}
	}

	qDebug() << "Profile not finded.";
	return nullptr;
}

void TcpServer::sendMessageToClient(QSslSocket * socket, NetPacket &packet)
{
	if (socket != nullptr)
	{
		socket->write(packet.toString());
		socket->waitForBytesWritten(10);
	}
}

void TcpServer::sendGlobalMessage(NetPacket &packet)
{
	for (auto it = m_Clients.begin(); it != m_Clients.end(); ++it)
	{
		// Send message only for authorizated clients
		if (it->profile != nullptr && it->socket != nullptr)
		{
			it->socket->write(packet.toString());
			it->socket->waitForBytesWritten(10);
		}
	}
}