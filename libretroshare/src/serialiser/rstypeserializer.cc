/*
 * libretroshare/src/serialiser: rstypeserializer.cc
 *
 * RetroShare Serialiser.
 *
 * Copyright (C) 2017  Cyril Soler
 * Copyright (C) 2018  Gioacchino Mazzurco <gio@eigenlab.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License Version 2 as published by the Free Software Foundation.
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
 * Please report all bugs and problems to "csoler@users.sourceforge.net".
 *
 */
#include "serialiser/rsserializer.h"
#include "serialiser/rstypeserializer.h"
#include "serialiser/rsbaseserial.h"
#include "serialiser/rstlvkeys.h"
#include "serialiser/rsserializable.h"

#include "util/rsprint.h"

#include <iomanip>
#include <time.h>
#include <string>
#include <typeinfo> // for typeid

#include <rapid_json/document.h>
#include <rapid_json/prettywriter.h>

//static const uint32_t MAX_SERIALIZED_ARRAY_SIZE = 500 ;
static const uint32_t MAX_SERIALIZED_CHUNK_SIZE = 10*1024*1024 ; // 10 MB.

#define SAFE_GET_JSON_V() \
	const char* mName = memberName.c_str(); \
	bool ret = jDoc.HasMember(mName); \
	if(!ret) \
    { \
	    std::cerr << __PRETTY_FUNCTION__ << " \"" << memberName \
	              << "\" not found in JSON:" << std::endl \
	              << jDoc << std::endl << std::endl; \
	    print_stacktrace(); \
	    return false; \
	} \
	rapidjson::Value& v = jDoc[mName]


//============================================================================//
//                             Integer types                                  //
//============================================================================//

template<> bool RsTypeSerializer::serialize(uint8_t data[], uint32_t size, uint32_t &offset, const bool& member)
{
	return setRawUInt8(data,size,&offset,member);
}
template<> bool RsTypeSerializer::serialize(uint8_t /*data*/[], uint32_t /*size*/, uint32_t& /*offset*/, const int32_t& /*member*/)
{
	std::cerr << __PRETTY_FUNCTION__ << " Not implemented!" << std::endl;
	print_stacktrace();
	return false;
}
template<> bool RsTypeSerializer::serialize(uint8_t data[], uint32_t size, uint32_t &offset, const uint8_t& member)
{ 
	return setRawUInt8(data,size,&offset,member);
}
template<> bool RsTypeSerializer::serialize(uint8_t data[], uint32_t size, uint32_t &offset, const uint16_t& member)
{
	return setRawUInt16(data,size,&offset,member);
}
template<> bool RsTypeSerializer::serialize(uint8_t data[], uint32_t size, uint32_t &offset, const uint32_t& member)
{ 
	return setRawUInt32(data,size,&offset,member);
}
template<> bool RsTypeSerializer::serialize(uint8_t data[], uint32_t size, uint32_t &offset, const uint64_t& member) 
{ 
	return setRawUInt64(data,size,&offset,member);
}
template<> bool RsTypeSerializer::serialize(uint8_t data[], uint32_t size, uint32_t &offset, const time_t& member)
{
	return setRawTimeT(data,size,&offset,member);
}

template<> bool RsTypeSerializer::deserialize(const uint8_t data[], uint32_t size, uint32_t &offset, bool& member)
{
	uint8_t m;
	bool ok = getRawUInt8(data,size,&offset,&m);
	member = m;
	return ok;
}
template<> bool RsTypeSerializer::deserialize(const uint8_t /*data*/[], uint32_t /*size*/, uint32_t& /*offset*/, int32_t& /*member*/)
{
	std::cerr << __PRETTY_FUNCTION__ << " Not implemented!" << std::endl;
	print_stacktrace();
	return false;
}
template<> bool RsTypeSerializer::deserialize(const uint8_t data[], uint32_t size, uint32_t &offset, uint8_t& member)
{ 
	return getRawUInt8(data,size,&offset,&member);
}
template<> bool RsTypeSerializer::deserialize(const uint8_t data[], uint32_t size, uint32_t &offset, uint16_t& member)
{
	return getRawUInt16(data,size,&offset,&member);
}
template<> bool RsTypeSerializer::deserialize(const uint8_t data[], uint32_t size, uint32_t &offset, uint32_t& member)
{ 
	return getRawUInt32(data,size,&offset,&member);
}
template<> bool RsTypeSerializer::deserialize(const uint8_t data[], uint32_t size, uint32_t &offset, uint64_t& member) 
{ 
	return getRawUInt64(data,size,&offset,&member);
}
template<> bool RsTypeSerializer::deserialize(const uint8_t data[], uint32_t size, uint32_t &offset, time_t& member)
{
	return getRawTimeT(data,size,&offset,member);
}

