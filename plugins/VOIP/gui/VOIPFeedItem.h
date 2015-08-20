/****************************************************************
 *  RetroShare is distributed under the following license:
 *
 *  Copyright (C) 2015
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

#pragma once
#include "ui_VOIPFeedItem.h"

#include "gui/VOIPNotify.h"
#include "interface/rsVOIP.h"

#include "gui/feeds/FeedHolder.h"
#include "gui/feeds/FeedItem.h"

class VOIPFeedItem : public FeedItem, private Ui::VOIPFeedItem
{
	Q_OBJECT
public:
	typedef enum{ Accept
		            , BandwidthInfo
		            , Data
		            , HangUp
		            , Invitation
		            , AudioCall
		            , VideoCall
	} voipFeedItem_Type;

	VOIPFeedItem(RsVOIP *VOIP, VOIPNotify *notify, FeedHolder *parent, const RsPeerId &peerId, const QString &msg, const voipFeedItem_Type type);
	~VOIPFeedItem();

	void updateItemStatic();

	/* FeedItem */
	virtual void expand(bool open);

private slots:
	/* default stuff */
	void removeItem();
	void toggle();

	void sendMsg();
	void openChat();

	void updateItem();

	void voipAcceptReceived(const RsPeerId& peer_id) ; // emitted when the peer accepts the call
	void voipBandwidthInfoReceived(const RsPeerId &peer_id, int bytes_per_sec) ; // emitted when measured bandwidth info is received by the peer.
	void voipDataReceived(const RsPeerId& peer_id) ;			// signal emitted when some voip data has been received
	void voipHangUpReceived(const RsPeerId& peer_id) ; // emitted when the peer closes the call (i.e. hangs up)
	void voipInvitationReceived(const RsPeerId& peer_id) ;	// signal emitted when an invitation has been received
	void voipAudioCallReceived(const RsPeerId &peer_id) ; // emitted when the peer is calling and own don't send audio
	void voipVideoCallReceived(const RsPeerId &peer_id) ; // emitted when the peer is calling and own don't send video

private:

	RsVOIP *mVOIP;
	VOIPNotify *mNotify;
	FeedHolder *mParent;

	RsPeerId mPeerId;
	QString mMsg;
	voipFeedItem_Type mType;

};
