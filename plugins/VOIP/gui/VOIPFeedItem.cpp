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
#include "VOIPFeedItem.h"
#include "ui_VOIPFeedItem.h"

/*libRetroshare*/
#include "retroshare/rspeers.h"

/*Retroshare-Gui*/
#include "gui/notifyqt.h"
#include "gui/common/StatusDefs.h"
#include "gui/msgs/MessageComposer.h"
#include "gui/notifyqt.h"
#include "retroshare-gui/RsAutoUpdatePage.h"
#include "util/DateTime.h"

#include <QDateTime>
#include <QTimer>


#define IMAGE_VOIPFEEDITEM ":/images/talking_on.svg"

/** Constructor */
VOIPFeedItem::VOIPFeedItem(RsVOIP *VOIP, VOIPNotify *notify, FeedHolder *parent, const RsPeerId &peerId, const QString &msg, const voipFeedItem_Type type)
  : FeedItem(NULL), mVOIP(VOIP), mNotify(notify), mParent(parent), mPeerId(peerId), mMsg(msg), mType(type)
{
	/* Invoke the Qt Designer generated object setup routine */
	setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose, true);

	sendmsgButton->setEnabled(false);

	/* general ones */
	connect( expandButton, SIGNAL( clicked( void ) ), this, SLOT( toggle ( void ) ) );
	connect( clearButton, SIGNAL( clicked( void ) ), this, SLOT( removeItem ( void ) ) );

	/* specific ones */
	connect( chatButton, SIGNAL( clicked( void ) ), this, SLOT( openChat ( void ) ) );
	connect( sendmsgButton, SIGNAL( clicked( ) ), this, SLOT( sendMsg() ) );

	connect(NotifyQt::getInstance(), SIGNAL(friendsChanged()), this, SLOT(updateItem()));

	/* set informations */
	avatar->setId(ChatId(mPeerId));

	expandFrame->hide();

	updateItemStatic();
	updateItem();

	if (mType == Accept) connect(mNotify, SIGNAL(voipAcceptReceived(const RsPeerId&)), this, SLOT(voipAcceptReceived(const RsPeerId&)), Qt::QueuedConnection);
	if (mType == BandwidthInfo) connect(mNotify, SIGNAL(voipBandwidthInfoReceived(const RsPeerId&, int)), this, SLOT(voipBandwidthInfoReceived(const RsPeerId&,int)), Qt::QueuedConnection);
	if (mType == Data) connect(mNotify, SIGNAL(voipDataReceived(const RsPeerId&)), this, SLOT(voipDataReceived(const RsPeerId&)), Qt::QueuedConnection);
	if (mType == HangUp) connect(mNotify, SIGNAL(voipHangUpReceived(const RsPeerId&)), this, SLOT(voipHangUpReceived(const RsPeerId&)), Qt::QueuedConnection);
	if (mType == Invitation) connect(mNotify, SIGNAL(voipInvitationReceived(const RsPeerId&)), this, SLOT(voipInvitationReceived(const RsPeerId&)), Qt::QueuedConnection);
	if (mType == AudioCall) connect(mNotify, SIGNAL(voipAudioCallReceived(const RsPeerId&)), this, SLOT(voipAudioCallReceived(const RsPeerId&)), Qt::QueuedConnection);
	if (mType == VideoCall) connect(mNotify, SIGNAL(voipVideoCallReceived(const RsPeerId&)), this, SLOT(voipVideoCallReceived(const RsPeerId&)), Qt::QueuedConnection);

}

VOIPFeedItem::~VOIPFeedItem()
{
}

void VOIPFeedItem::updateItemStatic()
{
	if (!rsPeers)
		return;

	/* fill in */
#ifdef DEBUG_ITEM
	std::cerr << "PeerItem::updateItemStatic()";
	std::cerr << std::endl;
#endif

	titleLabel->setText(mMsg);

	RsPeerDetails details;
	if (rsPeers->getPeerDetails(mPeerId, details))
	{
		/* set peer name */
		peerNameLabel->setText(QString::fromUtf8(details.name.c_str()));
		lastLabel-> setText(DateTime::formatLongDateTime(details.lastConnect));

		/* expanded Info */
		nameLabel->setText(QString::fromUtf8(details.name.c_str()));
		idLabel->setText(QString::fromStdString(details.id.toStdString()));
		locLabel->setText(QString::fromUtf8(details.location.c_str()));
	}
	else
	{
		statusLabel->setText(tr("Unknown Peer"));
		titleLabel->setText(tr("Unknown Peer"));
		trustLabel->setText(tr("Unknown Peer"));
		nameLabel->setText(tr("Unknown Peer"));
		idLabel->setText(tr("Unknown Peer"));
		locLabel->setText(tr("Unknown Peer"));
		ipLabel->setText(tr("Unknown Peer"));
		connLabel->setText(tr("Unknown Peer"));
		lastLabel->setText(tr("Unknown Peer"));

		chatButton->setEnabled(false);
	}

	loLabel->setText(DateTime::formatLongDateTime(QDateTime::currentDateTime()));
}

