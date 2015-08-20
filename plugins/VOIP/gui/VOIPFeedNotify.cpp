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

/*VOIP*/
#include "gui/VOIPFeedNotify.h"
#include "gui/VOIPFeedItem.h"

/*retroshare-gui*/
#include "gui/settings/rsharesettings.h"

/*libretroshare*/
#include "retroshare/rspeers.h"

VOIPFeedNotify::VOIPFeedNotify(RsVOIP *VOIP, VOIPNotify *notify, QObject *parent)
  : FeedNotify(parent), mVOIP(VOIP), mVOIPNotify(notify)
{
	mMutex = new QMutex();

#ifdef VOIPFEEDNOTIFY_ALL
	connect(mVOIPNotify, SIGNAL(voipAcceptReceived(const RsPeerId&)), this, SLOT(voipAcceptReceived(const RsPeerId&)), Qt::QueuedConnection);
	connect(mVOIPNotify, SIGNAL(voipBandwidthInfoReceived(const RsPeerId&, int)), this, SLOT(voipBandwidthInfoReceived(RsPeerId&, int)), Qt::QueuedConnection);
	connect(mVOIPNotify, SIGNAL(voipDataReceived(const RsPeerId&)), this, SLOT(voipDataReceived(const RsPeerId&)), Qt::QueuedConnection);
	connect(mVOIPNotify, SIGNAL(voipHangUpReceived(const RsPeerId&)), this, SLOT(voipHangUpReceived(const RsPeerId&)), Qt::QueuedConnection);
	connect(mVOIPNotify, SIGNAL(voipInvitationReceived(const RsPeerId&)), this, SLOT(voipInvitationReceived(const RsPeerId&)), Qt::QueuedConnection);
#endif
	connect(mVOIPNotify, SIGNAL(voipAudioCallReceived(const RsPeerId&)), this, SLOT(voipAudioCallReceived(const RsPeerId&)), Qt::QueuedConnection);
	connect(mVOIPNotify, SIGNAL(voipVideoCallReceived(const RsPeerId&)), this, SLOT(voipVideoCallReceived(const RsPeerId&)), Qt::QueuedConnection);
}

VOIPFeedNotify::~VOIPFeedNotify()
{
	delete(mMutex);
}

bool VOIPFeedNotify::hasSettings(QString &mainName, QMap<QString, QString> &tagAndTexts)
{
	mainName = tr("VOIP");
#ifdef VOIPFEEDNOTIFY_ALL
	tagAndTexts.insert("Accept", tr("Accept"));
	tagAndTexts.insert("BandwidthInfo", tr("Bandwidth Information"));
	tagAndTexts.insert("Data", tr("Audio or Video Data"));
	tagAndTexts.insert("HangUp", tr("HangUp"));
	tagAndTexts.insert("Invitation", tr("Invitation"));
#endif
	tagAndTexts.insert("AudioCall"    , tr("Audio Call"));
	tagAndTexts.insert("VideoCall"    , tr("Video Call"));
	return true;
}

bool VOIPFeedNotify::notifyEnabled(QString tag)
{
	return Settings->valueFromGroup("VOIP", QString("FeedNotifyEnable").append(tag), false).toBool();
}

void VOIPFeedNotify::setNotifyEnabled(QString tag, bool enabled)
{
	Settings->setValueToGroup("VOIP", QString("FeedNotifyEnable").append(tag), enabled);

	if (!enabled) {
		/* remove pending feed items */
		mMutex->lock();

#ifdef VOIPFEEDNOTIFY_ALL
		if(tag == "Accept") mPendingNewsFeedAccept.clear();
		if(tag == "BandwidthInfo") mPendingNewsFeedBandwidthInfo.clear();
		if(tag == "Data") mPendingNewsFeedData.clear();
		if(tag == "HangUp") mPendingNewsFeedHangUp.clear();
		if(tag == "Invitation") mPendingNewsFeedInvitation.clear();
#endif
		if(tag == "AudioCall") mPendingNewsFeedAudioCall.clear();
		if(tag == "VideoCall") mPendingNewsFeedVideoCall.clear();

		mMutex->unlock();
	}
}

