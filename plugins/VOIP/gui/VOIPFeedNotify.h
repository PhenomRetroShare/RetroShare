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

#include "gui/VOIPNotify.h"
#include "interface/rsVOIP.h"

#include "gui/common/FeedNotify.h"

#include <QMutex>

//#define VOIPFEEDNOTIFY_ALL //To get all feeds

class VOIPFeedNotify : public FeedNotify
{
	Q_OBJECT

protected:
	class FeedItemData
	{
	public:
		FeedItemData() {}

	public:
		RsPeerId mPeerId;
		QString mMsg;
	};

public:
	VOIPFeedNotify(RsVOIP *VOIP, VOIPNotify *notify, QObject *parent = 0);
	~VOIPFeedNotify();

	/// From FeedNotify ///
	virtual bool hasSettings(QString &mainName, QMap<QString,QString> &tagAndTexts);
	virtual bool notifyEnabled(QString tag);
	virtual void setNotifyEnabled(QString tag, bool enabled);
	virtual FeedItem *feedItem(FeedHolder *parent);
	virtual FeedItem *testFeedItem(QString tag, FeedHolder *parent);

private slots:
#ifdef VOIPFEEDNOTIFY_ALL
	void voipAcceptReceived(const RsPeerId& peer_id) ; // emitted when the peer accepts the call
	void voipBandwidthInfoReceived(const RsPeerId& peer_id,int bytes_per_sec) ; // emitted when measured bandwidth info is received by the peer.
	void voipDataReceived(const RsPeerId&) ;			// signal emitted when some voip data has been received
	void voipHangUpReceived(const RsPeerId& peer_id) ; // emitted when the peer closes the call (i.e. hangs up)
	void voipInvitationReceived(const RsPeerId&) ;	// signal emitted when an invitation has been received
#endif
	void voipAudioCallReceived(const RsPeerId &peer_id) ; // emitted when the peer is calling and own don't send audio
	void voipVideoCallReceived(const RsPeerId &peer_id) ; // emitted when the peer is calling and own don't send video

#ifdef VOIPFEEDNOTIFY_ALL
	void feedItemDestroyedAccept(FeedItem *feedItem) ;
	void feedItemDestroyedBandwidthInfo(FeedItem *feedItem) ;
	void feedItemDestroyedData(FeedItem *feedItem) ;
	void feedItemDestroyedHangUp(FeedItem *feedItem) ;
	void feedItemDestroyedInvitation(FeedItem *feedItem) ;
#endif
	void feedItemDestroyedAudioCall(FeedItem *feedItem) ;
	void feedItemDestroyedVideoCall(FeedItem *feedItem) ;

private:
	RsVOIP *mVOIP;
	VOIPNotify *mVOIPNotify;

	QMutex *mMutex;
#ifdef VOIPFEEDNOTIFY_ALL
	QList<FeedItemData> mPendingNewsFeedAccept;
	QList<FeedItemData> mPendingNewsFeedBandwidthInfo;
	QList<FeedItemData> mPendingNewsFeedData;
	QList<FeedItemData> mPendingNewsFeedHangUp;
	QList<FeedItemData> mPendingNewsFeedInvitation;
#endif
	QList<FeedItemData> mPendingNewsFeedAudioCall;
	QList<FeedItemData> mPendingNewsFeedVideoCall;

#ifdef VOIPFEEDNOTIFY_ALL
	QMap<RsPeerId, FeedItem *> mNewsFeedAccept;
	QMap<RsPeerId, FeedItem *> mNewsFeedBandwidthInfo;
	QMap<RsPeerId, FeedItem *> mNewsFeedData;
	QMap<RsPeerId, FeedItem *> mNewsFeedHangUp;
	QMap<RsPeerId, FeedItem *> mNewsFeedInvitation;
#endif
	QMap<RsPeerId, FeedItem *> mNewsFeedAudioCall;
	QMap<RsPeerId, FeedItem *> mNewsFeedVideoCall;
};