template<> uint32_t RsTypeSerializer::serial_size(const bool& /* member*/)
{
	return 1;
}
template<> uint32_t RsTypeSerializer::serial_size(const int32_t& /* member*/)
{
	std::cerr << __PRETTY_FUNCTION__ << " Not implemented!" << std::endl;
	print_stacktrace();
	return 0;
}
template<> uint32_t RsTypeSerializer::serial_size(const uint8_t& /* member*/)
{ 
	return 1;
}
template<> uint32_t RsTypeSerializer::serial_size(const uint16_t& /* member*/)
{
	return 2;
}
template<> uint32_t RsTypeSerializer::serial_size(const uint32_t& /* member*/)
{ 
	return 4;
}
template<> uint32_t RsTypeSerializer::serial_size(const uint64_t& /* member*/)
{ 
	return 8;
}
template<> uint32_t RsTypeSerializer::serial_size(const time_t& /* member*/)
{
	return 8;
}

template<> void RsTypeSerializer::print_data(const std::string& n, const bool & V)
{
    std::cerr << "  [bool       ] " << n << ": " << V << std::endl;
}
template<> void RsTypeSerializer::print_data(const std::string& n, const int32_t& V)
{
	std::cerr << "  [int32_t   ] " << n << ": " << V << std::endl;
}
template<> void RsTypeSerializer::print_data(const std::string& n, const uint8_t & V)
{
    std::cerr << "  [uint8_t    ] " << n << ": " << V << std::endl;
}
template<> void RsTypeSerializer::print_data(const std::string& n, const uint16_t& V)
{
    std::cerr << "  [uint16_t   ] " << n << ": " << V << std::endl;
}
template<> void RsTypeSerializer::print_data(const std::string& n, const uint32_t& V)
{
    std::cerr << "  [uint32_t   ] " << n << ": " << V << std::endl;
}
template<> void RsTypeSerializer::print_data(const std::string& n, const uint64_t& V)
{
    std::cerr << "  [uint64_t   ] " << n << ": " << V << std::endl;
}
template<> void RsTypeSerializer::print_data(const std::string& n, const time_t& V)
{
    std::cerr << "  [time_t     ] " << n << ": " << V << " (" << time(NULL)-V << " secs ago)" << std::endl;
}

#define SIMPLE_TO_JSON_DEF(T) \
template<> bool RsTypeSerializer::to_JSON( const std::string& memberName, \
	                                       const T& member, RsJson& jDoc ) \
{ \
	rapidjson::Document::AllocatorType& allocator = jDoc.GetAllocator(); \
	\
	rapidjson::Value key; \
	key.SetString(memberName.c_str(), memberName.length(), allocator); \
 \
	rapidjson::Value value(member); \
 \
	jDoc.AddMember(key, value, allocator); \
 \
	return true; \
}

SIMPLE_TO_JSON_DEF(bool)
SIMPLE_TO_JSON_DEF(int32_t)

template<> bool RsTypeSerializer::to_JSON( const std::string& memberName,
                                           const time_t& member, RsJson& jDoc )
{
	rapidjson::Document::AllocatorType& allocator = jDoc.GetAllocator();

	rapidjson::Value key;
	key.SetString(memberName.c_str(), memberName.length(), allocator);

	// without this compilation may break depending on how time_t is defined
	int64_t tValue = member;
	rapidjson::Value value(tValue);

	jDoc.AddMember(key, value, allocator);

	return true;
}

