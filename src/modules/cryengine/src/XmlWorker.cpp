/*************************************************************************
Copyright (C), chernecoff@gmail.com, 2016

*************************************************************************/

#include "StdAfx.h"
#include "Global.h"
#include <QXmlStreamReader>

void CXmlWorker::ReadXmlData(const char* data)
{
	QXmlStreamReader xml(data);

	xml.readNext();
	while (!xml.atEnd() && !xml.hasError())
	{
		QXmlStreamReader::TokenType token = xml.readNext();
		if (token == QXmlStreamReader::StartDocument)
			continue;
		if (token == QXmlStreamReader::StartElement)
		{
			if (xml.name() == "result")
			{
				//gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Parsing xml data....");

				QXmlStreamAttributes attributes = xml.attributes();
				QString type = attributes.value("type").toString();

				gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Result type = %s", type.toStdString().c_str());

				if (type == "auth_complete")
					onAuthComplete(data);
				if (type == "reg_complete")
					onRegisterComplete(data);
				if (type == "profile_data")
					onProfileDataRecived(data);
			}

			if (xml.name() == "shop")
				onShopItemsRecived(data);

			if (xml.name() == "chat")
				onChatMessageRecived(data);

			if (xml.name() == "error")
				onError(data);

			if (xml.name() == "invite")
				onInvite(data);

			if (xml.name() == "online_status")
				onFriendStatusUpdated(data);
		}
	}
}

void CXmlWorker::UpdateFriendList()
{
	CrySleep(60);

	QVector<SFriend>::iterator it;
	for (it = gCryModule->m_friends.begin(); it != gCryModule->m_friends.end(); ++it)
	{
		SUIArguments args;
		args.AddArgument(it->nickname.toStdString().c_str());
		args.AddArgument(it->uid);

		if (it->status == 0)
			args.AddArgument("offline");
		if (it->status == 1)
			args.AddArgument("online");
		if (it->status == 3)
			args.AddArgument("ingame");
		if (it->status == 4)
			args.AddArgument("afk");

		gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnFriendRecived, args);
	}
}

void CXmlWorker::onError(const char* data)
{
	QXmlStreamReader xml(data);

	while (!xml.atEnd() && !xml.hasError())
	{
		xml.readNext();

		if (xml.name() == "error")
		{
			QXmlStreamAttributes attributes = xml.attributes();
			QString type = attributes.value("type").toString();
			QString reason = attributes.value("reason").toString();

			gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Error type = '%s' reason = '%s'", type.toStdString().c_str(), reason.toStdString().c_str());

			if (!type.isEmpty() && !reason.isEmpty())
			{
				if (type == "auth_failed")
				{
					SUIArguments args;

					if(reason == "0")
						args.AddArgument("@loginNotFound");			
					if (reason == "1")
						args.AddArgument("@accountBlocked");
					if (reason == "2")
						args.AddArgument("@incorrectPassword");


					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnError, args);

					return;
				}

				if (type == "reg_failed")
				{
					SUIArguments args;

					if (reason == "0")
						args.AddArgument("@loginAlredyRegistered");
					if (reason == "1")
						args.AddArgument("@serverError");

					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnError, args);

					return;
				}

				if (type == "create_profile_failed")
				{
					SUIArguments args;

					if (reason == "0")
						args.AddArgument("@serverError");
					if (reason == "1")
						args.AddArgument("@nicknameAlredyRegister");
					if (reason == "2")
						args.AddArgument("@profileAlredyCreated");

					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnError, args);

					return;
				}

				if (type == "get_profile_failed")
				{
					SUIArguments args;

					if (reason == "0")
						args.AddArgument("@serverError");

					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnError, args);

					return;
				}

				if (type == "get_shop_items_failed")
				{
					SUIArguments args;

					if (reason == "0")
						args.AddArgument("@serverError");

					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnError, args);

					return;
				}

				if (type == "buy_item_failed")
				{
					SUIArguments args;

					if (reason == "0")
						args.AddArgument("@serverError");
					if (reason == "1")
						args.AddArgument("@itemNotFinded");
					if (reason == "2")
						args.AddArgument("@insufficientMoney");
					if (reason == "3")
						args.AddArgument("@serverError");
					if (reason == "4")
						args.AddArgument("@itemAlredyPurchased");
					if (reason == "5")
						args.AddArgument("@lowProfileLevel");

					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnError, args);

					return;
				}

				if (type == "remove_item_failed")
				{
					SUIArguments args;

					if (reason == "0")
						args.AddArgument("@serverError");
					if (reason == "1")
						args.AddArgument("@serverError");
					if (reason == "2")
						args.AddArgument("@serverError");
					if (reason == "3")
						args.AddArgument("@itemNotFound");

					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnError, args);

					return;
				}

				if (type == "invite_failed")
				{
					SUIArguments args;

					if (reason == "0")
						args.AddArgument("@userNotFound");
					if (reason == "1")
						args.AddArgument("@userNotOnline");
					if (reason == "2")
						args.AddArgument("@userDeclineInvite");

					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnError, args);

					return;
				}

				if (type == "add_friend_failed")
				{
					SUIArguments args;

					if (reason == "0")
						args.AddArgument("@friendNotFound");
					if (reason == "1")
						args.AddArgument("@serverError");
					if (reason == "2")
						args.AddArgument("@serverError");
					if (reason == "3")
						args.AddArgument("@cantAddYourselfInFriends");
					if (reason == "4")
						args.AddArgument("@friendAlredyAdded");

					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnError, args);

					return;
				}

				if (type == "remove_friend_failed")
				{
					SUIArguments args;

					if (reason == "0")
						args.AddArgument("@serverError");
					if (reason == "1")
						args.AddArgument("@serverError");
					if (reason == "2")
						args.AddArgument("@friendNotFound");

					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnError, args);

					return;
				}
			}

			break;
		}
	}
}

