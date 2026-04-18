/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

 #include "AddonIO.h"
 #include "BattlePassService.h"
 #include "AccountMgr.h"
 #include "StringFormat.h"
 #include "Guild.h"
 #include "ObjectAccessor.h"
 #include "ObjectMgr.h"
 #include "Chat.h"
 #include "World.h"
 #include <boost/algorithm/string.hpp>
 #include "PromotionCode/PromotionCodeMgr.h"
 #include "PreparedStatement.h"
 #include "WorldSessionMgr.h"
 #include "DatabaseEnv.h"
 #include "Log.h"
 #include "Item.h"
 #include "WorldSession.h"
 #include <sstream>
 #include <vector>
 #include <ctime>
 
 #define INSPECT_DISTANCE                28.0f
 
// Протокол баннера смерти хардкора: совпадает с FrameXML (Custom_hardCore), не аддон
#define ASMSG_HARDCORE_DEATH            "ASMSG_HARDCORE_DEATH"

 std::unordered_map<std::string, AddonMessageHandler> addonMessagesTable =
 {
     { "ACMSG_AVERAGE_ITEM_LEVEL_REQUEST",               &AddonIO::HandleAverageItemLevelRequest            },
     //Donate Service
     { "ACMSG_SHOP_BALANCE_REQUEST",                     &AddonIO::HandleShopBalanceRequest                 }, 
     { "ACMSG_PREMIUM_INFO_REQUEST",                     &AddonIO::HandlePremiumInfoRequest                 },
     { "ACMSG_PREMIUM_RENEW_REQUEST",                    &AddonIO::HandlePremiumRenewRequest                },
     { "ACMSG_SHOP_ITEM_LIST_REQUEST",                   &AddonIO::HandleShopItemListRequest                },
     /*{ "ACMSG_SHOP_REFUNDABLE_PURCHASE_LIST_REQUEST",    &AddonIO::HandleShopRefundableListRequest          },*/
     { "ACMSG_SHOP_VERSION",                             &AddonIO::HandleShopVersionRequest                 },
     { "ACMSG_SHOP_BUY_ITEM",                            &AddonIO::HandleShopBuyItemRequest                 },
     { "ACMSG_SHOP_SPECIAL_OFFER_LIST_REQUEST",          &AddonIO::HandleShopSpecialOfferListRequest        },
     /*{ "ACMSG_SHOP_SUBSCRIPTION_LIST_REQUEST",           &AddonIO::HandleShopSubcriptionListRequest         },*/
     /*{ "ACMSG_SHOP_CATEGORY_NEW_ITEMS_REQUEST",          &AddonIO::HandleShopCategoryNewItemsRequest        },*/
     /*{ "ACMSG_SHOP_SUBSCRIBE",                           &AddonIO::HandleShopSubscribeRequest               },*/
     /*{ "ACMSG_SHOP_PURCHASE_REFUND",                     &AddonIO::HandleShopPurchaseRefundRequest          },*/
    { "ACMSG_SHOP_COLLECTION_LOAD_REQUEST",              &AddonIO::HandleShopCollectionLoadRequest          },
    { "ACMSG_SHOP_ITEM_COUNT",                           &AddonIO::HandleShopItemCountRequest               },
    { "ACMSG_HARDCORE_CREATE_SET",                       &AddonIO::HandleHardcoreCreateSet                  },
    //Guild System
    { "ACMSG_GUILD_SPELLS_REQUEST",                      &AddonIO::HandleGuildSpellsRequest                 },
    { "ACMSG_GUILD_LEVEL_REQUEST",                       &AddonIO::HandleGuildLevelRequest                  },
    { "ACMSG_GUILD_ONLINE_REQUEST",                      &AddonIO::HandleGuildOnlineRequest                 },
    { "ACMSG_GUILD_ILVLS_REQUEST",                       &AddonIO::HandleGuildIlvlsRequest                  },
    { "ACMSG_GUILD_EMBLEM_REQUEST",                      &AddonIO::HandleGuildEmblemRequest                 },
    { "ACMSG_GUILD_GET_REPUTATION_REWARD",               &AddonIO::HandleGuildGetReputationReward           },

    { "ACMSG_PROMOCODE_REWARD",                          &AddonIO::HandlePromoCodeRewardRequest             },
    { "ACMSG_PROMOCODE_SUBMIT",                          &AddonIO::HandlePromoCodeSubmitRequest             },
    { "ACMSG_BATTLEPASS_GET",                            &AddonIO::HandleBattlePassGetRequest               },
    { "ACMSG_BATTLEPASS_POST",                           &AddonIO::HandleBattlePassPostRequest               },
    { "ACMSG_ONLINEREWARD_GET",                          &AddonIO::HandleOnlineRewardGetRequest             },
    { "ACMSG_ONLINEREWARD_POST",                         &AddonIO::HandleOnlineRewardPostRequest            },
    { "AC_CU_GET",                                       &AddonIO::HandleCustomGet                          },
    { "AC_CU_POST",                                      &AddonIO::HandleCustomPost                         },
 };
 
 /*********SHOPSERVICE*************/
 enum STORE_ENUM{
     STORE_PREMIUM_BUY_1         = 1,
     STORE_PREMIUM_BUY_2         = 2,
     STORE_PREMIUM_BUY_3         = 3,
     STORE_PREMIUM_BUY_4         = 4,
 
     COST_STORE_PREMIUM_BUY_1    = 1,
     COST_STORE_PREMIUM_BUY_2    = 6,
     COST_STORE_PREMIUM_BUY_3    = 12,
     COST_STORE_PREMIUM_BUY_4    = 25,
 
     TIME_STORE_PREMIUM_BUY_1    = 86400,
     TIME_STORE_PREMIUM_BUY_2    = 604800,
     TIME_STORE_PREMIUM_BUY_3    = 1209600,
     TIME_STORE_PREMIUM_BUY_4    = 2592000,
 };
 
 enum PAID_SERVICE {
     PAID_SERVICE_NAME_CHANGE            = 1000000,
     PAID_SERVICE_FACTION_CHANGE         = 1000001,
     PAID_SERVICE_RACE_CHANGE            = 1000002,
     PAID_SERVICE_GUILDNAME_CHANGE       = 1000003,
     PAID_SERVICE_GOLD_BUY               = 1000004,
     PAID_SERVICE_ALCHEMY_LEARN          = 1000005,
     PAID_SERVICE_BLACKSMITHING_LEARH    = 1000006,
     PAID_SERVICE_ENCHANTING_LEARN       = 1000007,
     PAID_SERVICE_ENGINEERING_LEARN       = 1000008,
     PAID_SERVICE_JEWELCRAFTING_LEARN    = 1000009,
     PAID_SERVICE_HERBALISM_LEARN        = 1000010,
     PAID_SERVICE_LEATHERWORKING_LEARN   = 1000011,
     PAID_SERVICE_MINING_LEARN           = 1000012,
     PAID_SERVICE_SKINNING_LEARN         = 1000013,
     PAID_SERVICE_TAILORING_LEARN        = 1000014,
     PAID_SERVICE_FISHING_LEARN          = 1000015,
     PAID_SERVICE_INSCRIPTION_LEARN      = 1000016,
     PAID_SERVICE_COOKING_LEARN          = 1000017,
     PAID_SERVICE_FIRST_AID_LEARN        = 1000018,
     PAID_SERVICE_LEVELUP                = 1000020,
     //MMOTOP
     PAID_SERVICE_PREMIUM_ONE_DAY        = 1000019,
 };
 
 #define ARMORY_CATEGORYID               4
 
 uint8 ShopProfesstionResponse(Player* pl, SkillType skill)
 {
     if (pl->GetLevel() < DEFAULT_MAX_LEVEL)
         return 7; //ERROR_LOW_LEVEL
     else if (pl->PlayerAlreadyHasTwoProfessions(pl) && !pl->IsSecondarySkill(skill))
         return 6; //ERROR_PROF_TWO_IS_EXIST
     else
     {
         if (pl->LearnAllRecipesInProfession(pl, skill))
             return 0; //OK
     }
 
     return 1; //ERROR
 }
 
 bool ShopSendItem(Player* m_sender, Player* m_receiver, std::string m_text, uint32 item, uint32 count)
 {
     Player* receiver = m_receiver;
     ObjectGuid receiverGuid = receiver->GetGUID();
 
     std::string subject = "Refund";
     std::string text = m_text;
 
     typedef std::pair<uint32, uint32> ItemPair;
     typedef std::list< ItemPair > ItemPairs;
     ItemPairs items;
 
     uint32 itemId = item;
 
     ItemTemplate const* item_proto = sObjectMgr->GetItemTemplate(itemId);
     if (!item_proto)
         return false;
 
     uint32 itemCount = count;
     if (itemCount < 1 || (item_proto->MaxCount > 0 && itemCount > uint32(item_proto->MaxCount)))
         return false;
 
 
     while (itemCount > item_proto->GetMaxStackSize())
     {
         items.push_back(ItemPair(itemId, item_proto->GetMaxStackSize()));
         itemCount -= item_proto->GetMaxStackSize();
     }
 
     items.push_back(ItemPair(itemId, itemCount));
 
     if (items.size() > MAX_MAIL_ITEMS)
         return false;
 
 
     MailSender sender(MAIL_NORMAL, m_sender->GetSession() ? m_sender->GetSession()->GetPlayer()->GetGUID() : 0, MAIL_STATIONERY_GM);
 
 
     MailDraft draft(subject, text);
 
     CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
 
     for (ItemPairs::const_iterator itr = items.begin(); itr != items.end(); ++itr)
     {
         if (Item* item = Item::CreateItem(itr->first, itr->second, m_sender->GetSession() ? m_sender->GetSession()->GetPlayer() : 0))
         {
             item->SaveToDB(trans);
             draft.AddItem(item);
         }
     }
 
     // draft.SendMailTo(trans, MailReceiver(receiver, GUID_LOPART(receiverGuid)), sender);
     draft.SendMailTo(trans, MailReceiver(receiver), sender);
     CharacterDatabase.CommitTransaction(trans);
 
     return true;
 }
 
 uint8 ShopAddItem(Player* player, Player* receiver, uint32 itemId, uint32 count, uint8 moneyID, uint32 cost, std::string text = "")
 {
     ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(itemId);
     if (!itemTemplate)
         return 4;
 
     if (moneyID == 10)
         player->ModifyMoney(-cost);
     else if (!player->GetSession()->SetAccountCurrency(cost, moneyID, false))
         return 1; //UNKNOWN_ERROR
     else if (moneyID == 1)
         player->GetSession()->WritePurchaseToLogs(player->GetSession(), "BUY ITEM", itemId, count, cost, uint32(time(nullptr)));
 
     uint32 noSpaceForCount = 0;
 
     ItemPosCountVec dest;
     InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount);
     if (msg != EQUIP_ERR_OK)
         count -= noSpaceForCount;
 
     Item* item = player->StoreNewItem(dest, itemId, true);
 
     if (player)
     {
         for (ItemPosCountVec::const_iterator itr = dest.begin(); itr != dest.end(); ++itr)
             if (Item* item1 = player->GetItemByPos(itr->pos))
                 item1->SetBinding(false);
     }
 
     if (count > 0 && item)
     {
         if (text == "")
             player->SendNewItem(item, count, false, true);
         else
             ShopSendItem(player, receiver, text, itemId, count);
     }
 
 
     if (noSpaceForCount > 0)
         ShopSendItem(player, player, "Return of lost items", itemId, noSpaceForCount);
 
     return 0;
 }
 
 uint8 ShopPaidService(Player* player, uint32 itemId, uint32 count, uint8 moneyID, uint32 cost, bool isProfession)
 {
     auto sess = player->GetSession();
     uint8 p_resp = 0;
 
     if (!player->GetSession()->SetAccountCurrency(cost, moneyID, isProfession))
         p_resp = 1;
     else
     {
         uint32 atLoginFlag = 0;
         bool isRaceFaction = false;
 
         switch (itemId)
         {
         case PAID_SERVICE_NAME_CHANGE:
         {
             atLoginFlag = AT_LOGIN_RENAME;
             player->SetAtLoginFlag(AT_LOGIN_RENAME);
             isRaceFaction = true;
             player->GetSession()->WritePurchaseToLogs(sess, "NAME_CHANGE", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_FACTION_CHANGE:
         {
             atLoginFlag = AT_LOGIN_CHANGE_FACTION;
             player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
             isRaceFaction = true;
             player->GetSession()->WritePurchaseToLogs(sess, "FACTION_CHANGE", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_RACE_CHANGE:
         {
             atLoginFlag = AT_LOGIN_CHANGE_RACE;
             player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
             isRaceFaction = true;
             player->GetSession()->WritePurchaseToLogs(sess, "RACE_CHANGE", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_GUILDNAME_CHANGE:
             break;
         case PAID_SERVICE_GOLD_BUY:
         {
             player->ModifyMoney(count * GOLD * 1000);
             player->GetSession()->WritePurchaseToLogs(sess, "GOLD_BUY", 0, count, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_ALCHEMY_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_ALCHEMY);
 
             if (p_resp == 0)
             {
                 player->GetSession()->WritePurchaseToLogs(sess, "ALCHEMY_LEARN", 0, 0, cost, uint32(time(nullptr)));
                 
                 player->removeSpell(28675, false, false);
                 player->removeSpell(28677, false, false);
                 player->removeSpell(28672, false, false);
 
             }
 
             break;
         }
         case PAID_SERVICE_BLACKSMITHING_LEARH:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_BLACKSMITHING);
 
             if (p_resp == 0)
             {
                 player->GetSession()->WritePurchaseToLogs(sess, "BLACKSMITHING_LEARN", 0, 0, cost, uint32(time(nullptr)));
                 player->removeSpell(9787, false, false);
                 player->removeSpell(17041, false, false);
                 player->removeSpell(17040, false, false);
                 player->removeSpell(17039, false, false);
                 player->removeSpell(9788, false, false);
             }
             
 
             break;
         }
         case PAID_SERVICE_ENCHANTING_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_ENCHANTING);
 
             if (p_resp == 0)
                 player->GetSession()->WritePurchaseToLogs(sess, "ENCHANTING_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_ENGINEERING_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_ENGINEERING);
 
             if (p_resp == 0)
             {
                 player->GetSession()->WritePurchaseToLogs(sess, "ENGINEERING_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
                 player->removeSpell(20222, false, false);
                 player->removeSpell(20219, false, false);
             }
 
             break;
         }
         case PAID_SERVICE_JEWELCRAFTING_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_JEWELCRAFTING);
 
             if (p_resp == 0)
                 player->GetSession()->WritePurchaseToLogs(sess, "JEWELCRAFTING_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_HERBALISM_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_HERBALISM);
 
             if (p_resp == 0)
             {
                 player->GetSession()->WritePurchaseToLogs(sess, "HERBALISM_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
                 player->removeSpell(2369, false, false);
                 player->removeSpell(2371, false, false);
             }
 
             break;
         }
         case PAID_SERVICE_LEATHERWORKING_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_LEATHERWORKING);
 
             if (p_resp == 0)
                 player->GetSession()->WritePurchaseToLogs(sess, "LEATHERWORKING_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_MINING_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_MINING);
 
             if (p_resp == 0)
                 player->GetSession()->WritePurchaseToLogs(sess, "MINING_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_SKINNING_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_SKINNING);
 
             if (p_resp == 0)
                 player->GetSession()->WritePurchaseToLogs(sess, "SKINNING_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_TAILORING_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_TAILORING);
 
             if (p_resp == 0)
                 player->GetSession()->WritePurchaseToLogs(sess, "TAILORING_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_FISHING_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_FISHING);
 
             if (p_resp == 0)
                 player->GetSession()->WritePurchaseToLogs(sess, "FISHING_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_INSCRIPTION_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_INSCRIPTION);
 
             if (p_resp == 0)
                 player->GetSession()->WritePurchaseToLogs(sess, "INSCRIPTION_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_FIRST_AID_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_FIRST_AID);
 
             if (p_resp == 0)
                 player->GetSession()->WritePurchaseToLogs(sess, "FIRST_AID_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_COOKING_LEARN:
         {
             p_resp = ShopProfesstionResponse(player, SKILL_COOKING);
 
             if (p_resp == 0)
                 player->GetSession()->WritePurchaseToLogs(sess, "COOKING_LEARN", 0, 0, cost, uint32(time(nullptr)));
 
             break;
         }
         case PAID_SERVICE_PREMIUM_ONE_DAY:
             //player->GetSession()->SetAccountPremium(TIME_STORE_PREMIUM_BUY_1);
             break;
         case PAID_SERVICE_LEVELUP:
             player->GiveLevel(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));
             break;
         default:
             break;
         }
 
         if (isRaceFaction)
         {
             CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
             stmt->SetData(0, uint16(atLoginFlag));
             stmt->SetData(1, player->GetGUID().GetCounter());
             CharacterDatabase.Execute(stmt);
         }
 
         if (isProfession && p_resp == 0)
             player->GetSession()->SetAccountCurrency(cost, moneyID, false);
     }
 
     return p_resp;
 }
 /******************************/
 
 AddonIO::AddonIO() { }
 AddonIO::~AddonIO() { }
 
 AddonIO* AddonIO::instance()
 {
     static AddonIO instance;
     return &instance;
 }
 
void AddonIO::HandleMessage(Player* player, std::string message)
{
    if (!player)
        return;

    std::vector<std::string> args;
    boost::split(args, message, boost::is_any_of("\t"));

    // Some clients send addon messages with empty body as just "Prefix"
    // (without the "\t"). Support both:
    //  1) "Prefix\tBody"
    //  2) "Prefix" (-> Body = "")
    if (args.size() == 1)
        args.push_back("");

    if (args.size() != 2)
    {
        LOG_ERROR("HardCore", "AddonIO HandleMessage: expected 'Prefix\\tBody', got {} parts (msg='{}')", args.size(), message);
        return;
    }

    auto itr = addonMessagesTable.find(args[0]);
    if (itr == addonMessagesTable.end())
        return;

    // Hardcore: магазин полностью недоступен до макс. уровня (80)
    if (player->IsHardcore() && player->GetLevel() < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
    {
        const char* shopPrefixes[] = {
            "ACMSG_SHOP_BALANCE_REQUEST", "ACMSG_PREMIUM_INFO_REQUEST", "ACMSG_PREMIUM_RENEW_REQUEST",
            "ACMSG_SHOP_ITEM_LIST_REQUEST", "ACMSG_SHOP_VERSION", "ACMSG_SHOP_BUY_ITEM",
            "ACMSG_SHOP_SPECIAL_OFFER_LIST_REQUEST", "ACMSG_SHOP_COLLECTION_LOAD_REQUEST", "ACMSG_SHOP_ITEM_COUNT"
        };
        for (const char* p : shopPrefixes)
            if (args[0] == p)
                return;
    }

   (this->*itr->second)(player, args[1]);
}

void AddonIO::HandleHardcoreCreateSet(Player* player, std::string body)
{
    LOG_ERROR("HardCore", "ACMSG_HARDCORE_CREATE_SET received: body='{}' player={} guid={} level={}",
        body, player ? player->GetName() : "nil", player ? player->GetGUID().ToString() : "nil", player ? player->GetLevel() : 0);

    if (!player)
    {
        LOG_ERROR("HardCore", "ACMSG_HARDCORE_CREATE_SET rejected: player is null");
        return;
    }
    if (body != "0" && body != "1")
    {
        LOG_ERROR("HardCore", "ACMSG_HARDCORE_CREATE_SET rejected: invalid body '{}' (expected '0' or '1')", body);
        return;
    }
    // Принимаем только для персонажей 1 уровня (AT_LOGIN_FIRST сбрасывается до прихода addon-сообщения)
    if (player->GetLevel() != 1)
    {
        LOG_ERROR("HardCore", "ACMSG_HARDCORE_CREATE_SET rejected: player {} level {} (only level 1 accepted)", player->GetName(), player->GetLevel());
        return;
    }
    uint8 hardcore = (body == "1") ? 1 : 0;
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_HARDCORE);
    stmt->SetData(0, hardcore);
    stmt->SetData(1, player->GetGUID().GetCounter());
    CharacterDatabase.Execute(stmt);
    player->SetHardcore(hardcore != 0);
    LOG_ERROR("HardCore", "ACMSG_HARDCORE_CREATE_SET applied: {} hardcore={} (DB updated)", player->GetName(), hardcore);
}

void AddonIO::BroadcastHardcoreDeath(std::string const& payload)
{
	if (payload.empty())
		return;
	std::string message = std::string(ASMSG_HARDCORE_DEATH) + "\t" + payload;
	sWorldSessionMgr->DoForAllOnlinePlayers([&message](Player* receiver)
	{
		if (!receiver || !receiver->GetSession())
			return;
		WorldPacket data;
		ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, ObjectGuid::Empty, receiver->GetGUID(), message, 0);
		receiver->GetSession()->SendPacket(&data);
	});
}

 /*****************************
 ********* HANDLERS ***********
 ******************************/

namespace
{
uint32 GetAccountLoyaltyLevel(uint32 accountId)
{
    LoginDatabasePreparedStatement* stmt =
        LoginDatabase.GetPreparedStatement(LOGIN_SEL_SHOP_LOYALTY_LEVEL);
    stmt->SetData(0, accountId);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (!result)
        return 0;

    Field* fields = result->Fetch();
    return fields[0].Get<uint32>();
}

uint32 GetAccountLoyaltyPoints(uint32 accountId)
{
    LoginDatabasePreparedStatement* stmt =
        LoginDatabase.GetPreparedStatement(LOGIN_SEL_SHOP_LOYALTY_POINTS);
    stmt->SetData(0, accountId);
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (!result)
        return 0;

    Field* fields = result->Fetch();
    return fields[0].Get<uint32>();
}

uint32 CalculateLoyaltyLevelFromPoints(uint32 points)
{
    QueryResult result = LoginDatabase.Query(
        "SELECT COALESCE(MAX(`level`), 0) FROM custom_store_loyalty_level WHERE required_points <= {}",
        points);

    if (!result)
        return 0;

    Field* fields = result->Fetch();
    return fields[0].Get<uint32>();
}

uint32 SyncAccountLoyaltyLevel(uint32 accountId)
{
    uint32 points = GetAccountLoyaltyPoints(accountId);
    uint32 level = CalculateLoyaltyLevelFromPoints(points);
    uint32 currentLevel = GetAccountLoyaltyLevel(accountId);

    if (currentLevel != level)
    {
        LoginDatabasePreparedStatement* stmt =
            LoginDatabase.GetPreparedStatement(LOGIN_UPD_STORE_LOYALTY);
        stmt->SetData(0, level);
        stmt->SetData(1, accountId);
        LoginDatabase.Execute(stmt);
    }

    return level;
}

uint32 ParseOpcodeFromBody(std::string const& body)
{
    try
    {
        std::vector<std::string> parts;
        boost::split(parts, body, boost::is_any_of("|"));
        if (!parts.empty())
            return std::stoul(parts[0]);
    }
    catch (...)
    {
    }

    return 0;
}

constexpr uint32 ONLINE_REWARD_REQUIRED_TIME = 10;
constexpr uint32 LUCKY_WHEEL_OPCODE_GET_STATE = 20;
constexpr uint32 LUCKY_WHEEL_OPCODE_GET_REWARDS = 21;
constexpr uint32 LUCKY_WHEEL_OPCODE_SPIN = 22;
} // namespace

 void AddonIO::HandleAverageItemLevelRequest(Player* player, std::string body)
 {
     if (!player)
         return;
     
     if (body.empty())
         return;
 
     ObjectGuid guid = ObjectGuid::Empty;
 
     try
     {
         uint64 value = std::stoull(body, nullptr, 16);
         guid = ObjectGuid(value);
     }
     catch (std::exception /*ex*/)
     {
         //TC_LOG_ERROR("network", "ACMSG_AVERAGE_ITEM_LEVEL_REQUEST: Received invalid parameters!");
         player->SendAddonMessage("ASMSG_AVERAGE_ITEM_LEVEL_RESPONSE\t-1");
         return;
     }
 
     Player* target = ObjectAccessor::FindPlayer(guid);
     if (!target || player->IsValidAttackTarget(target))
     {
         player->SendAddonMessage("ASMSG_AVERAGE_ITEM_LEVEL_RESPONSE\t-1");
         return;
     }
 
    player->SendAddonMessage("ASMSG_AVERAGE_ITEM_LEVEL_RESPONSE\t{}", static_cast<uint32>(std::floor(target->GetAverageItemLevel())));
 }

void AddonIO::HandleGuildGetReputationReward(Player* player, std::string body)
{
    if (!player)
        return;

    if (body.empty())
        return;

    uint32 itemId = 0;
    try
    {
        itemId = Acore::StringTo<uint32>(body).value_or(0);
    }
    catch (std::exception /*ex*/)
    {
        itemId = 0;
    }

    if (!itemId)
    {
        player->SendAddonMessage("ASMSG_GUILD_GET_REPUTATION_REWARD_RESPONSE\t{}", 1); // invalid item
        return;
    }

    Guild* guild = player->GetGuild();
    if (!guild)
    {
        player->SendAddonMessage("ASMSG_GUILD_GET_REPUTATION_REWARD_RESPONSE\t{}", 2); // not in guild
        return;
    }

    // Simple implementation: check custom table guild_reputation_rewards in world DB
    // Columns: itemEntry, minGuildLevel, costGold (in copper)
    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_GUILD_REP_REWARD);
    if (!stmt)
    {
        player->SendAddonMessage("ASMSG_GUILD_GET_REPUTATION_REWARD_RESPONSE\t{}", 3); // server not configured
        return;
    }

    stmt->SetData(0, itemId);
    if (PreparedQueryResult result = WorldDatabase.Query(stmt))
    {
        Field* fields = result->Fetch();
        uint8 minLevel = fields[0].Get<uint8>();
        uint32 cost = fields[1].Get<uint32>(); // in copper

        if (guild->GetLevel() < minLevel)
        {
            player->SendAddonMessage("ASMSG_GUILD_GET_REPUTATION_REWARD_RESPONSE\t{}", 4); // guild level too low
            return;
        }

        if (player->GetMoney() < cost)
        {
            player->SendAddonMessage("ASMSG_GUILD_GET_REPUTATION_REWARD_RESPONSE\t{}", 5); // not enough money
            return;
        }

        if (!sObjectMgr->GetItemTemplate(itemId))
        {
            player->SendAddonMessage("ASMSG_GUILD_GET_REPUTATION_REWARD_RESPONSE\t{}", 1); // invalid item
            return;
        }

        player->ModifyMoney(-int32(cost));
        player->AddItem(itemId, 1);

        player->SendAddonMessage("ASMSG_GUILD_GET_REPUTATION_REWARD_RESPONSE\t{}", 0); // OK
    }
    else
    {
        player->SendAddonMessage("ASMSG_GUILD_GET_REPUTATION_REWARD_RESPONSE\t{}", 3); // no config
    }
}

 void AddonIO::HandleShopBalanceRequest(Player* player, std::string body)
 {
     if (!player)
         return;
 
     auto sess = player->GetSession();
 
    uint32 loyaltyLevel = SyncAccountLoyaltyLevel(sess->GetAccountId());

    player->SendAddonMessage("ASMSG_SHOP_BALANCE_RESPONSE\t{}:{}:{}:{}:{}:{}:{}",
        sess->GetAccountBalance(), sess->GetAccountVote(), loyaltyLevel, 0, 0, 0, 0);
 }
 
 void AddonIO::HandleShopItemListRequest(Player* player, std::string body)
 {
     if (!player)
         return;
 
     if (!sWorld->getBoolConfig(CONFIG_SHOP_ENABLE))
         return;
 
     auto store = sWorld->GetStoreItem();
     auto collection = sWorld->GetStorCollection();
     
     if (!store.empty())
     {
         for (auto it = store.begin(); it != store.end(); ++it)
         {
             if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(it->second.itemEntry))
             {
                 if (it->second.CategoryID == ARMORY_CATEGORYID)
                 {
                     if (proto->Class == ITEM_CLASS_ARMOR && proto->SubClass == ITEM_SUBCLASS_ARMOR_PLATE && !player->HasSpell(750))
                         continue;
 
                     if (proto->Class == ITEM_CLASS_ARMOR && proto->SubClass == ITEM_SUBCLASS_ARMOR_MAIL && !player->HasSpell(8737))
                         continue;
 
                     if (proto->Class == ITEM_CLASS_ARMOR && proto->SubClass == ITEM_SUBCLASS_ARMOR_LEATHER && !player->HasSpell(9077))
                         continue;
 
                     if (!(1 << (player->getClass() - 1) & proto->AllowableClass))
                         continue;
 
                     if (!(1 << (player->getRace() - 1) & proto->AllowableRace))
                         continue;
                 }
                 
                 player->SendAddonMessage("ASMSG_SHOP_ITEM\t{}:{}:{}:{}:{}:{}:{}:{}:{}:{}:{}", it->first, it->second.itemEntry, it->second.count, it->second.price, it->second.discount, it->second.discountPrice, it->second.creatureEntry, it->second.storeFlags, it->second.CategoryID, it->second.SubCategoryID, it->second.MoneyID);
             }
         }
 
     }
     
 }
 
 void AddonIO::HandleShopVersionRequest(Player* player, std::string body)
 {
     if (!player)
         return;
 
     player->SendAddonMessage("ASMSG_SHOP_VERSION\t{}:{}", sWorld->GetShopVersion(), sWorld->getBoolConfig(CONFIG_SHOP_ENABLE) ? 1 : 0);
 }
 
 void AddonIO::HandleShopBuyItemRequest(Player* player, std::string body)
 {
     if (!player)
         return;
 
     if (body.empty())
         return;
 
     if (!sWorld->getBoolConfig(CONFIG_SHOP_ENABLE))
         return;
 
     auto sess = player->GetSession();
 
     uint8 p_resp = 1;
    uint32 item = 0, cost = 0, requiredLevel = 0, count = 1, db_count = 1, moneyID = 1, f_cost = 0, atLoginFlag = 0;
     int32 balance = 0;
 
     try
     {
         std::vector<std::string> par;
         boost::split(par, body, boost::is_any_of(":"));
 
         if (!par.empty())
         {
             auto store = sWorld->GetStoreItem();
             if (!store.empty())
             {
                 for (auto it = store.begin(); it != store.end(); ++it)
                     if (it->first == std::stoi(par[0]))
                     {
                         item = it->second.itemEntry;
                        cost = it->second.discountPrice;
                        requiredLevel = it->second.price;
                         db_count = it->second.count;
                         moneyID = it->second.MoneyID;
                         if (par.size() != 2)
                             count = std::stoi(par[1]);
                         break;
                     }
             }
 
            if (moneyID == 1)
                 balance = sess->GetAccountBalance();
            else if (moneyID == 2)
                 balance = sess->GetAccountVote();
            else if (moneyID == 3)
                balance = 2147483647;
            else
                balance = 0;
 
             if (db_count > 1 && db_count != count)
                 count = db_count;
 
             if (db_count == 1) f_cost = cost * count;
             else f_cost = cost;
 
             if (balance > 0)
                 if (balance >= f_cost)
                 {
                     switch (par.size())
                     {
                     case 4:
                     {
                         switch (item)
                         {
                         case PAID_SERVICE_ALCHEMY_LEARN:
                         case PAID_SERVICE_BLACKSMITHING_LEARH:
                         case PAID_SERVICE_ENCHANTING_LEARN:
                         case PAID_SERVICE_ENGINEERING_LEARN:
                         case PAID_SERVICE_JEWELCRAFTING_LEARN:
                         case PAID_SERVICE_HERBALISM_LEARN:
                         case PAID_SERVICE_LEATHERWORKING_LEARN:
                         case PAID_SERVICE_MINING_LEARN:
                         case PAID_SERVICE_SKINNING_LEARN:
                         case PAID_SERVICE_TAILORING_LEARN:
                         case PAID_SERVICE_FISHING_LEARN:
                         case PAID_SERVICE_INSCRIPTION_LEARN:
                         case PAID_SERVICE_FIRST_AID_LEARN:
                         case PAID_SERVICE_COOKING_LEARN:
                             p_resp = ShopPaidService(player, item, count, moneyID, f_cost, true);
                             break;
                         case PAID_SERVICE_NAME_CHANGE:
                         case PAID_SERVICE_FACTION_CHANGE:
                         case PAID_SERVICE_RACE_CHANGE:
                         case PAID_SERVICE_GUILDNAME_CHANGE:
                         case PAID_SERVICE_GOLD_BUY:
                         case PAID_SERVICE_PREMIUM_ONE_DAY:
                         case PAID_SERVICE_LEVELUP:
                             p_resp = ShopPaidService(player, item, count, moneyID, f_cost, false);
                             break;
                        default:
                        {
                            if (moneyID == 3)
                            {
                                uint32 accountLoyaltyLevel = SyncAccountLoyaltyLevel(sess->GetAccountId());
                                if (accountLoyaltyLevel < requiredLevel)
                                    p_resp = 1;
                                else
                                    p_resp = ShopAddItem(player, player, item, count, 10, 0);
                            }
                            else
                                p_resp = ShopAddItem(player, player, item, count, moneyID, f_cost);
                            break;
                        }
                         }
 
                         break;
                     }
                     case 6:
                     {
                         switch (item)
                         {
                         case PAID_SERVICE_NAME_CHANGE:
                         case PAID_SERVICE_FACTION_CHANGE:
                         case PAID_SERVICE_RACE_CHANGE:
                         case PAID_SERVICE_GUILDNAME_CHANGE:
                         case PAID_SERVICE_GOLD_BUY:
                         case PAID_SERVICE_ALCHEMY_LEARN:
                         case PAID_SERVICE_BLACKSMITHING_LEARH:
                         case PAID_SERVICE_ENCHANTING_LEARN:
                         case PAID_SERVICE_ENGINEERING_LEARN:
                         case PAID_SERVICE_JEWELCRAFTING_LEARN:
                         case PAID_SERVICE_HERBALISM_LEARN:
                         case PAID_SERVICE_LEATHERWORKING_LEARN:
                         case PAID_SERVICE_MINING_LEARN:
                         case PAID_SERVICE_SKINNING_LEARN:
                         case PAID_SERVICE_TAILORING_LEARN:
                         case PAID_SERVICE_FISHING_LEARN:
                         case PAID_SERVICE_INSCRIPTION_LEARN:
                         case PAID_SERVICE_COOKING_LEARN:
                         case PAID_SERVICE_FIRST_AID_LEARN:
                         case PAID_SERVICE_PREMIUM_ONE_DAY:
                         case PAID_SERVICE_LEVELUP:
                             break;
                         default:
                         {
                             Player* target;
                             std::string targetName;
                             //uint32 parseGUID = MAKE_NEW_GUID(atol(par[3].c_str()), 0, HighGuid::Player);
                             //uint32 parseGUID = ObjectGuid::Create<LowGuid::Player>(atol(par[3].c_str()));
                             ObjectGuid parseGUID = ObjectGuid::Create<HighGuid::Player>(atol(par[3].c_str()));
                             if (sCharacterCache->GetCharacterNameByGuid(parseGUID, targetName))
                                 target = ObjectAccessor::FindPlayer(parseGUID);
 
                             if (target == nullptr)
                             {
                                 p_resp = 2; //ERROR_RECEIVER_NOT_FOUND
                                 break;
                             }
 
                             if (player == target)
                             {
                                 p_resp = 3; //ERROR_CANNOT_GIFT_TO_SELF
                                 break;
                             }
 
                            if (moneyID == 3)
                            {
                                uint32 accountLoyaltyLevel = SyncAccountLoyaltyLevel(sess->GetAccountId());
                                if (accountLoyaltyLevel < requiredLevel)
                                    p_resp = 1;
                                else
                                    p_resp = ShopAddItem(player, target, item, count, 10, 0, par[5].c_str());
                            }
                            else
                                p_resp = ShopAddItem(player, target, item, count, moneyID, f_cost, par[5].c_str());
 
                             break;
                         }
                         }
 
                         break;
                     }
                     }
                 }
         }
 
         player->SendAddonMessage("ASMSG_SHOP_BUY_ITEM_RESPONSE\t{}:{}", p_resp, item);
     }
     catch (std::exception /*ex*/)
     {
         return;
     }
 }
 
 void AddonIO::HandleShopSpecialOfferListRequest(Player* player, std::string body)
 {
     if (!player)
         return;
 
     auto sess = player->GetSession();
     auto offer = sWorld->GetStoreSpecialOffer();
     auto details = sWorld->GetStoreSpecialDetails();
 
     uint32 detailId = 0;
     char sub_resp[256], response[256];
 
     /*if (!offer.empty())
         for (auto it = offer.begin(); it != offer.end(); ++it)
         {
             detailId = it->second.details;
             Acore::StringFormat("ASMSG_SHOP_SPECIAL_OFFER_INFO\t{}|{}|{}|{}|{}|{}|{}|{}|{}|0|0|0", it->first, it->second.background, it->second.headline, it->second.title, it->second.description, it->second.time, it->second.productID, it->second.itemEntry, it->second.price).copy(response, 255);
             response[255] = '\0';
             Acore::StringFormat("ASMSG_SHOP_SPECIAL_OFFER_DETAILS\t{}|{}|{}|{}|", it->first, it->second.title, it->second.detailsTitle, it->second.price).copy(sub_resp, 255);
             sub_resp[255] = '\0';
         }*/
 
     if (!details.empty())
     {
         //offerId|title|detailTitle|price|data: {itemId<role><count>:..:}
         for (auto it = details.begin(); it != details.end(); ++it)
             if (it->first == detailId)
             {
                 char a[256];
                 Acore::StringFormat("{}<{}><{}>:", it->second.itemID, it->second.role, it->second.count).copy(a, 255);
                 a[255] = '\0';
                 strcat(sub_resp, a);
             }
 
         player->SendAddonMessage(sub_resp);
     }
 }
 
 void AddonIO::HandleShopCollectionLoadRequest(Player* player, std::string body)
 {
     if (!player)
         return;
 
     auto store = sWorld->GetStoreItem();
 
     if (!store.empty())
         for (auto it = store.begin(); it != store.end(); ++it)
             if (it->second.creatureEntry != 0)
                 player->GetSession()->HandleShopCreatureOpcode(it->second.creatureEntry);
 }
 
 void AddonIO::HandleShopItemCountRequest(Player* player, std::string body)
 {
     if (!player)
         return;
 
     if (!sWorld->getBoolConfig(CONFIG_SHOP_ENABLE))
         return;
 
     player->SendAddonMessage("ASMSG_SHOP_ITEM_COUNT\t{}", sWorld->GetStoreItems());
 }

void AddonIO::HandlePremiumInfoRequest(Player* player, std::string /*body*/)
{
    if (!player || !player->GetSession())
        return;

    // Use account DB (same as LoadPremiumStatusToPlayer), not player cache, so the store
    // shows correct premium state even when the request is handled before or during
    // character load (e.g. new character, first login).
    uint32 accountId = player->GetSession()->GetAccountId();
    bool vip = AccountMgr::GetVipStatus(accountId);
    time_t unset = AccountMgr::GetVIPunsetDate(accountId);
    time_t now = time(nullptr);
    uint32 remaining = (vip && unset > now) ? static_cast<uint32>(unset - now) : 0;

    // Keep player cache in sync so other code (auras, etc.) sees correct state
    if (player->IsPremium() != vip || player->GetPremiumUnsetdate() != unset)
    {
        player->SetPremiumStatus(vip);
        player->SetPremiumUnsetdate(vip ? unset : 0);
    }

    player->SendAddonMessage("ASMSG_PREMIUM_INFO_RESPONSE\t{}", remaining);
}

void AddonIO::HandlePremiumRenewRequest(Player* player, std::string body)
{
    if (!player || !player->GetSession() || body.empty() || !sWorld->getBoolConfig(CONFIG_SHOP_ENABLE))
        return;

    try
    {
        auto mes = std::stoi(body);

        auto sess = player->GetSession();
        uint32 cost = 0, prem_time = 0;
        uint8 p_resp = 1;

        switch (mes)
        {
        case STORE_PREMIUM_BUY_1:
            cost = COST_STORE_PREMIUM_BUY_1;
            prem_time = TIME_STORE_PREMIUM_BUY_1;
            break;
        case STORE_PREMIUM_BUY_2:
            cost = COST_STORE_PREMIUM_BUY_2;
            prem_time = TIME_STORE_PREMIUM_BUY_2;
            break;
        case STORE_PREMIUM_BUY_3:
            cost = COST_STORE_PREMIUM_BUY_3;
            prem_time = TIME_STORE_PREMIUM_BUY_3;
            break;
        case STORE_PREMIUM_BUY_4:
            cost = COST_STORE_PREMIUM_BUY_4;
            prem_time = TIME_STORE_PREMIUM_BUY_4;
            break;
        default:
            break;
        }

        if (cost && sess->GetAccountBalance() >= cost)
        {
            if (sess->SetAccountCurrency(cost, 1, false))
            {
                bool vip = AccountMgr::GetVipStatus(sess->GetAccountId());
                if (vip)
                {
                    player->SetPremiumUnsetdate(prem_time + player->GetPremiumUnsetdate());
                    AccountMgr::UpdateVipStatus(sess->GetAccountId(), prem_time + player->GetPremiumUnsetdate());
                    player->SetPremiumStatus(true);
                }
                else
                {
                    player->SetPremiumUnsetdate(prem_time + time(nullptr));
                    AccountMgr::SetVipStatus(sess->GetAccountId(), prem_time + time(nullptr));
                    player->SetPremiumStatus(true);
                }
            }

            sess->WritePurchaseToLogs(sess, "PREMIUM", prem_time, 0, cost, uint32(time(nullptr)));

            p_resp = 0; //ok

            if (sWorld->getBoolConfig(CONFIG_VIP_DEBUFF) && !player->InBattleground() && !player->HasStealthAura() && player->IsAlive())
            {
                uint32 vipSpellId = sWorld->getIntConfig(CONFIG_VIP_DEBUFF_SPELL);
                // Remove negative auras but never the VIP premium aura (renewal must not strip it)
                player->RemoveAppliedAuras([vipSpellId](AuraApplication const* aurApp) {
                    return !aurApp->IsPositive() && aurApp->GetBase()->GetId() != vipSpellId;
                });
                if (vipSpellId)
                    player->CastSpell(player, vipSpellId, true);
            }
        }

        player->SendAddonMessage("ASMSG_PREMIUM_RENEW_RESPONSE\t{}", p_resp);

    }
    catch (std::exception /*ex*/)
    {
        return;
    }

}

void AddonIO::HandleGuildSpellsRequest(Player* player, std::string /*body*/)
{
    if (!player)
        return;

    if (!sGuildPerkSpellsStore.empty())
    {
        std::string response = "ASMSG_GUILD_SPELLS_RESPONSE\t";
        for (auto it = sGuildPerkSpellsStore.begin(); it != sGuildPerkSpellsStore.end(); ++it)
            response += std::to_string(it->second) + ":" + std::to_string(it->first) + ",";

            LOG_ERROR("guild", "Sending Guild Spell Response: {}", response);

        player->SendAddonMessage(response.c_str());
    }
}

void AddonIO::HandleGuildLevelRequest(Player* player, std::string /*body*/)
{
    if (!player)
        return;

    if (Guild* guild = player->GetGuild())
    {
        uint8 lvl = guild->GetLevel() == GUILD_MAX_LEVEL ? (GUILD_MAX_LEVEL - 1) : guild->GetLevel();
        uint32 xp_for_old_lvl = lvl != 0 ? sWorld->GetXpForNextLevel(lvl - 1) : 0;
        uint32 xp_for_next_lvl = sWorld->GetXpForNextLevel(lvl);
        uint32 totalxp = xp_for_next_lvl - xp_for_old_lvl;
        uint32 xp = guild->GetCurrentXP() - xp_for_old_lvl;
        uint32 dailyCap = sWorld->getIntConfig(CONFIG_GUILD_DAILY_XP_CAP);

        //player->SendAddonMessage("ASMSG_GUILD_LEVEL_INFO\t%d:%d:%d:%d:%d", guild->GetLevel(), xp, totalxp, guild->GetGuildTodayXP(), dailyCap);
        //Rewriting line above , testing with buffer
        std::string message = fmt::format("ASMSG_GUILD_LEVEL_INFO\t{}:{}:{}:{}:{}",
            guild->GetLevel(), xp, totalxp, guild->GetGuildTodayXP(), dailyCap);

        LOG_ERROR("guild", "Sent HandleGuildLevelRequest message: {}", message);

        player->SendAddonMessage(message);

    }
}

void AddonIO::HandleGuildOnlineRequest(Player* player, std::string /*body*/)
{
    if (!player)
        return;

    if (Guild* guild = player->GetGuild())
    {
        int online = guild->GetOnlineMembers();
        int total = guild->GetMemberCount();

        LOG_ERROR("guild", "Sending ASMSG_GUILD_PLAYERS_COUNT: {}:{}", online, total);

        //Outdated sends %d:%d
        //player->SendAddonMessage("ASMSG_GUILD_PLAYERS_COUNT\t%d:%d", online, total);

        player->SendAddonMessage(fmt::format("ASMSG_GUILD_PLAYERS_COUNT\t{}:{}",
            guild->GetOnlineMembers(), guild->GetMemberCount()));

    }
}

void AddonIO::HandleGuildIlvlsRequest(Player* player, std::string /*body*/)
{
    if (!player)
        return;

    if (Guild* guild = player->GetGuild())
    {
        std::string response = "ASMSG_GUILD_PLAYERS_ILVL\t";
        std::unordered_map<uint32, Guild::Member> members = guild->GetMembers();
        for (const auto& itr : members)
            response += itr.second.GetName() + ":" + std::to_string(itr.second.GetAverageLvl()) + "|";

            LOG_ERROR("guild", "Sending ASMSG_GUILD_PLAYERS_ILVL: {}", response);

        player->SendAddonMessage(response.c_str());
    }
}

void AddonIO::HandleGuildEmblemRequest(Player* player, std::string /*body*/)
{
    if (!player)
        return;

    if (Guild* guild = player->GetGuild())
    {
        EmblemInfo emblem = guild->GetEmblemInfo();

        int s = emblem.GetStyle();
        int c = emblem.GetColor();
        int bs = emblem.GetBorderStyle();
        int bc = emblem.GetBorderColor();
        int bg = emblem.GetBackgroundColor();

        LOG_ERROR("guild", "Sending ASMSG_PLAYER_GUILD_EMBLEM_INFO: {}:{}:{}:{}:{}", s, c, bs, bc, bg);

        //Outdated sends %d:%d:%d:%d:%d
        //player->SendAddonMessage("ASMSG_PLAYER_GUILD_EMBLEM_INFO\t%d:%d:%d:%d:%d", s, c, bs, bc, bg);

        player->SendAddonMessage(fmt::format("ASMSG_PLAYER_GUILD_EMBLEM_INFO\t{}:{}:{}:{}:{}",
            emblem.GetStyle(), emblem.GetColor(), emblem.GetBorderStyle(),
            emblem.GetBorderColor(), emblem.GetBackgroundColor()));

    }
}

void AddonIO::HandlePromoCodeRewardRequest(Player* player, std::string body)
{
    if (!player)
        return;

    if (body.empty())
    {
        LOG_WARN("shop", "HandlePromoCodeRewardRequest: Empty promo code from player {} (Account: {})",
            player->GetName(), player->GetSession()->GetAccountId());
        player->SendAddonMessage("ASMSG_PROMOCODE_REWARD\t1"); // ERROR_INVALID_CODE
        return;
    }

    boost::algorithm::trim(body);
    if (body.empty())
    {
        player->SendAddonMessage("ASMSG_PROMOCODE_REWARD\t1"); // ERROR_INVALID_CODE
        return;
    }

    LOG_INFO("shop", "HandlePromoCodeRewardRequest: Player {} (Account: {}) requested rewards for promo code: {}",
        player->GetName(), player->GetSession()->GetAccountId(), body);

    uint32 codeId = 0;
    PromotionCodes const* promoCode = sPromotionCodeMgr->GetPromoCode(body, codeId);

    if (!promoCode)
    {
        LOG_WARN("shop", "HandlePromoCodeRewardRequest: Promo code '{}' not found for player {} (Account: {})",
            body, player->GetName(), player->GetSession()->GetAccountId());
        player->SendAddonMessage("ASMSG_PROMOCODE_REWARD\t1"); // ERROR_CODE_NOT_FOUND
        return;
    }

    if (promoCode->exist_count == 0)
    {
        LOG_WARN("shop", "HandlePromoCodeRewardRequest: Promo code '{}' has no remaining uses for player {} (Account: {})",
            body, player->GetName(), player->GetSession()->GetAccountId());
        player->SendAddonMessage("ASMSG_PROMOCODE_REWARD\t2"); // ERROR_CODE_EXPIRED
        return;
    }

    bool canUseCode = sPromotionCodeMgr->CanUsePromoCode(body, player);
    if (!canUseCode)
    {
        LOG_WARN("shop", "HandlePromoCodeRewardRequest: Player {} (Account: {}) already used promo code '{}'",
            player->GetName(), player->GetSession()->GetAccountId(), body);
        player->SendAddonMessage("ASMSG_PROMOCODE_REWARD\t3"); // ERROR_ALREADY_USED
        return;
    }

    std::ostringstream response;
    bool hasRewards = false;

    if (promoCode->item_1 > 0)
    {
        uint32 count = (promoCode->item_count_1 > 1) ? promoCode->item_count_1 : 1;
        response << "0:" << promoCode->item_1 << ":" << count;
        hasRewards = true;
    }

    if (promoCode->item_2 > 0)
    {
        if (hasRewards)
            response << "|";
        uint32 count = (promoCode->item_count_2 > 1) ? promoCode->item_count_2 : 1;
        response << "0:" << promoCode->item_2 << ":" << count;
        hasRewards = true;
    }

    if (promoCode->item_3 > 0)
    {
        if (hasRewards)
            response << "|";
        uint32 count = (promoCode->item_count_3 > 1) ? promoCode->item_count_3 : 1;
        response << "0:" << promoCode->item_3 << ":" << count;
        hasRewards = true;
    }

    if (promoCode->money > 0)
    {
        if (hasRewards)
            response << "|";
        uint32 gold = promoCode->money / 10000;
        response << "2:2:" << gold;
        hasRewards = true;
    }

    if (promoCode->coin > 0)
    {
        if (hasRewards)
            response << "|";
        response << "1:1:" << promoCode->coin;
        hasRewards = true;
    }

    bool canActivate = sPromotionCodeMgr->CanUsePromoCode(body, player);

    if (!hasRewards)
        response << "0:0:0";

    response << "|" << (canActivate ? 1 : 0);

    std::string responseStr = "ASMSG_PROMOCODE_REWARD\t" + response.str();
    player->SendAddonMessage(responseStr);

    LOG_INFO("shop", "HandlePromoCodeRewardRequest: Sent rewards for promo code '{}' to player {} (Account: {}). Can activate: {}",
        body, player->GetName(), player->GetSession()->GetAccountId(), canActivate ? 1 : 0);
}

void AddonIO::HandlePromoCodeSubmitRequest(Player* player, std::string body)
{
    if (!player)
        return;

    if (body.empty())
    {
        LOG_WARN("shop", "HandlePromoCodeSubmitRequest: Empty promo code from player {} (Account: {})",
            player->GetName(), player->GetSession()->GetAccountId());
        player->SendAddonMessage("ASMSG_PROMOCODE_SUBMIT\t1"); // ERROR_INVALID_CODE
        return;
    }

    boost::algorithm::trim(body);
    if (body.empty())
    {
        player->SendAddonMessage("ASMSG_PROMOCODE_SUBMIT\t1"); // ERROR_INVALID_CODE
        return;
    }

    LOG_INFO("shop", "HandlePromoCodeSubmitRequest: Player {} (Account: {}) trying to activate promo code: {}",
        player->GetName(), player->GetSession()->GetAccountId(), body);

    bool success = sPromotionCodeMgr->CheckedEnteredCodeByPlayer(body, player, 0);

    if (success)
    {
        uint32 codeId = 0;
        PromotionCodes const* promoCode = sPromotionCodeMgr->GetPromoCode(body, codeId);

        std::string successMessage = "Промокод успешно активирован!";
        if (promoCode)
        {
            std::vector<std::string> rewards;

            if (promoCode->money > 0)
            {
                uint32 gold = promoCode->money / 10000;
                rewards.push_back(Acore::StringFormat("{} золота", gold));
            }

            if (promoCode->coin > 0)
                rewards.push_back(Acore::StringFormat("{} бонусов", promoCode->coin));

            if (promoCode->item_1 > 0 || promoCode->item_2 > 0 || promoCode->item_3 > 0)
                rewards.push_back("предметы");

            if (promoCode->honor > 0)
                rewards.push_back(Acore::StringFormat("{} очков чести", promoCode->honor));

            if (promoCode->arena > 0)
                rewards.push_back(Acore::StringFormat("{} очков арены", promoCode->arena));

            if (!rewards.empty())
            {
                std::string rewardsStr;
                for (size_t i = 0; i < rewards.size(); ++i)
                {
                    if (i > 0)
                    {
                        if (i == rewards.size() - 1)
                            rewardsStr += " и ";
                        else
                            rewardsStr += ", ";
                    }
                    rewardsStr += rewards[i];
                }
                successMessage = Acore::StringFormat("Промокод активирован! Получено: {}.", rewardsStr);
            }
        }

        std::string response = Acore::StringFormat("ASMSG_PROMOCODE_SUBMIT\t1:{}", successMessage);
        player->SendAddonMessage(response);

        LOG_INFO("shop", "HandlePromoCodeSubmitRequest: Promo code '{}' successfully activated for player {} (Account: {})",
            body, player->GetName(), player->GetSession()->GetAccountId());
    }
    else
    {
        uint32 codeId = 0;
        PromotionCodes const* promoCode = sPromotionCodeMgr->GetPromoCode(body, codeId);

        uint32 errorId = 1;

        if (!promoCode)
        {
            errorId = 1;
            LOG_WARN("shop", "HandlePromoCodeSubmitRequest: Promo code '{}' not found for player {} (Account: {})",
                body, player->GetName(), player->GetSession()->GetAccountId());
        }
        else if (promoCode->exist_count == 0)
        {
            errorId = 2;
            LOG_WARN("shop", "HandlePromoCodeSubmitRequest: Promo code '{}' has no remaining uses for player {} (Account: {})",
                body, player->GetName(), player->GetSession()->GetAccountId());
        }
        else
        {
            errorId = 3;
            LOG_WARN("shop", "HandlePromoCodeSubmitRequest: Promo code '{}' already used by player {} (Account: {})",
                body, player->GetName(), player->GetSession()->GetAccountId());
        }

        player->SendAddonMessage(fmt::format("ASMSG_PROMOCODE_SUBMIT\t{}", errorId));
    }
}

void AddonIO::HandleCustomGet(Player* player, std::string body)
{
    // Backward compatibility transport:
    // LuckyWheel uses opcodes 20/21 on AC_CU_GET, BattlePass uses its own opcode space.
    uint32 opcode = ParseOpcodeFromBody(body);

    if (opcode == 20 || opcode == 21)
    {
        HandleLuckyWheelGetState(player, body);
        return;
    }

    BattlePassService::HandleGet(player, body);
}

void AddonIO::HandleCustomPost(Player* player, std::string body)
{
    // Backward compatibility transport:
    // LuckyWheel uses opcode 22 on AC_CU_POST.
    uint32 opcode = ParseOpcodeFromBody(body);

    if (opcode == 22)
    {
        HandleLuckyWheelSpin(player, body);
        return;
    }

    BattlePassService::HandlePost(player, body);
}

void AddonIO::HandleBattlePassGetRequest(Player* player, std::string body)
{
    BattlePassService::HandleGet(player, body);
}

void AddonIO::HandleBattlePassPostRequest(Player* player, std::string body)
{
    BattlePassService::HandlePost(player, body);
}

void AddonIO::HandleOnlineRewardGetRequest(Player* player, std::string body)
{
    if (!player)
        return;

    uint32 opcode = ParseOpcodeFromBody(body);
    if (opcode == LUCKY_WHEEL_OPCODE_GET_STATE)
        HandleLuckyWheelStateRequest(player);
    else if (opcode == LUCKY_WHEEL_OPCODE_GET_REWARDS)
        HandleLuckyWheelRewardsRequest(player);
}

void AddonIO::HandleOnlineRewardPostRequest(Player* player, std::string body)
{
    if (!player)
        return;

    uint32 opcode = ParseOpcodeFromBody(body);
    if (opcode == LUCKY_WHEEL_OPCODE_SPIN)
        HandleLuckyWheelSpinRequest(player);
}

// ============================================================================
// LuckyWheel Handlers
// ============================================================================

void AddonIO::HandleLuckyWheelGetState(Player* player, std::string body)
{
    // Legacy bridge: redirect old transport to new online reward GET.
    HandleOnlineRewardGetRequest(player, body);
}

void AddonIO::HandleLuckyWheelSpin(Player* player, std::string body)
{
    // Legacy bridge: redirect old transport to new online reward POST.
    HandleOnlineRewardPostRequest(player, body);
}

void AddonIO::HandleLuckyWheelStateRequest(Player* player)
{
    if (!player)
        return;

    uint32 totalOnlineTime = player->GetSession()->GetTotalOnlineTime();
    uint32 lastRewardTime = player->GetSession()->GetLastRewardTime();
    // Check if player has enough online time
    bool isAvailable = (totalOnlineTime >= ONLINE_REWARD_REQUIRED_TIME);
    
    // Get total spins from database
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ONLINE_REWARDS);
    stmt->SetData(0, player->GetGUID().GetCounter());
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
    
    uint32 totalSpins = 0;
    if (result)
    {
        // We can get total_rewards_claimed from a separate query if needed
        // For now, we'll use 0
    }

    // Format: available,accumulatedTime,requiredTime,totalSpins,lastSpinTime|rewardCount
    // rewardCount will be sent separately in rewards response
    std::string response = Acore::StringFormat("ASMSG_LUCKY_WHEEL_STATE\t{},{},{},{},{}|0",
        isAvailable ? 1 : 0,
        totalOnlineTime,
        ONLINE_REWARD_REQUIRED_TIME,
        totalSpins,
        lastRewardTime);

    player->SendAddonMessage(response);
}

void AddonIO::HandleLuckyWheelRewardsRequest(Player* player)
{
    if (!player)
        return;

    // Загружаем награды из базы данных
    std::vector<LuckyWheelRewardData> rewards = sWorld->GetLuckyWheelRewards();

    // Если наград нет в базе, используем дефолтные (fallback)
    if (rewards.empty())
    {
        LOG_WARN("shop", "HandleLuckyWheelRewardsRequest: No rewards in database, using default rewards");
        // Можно оставить пустым или добавить дефолтные награды здесь
    }

    // Format: id,type,value,count,chance,name,icon,color|id,type,...
    std::ostringstream response;
    response << "ASMSG_LUCKY_WHEEL_REWARDS\t";
    
    for (size_t i = 0; i < rewards.size(); ++i)
    {
        const LuckyWheelRewardData& r = rewards[i];
        response << r.id << "," << r.rewardType << "," << r.rewardValue << "," << r.rewardCount << ","
                 << r.chance << "," << r.name << "," << r.icon << "," << r.color;
        
        if (i < rewards.size() - 1)
            response << "|";
    }

    player->SendAddonMessage(response.str());
}

void AddonIO::HandleLuckyWheelSpinRequest(Player* player)
{
    if (!player)
        return;

    // Check if player has enough online time
    uint32 totalOnlineTime = player->GetSession()->GetTotalOnlineTime();
    
    if (totalOnlineTime < ONLINE_REWARD_REQUIRED_TIME)
    {
        // Send failure response
        player->SendAddonMessage("ASMSG_LUCKY_WHEEL_SPIN_RESULT\t0|0|0|0|0|Недостаточно времени онлайн|INV_Misc_QuestionMark|1");
        return;
    }

    // Загружаем награды из базы данных
    std::vector<LuckyWheelRewardData> rewards = sWorld->GetLuckyWheelRewards();

    if (rewards.empty())
    {
        LOG_WARN("shop", "HandleLuckyWheelSpinRequest: No rewards in database");
        player->SendAddonMessage("ASMSG_LUCKY_WHEEL_SPIN_RESULT\t0|0|0|0|0|Нет доступных наград|INV_Misc_QuestionMark|1");
        return;
    }

    // Calculate total chance
    float totalChance = 0.0f;
    for (const LuckyWheelRewardData& r : rewards)
        totalChance += r.chance;

    // Roll random reward based on chances
    float roll = frand(0.0f, totalChance);
    float currentChance = 0.0f;
    const LuckyWheelRewardData* selectedReward = nullptr;

    for (const LuckyWheelRewardData& r : rewards)
    {
        currentChance += r.chance;
        if (roll <= currentChance)
        {
            selectedReward = &r;
            break;
        }
    }

    if (!selectedReward)
        selectedReward = &rewards[0]; // Fallback to first reward

    // Give reward to player
    bool rewardGiven = false;
    if (selectedReward->rewardType == 0) // Gold
    {
        // Золото добавляется напрямую в инвентарь игрока
        player->ModifyMoney(selectedReward->rewardCount);
        rewardGiven = true;
    }
    else if (selectedReward->rewardType == 1) // Item
    {
        // Предметы: сначала пытаемся добавить в инвентарь, если места нет - отправляем на почту
        uint32 noSpaceForCount = 0;
        ItemPosCountVec dest;
        InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, selectedReward->rewardValue, selectedReward->rewardCount, &noSpaceForCount);
        
        uint32 itemsToGive = selectedReward->rewardCount;
        if (msg != EQUIP_ERR_OK)
            itemsToGive -= noSpaceForCount;

        // Добавляем предметы, которые помещаются в инвентарь
        if (itemsToGive > 0)
        {
            if (Item* item = player->StoreNewItem(dest, selectedReward->rewardValue, true))
            {
                player->SendNewItem(item, itemsToGive, false, true);
                rewardGiven = true;
            }
        }

        // Если часть предметов не поместилась, отправляем на почту
        if (noSpaceForCount > 0)
        {
            ShopSendItem(player, player, "Награда за онлайн время", selectedReward->rewardValue, noSpaceForCount);
            rewardGiven = true;
        }
    }
    else if (selectedReward->rewardType == 2) // Currency
    {
        // Валюта добавляется на аккаунт (бонусная валюта)
        if (selectedReward->rewardValue == 1) // Bonus currency
        {
            // Используем AddDonateBonusOrVote для добавления валюты (не SetAccountCurrency, который вычитает!)
            if (player->GetSession()->AddDonateBonusOrVote(selectedReward->rewardCount, 1, false))
            {
                rewardGiven = true;
            }
        }
    }

    if (rewardGiven)
    {
        // Reset online time and update last reward time
        uint32 currentTime = time(nullptr);
        player->GetSession()->ResetOnlineTime();
        player->GetSession()->SetLastRewardTime(currentTime);

        // Update database
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ONLINE_REWARDS_REWARD);
        stmt->SetData(0, currentTime);
        stmt->SetData(1, player->GetGUID().GetCounter());
        CharacterDatabase.Execute(stmt);

        // Send success response
        // Format: success|rewardId|rewardType|rewardValue|rewardCount|rewardName|rewardIcon|rewardColor
        std::string response = Acore::StringFormat("ASMSG_LUCKY_WHEEL_SPIN_RESULT\t1|{}|{}|{}|{}|{}|{}|{}",
            selectedReward->id,
            selectedReward->rewardType,
            selectedReward->rewardValue,
            selectedReward->rewardCount,
            selectedReward->name,
            selectedReward->icon,
            selectedReward->color);

        player->SendAddonMessage(response);
    }
    else
    {
        // Send failure response
        player->SendAddonMessage("ASMSG_LUCKY_WHEEL_SPIN_RESULT\t0|0|0|0|0|Ошибка выдачи награды|INV_Misc_QuestionMark|1");
    }
}