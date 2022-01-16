/*******************************************************************************
 * plugins/FeedReader/services/rsFeedReaderItems.h                             *
 *                                                                             *
 * Copyright (C) 2012 by Thunder <retroshare.project@gmail.com>                *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Affero General Public License as              *
 * published by the Free Software Foundation, either version 3 of the          *
 * License, or (at your option) any later version.                             *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
 * GNU Affero General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Affero General Public License    *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.       *
 *                                                                             *
 *******************************************************************************/

#ifndef RS_FEEDREADER_ITEMS_H
#define RS_FEEDREADER_ITEMS_H

#include "rsitems/rsserviceids.h"
#include "serialiser/rsserial.h"
#include "serialiser/rstlvstring.h"

#include "p3FeedReader.h"

const uint8_t RS_PKT_SUBTYPE_FEEDREADER_FEED  = 0x02;
const uint8_t RS_PKT_SUBTYPE_FEEDREADER_MSG   = 0x03;

/**************************************************************************/

#define RS_FEED_FLAG_FOLDER                        0x001
#define RS_FEED_FLAG_INFO_FROM_FEED                0x002
#define RS_FEED_FLAG_STANDARD_STORAGE_TIME         0x004
#define RS_FEED_FLAG_STANDARD_UPDATE_INTERVAL      0x008
#define RS_FEED_FLAG_STANDARD_PROXY                0x010
#define RS_FEED_FLAG_AUTHENTICATION                0x020
#define RS_FEED_FLAG_DEACTIVATED                   0x040
#define RS_FEED_FLAG_FORUM                         0x080
#define RS_FEED_FLAG_UPDATE_FORUM_INFO             0x100
#define RS_FEED_FLAG_EMBED_IMAGES                  0x200
#define RS_FEED_FLAG_SAVE_COMPLETE_PAGE            0x400

class RsFeedReaderFeed : public RsItem
{
public:
	enum WorkState {
		WAITING,
		WAITING_TO_DOWNLOAD,
		DOWNLOADING,
		WAITING_TO_PROCESS,
		PROCESSING
	};

public:
	RsFeedReaderFeed();
	virtual ~RsFeedReaderFeed() {}

protected:
	// The functions below handle the serialisation of data that is specific to the bouncing object level.
	// They are called by serial_size() and serialise() from children, but should not overload the serial_size() and
	// serialise() methods, otherwise the wrong method will be called when serialising from this top level class.
	virtual void serial_process(RsGenericSerializer::SerializeJob j,RsGenericSerializer::SerializeContext& ctx);

public:
	virtual void clear();

	uint32_t                 feedId;
	uint32_t                 parentId;
	std::string              url;
	std::string              name;
	std::string              description;
	std::string              icon;
	std::string              user;
	std::string              password;
	std::string              proxyAddress;
	uint16_t                 proxyPort;
	uint32_t                 updateInterval;
	time_t                   lastUpdate;
	uint32_t                 storageTime;
	uint32_t                 flag; // RS_FEED_FLAG_...
	std::string              forumId;
	RsFeedReaderErrorState   errorState;
	std::string              errorString;

	RsFeedTransformationType transformationType;
	RsTlvStringSet           xpathsToUse;
	RsTlvStringSet           xpathsToRemove;
	std::string              xslt;

	/* Not Serialised */
	bool        preview;
	WorkState   workstate;
	std::string content;

	std::map<std::string, RsFeedReaderMsg*> msgs;
};

#define RS_FEEDMSG_FLAG_DELETED                   1
#define RS_FEEDMSG_FLAG_NEW                       2
#define RS_FEEDMSG_FLAG_READ                      4

class RsFeedReaderMsg : public RsItem
{
public:
	RsFeedReaderMsg();
	virtual ~RsFeedReaderMsg() {}

protected:
	// The functions below handle the serialisation of data that is specific to the bouncing object level.
	// They are called by serial_size() and serialise() from children, but should not overload the serial_size() and
	// serialise() methods, otherwise the wrong method will be called when serialising from this top level class.
	virtual void serial_process(RsGenericSerializer::SerializeJob j,RsGenericSerializer::SerializeContext& ctx);

public:
	virtual void clear();

	std::string msgId;
	uint32_t feedId;
	std::string title;
	std::string link;
	std::string author;
	std::string description;
	std::string descriptionTransformed;
	time_t      pubDate;
	uint32_t    flag; // RS_FEEDMSG_FLAG_...
};

class RsFeedReaderSerialiser: public RsServiceSerializer
{
public:
	RsFeedReaderSerialiser()	: RsServiceSerializer(RS_SERVICE_TYPE_PLUGIN_FEEDREADER) {}
	virtual ~RsFeedReaderSerialiser() {}

	virtual RsItem *create_item(uint16_t service,uint8_t type) const ;
};

#endif