void CXmlWorker::onRegisterComplete(const char* data)
{
	QXmlStreamReader xml(data);
	xml.readNext();
	while (!xml.atEnd() && !xml.hasError())
	{
		xml.readNext();

		if (xml.name() == "data")
		{
			QXmlStreamAttributes attributes = xml.attributes();
			QString uid = attributes.value("uid").toString();

			if (!uid.isEmpty())
			{
				gCryModule->pUIEvents->SendEmptyEvent(CModuleUIEvents::eUIGE_OnRegComplete);
				gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Register complete!");
				return;
			}
			else
			{
				gEnv->pLog->LogWarning(TITLE ONLINE_TITLE "Register failed!");
				return;
			}
		}
	}
}

void CXmlWorker::onAuthComplete(const char* data)
{
	QXmlStreamReader xml(data);
	xml.readNext();
	while (!xml.atEnd() && !xml.hasError())
	{
		xml.readNext();

		if (xml.name() == "data")
		{
			QXmlStreamAttributes attributes = xml.attributes();
			QString uid = attributes.value("uid").toString();

			if (!uid.isEmpty())
			{
				gCryModule->pUIEvents->SendEmptyEvent(CModuleUIEvents::eUIGE_OnLoginComplete);
				gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Authorization complete!");
				return;
			}
			else
			{
				gEnv->pLog->LogWarning(TITLE ONLINE_TITLE "Authorization failed!");
				return;
			}
		}
	}
}