void VOIPFeedItem::updateItem()
{
	if (!rsPeers)
		return;

	/* fill in */
	if(!RsAutoUpdatePage::eventsLocked()) {
		RsPeerDetails details;
		if (!rsPeers->getPeerDetails(mPeerId, details))
		{
			chatButton->setEnabled(false);
			sendmsgButton->setEnabled(false);

			return;
		}

		/* top Level info */
		QString status = StatusDefs::peerStateString(details.state);

		statusLabel->setText(status);
		trustLabel->setText(QString::fromStdString(RsPeerTrustString(details.trustLvl)));

		ipLabel->setText(QString("%1:%2/%3:%4").arg(QString::fromStdString(details.localAddr)).arg(details.localPort).arg(QString::fromStdString(details.extAddr)).arg(details.extPort));

		connLabel->setText(StatusDefs::connectStateString(details));

		/* do buttons */
		chatButton->setEnabled(details.state & RS_PEER_STATE_CONNECTED);
		if (details.state & RS_PEER_STATE_FRIEND)
		{
			sendmsgButton->setEnabled(true);
		}
		else
		{
			sendmsgButton->setEnabled(false);
		}
	}

	/* slow Tick  */
	int msec_rate = 10129;

	QTimer::singleShot( msec_rate, this, SLOT(updateItem( void ) ));
	return;
}

void VOIPFeedItem::toggle()
{
	expand(expandFrame->isHidden());
}

void VOIPFeedItem::expand(bool open)
{
	if (mParent) {
		mParent->lockLayout(this, true);
	}

	if (open)
	{
		expandFrame->show();
		expandButton->setIcon(QIcon(QString(":/images/edit_remove24.png")));
		expandButton->setToolTip(tr("Hide"));
	}
	else
	{
		expandFrame->hide();
		expandButton->setIcon(QIcon(QString(":/images/edit_add24.png")));
		expandButton->setToolTip(tr("Expand"));
	}

	emit sizeChanged(this);

	if (mParent) {
		mParent->lockLayout(this, false);
	}
}

void VOIPFeedItem::removeItem()
{
	mParent->lockLayout(this, true);
	hide();
	mParent->lockLayout(this, false);

	if (mParent) {
		mParent->deleteFeedItem(this, 0);
	}
}

/*********** SPECIFIC FUNCTIONS ***********************/

void VOIPFeedItem::sendMsg()
{
	MessageComposer *nMsgDialog = MessageComposer::newMsg();
	if (nMsgDialog == NULL) {
		return;
	}

	nMsgDialog->addRecipient(MessageComposer::TO, mPeerId);
	nMsgDialog->show();
	nMsgDialog->activateWindow();

	/* window will destroy itself! */
}

void VOIPFeedItem::openChat()
{
	if (mParent)
	{
		mParent->openChat((RsPeerId)mPeerId);
	}
}

void VOIPFeedItem::voipAcceptReceived(const RsPeerId &)
{
	if (mType == Accept) loLabel->setText(DateTime::formatLongDateTime(QDateTime::currentDateTime()));
}

void VOIPFeedItem::voipBandwidthInfoReceived(const RsPeerId &, int )
{
	if (mType == BandwidthInfo) loLabel->setText(DateTime::formatLongDateTime(QDateTime::currentDateTime()));
}

void VOIPFeedItem::voipDataReceived(const RsPeerId &)
{
	if (mType == Data) loLabel->setText(DateTime::formatLongDateTime(QDateTime::currentDateTime()));
}

void VOIPFeedItem::voipHangUpReceived(const RsPeerId &)
{
	if (mType == HangUp) loLabel->setText(DateTime::formatLongDateTime(QDateTime::currentDateTime()));
}

void VOIPFeedItem::voipInvitationReceived(const RsPeerId &)
{
	if (mType == Invitation) loLabel->setText(DateTime::formatLongDateTime(QDateTime::currentDateTime()));
}

void VOIPFeedItem::voipAudioCallReceived(const RsPeerId &)
{
	if (mType == AudioCall) loLabel->setText(DateTime::formatLongDateTime(QDateTime::currentDateTime()));
}

void VOIPFeedItem::voipVideoCallReceived(const RsPeerId &)
{
	if (mType == VideoCall) loLabel->setText(DateTime::formatLongDateTime(QDateTime::currentDateTime()));
}
