/****************************************************************
 *  RetroShare is distributed under the following license:
 *
 *  Copyright (C) 2012 RetroShare Team
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

#ifndef FEEDNOTIFY_H
#define FEEDNOTIFY_H

#include <QMap>
#include <QObject>

class FeedHolder;
class FeedItem;

class FeedNotify : public QObject
{
	Q_OBJECT

public:
	FeedNotify(QObject *parent = 0);
	~FeedNotify();

	virtual bool hasSetting(QString &/*name*/);
	virtual bool notifyEnabled();
	virtual void setNotifyEnabled(bool /*enabled*/);
	virtual FeedItem *feedItem(FeedHolder */*parent*/);
	virtual FeedItem *testFeedItem(FeedHolder */*parent*/);

	//For Plugin with differents Feeds
	virtual bool hasSettings(QString &/*mainName*/, QMap<QString,QString> &/*tagAndTexts*/);
	virtual bool notifyEnabled(QString /*tag*/);
	virtual void setNotifyEnabled(QString /*tag*/, bool /*enabled*/);
	virtual FeedItem *testFeedItem(QString /*tag*/, FeedHolder */*parent*/);
};

#endif // FEEDNOTIFY_H