void CXmlWorker::onProfileDataRecived(const char*data)
{
	if (gCryModule->m_profile == nullptr)
		gCryModule->m_profile = new SProfile;

	SProfile* m_profile = gCryModule->m_profile;
	m_profile->items.clear();
	m_profile->friends.clear();

	int oldItemsCount = gCryModule->m_items.size();
	int oldFriendCount = gCryModule->m_friends.size();

	QVector<SItem> m_ItemsBuffer;
	QVector<SFriend> m_FriendsBuffer;

	QXmlStreamReader xml(data);
	xml.readNext();
	while (!xml.atEnd() && !xml.hasError())
	{
		xml.readNext();

		if (xml.name() == "profile")
		{
			QXmlStreamAttributes attributes = xml.attributes();
			if (!attributes.isEmpty())
			{
				int uid = attributes.value("id").toInt();
				QString nickname = attributes.value("nickname").toString();
				QString model = attributes.value("model").toString();
				int money = attributes.value("money").toInt();
				int xp = attributes.value("xp").toInt();
				int lvl = attributes.value("lvl").toInt();

				m_profile->uid = uid;
				m_profile->nickname = nickname;
				m_profile->model = model;
				m_profile->money = money;
				m_profile->xp = xp;
				m_profile->lvl = lvl;


				SUIArguments args;
				args.AddArgument(nickname.toStdString().c_str());
				args.AddArgument(model.toStdString().c_str());
				args.AddArgument(lvl);
				args.AddArgument(xp);
				args.AddArgument(money);

				gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnProfileDataRecived, args);

				gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Recived profile data. Nickname = %s, Model = %s, Lvl = %d, XP = %d, Money = %d", nickname.toStdString().c_str(), model.toStdString().c_str(), lvl, xp, money);
			}
		}

		if (xml.name() == "item")
		{
			QXmlStreamAttributes attributes = xml.attributes();
			if (!attributes.isEmpty())
			{
				QString name = attributes.value("name").toString();
				QString icon = attributes.value("icon").toString();
				QString description = attributes.value("description").toString();

				SItem item;
				item.name = name;
				item.icon = icon;
				item.description = description;

				//gCryModule->m_items.push_back(item);
				m_ItemsBuffer.push_back(item);

				m_profile->items = m_profile->items + "<item name='" + name + "' icon='" + icon + "' description='" + description + "'/>";	

				SUIArguments args;
				args.AddArgument(name.toStdString().c_str());
				args.AddArgument(icon.toStdString().c_str());
				args.AddArgument(description.toStdString().c_str());

				gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnInventoryItemRecived, args);

				gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Recived inventory item. Name = %s, Icon = %s, Decription = %s", name.toStdString().c_str(), icon.toStdString().c_str(), description.toStdString().c_str());
			}
		}

		if (xml.name() == "friend")
		{

			QXmlStreamAttributes attributes = xml.attributes();
			if (!attributes.isEmpty())
			{
				QString friendName = attributes.value("name").toString();
				QString friendUid = attributes.value("uid").toString();
				QString friendStatus = attributes.value("status").toString();

				SFriend m_friend;
				m_friend.nickname = friendName;
				m_friend.uid = friendUid.toInt();
				m_friend.status = friendStatus.toInt();

				//gCryModule->m_friends.push_back(m_friend);
				m_FriendsBuffer.push_back(m_friend);

				m_profile->friends = m_profile->friends + "<friend name='" + friendName + "' uid='" + friendUid + "' status='" + friendStatus + "'/>";

				gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Recived friend. Name = %s, uid = %d, status = %d", friendName.toStdString().c_str(), friendUid.toInt(), friendStatus.toInt());
			}
		}

		if (xml.name() == "stats")
		{
			QXmlStreamAttributes attributes = xml.attributes();
			if (!attributes.isEmpty())
			{
				int kills = attributes.value("kills").toInt();
				int deaths = attributes.value("deaths").toInt();
				QString kd = attributes.value("kd").toString();

				m_profile->stats = "<stat kills='" + QString::number(kills) + "' deaths='" + QString::number(deaths) + "' kd='" + kd + "'/>";

				SUIArguments args;
				args.AddArgument(kills);
				args.AddArgument(deaths);
				args.AddArgument(kd.toStdString().c_str());

				gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnStatsRecived, args);

				gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Recived character statistic. Kills = %d, Deaths = %d, KD = %s", kills, deaths, kd.toStdString().c_str());
			}
		}

		if (xml.name() == "achievement")
		{

			QXmlStreamAttributes attributes = xml.attributes();
			if (!attributes.isEmpty())
			{
				QString name = attributes.value("name").toString();
				QString icon = attributes.value("icon").toString();
				QString description = attributes.value("description").toString();

				gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Recived achievement . Name = %s, Icon = %s, Decription = %s", name.toStdString().c_str(), icon.toStdString().c_str(), description.toStdString().c_str());

			}
		}
	}

	if (m_ItemsBuffer.size() != oldItemsCount)
	{
		gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Items list need update!");

		gCryModule->m_items.clear();
		gCryModule->m_items = m_ItemsBuffer;
	}

	if (m_FriendsBuffer.size() != oldFriendCount)
	{
		gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Friend list need update!");
		gCryModule->pUIEvents->SendEmptyEvent(CModuleUIEvents::eUIGE_OnFriendListNeedUpdate);

		gCryModule->m_friends.clear();
		gCryModule->m_friends = m_FriendsBuffer;
		UpdateFriendList();
	}
}