SIMPLE_TO_JSON_DEF(uint8_t)
SIMPLE_TO_JSON_DEF(uint16_t)
SIMPLE_TO_JSON_DEF(uint32_t)
SIMPLE_TO_JSON_DEF(uint64_t)

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName, bool& member,
                                  RsJson& jDoc )
{
	SAFE_GET_JSON_V();
	ret = ret && v.IsBool();
	if(ret) member = v.GetBool();
	return ret;
}

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName,
                                  int32_t& member, RsJson& jDoc )
{
	SAFE_GET_JSON_V();
	ret = ret && v.IsInt();
	if(ret) member = v.GetInt();
	return ret;
}

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName, time_t& member,
                                  RsJson& jDoc )
{
	SAFE_GET_JSON_V();
	ret = ret && v.IsInt();
	if(ret) member = v.GetInt();
	return ret;
}

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName,
                                  uint8_t& member, RsJson& jDoc )
{
	SAFE_GET_JSON_V();
	ret = ret && v.IsUint();
	if(ret) member = v.GetUint();
	return ret;
}

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName,
                                  uint16_t& member, RsJson& jDoc )
{
	SAFE_GET_JSON_V();
	ret = ret && v.IsUint();
	if(ret) member = v.GetUint();
	return ret;
}

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName,
                                  uint32_t& member, RsJson& jDoc )
{
	SAFE_GET_JSON_V();
	ret = ret && v.IsUint();
	if(ret) member = v.GetUint();
	return ret;
}

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName,
                                  uint64_t& member, RsJson& jDoc )
{
	SAFE_GET_JSON_V();
	ret = ret && v.IsUint();
	if(ret) member = v.GetUint();
	return ret;
}


//============================================================================//
//                                 Floats                                     //
//============================================================================//

template<> uint32_t RsTypeSerializer::serial_size(const float&){ return 4; }

template<> bool RsTypeSerializer::serialize(uint8_t data[], uint32_t size, uint32_t& offset, const float& f)
{
    return setRawUFloat32(data,size,&offset,f) ;
}
template<> bool RsTypeSerializer::deserialize(const uint8_t data[], uint32_t size, uint32_t &offset, float& f)
{
    return getRawUFloat32(data,size,&offset,f) ;
}
template<> void RsTypeSerializer::print_data(const std::string& n, const float& V)
{
    std::cerr << "  [float      ] " << n << ": " << V << std::endl;
}

SIMPLE_TO_JSON_DEF(float)

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName,
                                  float& member, RsJson& jDoc )
{
	SAFE_GET_JSON_V();
	ret = ret && v.IsFloat();
	if(ret) member = v.GetFloat();
	return ret;
}


//============================================================================//
//                             std::string                                    //
//============================================================================//

template<> uint32_t RsTypeSerializer::serial_size(const std::string& str)
{
	return getRawStringSize(str);
}
template<> bool RsTypeSerializer::serialize( uint8_t data[], uint32_t size,
                                             uint32_t& offset,
                                             const std::string& str )
{
	return setRawString(data, size, &offset, str);
}
template<> bool RsTypeSerializer::deserialize( const uint8_t data[],
                                               uint32_t size, uint32_t &offset,
                                               std::string& str )
{
	return getRawString(data, size, &offset, str);
}
template<> void RsTypeSerializer::print_data( const std::string& n,
                                              const std::string& str )
{
	std::cerr << "  [std::string] " << n << ": " << str << std::endl;
}
template<>  /*static*/
bool RsTypeSerializer::to_JSON( const std::string& membername,
                                const std::string& member, RsJson& jDoc )
{
	rapidjson::Document::AllocatorType& allocator = jDoc.GetAllocator();

	rapidjson::Value key;
	key.SetString(membername.c_str(), membername.length(), allocator);

	rapidjson::Value value;;
	value.SetString(member.c_str(), member.length(), allocator);

	jDoc.AddMember(key, value, allocator);

	return true;
}
template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName,
                                  std::string& member, RsJson& jDoc )
{
	SAFE_GET_JSON_V();
	ret = ret && v.IsString();
	if(ret) member = v.GetString();
	return ret;
}



