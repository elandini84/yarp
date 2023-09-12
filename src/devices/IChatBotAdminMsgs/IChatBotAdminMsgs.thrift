/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct return_backupBot{
    1: bool result;
    2: string backupString;
}

service IChatBotAdminMsgs {
    return_backupBot backupBotRPC();
    bool restoreBotRPC(1: string botToRestore);
}