void CXmlWorker::onShopItemsRecived(const char* data)
{
	QXmlStreamReader xml(data);
	xml.readNext();
	while (!xml.atEnd() && !xml.hasError())
	{
		xml.readNext();

		if (xml.name() == "item")
		{
			QXmlStreamAttributes attributes = xml.attributes();
			if (!attributes.isEmpty())
			{
				QString name = attributes.value("name").toString();
				QString icon = attributes.value("icon").toString();
				QString description = attributes.value("description").toString();
				int cost = attributes.value("cost").toInt();
				int minLvl = attributes.value("minLvl").toInt();

				SUIArguments args;
				args.AddArgument(name.toStdString().c_str());
				args.AddArgument(icon.toStdString().c_str());
				args.AddArgument(description.toStdString().c_str());
				args.AddArgument(cost);
				args.AddArgument(minLvl);

				gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnShopItemRecived, args);

				gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Recived shop item. Name = %s, Icon = %s, Description = %s, Cost = %d, MinLvl = %d", name.toStdString().c_str(), icon.toStdString().c_str(), description.toStdString().c_str(), cost, minLvl);
				return;
			}
		}
	}
}

void CXmlWorker::onChatMessageRecived(const char * data)
{
	QXmlStreamReader xml(data);
	xml.readNext();
	while (!xml.atEnd() && !xml.hasError())
	{
		xml.readNext();

		if (xml.name() == "message")
		{
			QXmlStreamAttributes attributes = xml.attributes();
			if (!attributes.isEmpty())
			{
				QString type = attributes.value("type").toString();
				QString message = attributes.value("message").toString();
				QString from = attributes.value("from").toString();

				if (type == "global")
				{
					/*SUIArguments args;
					args.AddArgument(message.toStdString().c_str());
					args.AddArgument(from.toStdString().c_str());
					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnGlobalChatMessageRecived, args);

					gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Recived global chat message (%s) from (%s)", message.toStdString().c_str(), from.toStdString().c_str());*/
					return;
				}

				if (type == "private")
				{
					QString complete = message.replace("#comma#", ",");

					SUIArguments args;
					args.AddArgument(complete.toStdString().c_str());
					args.AddArgument(from.toStdString().c_str());
					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnPrivateChatMessageRecived, args);

					gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Recived private chat message (%s) from (%s)", message.toStdString().c_str(), from.toStdString().c_str());
					return;
				}
				
			}
		}
	}
}

void CXmlWorker::onInvite(const char * data)
{
	QXmlStreamReader xml(data);

	while (!xml.atEnd() && !xml.hasError())
	{
		xml.readNext();

		if (xml.name() == "invite")
		{
			QXmlStreamAttributes attributes = xml.attributes();
			QString type = attributes.value("type").toString();
			QString from = attributes.value("from").toString();

			gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Invite type = '%s' From = '%s'", type.toStdString().c_str(), from.toStdString().c_str());

			if (!type.isEmpty() && !from.isEmpty())
			{
				if (type == "friend_invite")
				{
					SUIArguments args;
					args.AddArgument(from.toStdString().c_str());
					gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnFriendInviteRecived, args);

					gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "Recived friend invite from (%s)", from.toStdString().c_str());

					return;
				}
				if (type == "game_invite")
				{
				}
				if (type == "clan_invite")
				{
				}
			}
		}
	}
}

void CXmlWorker::onFriendStatusUpdated(const char * data)
{
	QXmlStreamReader xml(data);

	while (!xml.atEnd() && !xml.hasError())
	{
		xml.readNext();

		if (xml.name() == "online_status")
		{
			QXmlStreamAttributes attributes = xml.attributes();
			QString type = attributes.value("type").toString();
			QString from = attributes.value("from").toString();
			int status = attributes.value("status").toInt();

			if (!type.isEmpty() && !from.isEmpty())
			{
				SUIArguments args;
				args.AddArgument(from.toStdString().c_str());

				if (status == 0)
					args.AddArgument("offline");
				if (status == 1)
					args.AddArgument("online");
				if (status == 3)
					args.AddArgument("ingame");
				if (status == 4)
					args.AddArgument("afk");

				gCryModule->pUIEvents->SendEvent(CModuleUIEvents::eUIGE_OnFriendUpdateStatus, args);

				gEnv->pLog->LogAlways(TITLE ONLINE_TITLE "<%s> change online status to <%d>", from.toStdString().c_str(), status);
				return;
			}
			else
			{
				gEnv->pLog->LogWarning(TITLE ONLINE_TITLE "Failed update friend status, because some values empty!");
				return;
			}
		}
	}
}

