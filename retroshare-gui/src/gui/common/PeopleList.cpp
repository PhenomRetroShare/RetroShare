/*
 * Retroshare Identity.
 *
 * Copyright 2012-2012 by Robert Fernie.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License Version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * Please report all bugs and problems to "retroshare@lunamutt.com".
 *
 */

#include <QMessageBox>
#include <QMenu>

#include "PeopleList.h"
#include "ui_PeopleList.h"
#include "gui/Identity/IdEditDialog.h"
#include "gui/Identity/IdDetailsDialog.h"
#include "gui/gxs/GxsIdDetails.h"
#include "gui/common/UIStateHelper.h"
#include "gui/chat/ChatDialog.h"
#include "gui/settings/rsharesettings.h"
#include "gui/msgs/MessageComposer.h"

#include <retroshare/rspeers.h>
#include "retroshare/rsgxsflags.h"
#include "retroshare/rsmsgs.h" 
#include <iostream>
#include <algorithm>


// Data Requests.
#define IDDIALOG_IDLIST     1

/****************************************************************
 */

#define RSID_COL_NICKNAME   0
#define RSID_COL_KEYID      1
#define RSID_COL_LASTACTIVITY     2
#define RSID_COL_IDTYPE     3

#define RSID_FILTER_OWNED_BY_YOU 0x0001
#define RSID_FILTER_FRIENDS      0x0002
#define RSID_FILTER_OTHERS       0x0004
#define RSID_FILTER_PSEUDONYMS   0x0008
#define RSID_FILTER_YOURSELF     0x0010
#define RSID_FILTER_ALL          0xffff

#define IMAGE_EDIT                 ":/images/edit_16.png"

/** Constructor */
PeopleList::PeopleList(QWidget *parent) :
    RsGxsUpdateBroadcastPage(rsIdentity, parent),
    ui(new Ui::PeopleList)
{
	ui->setupUi(this);

	mIdQueue = NULL;

	/* Setup UI helper */
	mStateHelper = new UIStateHelper(this);
//	mStateHelper->addWidget(IDDIALOG_IDLIST, ui->idTreeWidget);
	mStateHelper->addLoadPlaceholder(IDDIALOG_IDLIST, ui->idTreeWidget, false);
	mStateHelper->addClear(IDDIALOG_IDLIST, ui->idTreeWidget);


	/* Connect signals */
	connect(ui->toolButton_NewId, SIGNAL(clicked()), this, SLOT(addIdentity()));

	connect(ui->removeIdentity, SIGNAL(triggered()), this, SLOT(removeIdentity()));
	connect(ui->editIdentity, SIGNAL(triggered()), this, SLOT(editIdentity()));
	connect(ui->chatIdentity, SIGNAL(triggered()), this, SLOT(chatIdentity()));

	connect(ui->idTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
	connect(ui->idTreeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(IdListCustomPopupMenu(QPoint)));

	connect(ui->filterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterComboBoxChanged()));
	connect(ui->filterLineEdit, SIGNAL(textChanged(QString)), this, SLOT(filterChanged(QString)));

	/* Add filter types */
    ui->filterComboBox->addItem(tr("All"), RSID_FILTER_ALL);
    ui->filterComboBox->addItem(tr("Owned by you"), RSID_FILTER_OWNED_BY_YOU);
    ui->filterComboBox->addItem(tr("Linked to your node"), RSID_FILTER_YOURSELF);
    ui->filterComboBox->addItem(tr("Linked to neighbor nodes"), RSID_FILTER_FRIENDS);
    ui->filterComboBox->addItem(tr("Linked to distant nodes"), RSID_FILTER_OTHERS);
    ui->filterComboBox->addItem(tr("Anonymous"), RSID_FILTER_PSEUDONYMS);
	ui->filterComboBox->setCurrentIndex(0);
	
	ui->filterButton->setVisible(false);

	/* Add filter actions */
	QTreeWidgetItem *headerItem = ui->idTreeWidget->headerItem();
	QString headerText = headerItem->text(RSID_COL_NICKNAME);
	ui->filterLineEdit->addFilter(QIcon(), headerText, RSID_COL_NICKNAME, QString("%1 %2").arg(tr("Search"), headerText));
	headerText = headerItem->text(RSID_COL_KEYID);
	ui->filterLineEdit->addFilter(QIcon(), headerItem->text(RSID_COL_KEYID), RSID_COL_KEYID, QString("%1 %2").arg(tr("Search"), headerText));

	/* Setup tree */
	ui->idTreeWidget->sortByColumn(RSID_COL_NICKNAME, Qt::AscendingOrder);

	ui->idTreeWidget->enableColumnCustomize(true);
	ui->idTreeWidget->setColumnCustomizable(RSID_COL_NICKNAME, false);
	
	ui->idTreeWidget->setColumnHidden(RSID_COL_KEYID, true);
	ui->idTreeWidget->setColumnHidden(RSID_COL_LASTACTIVITY, true);

	/* Set initial column width */
	int fontWidth = QFontMetricsF(ui->idTreeWidget->font()).width("W");
	ui->idTreeWidget->setColumnWidth(RSID_COL_NICKNAME, 18 * fontWidth);
	ui->idTreeWidget->setColumnWidth(RSID_COL_KEYID, 25 * fontWidth);
	ui->idTreeWidget->setColumnWidth(RSID_COL_IDTYPE, 18 * fontWidth);

	mIdQueue = new TokenQueue(rsIdentity->getTokenService(), this);


}