FeedItem *VOIPFeedNotify::feedItem(FeedHolder *parent)
{
	VOIPFeedItem *feedItem = NULL;

#ifdef VOIPFEEDNOTIFY_ALL
	if (!mPendingNewsFeedAccept.empty() && !feedItem) {
		mMutex->lock();
		FeedItemData feedItemData = mPendingNewsFeedAccept.takeFirst();
		feedItem = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, feedItemData.mPeerId, feedItemData.mMsg, VOIPFeedItem::Accept);
		connect(feedItem, SIGNAL(feedItemDestroyed(FeedItem*)), this, SLOT(feedItemDestroyedAccept(FeedItem*)));
		mNewsFeedAccept.insert(feedItemData.mPeerId, feedItem);
		mMutex->unlock();
	}
	if (!mPendingNewsFeedBandwidthInfo.empty() && !feedItem) {
		mMutex->lock();
		FeedItemData feedItemData = mPendingNewsFeedBandwidthInfo.takeFirst();
		feedItem = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, feedItemData.mPeerId, feedItemData.mMsg, VOIPFeedItem::BandwidthInfo);
		connect(feedItem, SIGNAL(feedItemDestroyed(FeedItem*)), this, SLOT(feedItemDestroyedBandwidthInfo(FeedItem*)));
		mNewsFeedBandwidthInfo.insert(feedItemData.mPeerId, feedItem);
		mMutex->unlock();
	}
	if (!mPendingNewsFeedData.empty() && !feedItem) {
		mMutex->lock();
		FeedItemData feedItemData = mPendingNewsFeedData.takeFirst();
		feedItem = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, feedItemData.mPeerId, feedItemData.mMsg, VOIPFeedItem::Data);
		connect(feedItem, SIGNAL(feedItemDestroyed(FeedItem*)), this, SLOT(feedItemDestroyedData(FeedItem*)));
		mNewsFeedData.insert(feedItemData.mPeerId, feedItem);
		mMutex->unlock();
	}
	if (!mPendingNewsFeedHangUp.empty() && !feedItem) {
		mMutex->lock();
		FeedItemData feedItemData = mPendingNewsFeedHangUp.takeFirst();
		feedItem = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, feedItemData.mPeerId, feedItemData.mMsg, VOIPFeedItem::HangUp);
		connect(feedItem, SIGNAL(feedItemDestroyed(FeedItem*)), this, SLOT(feedItemDestroyedHangUp(FeedItem*)));
		mNewsFeedHangUp.insert(feedItemData.mPeerId, feedItem);
		mMutex->unlock();
	}
	if (!mPendingNewsFeedInvitation.empty() && !feedItem) {
		mMutex->lock();
		FeedItemData feedItemData = mPendingNewsFeedInvitation.takeFirst();
		feedItem = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, feedItemData.mPeerId, feedItemData.mMsg, VOIPFeedItem::Invitation);
		connect(feedItem, SIGNAL(feedItemDestroyed(FeedItem*)), this, SLOT(feedItemDestroyedInvitation(FeedItem*)));
		mNewsFeedInvitation.insert(feedItemData.mPeerId, feedItem);
		mMutex->unlock();
	}
#endif
	if (!mPendingNewsFeedAudioCall.empty() && !feedItem) {
		mMutex->lock();
		FeedItemData feedItemData = mPendingNewsFeedAudioCall.takeFirst();
		feedItem = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, feedItemData.mPeerId, feedItemData.mMsg, VOIPFeedItem::AudioCall);
		connect(feedItem, SIGNAL(feedItemDestroyed(FeedItem*)), this, SLOT(feedItemDestroyedAudioCall(FeedItem*)));
		mNewsFeedAudioCall.insert(feedItemData.mPeerId, feedItem);
		mMutex->unlock();
	}
	if (!mPendingNewsFeedVideoCall.empty() && !feedItem) {
		mMutex->lock();
		FeedItemData feedItemData = mPendingNewsFeedVideoCall.takeFirst();
		feedItem = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, feedItemData.mPeerId, feedItemData.mMsg, VOIPFeedItem::VideoCall);
		connect(feedItem, SIGNAL(feedItemDestroyed(FeedItem*)), this, SLOT(feedItemDestroyedVideoCall(FeedItem*)));
		mNewsFeedAudioCall.insert(feedItemData.mPeerId, feedItem);
		mMutex->unlock();
	}

	return feedItem;
}

