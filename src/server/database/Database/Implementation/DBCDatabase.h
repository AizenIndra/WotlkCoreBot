/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This file is free software; as a special exception the author gives
 * unlimited permission to copy and/or distribute it, with or without
 * modifications, as long as this notice is preserved.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY, to the extent permitted by law; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _DBC_DATABASE_H
#define _DBC_DATABASE_H

#include "MySQLConnection.h"

enum DBCDatabaseStatements : uint32
{
    /* Add prepared statements when code reads/writes DBC tables in acore_dbc. */

    MAX_DBC_DATABASE_STATEMENTS
};

class AC_DATABASE_API DBCDatabaseConnection : public MySQLConnection
{
public:
    typedef DBCDatabaseStatements Statements;

    DBCDatabaseConnection(MySQLConnectionInfo& connInfo);
    DBCDatabaseConnection(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo);
    ~DBCDatabaseConnection() override;

    void DoPrepareStatements() override;
};

#endif
