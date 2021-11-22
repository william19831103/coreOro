

#include "ScriptPCH.h"

bool GossipHello_Item_Requester_NPC(Player* player, Creature* creature)
{
    if (player->IsInCombat())
    {
        player->GetSession()->SendNotification("You are in combat!");
        return true;
    }

    player->PlayerTalkClass->ClearMenus();
	//player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Fine! I got one.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "", true);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Let me browse through your stuff.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
	player->SEND_GOSSIP_MENU(600018, creature->GetObjectGuid());
    return true;
}

bool GossipSelect_Item_Requester_NPC(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();
    if (action == GOSSIP_ACTION_INFO_DEF+2)
    {
		player->SetAuctionAccessMode(1);
		player->GetSession()->SendAuctionHello(player);
    }

    return true;
}

bool OnGossipSelectCode_Item_Requester_NPC(Player* player, Creature* creature, uint32 sender, uint32 action, char const* code)
{
    player->PlayerTalkClass->ClearMenus();
    if (sender == GOSSIP_SENDER_MAIN)
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
			{
                if(strlen(code) > 5)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("Invalid ItemId");
                    player->CLOSE_GOSSIP_MENU();
                    return false;
                }

                uint32 itemId = atoi(code);

                if(!itemId)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("Invalid ItemId");
                    player->CLOSE_GOSSIP_MENU();
                    return false;
                }

                ItemPrototype const* item_proto = ObjectMgr::GetItemPrototype(itemId);

                if (!item_proto)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("Item does not exist.");
                    player->CLOSE_GOSSIP_MENU();
                    return false;
                }

                if (player->GetSession()->GetSecurity() < SEC_GAMEMASTER)
                {
                    /*
                    switch(item_proto->InventoryType)
                    {
                        case INVTYPE_BAG:
                        case INVTYPE_NON_EQUIP:
                        case INVTYPE_BODY:
                        case INVTYPE_TABARD:
                            ChatHandler(player->GetSession()).PSendSysMessage("Item is on the exclusion list.");
                            player->CLOSE_GOSSIP_MENU();
                            return false;
                    }
                    */

                    if (item_proto->ItemLevel > 92)
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("Item level too high.");
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }

                    // Item Ids Here.

                    if (item_proto->ItemId == 12947)
                    {
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }

                    if (item_proto->ItemId == 18970)
                    {
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }
                    // GM OUTFIT
                    if (item_proto->ItemId == 2586)
                    {
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }
                    // GM OUTFIT
                    if (item_proto->ItemId == 11508)
                    {
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }
                    // GM OUTFIT
                    if (item_proto->ItemId == 12064)
                    {
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }

                    // Item Ids Above.

                    if (item_proto->Quality > 4)
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("Item is on the exclusion list.");
                        player->CLOSE_GOSSIP_MENU();
                        return false;
                    }
                }

                /*if(item_proto->Flags & ITEM_FLAGS_EXTRA_HORDE_ONLY && player->GetTeam() == ALLIANCE)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("This item is not for your faction.");
                    player->CLOSE_GOSSIP_MENU();
                    return false;
                }

                if(item_proto->Flags & ITEM_FLAGS_EXTRA_ALLIANCE_ONLY && player->GetTeam() == HORDE)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("This item is not for your faction.");
                    player->CLOSE_GOSSIP_MENU();
                    return false;
                }*/
                
				// Check if we need some Reputation for the Item and set it to Exalted.
				if (item_proto->RequiredReputationFaction && item_proto->RequiredReputationRank > 0)
				{
					if (ReputationRank(item_proto->RequiredReputationRank) > player->GetReputationRank(item_proto->RequiredReputationFaction))
						player->GetReputationMgr().ModifyReputation(sObjectMgr.GetFactionEntry(item_proto->RequiredReputationFaction), 85000);
				}

                    player->CastSpell(player, 28136, false);
                    player->AddItem(itemId, item_proto->GetMaxStackSize());
                    player->CLOSE_GOSSIP_MENU();
                    return true;
					GossipHello_Item_Requester_NPC(player, creature);
					}

        }
    }
    
    return false;
}

void AddSC_Item_Requester_NPC()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "Item_Requester_NPC";
    pNewScript->pGossipHello = &GossipHello_Item_Requester_NPC;
    pNewScript->pGossipSelect = &GossipSelect_Item_Requester_NPC;
    pNewScript->pGossipSelectWithCode = &OnGossipSelectCode_Item_Requester_NPC;
    pNewScript->RegisterSelf();
}