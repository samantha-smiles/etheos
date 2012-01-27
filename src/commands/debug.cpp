
/* $Id$
 * EOSERV is released under the zlib license.
 * See LICENSE.txt for more info.
 */

#include "commands.hpp"

#include "../util.hpp"

#include "../eoplus.hpp"
#include "../map.hpp"
#include "../npc.hpp"
#include "../packet.hpp"
#include "../player.hpp"
#include "../quest.hpp"
#include "../world.hpp"

namespace Commands
{

void SpawnItem(const std::vector<std::string>& arguments, Character* from)
{
	int id = util::to_int(arguments[0]);
	int amount = (arguments.size() >= 2) ? util::to_int(arguments[1]) : 1;

	if (from->AddItem(id, amount))
	{
		PacketBuilder reply(PACKET_ITEM, PACKET_GET, 9);
		reply.AddShort(0); // UID
		reply.AddShort(id);
		reply.AddThree(amount);
		reply.AddChar(from->weight);
		reply.AddChar(from->maxweight);
		from->Send(reply);
	}
}

void DropItem(const std::vector<std::string>& arguments, Character* from)
{
	if (from->trading) return;

	int id = util::to_int(arguments[0]);
	int amount = (arguments.size() >= 2) ? util::to_int(arguments[1]) : 1;
	int x = (arguments.size() >= 3) ? util::to_int(arguments[2]) : from->x;
	int y = (arguments.size() >= 4) ? util::to_int(arguments[3]) : from->y;

	if (amount > 0 && from->HasItem(id) >= amount)
	{
		Map_Item *item = from->map->AddItem(id, amount, x, y, from);

		if (item)
		{
			item->owner = from->player->id;
			item->unprotecttime = Timer::GetTime() + double(from->world->config["ProtectPlayerDrop"]);
			from->DelItem(id, amount);

			PacketBuilder reply(PACKET_ITEM, PACKET_DROP, 15);
			reply.AddShort(id);
			reply.AddThree(amount);
			reply.AddInt(from->HasItem(id));
			reply.AddShort(item->uid);
			reply.AddChar(x);
			reply.AddChar(y);
			reply.AddChar(from->weight);
			reply.AddChar(from->maxweight);
			from->Send(reply);
		}
	}
}

void SpawnNPC(const std::vector<std::string>& arguments, Character* from)
{
	int id = util::to_int(arguments[0]);
	int amount = (arguments.size() >= 2) ? util::to_int(arguments[1]) : 1;

	if (id < 0 || std::size_t(id) > from->world->enf->data.size())
		return;

	for (int i = 0; i < amount; ++i)
	{
		unsigned char index = from->map->GenerateNPCIndex();

		if (index > 250)
			break;

		NPC *npc = new NPC(from->map, id, from->x, from->y, 1, 1, index, true);
		from->map->npcs.push_back(npc);
		npc->Spawn();
	}
}

void Learn(const std::vector<std::string>& arguments, Character* from)
{
	short skill_id = util::to_int(arguments[0]);
	short level = -1;

	if (arguments.size() >= 2)
		level = std::max(0, std::min<int>(from->world->config["MaxSkillLevel"], util::to_int(arguments[1])));

	if (from->AddSpell(skill_id))
	{
		PacketBuilder builder(PACKET_STATSKILL, PACKET_TAKE, 6);
		builder.AddShort(skill_id);
		builder.AddInt(from->HasItem(1));
		from->Send(builder);
	}

	if (level >= 0)
	{
		auto it = std::find_if(UTIL_RANGE(from->spells), [&](Character_Spell spell) { return spell.id == skill_id; });

		if (it != from->spells.end())
		{
			it->level = level;

			PacketBuilder builder(PACKET_STATSKILL, PACKET_ACCEPT, 6);
			builder.AddShort(from->skillpoints);
			builder.AddShort(skill_id);
			builder.AddShort(it->level);
			from->Send(builder);
		}
	}
}

void QuestState(const std::vector<std::string>& arguments, Character* from)
{
	World* world = from->SourceWorld();
	short quest_id = util::to_int(arguments[0]);

	Quest_Context* quest = from->GetQuest(quest_id);

	if (!quest)
	{
		auto it = world->quests.find(quest_id);

		if (it != world->quests.end())
		{
			// WARNING: holds a non-tracked reference to shared_ptr
			quest = new Quest_Context(from, it->second.get());
			from->quests.insert(std::make_pair(it->first, quest));
			quest->SetState("begin", true);
		}
	}

	if (!quest)
	{
		from->ServerMsg(world->i18n.Format("quest_not_found"));
	}
	else
	{
		try
		{
			quest->SetState(arguments[1]);
		}
		catch (EOPlus::Runtime_Error& e)
		{
			from->ServerMsg(world->i18n.Format("quest_state_not_found"));
		}
	}
}

COMMAND_HANDLER_REGISTER()
	RegisterCharacter({"sitem", {"item"}, {"amount"}, 2}, SpawnItem);
	RegisterCharacter({"ditem", {"item"}, {"amount", "x", "y"}, 2}, DropItem);
	RegisterCharacter({"snpc", {"npc"}, {"amount"}, 2}, SpawnNPC);
	RegisterCharacter({"learn", {"skill"}, {"level"}}, Learn);
	RegisterCharacter({"qstate", {"quest", "state"}}, QuestState);
	RegisterAlias("si", "sitem");
	RegisterAlias("di", "ditem");
	RegisterAlias("sn", "snpc");
COMMAND_HANDLER_REGISTER_END()

}