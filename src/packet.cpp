
#include "packet.hpp"

#include <string>

#include "util.hpp"

PacketProcessor::PacketProcessor()
{
	this->firstdec = true;
}

std::string PacketProcessor::GetFamilyName(uint8_t family)
{
	switch (family)
	{
		case PACKET_CONNECTION: return "Connection";
		case PACKET_ACCOUNT: return "Account";
		case PACKET_CHARACTER: return "Character";
		case PACKET_LOGIN: return "Login";
		case PACKET_WELCOME: return "Welcome";
		case PACKET_WALK: return "Walk";
		case PACKET_FACE: return "Face";
		case PACKET_CHAIR: return "Chair";
		case PACKET_EMOTE: return "Emote";
		case PACKET_ATTACK: return "Attack";
		case PACKET_SHOP: return "Shop";
		case PACKET_ITEM: return "Item";
		case PACKET_SKILLMASTER: return "SkillMaster";
		case PACKET_GLOBAL: return "Global";
		case PACKET_TALK: return "Talk";
		case PACKET_WARP: return "Warp";
		case PACKET_JUKEBOX: return "Jukebox";
		case PACKET_PLAYERS: return "Players";
		case PACKET_PARTY: return "Party";
		case PACKET_REFRESH: return "Refresh";
		case PACKET_PAPERDOLL: return "Paperdoll";
		case PACKET_TRADE: return "Trade";
		case PACKET_CHEST: return "Chest";
		case PACKET_DOOR: return "Door";
		case PACKET_PING: return "Ping";
		case PACKET_BANK: return "Bank";
		case PACKET_LOCKER: return "Locker";
		case PACKET_GUILD: return "Guild";
		case PACKET_SIT: return "Sit";
		case PACKET_BOARD: return "Board";
		case PACKET_ARENA: return "Arena";
		case PACKET_ADMININTERACT: return "AdminInteract";
		case PACKET_CITIZEN: return "Citizen";
		case PACKET_QUEST: return "Quest";
		case PACKET_BOOK: return "Book";
		case PACKET_INIT: return "Init";
		default: return "UNKOWN";
	}
}

std::string PacketProcessor::GetActionName(uint8_t action)
{
	switch (action)
	{
		case PACKET_REQUEST: return "Request";
		case PACKET_ACCEPT: return "Accept";
		case PACKET_REPLY: return "Reply";
		case PACKET_REMOVE: return "Remove";
		case PACKET_AGREE: return "Agree";
		case PACKET_CREATE: return "Create";
		case PACKET_ADD: return "Add";
		case PACKET_PLAYER: return "Player";
		case PACKET_TAKE: return "Take";
		case PACKET_USE: return "Use";
		case PACKET_BUY: return "Buy";
		case PACKET_SELL: return "Sell";
		case PACKET_OPEN: return "Open";
		case PACKET_CLOSE: return "Close";
		case PACKET_MSG: return "Msg";
		case PACKET_MOVESPEC: return "MoveSpec";
		case PACKET_LIST: return "List";
		case PACKET_TELL: return "Tell";
		case PACKET_REPORT: return "Report";
		case PACKET_DROP: return "Drop";
		case PACKET_JUNK: return "Junk";
		case PACKET_GET: return "Get";
		case PACKET_NET: return "Net";
		case PACKET_INIT: return "Init";
		default: return "UNKOWN";
	}
}

std::string PacketProcessor::Decode(const std::string &str)
{
	std::string newstr;
	std::size_t length = str.length();
	std::size_t i = 0;
	std::size_t ii = 0;

	if (this->firstdec)
	{
		this->firstdec = false;
		return str;
	}

	newstr.resize(length);

	while (i < length)
	{
		newstr[ii++] = (unsigned char)str[i] ^ 0x80;
		i += 2;
	}

	--i;

	if (length % 2)
	{
		i -= 2;
	}

	do
	{
		newstr[ii++] = (unsigned char)str[i] ^ 0x80;
		i -= 2;
	} while (i >= 2);

	return this->DickWinderD(newstr);
}

