#include "AccountMgr.h"
#include "Chat.h"
#include "CommandScript.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "Util.h"

using namespace Acore::ChatCommands;

class autoprem_commandscript : public CommandScript
{
public:
    autoprem_commandscript() : CommandScript("autoprem_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable autoPremCommandTable =
        {
            { "status", HandleAutopremStatusCommand, SEC_PLAYER, Console::No }
        };

        static ChatCommandTable commandTable =
        {
            { "autoprem", autoPremCommandTable }
        };

        return commandTable;
    }

    static bool HandleAutopremStatusCommand(ChatHandler* handler, Optional<uint32> accountIdArg)
    {
        if (!handler || !handler->GetSession())
            return false;

        uint32 accountId = handler->GetSession()->GetAccountId();
        if (accountIdArg && handler->GetSession()->GetSecurity() >= SEC_GAMEMASTER)
            accountId = *accountIdArg;

        QueryResult result = LoginDatabase.Query(
            "SELECT account_name, character_guid, character_name, granted_at, premium_unsetdate "
            "FROM account_first_character_premium WHERE account_id = {} LIMIT 1", accountId);

        if (!result)
        {
            handler->PSendSysMessage("Auto premium not granted for account {}.", accountId);
            return true;
        }

        Field* fields = result->Fetch();
        std::string accountName = fields[0].Get<std::string>();
        uint32 characterGuid = fields[1].Get<uint32>();
        std::string characterName = fields[2].Get<std::string>();
        uint32 grantedAt = fields[3].Get<uint32>();
        uint32 unsetDate = fields[4].Get<uint32>();
        time_t now = GameTime::GetGameTime().count();
        uint32 remaining = unsetDate > now ? unsetDate - static_cast<uint32>(now) : 0;

        handler->PSendSysMessage(
            "Auto premium account {} ({}): character {} (guid {}).",
            accountId, accountName, characterName, characterGuid);
        handler->PSendSysMessage(
            "GrantedAt: {}, PremiumUnset: {}, Remaining: {}",
            grantedAt, unsetDate, secsToTimeString(remaining, true));

        return true;
    }
};

void AddSC_autoprem_commandscript()
{
    new autoprem_commandscript();
}
