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

#include <string>

#include "plugin_manager/plugin.hpp"
#include "plugin_manager/plugin_api.hpp"
#include "plugin_manager/plugin_manager_core.hpp"

class PluginWithoutDestroy: public Plugin
{
public:
    PluginWithoutDestroy() { }
    ~PluginWithoutDestroy() { }
};

static Plugin *NewPluginWithoutDestroy(PluginManagerCore *core);
static void DeletePluginWithoutDestroy(Plugin *plugin);

extern "C" const plugin_api_t PLUGIN_API =
{
    .version = { 0, 0, 0},
    .create = NewPluginWithoutDestroy,
    .destroy = NULL,
};