FeedItem* VOIPFeedNotify::testFeedItem(QString tag, FeedHolder *parent)
{
	FeedItem* feed = NULL;
	RsPeerId ownId = rsPeers->getOwnId();
#ifdef VOIPFEEDNOTIFY_ALL
	if (tag == "Accept") feed = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, ownId, tr("Test VOIP Accept"), VOIPFeedItem::Accept);
	if (tag == "BandwidthInfo") feed = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, ownId, tr("Test VOIP BandwidthInfo"), VOIPFeedItem::BandwidthInfo);
	if (tag == "Data") feed = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, ownId, tr("Test VOIP Data"), VOIPFeedItem::Data);
	if (tag == "HangUp") feed = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, ownId, tr("Test VOIP HangUp"), VOIPFeedItem::HangUp);
	if (tag == "Invitation") feed = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, ownId, tr("Test VOIP Invitation"), VOIPFeedItem::Invitation);
#endif
	if (tag == "AudioCall") feed = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, ownId, tr("Test VOIP Audio Call"), VOIPFeedItem::AudioCall);
	if (tag == "VideoCall") feed = new VOIPFeedItem(mVOIP, mVOIPNotify, parent, ownId, tr("Test VOIP Video Call"), VOIPFeedItem::VideoCall);

	return feed;
}

#ifdef VOIPFEEDNOTIFY_ALL
void VOIPFeedNotify::voipAcceptReceived(const RsPeerId &peer_id)
{
	if (peer_id.isNull()) {
		return;
	}

	if (!notifyEnabled("Accept")) {
		return;
	}

	mMutex->lock();

	if (!mNewsFeedAccept.contains(peer_id)){
		FeedItemData feedItemData;
		feedItemData.mPeerId = peer_id;
		feedItemData.mMsg = tr("Accept received from this peer.");

		mPendingNewsFeedAccept.push_back(feedItemData);
		mNewsFeedAccept.insert(peer_id, NULL);
	}

	mMutex->unlock();
}

void VOIPFeedNotify::voipBandwidthInfoReceived(const RsPeerId &peer_id,int bytes_per_sec)
{
	if (peer_id.isNull()) {
		return;
	}

	if (!notifyEnabled("BandwidthInfo")) {
		return;
	}

	mMutex->lock();

	if (!mNewsFeedBandwidthInfo.contains(peer_id)){
		FeedItemData feedItemData;
		feedItemData.mPeerId = peer_id;
		feedItemData.mMsg = tr("Bandwidth Info received from this peer:%1").arg(bytes_per_sec);

		mPendingNewsFeedBandwidthInfo.push_back(feedItemData);
		mNewsFeedBandwidthInfo.insert(peer_id, NULL);
	}

	mMutex->unlock();
}

void VOIPFeedNotify::voipDataReceived(const RsPeerId &peer_id)
{
	if (peer_id.isNull()) {
		return;
	}

	if (!notifyEnabled("Data")) {
		return;
	}

	mMutex->lock();

	if (!mNewsFeedData.contains(peer_id)){
		FeedItemData feedItemData;
		feedItemData.mPeerId = peer_id;
		feedItemData.mMsg = tr("Audio or Video Data received from this peer.");

		mPendingNewsFeedData.push_back(feedItemData);
		mNewsFeedData.insert(peer_id, NULL);
	}

	mMutex->unlock();
}

void VOIPFeedNotify::voipHangUpReceived(const RsPeerId &peer_id)
{
	if (peer_id.isNull()) {
		return;
	}

	if (!notifyEnabled("HangUp")) {
		return;
	}

	mMutex->lock();

	if (!mNewsFeedHangUp.contains(peer_id)){
		FeedItemData feedItemData;
		feedItemData.mPeerId = peer_id;
		feedItemData.mMsg = tr("HangUp received from this peer.");

		mPendingNewsFeedHangUp.push_back(feedItemData);
		mNewsFeedHangUp.insert(peer_id, NULL);
	}

	mMutex->unlock();
}