//============================================================================//
//                      TlvString with subtype                                //
//============================================================================//

template<> uint32_t RsTypeSerializer::serial_size(uint16_t /* type_subtype */,const std::string& s)
{ 
	return GetTlvStringSize(s) ;
}

template<> bool RsTypeSerializer::serialize(uint8_t data[], uint32_t size, uint32_t &offset,uint16_t type_substring,const std::string& s)
{
	return SetTlvString(data,size,&offset,type_substring,s) ;
}

template<> bool RsTypeSerializer::deserialize(const uint8_t data[], uint32_t size,uint32_t& offset,uint16_t type_substring,std::string& s)
{
	return GetTlvString((void*)data,size,&offset,type_substring,s) ;
}

template<> void RsTypeSerializer::print_data(const std::string& n, uint16_t type_substring,const std::string& V)
{
	std::cerr << "  [TlvString  ] " << n << ": type=" << std::hex <<std::setw(4)<<std::setfill('0') <<  type_substring << std::dec << " s=\"" << V<< "\"" << std::endl;
}

template<> /*static*/
bool RsTypeSerializer::to_JSON( const std::string& memberName,
                                uint16_t /*sub_type*/,
                                const std::string& member, RsJson& jDoc )
{
	return to_JSON<std::string>(memberName, member, jDoc);
}

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName,
                                  uint16_t /*sub_type*/,
                                  std::string& member, RsJson& jDoc )
{
	return from_JSON<std::string>(memberName, member, jDoc);
}

//============================================================================//
//                          TlvInt with subtype                               //
//============================================================================//

template<> uint32_t RsTypeSerializer::serial_size( uint16_t /* type_subtype */,
                                                   const uint32_t& /*s*/ )
{
	return GetTlvUInt32Size();
}

template<> bool RsTypeSerializer::serialize( uint8_t data[], uint32_t size,
                                             uint32_t &offset,uint16_t sub_type,
                                             const uint32_t& s)
{
	return SetTlvUInt32(data,size,&offset,sub_type,s);
}

template<> bool RsTypeSerializer::deserialize( const uint8_t data[],
                                               uint32_t size, uint32_t& offset,
                                               uint16_t sub_type, uint32_t& s)
{
	return GetTlvUInt32((void*)data, size, &offset, sub_type, &s);
}

template<> void RsTypeSerializer::print_data(const std::string& n, uint16_t sub_type,const uint32_t& V)
{
	std::cerr << "  [TlvUInt32  ] " << n << ": type=" << std::hex
	          << std::setw(4) << std::setfill('0') << sub_type << std::dec
	          << " s=\"" << V << "\"" << std::endl;
}

template<> /*static*/
bool RsTypeSerializer::to_JSON( const std::string& memberName,
                                uint16_t /*sub_type*/,
                                const uint32_t& member, RsJson& jDoc )
{
	return to_JSON<uint32_t>(memberName, member, jDoc);
}

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& memberName,
                                  uint16_t /*sub_type*/,
                                  uint32_t& member, RsJson& jDoc )
{
	return from_JSON<uint32_t>(memberName, member, jDoc);
}


//============================================================================//
//                              TlvItems                                      //
//============================================================================//

template<> uint32_t RsTypeSerializer::serial_size(const RsTlvItem& s)
{
	return s.TlvSize();
}

template<> bool RsTypeSerializer::serialize(uint8_t data[], uint32_t size,
                                            uint32_t &offset,const RsTlvItem& s)
{
	return s.SetTlv(data,size,&offset);
}

template<> bool RsTypeSerializer::deserialize(const uint8_t data[],
                                              uint32_t size,uint32_t& offset,
                                              RsTlvItem& s)
{
	return s.GetTlv((void*)data,size,&offset) ;
}