std::string PacketProcessor::Encode(const std::string &rawstr)
{
	std::string str = this->DickWinderE(rawstr);
	std::string newstr;
	std::size_t length = str.length();
	std::size_t i = 2;
	std::size_t ii = 2;

	newstr.resize(length);

	newstr[0] = str[0];
	newstr[1] = str[1];

	while (i < length)
	{
		newstr[i] = (unsigned char)str[ii++] ^ 0x80;
		i += 2;
	}

	i = length - 1;

	if (length % 2)
	{
		--i;
	}

	while (i >= 2)
	{
		newstr[i] = (unsigned char)str[ii++] ^ 0x80;
		i -= 2;
	}

	return newstr;
}

std::string PacketProcessor::DickWinder(const std::string &str, unsigned char emulti)
{
	std::string newstr;
	std::size_t length = str.length();
	std::string buffer;
	unsigned char c;

	if (emulti == 0)
	{
		return str;
	}

	for (std::size_t i = 0; i < length; ++i)
	{
		c = str[i];

		if (c % emulti == 0)
		{
			buffer += c;
		}
		else
		{
			if (buffer.length() > 0)
			{
				std::reverse(buffer.begin(), buffer.end());
				newstr += buffer;
				buffer.clear();
			}
			newstr += c;
		}
	}

	if (buffer.length() > 0)
	{
		std::reverse(buffer.begin(), buffer.end());
		newstr += buffer;
	}

	return newstr;
}

std::string PacketProcessor::DickWinderE(const std::string &str)
{
	return this->DickWinder(str, this->emulti_e);
}

std::string PacketProcessor::DickWinderD(const std::string &str)
{
	return this->DickWinder(str, this->emulti_d);
}

void PacketProcessor::SetEMulti(uint8_t emulti_e, uint8_t emulti_d)
{
	this->emulti_e = emulti_e;
	this->emulti_d = emulti_d;
}

uint32_t PacketProcessor::Number(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
	if (b1 == 0 || b1 == 254) b1 = 1;
	if (b2 == 0 || b2 == 254) b2 = 1;
	if (b3 == 0 || b3 == 254) b3 = 1;
	if (b4 == 0 || b4 == 254) b4 = 1;

	--b1;
	--b2;
	--b3;
	--b4;

	return (b4*PacketProcessor::MAX3 + b3*PacketProcessor::MAX2 + b2*PacketProcessor::MAX1 + b1);
}

quadchar PacketProcessor::ENumber(uint32_t number)
{
	std::size_t throwaway;

	return PacketProcessor::ENumber(number, throwaway);
}