void VOIPFeedNotify::voipInvitationReceived(const RsPeerId &peer_id)
{
	if (peer_id.isNull()) {
		return;
	}

	if (!notifyEnabled("Invitation")) {
		return;
	}

	mMutex->lock();

	if (!mNewsFeedInvitation.contains(peer_id)){
		FeedItemData feedItemData;
		feedItemData.mPeerId = peer_id;
		feedItemData.mMsg = tr("Invitation received from this peer.");

		mPendingNewsFeedInvitation.push_back(feedItemData);
		mNewsFeedInvitation.insert(peer_id, NULL);
	}

	mMutex->unlock();
}
#endif

void VOIPFeedNotify::voipAudioCallReceived(const RsPeerId &peer_id)
{
	if (peer_id.isNull()) {
		return;
	}

	if (!notifyEnabled("AudioCall")) {
		return;
	}

	mMutex->lock();

	if (!mNewsFeedAudioCall.contains(peer_id)){
		FeedItemData feedItemData;
		feedItemData.mPeerId = peer_id;
		feedItemData.mMsg = tr("Audio Call received from this peer.");

		mPendingNewsFeedAudioCall.push_back(feedItemData);
		mNewsFeedAudioCall.insert(peer_id, NULL);
	}

	mMutex->unlock();
}

void VOIPFeedNotify::voipVideoCallReceived(const RsPeerId &peer_id)
{
	if (peer_id.isNull()) {
		return;
	}

	if (!notifyEnabled("VideoCall")) {
		return;
	}

	mMutex->lock();

	if (!mNewsFeedVideoCall.contains(peer_id)){
		FeedItemData feedItemData;
		feedItemData.mPeerId = peer_id;
		feedItemData.mMsg = tr("Video Call received from this peer.");

		mPendingNewsFeedVideoCall.push_back(feedItemData);
		mNewsFeedVideoCall.insert(peer_id, NULL);
	}

	mMutex->unlock();
}

#ifdef VOIPFEEDNOTIFY_ALL
void VOIPFeedNotify::feedItemDestroyedAccept(FeedItem *feedItem)
{
	RsPeerId key = mNewsFeedAccept.key(feedItem, RsPeerId());
	if (!key.isNull()) mNewsFeedAccept.remove(key);
}

void VOIPFeedNotify::feedItemDestroyedBandwidthInfo(FeedItem *feedItem)
{
	RsPeerId key = mNewsFeedBandwidthInfo.key(feedItem, RsPeerId());
	if (!key.isNull()) mNewsFeedBandwidthInfo.remove(key);
}

void VOIPFeedNotify::feedItemDestroyedData(FeedItem *feedItem)
{
	RsPeerId key = mNewsFeedData.key(feedItem, RsPeerId());
	if (!key.isNull()) mNewsFeedData.remove(key);
}

void VOIPFeedNotify::feedItemDestroyedHangUp(FeedItem *feedItem)
{
	RsPeerId key = mNewsFeedHangUp.key(feedItem, RsPeerId());
	if (!key.isNull()) mNewsFeedHangUp.remove(key);
}

void VOIPFeedNotify::feedItemDestroyedInvitation(FeedItem *feedItem)
{
	RsPeerId key = mNewsFeedInvitation.key(feedItem, RsPeerId());
	if (!key.isNull()) mNewsFeedInvitation.remove(key);
}
#endif

void VOIPFeedNotify::feedItemDestroyedAudioCall(FeedItem *feedItem)
{
	RsPeerId key = mNewsFeedAudioCall.key(feedItem, RsPeerId());
	if (!key.isNull()) mNewsFeedAudioCall.remove(key);
}

void VOIPFeedNotify::feedItemDestroyedVideoCall(FeedItem *feedItem)
{
	RsPeerId key = mNewsFeedVideoCall.key(feedItem, RsPeerId());
	if (!key.isNull()) mNewsFeedVideoCall.remove(key);
}