PeopleList::~PeopleList()
{
	// save settings
	processSettings(false);

	delete(ui);
	delete(mIdQueue);
}

void PeopleList::processSettings(bool load)
{
	Settings->beginGroup("PeopleList");

	// state of peer tree
	ui->idTreeWidget->processSettings(load);

	if (load) {
		// load settings

		// filterColumn
		ui->filterLineEdit->setCurrentFilter(Settings->value("filterColumn", RSID_COL_NICKNAME).toInt());

		// state of splitter
		//ui->splitter->restoreState(Settings->value("splitter").toByteArray());
	} else {
		// save settings

		// filterColumn
		Settings->setValue("filterColumn", ui->filterLineEdit->currentFilter());

		// state of splitter
		//Settings->setValue("splitter", ui->splitter->saveState());
	}

	Settings->endGroup();
}

void PeopleList::filterComboBoxChanged()
{
	requestIdList();
}

void PeopleList::filterChanged(const QString& /*text*/)
{
	filterIds();
}

void PeopleList::updateSelection()
{
	QTreeWidgetItem *item = ui->idTreeWidget->currentItem();
	RsGxsGroupId id;

	if (item) {
		id = RsGxsGroupId(item->text(RSID_COL_KEYID).toStdString());
	}

	if (id != mId) {
		mId = id;
		//requestIdDetails();
		//requestRepList();
	}
}

static QString getHumanReadableDuration(uint32_t seconds)
{
    if(seconds < 60)
        return QString(QObject::tr("%1 seconds ago")).arg(seconds) ;
    else if(seconds < 120)
        return QString(QObject::tr("%1 minute ago")).arg(seconds/60) ;
    else if(seconds < 3600)
        return QString(QObject::tr("%1 minutes ago")).arg(seconds/60) ;
    else if(seconds < 7200)
        return QString(QObject::tr("%1 hour ago")).arg(seconds/3600) ;
    else if(seconds < 24*3600)
        return QString(QObject::tr("%1 hours ago")).arg(seconds/3600) ;
    else if(seconds < 2*24*3600)
        return QString(QObject::tr("%1 day ago")).arg(seconds/86400) ;
    else 
        return QString(QObject::tr("%1 days ago")).arg(seconds/86400) ;
}

void PeopleList::requestIdList()
{
	//Disable by default, will be enable by insertIdDetails()
	ui->removeIdentity->setEnabled(false);
	ui->editIdentity->setEnabled(false);

	if (!mIdQueue)
		return;

	mStateHelper->setLoading(IDDIALOG_IDLIST, true);

	mIdQueue->cancelActiveRequestTokens(IDDIALOG_IDLIST);

	RsTokReqOptions opts;
	opts.mReqType = GXS_REQUEST_TYPE_GROUP_DATA;

	uint32_t token;

	mIdQueue->requestGroupInfo(token, RS_TOKREQ_ANSTYPE_DATA, opts, IDDIALOG_IDLIST);
}