quadchar PacketProcessor::ENumber(uint32_t number, std::size_t &size)
{
	quadchar bytes(254);
	unsigned int onumber = number;

	if (onumber >= PacketProcessor::MAX3)
	{
		bytes[3] = number / PacketProcessor::MAX3 + 1;
		number = number % PacketProcessor::MAX3;
	}

	if (onumber >= PacketProcessor::MAX2)
	{
		bytes[2] = number / PacketProcessor::MAX2 + 1;
		number = number % PacketProcessor::MAX2;
	}

	if (onumber >= PacketProcessor::MAX1)
	{
		bytes[1] = number / PacketProcessor::MAX1 + 1;
		number = number % PacketProcessor::MAX1;
	}

	bytes[0] = number + 1;

	for (int i = 3; i >= 0; ++i)
	{
		if (i == 0)
		{
			size = 1;
			break;
		}
		else if (bytes[i] > 0)
		{
			size = i + 1;
			break;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		if (bytes[i] == 128)
		{
			bytes[i] = 0;
		}
		else if (bytes[i] == 0)
		{
			bytes[i] = 128;
		}
	}

	return bytes;
}

uint16_t PacketProcessor::PID(uint8_t b1, uint8_t b2)
{
	return b1 + b2*256;
}

pairchar PacketProcessor::EPID(uint16_t pid)
{
	pairchar bytes;

	bytes[1] = pid % 256;
	bytes[0] = pid / 256;

	return bytes;
}

PacketReader::PacketReader(const std::string &data)
{
	this->data = data;
	this->length = data.length();
}

std::size_t PacketReader::Length()
{
	return this->length;
}

std::size_t PacketReader::Remaining()
{
	return this->data.length();
}

uint8_t PacketReader::GetByte()
{
	uint8_t ret;

	if (this->data.length() < 1)
	{
		return 0;
	}

	ret = this->data[0];
	this->data.erase(0, 1);

	return ret;
}

uint8_t PacketReader::GetChar()
{
	uint8_t ret;

	if (this->data.length() < 1)
	{
		return 0;
	}

	ret = PacketProcessor::Number(this->data[0]);
	this->data.erase(0, 1);

	return ret;
}

uint16_t PacketReader::GetShort()
{
	uint16_t ret;

	if (this->data.length() < 2)
	{
		return 0;
	}

	ret = PacketProcessor::Number(this->data[0], this->data[1]);
	this->data.erase(0, 2);

	return ret;
}

uint32_t PacketReader::GetThree()
{
	uint32_t ret;

	if (this->data.length() < 3)
	{
		return 0;
	}

	ret = PacketProcessor::Number(this->data[0], this->data[1], this->data[2]);
	this->data.erase(0, 3);

	return ret;
}

uint32_t PacketReader::GetInt()
{
	uint32_t ret;

	if (this->data.length() < 4)
	{
		return 0;
	}

	ret = PacketProcessor::Number(this->data[0], this->data[1], this->data[2], this->data[3]);
	this->data.erase(0, 4);

	return ret;
}

std::string PacketReader::GetFixedString(size_t length)
{
	std::string ret;

	if (length == 0 || this->data.length() < length)
	{
		return ret;
	}

	ret = this->data.substr(0, length);
	this->data.erase(0, length);

	return ret;
}

std::string PacketReader::GetBreakString(unsigned char breakchar)
{
	std::string ret;
	std::size_t length;

	length = this->data.find_first_of(breakchar);

	if (length == std::string::npos)
	{
		return ret;
	}

	ret = this->data.substr(0, length);
	this->data.erase(0, length+1);

	return ret;
}

std::string PacketReader::GetEndString()
{
	std::string ret = this->data;

	this->data.erase();

	return ret;
}


PacketBuilder::PacketBuilder()
{
	this->length = 0;
	this->id = 0;
}

PacketBuilder::PacketBuilder(uint16_t id)
{
	this->length = 0;
	this->SetID(id);
}

PacketBuilder::PacketBuilder(uint8_t family, uint8_t action)
{
	this->length = 0;
	this->SetID(family, action);
}

uint16_t PacketBuilder::SetID(uint16_t id)
{
	if (id == 0)
	{
		id = PacketProcessor::PID(255, 255);
	}

	this->id = id;

	return this->id;
}

uint16_t PacketBuilder::SetID(uint8_t family, uint8_t action)
{
	return this->SetID(PacketProcessor::PID(family,action));
}

uint8_t PacketBuilder::AddByte(uint8_t byte)
{
	++this->length;
	this->data += byte;
	return byte;
}

uint8_t PacketBuilder::AddChar(uint8_t num)
{
	quadchar bytes;
	++this->length;
	bytes = PacketProcessor::ENumber(num);
	this->data += bytes[0];
	return num;
}

unsigned short PacketBuilder::AddShort(unsigned short num)
{
	quadchar bytes;
	this->length += 2;
	bytes = PacketProcessor::ENumber(num);
	this->data += bytes[0];
	this->data += bytes[1];
	return num;
}

unsigned int PacketBuilder::AddThree(unsigned int num)
{
	quadchar bytes;
	this->length += 3;
	bytes = PacketProcessor::ENumber(num);
	this->data += bytes[0];
	this->data += bytes[1];
	this->data += bytes[2];
	return num;
}

unsigned int PacketBuilder::AddInt(unsigned int num)
{
	quadchar bytes;
	this->length += 4;
	bytes = PacketProcessor::ENumber(num);
	this->data += bytes[0];
	this->data += bytes[1];
	this->data += bytes[2];
	this->data += bytes[3];
	return num;
}

const std::string &PacketBuilder::AddString(const std::string &str)
{
	this->length += str.length();
	this->data += str;

	return str;
}

const std::string &PacketBuilder::AddBreakString(const std::string &str, unsigned char breakchar)
{
	this->length += str.length() + 1;
	this->data += str;
	this->data += breakchar;

	return str;
}

std::string PacketBuilder::Get()
{
	std::string retdata;
	pairchar id = PacketProcessor::EPID(this->id);
	quadchar length = PacketProcessor::ENumber(this->length + 2);

	retdata += length[0];
	retdata += length[1];
	retdata += id[0];
	retdata += id[1];
	retdata += this->data;

	return retdata;
}

PacketBuilder::operator std::string()
{
	return this->Get();
}
