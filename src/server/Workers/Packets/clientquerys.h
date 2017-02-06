// Copyright (C) 2014-2017 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/FireNET/blob/master/LICENSE

#ifndef CLIENTQUERYS_H
#define CLIENTQUERYS_H

#include <QObject>

#include "global.h"

class NetPacket;
class TcpConnection;

class ClientQuerys : public QObject
{
    Q_OBJECT
public:
    explicit ClientQuerys(QObject *parent = nullptr);
	~ClientQuerys();
public:
	void           SetSocket(QSslSocket* socket) { this->m_socket = socket; }
	void           SetClient(SClient* client);
	void           SetConnection(TcpConnection* connection) { this->m_Connection = connection; }

    void           onLogin(NetPacket &packet);
    void           onRegister(NetPacket &packet);

    void           onCreateProfile(NetPacket &packet);
    void           onGetProfile();

    void           onGetShopItems();
    void           onBuyItem(NetPacket &packet);
	void           onRemoveItem(NetPacket &packet);

    void           onAddFriend(NetPacket &packet);
    void           onRemoveFriend(NetPacket &packet);

	void           onChatMessage(NetPacket &packet);

	void           onInvite(NetPacket &packet);
	void		   onDeclineInvite(NetPacket &packet);

	void		   onGetGameServer(NetPacket &packet);
private:
    bool           UpdateProfile(SProfile* profile);
	// Depricated. TODO - Remove this
	SShopItem      GetShopItemByName(const QString &name);
private:
	QSslSocket*    m_socket;
	SClient*       m_Client;
	TcpConnection* m_Connection;
	bool		   bAuthorizated;
	bool		   bRegistered;
	bool		   bProfileCreated;
};
#endif // CLIENTQUERYS_H