bool PeopleList::fillIdListItem(const RsGxsIdGroup& data, QTreeWidgetItem *&item, const RsPgpId &ownPgpId, int accept)
{
    bool isLinkedToOwnNode = (data.mPgpKnown && (data.mPgpId == ownPgpId)) ;
    bool isOwnId = (data.mMeta.mSubscribeFlags & GXS_SERV::GROUP_SUBSCRIBE_ADMIN);
    uint32_t item_flags = 0 ;

	/* do filtering */
	bool ok = false;
	if (data.mMeta.mGroupFlags & RSGXSID_GROUPFLAG_REALID)
    {
        if (isLinkedToOwnNode && (accept & RSID_FILTER_YOURSELF))
        {
            ok = true;
            item_flags |= RSID_FILTER_YOURSELF ;
        }

        if (data.mPgpKnown && (accept & RSID_FILTER_FRIENDS))
        {
            ok = true;
            item_flags |= RSID_FILTER_FRIENDS ;
        }

        if (accept & RSID_FILTER_OTHERS)
        {
            ok = true;
            item_flags |= RSID_FILTER_OTHERS ;
        }
    }
    else if (accept & RSID_FILTER_PSEUDONYMS)
    {
            ok = true;
            item_flags |= RSID_FILTER_PSEUDONYMS ;
    }

    if (isOwnId && (accept & RSID_FILTER_OWNED_BY_YOU))
    {
        ok = true;
            item_flags |= RSID_FILTER_OWNED_BY_YOU ;
    }

	if (!ok)
		return false;

	if (!item)
        item = new QTreeWidgetItem();

    item->setText(RSID_COL_NICKNAME, QString::fromUtf8(data.mMeta.mGroupName.c_str()).left(RSID_MAXIMUM_NICKNAME_SIZE));
    item->setText(RSID_COL_KEYID, QString::fromStdString(data.mMeta.mGroupId.toStdString()));

    time_t now = time(NULL) ;
    item->setText(RSID_COL_LASTACTIVITY, getHumanReadableDuration(now - data.mLastUsageTS)) ;

    item->setData(RSID_COL_KEYID, Qt::UserRole,QVariant(item_flags)) ;
 
    item->setData(RSID_COL_LASTACTIVITY, Qt::UserRole, QString::number(now - data.mLastUsageTS));

	 if(isOwnId)
	 {
        QFont font = item->font(RSID_COL_NICKNAME) ;
		font.setBold(true) ;
		item->setFont(RSID_COL_NICKNAME,font) ;
		item->setFont(RSID_COL_IDTYPE,font) ;
		item->setFont(RSID_COL_KEYID,font) ;

		QString tooltip = tr("This identity is owned by you");
		item->setToolTip(RSID_COL_NICKNAME, tooltip) ;
		item->setToolTip(RSID_COL_KEYID, tooltip) ;
		item->setToolTip(RSID_COL_IDTYPE, tooltip) ;
	 }

#ifdef ID_DEBUG
	std::cerr << "Setting item image : " << pixmap.width() << " x " << pixmap.height() << std::endl;
#endif
    QPixmap pixmap ;

    if(data.mImage.mSize == 0 || !pixmap.loadFromData(data.mImage.mData, data.mImage.mSize, "PNG"))
        pixmap = QPixmap::fromImage(GxsIdDetails::makeDefaultIcon(RsGxsId(data.mMeta.mGroupId))) ;

    item->setIcon(RSID_COL_NICKNAME, QIcon(pixmap));

    QString tooltip;

	if (data.mMeta.mGroupFlags & RSGXSID_GROUPFLAG_REALID)
	{
		if (data.mPgpKnown)
		{
			RsPeerDetails details;
			rsPeers->getGPGDetails(data.mPgpId, details);
			item->setText(RSID_COL_IDTYPE, QString::fromUtf8(details.name.c_str()));
			item->setToolTip(RSID_COL_IDTYPE,QString::fromStdString(data.mPgpId.toStdString())) ;
			
			
			tooltip += tr("Node name:")+" " + QString::fromUtf8(details.name.c_str()) + "\n";
			tooltip += tr("Node Id  :")+" " + QString::fromStdString(data.mPgpId.toStdString()) ;
			item->setToolTip(RSID_COL_KEYID,tooltip) ;
		}
		else
		{
			item->setText(RSID_COL_IDTYPE, tr("Unknown PGP key"));
			item->setToolTip(RSID_COL_IDTYPE,tr("Unknown key ID")) ;
			item->setToolTip(RSID_COL_KEYID,tr("Unknown key ID")) ;

		}
	}
	else
	{
		item->setText(RSID_COL_IDTYPE, QString()) ;
		item->setToolTip(RSID_COL_IDTYPE,QString()) ;
	}

	return true;
}

