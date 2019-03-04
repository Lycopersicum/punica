/*
 * Punica - LwM2M server with REST API
 * Copyright (C) 2019 8devices
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef BASIC_PLUGIN_MANAGER_H
#define BASIC_PLUGIN_MANAGER_H

#include "basic_plugin_manager_core.h"

enum
{
    J_MAX_LENGTH_PLUGIN_NAME = 1024,
    J_MAX_LENGTH_PLUGIN_PATH = 1024,
};

struct CBasicPluginManager;
typedef struct CBasicPluginManager CBasicPluginManager;

CBasicPluginManager *new_BasicPluginManager(CBasicPluginManagerCore *c_manager_core);
void delete_BasicPluginManager(CBasicPluginManager *c_manager);
int BasicPluginManager_loadPlugin(CBasicPluginManager *c_manager,
                                  const char *c_path,
                                  const char *c_name);
int BasicPluginManager_unloadPlugin(CBasicPluginManager *c_manager,
                                    const char *c_name);

#endif // BASIC_PLUGIN_MANAGER_H