template<> void RsTypeSerializer::print_data( const std::string& n,
                                              const RsTlvItem& s )
{
	std::cerr << "  [" << typeid(s).name() << "] " << n << std::endl;
}

template<> /*static*/
bool RsTypeSerializer::to_JSON( const std::string& memberName,
                                const RsTlvItem& member, RsJson& jDoc )
{
	rapidjson::Document::AllocatorType& allocator = jDoc.GetAllocator();

	rapidjson::Value key;
	key.SetString(memberName.c_str(), memberName.length(), allocator);

	rapidjson::Value value;
	const char* tName = typeid(member).name();
	value.SetString(tName, allocator);

	jDoc.AddMember(key, value, allocator);

	return true;
}

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& /*memberName*/,
                                  RsTlvItem& member, RsJson& /*jDoc*/)
{
	member.TlvClear();
	return true;
}


//============================================================================//
//                              Binary blocks                                 //
//============================================================================//

template<> uint32_t RsTypeSerializer::serial_size(const RsTypeSerializer::TlvMemBlock_proxy& r) { return 4 + r.second ; }

template<> bool RsTypeSerializer::deserialize(const uint8_t data[],uint32_t size,uint32_t& offset,RsTypeSerializer::TlvMemBlock_proxy& r)
{
    uint32_t saved_offset = offset ;

    bool ok = deserialize<uint32_t>(data,size,offset,r.second) ;

    if(r.second == 0)
    {
        r.first = NULL ;

        if(!ok)
			offset = saved_offset ;

        return ok ;
    }
    if(r.second > MAX_SERIALIZED_CHUNK_SIZE)
    {
        std::cerr << "(EE) RsTypeSerializer::deserialize<TlvMemBlock_proxy>(): data chunk has size larger than safety size (" << MAX_SERIALIZED_CHUNK_SIZE << "). Item will be dropped." << std::endl;
        offset = saved_offset ;
        return false ;
    }

    r.first = (uint8_t*)rs_malloc(r.second) ;

    ok = ok && (NULL != r.first);

    if (ok)
    {
        memcpy(r.first,&data[offset],r.second) ;
        offset += r.second ;
    } else  {
        offset = saved_offset ;
    }

    return ok;
}

template<> bool RsTypeSerializer::serialize(uint8_t data[],uint32_t size,uint32_t& offset,const RsTypeSerializer::TlvMemBlock_proxy& r)
{
    uint32_t saved_offset = offset ;

    bool ok = serialize<uint32_t>(data,size,offset,r.second) ;

    memcpy(&data[offset],r.first,r.second) ;
    offset += r.second ;

    if(!ok)
        offset = saved_offset ;

    return ok;
}

template<> void RsTypeSerializer::print_data(const std::string& n, const RsTypeSerializer::TlvMemBlock_proxy& s)
{
    std::cerr << "  [Binary data] " << n << ", length=" << s.second << " data=" << RsUtil::BinToHex((uint8_t*)s.first,std::min(50u,s.second)) << ((s.second>50)?"...":"") << std::endl;
}

template<> /*static*/
bool RsTypeSerializer::to_JSON(
        const std::string& memberName,
        const RsTypeSerializer::TlvMemBlock_proxy& member, RsJson& jDoc )
{
	rapidjson::Document::AllocatorType& allocator = jDoc.GetAllocator();

	rapidjson::Value key;
	key.SetString(memberName.c_str(), memberName.length(), allocator);

	rapidjson::Value value;
	const char* tName = typeid(member).name();
	value.SetString(tName, allocator);

	jDoc.AddMember(key, value, allocator);

	return true;
}

template<> /*static*/
bool RsTypeSerializer::from_JSON( const std::string& /*memberName*/,
                                  RsTypeSerializer::TlvMemBlock_proxy&,
                                  RsJson& /*jDoc*/)
{ return true; }


//============================================================================//
//                      RsJson std:ostream support                            //
//============================================================================//

std::ostream &operator<<(std::ostream &out, const RsJson &jDoc)
{
	rapidjson::StringBuffer buffer; buffer.Clear();
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	jDoc.Accept(writer);
	return out << buffer.GetString();
}