void PeopleList::insertIdList(uint32_t token)
{
	mStateHelper->setLoading(IDDIALOG_IDLIST, false);

	int accept = ui->filterComboBox->itemData(ui->filterComboBox->currentIndex()).toInt();

	RsGxsIdGroup data;
	std::vector<RsGxsIdGroup> datavector;
	std::vector<RsGxsIdGroup>::iterator vit;
	if (!rsIdentity->getGroupData(token, datavector))
	{
#ifdef ID_DEBUG
		std::cerr << "PeopleList::insertIdList() Error getting GroupData";
		std::cerr << std::endl;
#endif

		mStateHelper->setActive(IDDIALOG_IDLIST, false);
		mStateHelper->clear(IDDIALOG_IDLIST);

		return;
	}

	mStateHelper->setActive(IDDIALOG_IDLIST, true);

	RsPgpId ownPgpId  = rsPeers->getGPGOwnId();

	/* Update existing and remove not existing items */
	QTreeWidgetItemIterator itemIterator(ui->idTreeWidget);
	QTreeWidgetItem *item = NULL;
	while ((item = *itemIterator) != NULL) {
		++itemIterator;

		for (vit = datavector.begin(); vit != datavector.end(); ++vit)
		{
			if (vit->mMeta.mGroupId == RsGxsGroupId(item->text(RSID_COL_KEYID).toStdString()))
			{
				break;
			}
		}
		if (vit == datavector.end())
		{
			delete(item);
		} else {
			if (!fillIdListItem(*vit, item, ownPgpId, accept))
			{
				delete(item);
			}
			datavector.erase(vit);
		}
	}

	/* Insert new items */
	for (vit = datavector.begin(); vit != datavector.end(); ++vit)
	{
		data = (*vit);

		item = NULL;
		if (fillIdListItem(*vit, item, ownPgpId, accept))
		{
			ui->idTreeWidget->addTopLevelItem(item);
		}
	}

	filterIds();

	updateSelection();
}
	
void PeopleList::updateDisplay(bool complete)
{
	/* Update identity list */

	if (complete) {
		/* Fill complete */
		requestIdList();
		//requestIdDetails();
		//requestRepList();

		return;
	}

	std::list<RsGxsGroupId> grpIds;
	getAllGrpIds(grpIds);
	if (!getGrpIds().empty()) {
		requestIdList();

		if (!mId.isNull() && std::find(grpIds.begin(), grpIds.end(), mId) != grpIds.end()) {
			//requestIdDetails();
			//requestRepList();
		}
	}
}

void PeopleList::addIdentity()
{
	IdEditDialog dlg(this);
	dlg.setupNewId(false);
	dlg.exec();
}

