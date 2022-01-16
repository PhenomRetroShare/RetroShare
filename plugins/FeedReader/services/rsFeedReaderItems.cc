/*******************************************************************************
 * plugins/FeedReader/services/rsFeedReaderItems.cc                            *
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

#include "serialiser/rsbaseserial.h"
#include "serialiser/rstlvbase.h"
#include "rsFeedReaderItems.h"

/*************************************************************************/

RsFeedReaderFeed::RsFeedReaderFeed()
  : RsItem(RS_PKT_VERSION_SERVICE, RS_SERVICE_TYPE_PLUGIN_FEEDREADER, RS_PKT_SUBTYPE_FEEDREADER_FEED)
  , xpathsToUse(TLV_TYPE_STRINGSET), xpathsToRemove(TLV_TYPE_STRINGSET)
{
	clear();
}

void RsFeedReaderFeed::clear()
{
	feedId = 0;
	parentId = 0;
	url.clear();
	name.clear();
	description.clear();
	icon.clear();
	user.clear();
	password.clear();
	proxyAddress.clear();
	proxyPort = 0;
	updateInterval = 0;
	lastUpdate = 0;
	storageTime = 0;
	flag = 0;
	forumId.clear();
	errorState = RS_FEED_ERRORSTATE_OK;
	errorString.clear();
	transformationType = RS_FEED_TRANSFORMATION_TYPE_NONE;
	xpathsToUse.ids.clear();
	xpathsToRemove.ids.clear();
	xslt.clear();

	preview = false;
	workstate = WAITING;
	content.clear();
}

void RsFeedReaderFeed::serial_process(RsGenericSerializer::SerializeJob j,RsGenericSerializer::SerializeContext& ctx)
{
	//RsTypeSerializer::serial_process<uint32_t> (j,ctx,                     feedId            ,"feedId") ;
	//RsTypeSerializer::serial_process<uint32_t> (j,ctx,                     parentId          ,"parentId") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_LINK,   url               ,"url") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_NAME,   name              ,"name") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_COMMENT,description       ,"description") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_VALUE,  icon              ,"icon") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_VALUE,  user              ,"user") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_VALUE,  password          ,"password") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_VALUE,  proxyAddress      ,"proxyAddress") ;
	//RsTypeSerializer::serial_process<uint16_t> (j,ctx,                     proxyPort         ,"proxyPort") ;
	//RsTypeSerializer::serial_process<uint32_t> (j,ctx,                     updateInterval    ,"updateInterval") ;
	//RsTypeSerializer::serial_process           (j,ctx,                     lastUpdate        ,"lastUpdate") ;
	//RsTypeSerializer::serial_process<uint32_t> (j,ctx,                     storageTime       ,"storageTime") ;
	//RsTypeSerializer::serial_process<uint32_t> (j,ctx,                     flag              ,"flag") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_VALUE,  forumId           ,"forumId") ;
	//RsTypeSerializer::serial_process           (j,ctx,                     errorState        ,"errorState") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_VALUE,  errorString       ,"errorString") ;
	//RsTypeSerializer::serial_process           (j,ctx,                     transformationType,"transformationType") ;
	//RsTypeSerializer::serial_process<RsTlvItem>(j,ctx,                     xpathsToUse       ,"xpathsToUse") ;
	//RsTypeSerializer::serial_process<RsTlvItem>(j,ctx,                     xpathsToRemove    ,"xpathsToRemove") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_VALUE,  xslt              ,"xslt") ;

	RS_SERIAL_PROCESS(feedId            );
	RS_SERIAL_PROCESS(parentId          );
	RS_SERIAL_PROCESS(url               );
	RS_SERIAL_PROCESS(name              );
	RS_SERIAL_PROCESS(description       );
	RS_SERIAL_PROCESS(icon              );
	RS_SERIAL_PROCESS(user              );
	RS_SERIAL_PROCESS(password          );
	RS_SERIAL_PROCESS(proxyAddress      );
	RS_SERIAL_PROCESS(proxyPort         );
	RS_SERIAL_PROCESS(updateInterval    );
	RS_SERIAL_PROCESS(lastUpdate        );
	RS_SERIAL_PROCESS(storageTime       );
	RS_SERIAL_PROCESS(flag              );
	RS_SERIAL_PROCESS(forumId           );
	RS_SERIAL_PROCESS(errorState        );
	RS_SERIAL_PROCESS(errorString       );
	RS_SERIAL_PROCESS(transformationType);
	RS_SERIAL_PROCESS(xpathsToUse       );
	RS_SERIAL_PROCESS(xpathsToRemove    );
	RS_SERIAL_PROCESS(xslt              );
}

/*************************************************************************/

RsFeedReaderMsg::RsFeedReaderMsg()
  : RsItem(RS_PKT_VERSION_SERVICE, RS_SERVICE_TYPE_PLUGIN_FEEDREADER, RS_PKT_SUBTYPE_FEEDREADER_MSG)
{
	clear();
}

void RsFeedReaderMsg::clear()
{
	msgId.clear();
	feedId = 0;
	title.clear();
	link.clear();
	author.clear();
	description.clear();
	descriptionTransformed.clear();
	pubDate = 0;
	flag = 0;
}

void RsFeedReaderMsg::serial_process(RsGenericSerializer::SerializeJob j,RsGenericSerializer::SerializeContext& ctx)
{
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_GENID,  msgId                 ,"msgId") ;
	//RsTypeSerializer::serial_process<uint32_t> (j,ctx,                     feedId                ,"feedId") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_NAME,   title                 ,"title") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_LINK,   link                  ,"link") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_VALUE,  author                ,"author") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_COMMENT,description           ,"description") ;
	//RsTypeSerializer::serial_process           (j,ctx,TLV_TYPE_STR_COMMENT,descriptionTransformed,"descriptionTransformed") ;
	//RsTypeSerializer::serial_process           (j,ctx,                     pubDate               ,"pubDate") ;
	//RsTypeSerializer::serial_process<uint32_t> (j,ctx,                     flag                  ,"flag") ;

	RS_SERIAL_PROCESS(msgId                 );
	RS_SERIAL_PROCESS(feedId                );
	RS_SERIAL_PROCESS(title                 );
	RS_SERIAL_PROCESS(link                  );
	RS_SERIAL_PROCESS(author                );
	RS_SERIAL_PROCESS(description           );
	RS_SERIAL_PROCESS(descriptionTransformed);
	RS_SERIAL_PROCESS(pubDate               );
	RS_SERIAL_PROCESS(flag                  );
}

/*************************************************************************/

RsItem *RsFeedReaderSerialiser::create_item(uint16_t service,uint8_t type) const
{
	if(service != RS_SERVICE_TYPE_PLUGIN_FEEDREADER)
		return NULL ;

	switch(type)
	{
		case RS_PKT_SUBTYPE_FEEDREADER_FEED: return new RsFeedReaderFeed();
		case RS_PKT_SUBTYPE_FEEDREADER_MSG : return new RsFeedReaderMsg();
		default:
		return NULL ;
	}
}