void PeopleList::removeIdentity()
{
	QTreeWidgetItem *item = ui->idTreeWidget->currentItem();
	if (!item)
	{
#ifdef ID_DEBUG
		std::cerr << "PeopleList::editIdentity() Invalid item";
		std::cerr << std::endl;
#endif
		return;
	}

	if ((QMessageBox::question(this, tr("Really delete?"), tr("Do you really want to delete this identity?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No))== QMessageBox::Yes)
	{
		std::string keyId = item->text(RSID_COL_KEYID).toStdString();

		uint32_t dummyToken = 0;
		RsGxsIdGroup group;
		group.mMeta.mGroupId=RsGxsGroupId(keyId);
		rsIdentity->deleteIdentity(dummyToken, group);
	}
}

void PeopleList::editIdentity()
{
	QTreeWidgetItem *item = ui->idTreeWidget->currentItem();
	if (!item)
	{
#ifdef ID_DEBUG
		std::cerr << "PeopleList::editIdentity() Invalid item";
		std::cerr << std::endl;
#endif
		return;
	}

	RsGxsGroupId keyId = RsGxsGroupId(item->text(RSID_COL_KEYID).toStdString());
	if (keyId.isNull()) {
		return;
	}

	IdEditDialog dlg(this);
	dlg.setupExistingId(keyId);
	dlg.exec();
}

void PeopleList::filterIds()
{
	int filterColumn = ui->filterLineEdit->currentFilter();
	QString text = ui->filterLineEdit->text();

	ui->idTreeWidget->filterItems(filterColumn, text);
}

void PeopleList::loadRequest(const TokenQueue * queue, const TokenRequest &req)
{
#ifdef ID_DEBUG
	std::cerr << "PeopleList::loadRequest() UserType: " << req.mUserType;
	std::cerr << std::endl;
#endif

    if(queue == mIdQueue)
    {
	    switch(req.mUserType)
	    {
	    case IDDIALOG_IDLIST:
		    insertIdList(req.mToken);
		    break;

	    //case IDDIALOG_IDDETAILS:
		    //insertIdDetails(req.mToken);
		    //break;

	    //case IDDIALOG_REPLIST:
		    //insertRepList(req.mToken);
		    //break;

	    //case IDDIALOG_REFRESH:
		    // replaced by RsGxsUpdateBroadcastPage
		    //			updateDisplay(true);
		    //break;
	    default:
		    std::cerr << "PeopleList::loadRequest() ERROR";
		    std::cerr << std::endl;
		    break;
	    }
    }
}
    

void PeopleList::IdListCustomPopupMenu( QPoint )
{
	QMenu contextMnu( this );


	std::list<RsGxsId> own_identities ;
	rsIdentity->getOwnIds(own_identities) ;

	QTreeWidgetItem *item = ui->idTreeWidget->currentItem();
	if (item) {
		uint32_t item_flags = item->data(RSID_COL_KEYID,Qt::UserRole).toUInt() ;

		if(!(item_flags & RSID_FILTER_OWNED_BY_YOU))
		{
			if(own_identities.size() <= 1)
			{
				QAction *action = contextMnu.addAction(QIcon(":/images/chat_24.png"), tr("Chat with this person"), this, SLOT(chatIdentity()));

				if(own_identities.empty())
					action->setEnabled(false) ;
				else
					action->setData(QString::fromStdString((own_identities.front()).toStdString())) ;
			}
			else
			{
				QMenu *mnu = contextMnu.addMenu(QIcon(":/images/chat_24.png"),tr("Chat with this person as...")) ;

				for(std::list<RsGxsId>::const_iterator it=own_identities.begin();it!=own_identities.end();++it)
				{
					RsIdentityDetails idd ;
					rsIdentity->getIdDetails(*it,idd) ;

					QPixmap pixmap ;

					if(idd.mAvatar.mSize == 0 || !pixmap.loadFromData(idd.mAvatar.mData, idd.mAvatar.mSize, "PNG"))
						pixmap = QPixmap::fromImage(GxsIdDetails::makeDefaultIcon(*it)) ;

					QAction *action = mnu->addAction(QIcon(pixmap), QString("%1 (%2)").arg(QString::fromUtf8(idd.mNickname.c_str()), QString::fromStdString((*it).toStdString())), this, SLOT(chatIdentity()));
					action->setData(QString::fromStdString((*it).toStdString())) ;
				}
			}

			contextMnu.addAction(QIcon(":/images/mail_new.png"), tr("Send message to this person"), this, SLOT(sendMsg()));
			
			contextMnu.addAction(QIcon(":/images/info16.png"), tr("Person details"), this, SLOT(personDetails()));
		}
	}

	contextMnu.addSeparator();

	contextMnu.addAction(ui->editIdentity);
	contextMnu.addAction(ui->removeIdentity);
	
	contextMnu.addSeparator();

	contextMnu.exec(QCursor::pos());
}

void PeopleList::chatIdentity()
{
	QTreeWidgetItem *item = ui->idTreeWidget->currentItem();
	if (!item)
	{
		std::cerr << "PeopleList::editIdentity() Invalid item";
		std::cerr << std::endl;
		return;
	}

	std::string keyId = item->text(RSID_COL_KEYID).toStdString();

	QAction *action = qobject_cast<QAction *>(QObject::sender());
	if (!action)
		return ;

	RsGxsId from_gxs_id(action->data().toString().toStdString());
	uint32_t error_code ;

	if(!rsMsgs->initiateDistantChatConnexion(RsGxsId(keyId), from_gxs_id, error_code))
		QMessageBox::information(NULL, tr("Distant chat cannot work"), QString("%1 %2: %3").arg(tr("Distant chat refused with this person.")).arg(tr("Error code")).arg(error_code)) ;
}

void PeopleList::sendMsg()
{
	QTreeWidgetItem *item = ui->idTreeWidget->currentItem();
	if (!item)
	{
		return;
	}

	MessageComposer *nMsgDialog = MessageComposer::newMsg();
	if (nMsgDialog == NULL) {
		return;
	}

    std::string keyId = item->text(RSID_COL_KEYID).toStdString();

    nMsgDialog->addRecipient(MessageComposer::TO,  RsGxsId(keyId));
		nMsgDialog->show();
		nMsgDialog->activateWindow();

    /* window will destroy itself! */

}

void PeopleList::personDetails()
{
	QTreeWidgetItem *item = ui->idTreeWidget->currentItem();
	if (!item)
	{
		return;
	}

    std::string keyId = item->text(RSID_COL_KEYID).toStdString();

    IdDetailsDialog *dialog = new IdDetailsDialog(RsGxsGroupId(keyId));
    dialog->show();

    /* Dialog will destroy itself */



}
