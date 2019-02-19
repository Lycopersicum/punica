/*
 * Punica - LwM2M server with REST API
 * Copyright (C) 2018 8devices
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

#include "devices_database.h"

#include <string.h>

#include "linked_list.h"
#include "logging.h"
#include "punica_core.h"
#include "rest_core_types.h"
#include "settings.h"
#include "utils.h"

static char *logging_section = "[DEVICES DATABASE]";

static void devices_database_entry_free_data(database_entry_t *device)
{
    if (device != NULL)
    {
        if (device->uuid)
        {
            free(device->uuid);
        }
        if (device->psk)
        {
            free(device->psk);
        }
        if (device->psk_id)
        {
            free(device->psk_id);
        }
    }
}

void devices_database_entry_free(database_entry_t *device)
{
    devices_database_entry_free_data(device);

    if (device != NULL)
    {
        free(device);
    }
}

database_entry_t *devices_database_get_by_uuid(linked_list_t *devices,
                                               const char *uuid)
{
    database_entry_t *device = NULL;
    linked_list_entry_t *list_entry;

    for (list_entry = devices->head;
         list_entry != NULL; list_entry = list_entry->next)
    {
        device = (database_entry_t *)list_entry->data;

        if (strcasecmp(uuid, device->uuid) == 0)
        {
            return device;
            device = (database_entry_t *)list_entry->data;
        }
    }

    return NULL;
}

int devices_database_delete_by_uuid(linked_list_t *devices, const char *uuid)
{
    database_entry_t *device = devices_database_get_by_uuid(devices, uuid);

    if (device == NULL)
    {
        return -1;
    }

    devices_database_entry_free_data(device);
    linked_list_remove(devices, (void *) device);

    return 0;
}

int devices_database_new_entry_validate(json_t *j_new_device)
{
    json_t *j_value;
    const char *key;
    uint8_t buffer[512];
    size_t buffer_len = sizeof(buffer);
    int key_check = 0;

    if (!json_is_object(j_new_device))
    {
        return -1;
    }

    json_object_foreach(j_new_device, key, j_value)
    {
        if (!json_is_string(j_value))
        {
            return -1;
        }

        if (strcasecmp(key, "psk") == 0)
        {
            if (base64_decode(json_string_value(j_value),
                              buffer, &buffer_len) != 0)
            {
                return -1;
            }

            key_check |= DATABASE_PSK_KEY_BIT;
        }
        else if (strcasecmp(key, "psk_id") == 0)
        {
            if (base64_decode(json_string_value(j_value),
                              buffer, &buffer_len) != 0)
            {
                return -1;
            }

            key_check |= DATABASE_PSK_ID_KEY_BIT;
        }
    }

    if (key_check != DATABASE_ALL_NEW_KEYS_SET)
    {
        return -1;
    }

    return 0;
}

int devices_database_entry_validate(json_t *j_device)
{
    json_t *j_value;
    const char *key;
    uint8_t buffer[512];
    size_t buffer_len = sizeof(buffer);
    int key_check = 0;

    if (!json_is_object(j_device))
    {
        return -1;
    }

    json_object_foreach(j_device, key, j_value)
    {
        if (!json_is_string(j_value))
        {
            return -1;
        }
        if (strcasecmp(key, "uuid") == 0)
        {
            key_check |= DATABASE_UUID_KEY_BIT;
        }
        else if (strcasecmp(key, "psk") == 0)
        {
            if (base64_decode(json_string_value(j_value),
                              buffer, &buffer_len) != 0)
            {
                return -1;
            }
            key_check |= DATABASE_PSK_KEY_BIT;
        }
        else if (strcasecmp(key, "psk_id") == 0)
        {
            if (base64_decode(json_string_value(j_value),
                              buffer, &buffer_len) != 0)
            {
                return -1;
            }
            key_check |= DATABASE_PSK_ID_KEY_BIT;
        }
    }

    /* function does not check for duplicate keys */
    if (key_check != DATABASE_ALL_KEYS_SET)
    {
        return -1;
    }

    return 0;
}

int devices_database_entry_from_json(json_t *j_device,
                                     database_entry_t *device)
{
    json_t *j_value;
    const char *json_string;

    if (j_device == NULL
        || device == NULL)
    {
        return -1;
    }

    j_value = json_object_get(j_device, "uuid");
    json_string = json_string_value(j_value);

    device->uuid = strdup(json_string);
    if (device->uuid == NULL)
    {
        return -1;
    }


    j_value = json_object_get(j_device, "psk");
    json_string = json_string_value(j_value);

    base64_decode(json_string, NULL, &device->psk_len);

    device->psk = (uint8_t *)malloc(device->psk_len);
    if (device->psk == NULL)
    {
        return -1;
    }
    base64_decode(json_string, device->psk, &device->psk_len);


    j_value = json_object_get(j_device, "psk_id");
    json_string = json_string_value(j_value);

    base64_decode(json_string, NULL, &device->psk_id_len);

    device->psk_id = (uint8_t *)malloc(device->psk_id_len);
    if (device->psk_id == NULL)
    {
        return -1;
    }
    base64_decode(json_string,
                  device->psk_id, &device->psk_id_len);

    return 0;
}

int devices_database_entry_new_from_json(json_t *j_new_device,
                                         database_entry_t *new_device)
{
    json_t *j_device;
    char uuid[37];
    int return_code;

    if (j_new_device == NULL
        || new_device == NULL)
    {
        return -1;
    }

    if (utils_generate_uuid(uuid) != 0
        || json_object_set_new(j_new_device, "uuid",
                               json_stringn(uuid, sizeof(uuid))) != 0)
    {
        return -1;
    }

    j_device = json_deep_copy(j_new_device);
    return_code = devices_database_entry_from_json(j_device, new_device);
    json_decref(j_device);

    return return_code;
}

json_t *devices_database_entry_get_public_json(database_entry_t *device)
{
    char psk_string[MAX_LENGTH_PSK], psk_id_string[MAX_LENGTH_PSK_ID];
    size_t psk_string_length = sizeof(psk_string),
           psk_id_string_length = sizeof(psk_id_string);

    if (device == NULL)
    {
        return NULL;
    }

    base64_encode(device->psk, device->psk_len,
                  psk_string, &psk_string_length);
    base64_encode(device->psk_id, device->psk_id_len,
                  psk_id_string, &psk_id_string_length);

    return json_pack("{s:s, s:s}",
                     "psk_id", psk_id_string,
                     "uuid", device->uuid);
}

json_t *devices_database_entry_get_json(database_entry_t *device)
{
    char psk_string[MAX_LENGTH_PSK], psk_id_string[MAX_LENGTH_PSK_ID];
    size_t psk_string_length = sizeof(psk_string),
           psk_id_string_length = sizeof(psk_id_string);

    if (device == NULL)
    {
        return NULL;
    }

    base64_encode(device->psk, device->psk_len,
                  psk_string, &psk_string_length);
    base64_encode(device->psk_id, device->psk_id_len,
                  psk_id_string, &psk_id_string_length);

    return json_pack("{s:s, s:s, s:s}",
                     "psk", psk_string,
                     "psk_id", psk_id_string,
                     "uuid", device->uuid);
}

int devices_database_to_public_json(linked_list_t *devices, json_t *j_devices)
{
    linked_list_entry_t *list_entry;
    database_entry_t *device;
    json_t *j_device = NULL;

    if (devices == NULL
        || !json_is_array(j_devices))
    {
        return -1;
    }

    for (list_entry = devices->head;
         list_entry != NULL; list_entry = list_entry->next)
    {
        device = (database_entry_t *)list_entry->data;
        j_device = devices_database_entry_get_public_json(device);

        if (j_device == NULL)
        {
            continue;
        }

        if (json_array_append_new(j_devices, j_device))
        {
            return -1;
        }
    }

    return 0;
}

int devices_database_to_json(linked_list_t *devices, json_t *j_devices)
{
    linked_list_entry_t *list_entry;
    database_entry_t *device;
    json_t *j_device = NULL;

    if (devices == NULL
        || !json_is_array(j_devices))
    {
        return -1;
    }

    for (list_entry = devices->head;
         list_entry != NULL; list_entry = list_entry->next)
    {
        device = (database_entry_t *) list_entry->data;
        j_device = devices_database_entry_get_json(device);

        if (j_device == NULL)
        {
            continue;
        }

        if (json_array_append_new(j_devices, j_device))
        {
            return -1;
        }
    }

    return 0;
}

int devices_database_from_file(punica_context_t *punica)
{
    linked_list_t *devices;
    database_entry_t *device;
    json_error_t error;
    json_t *j_device, *j_devices = NULL;
    size_t index;

    devices = linked_list_new();
    if (devices == NULL)
    {
        log_message(LOG_LEVEL_ERROR,
                    "%s Failed to allocate device database at %s:%s:%d\n",
                    logging_section, __FILE__, __func__, __LINE__);
        return 0;
    }

    punica->rest_devices = devices;
    if (punica->settings->coap.database_file == NULL)
    {
        /* internal list created, nothing more to do here */
        return 0;
    }

    j_devices = json_load_file(punica->settings->coap.database_file, 0, &error);
    if (j_devices == NULL)
    {
        log_message(LOG_LEVEL_INFO,
                    "%s Failed to load JSON from devices database file.\n",
                    logging_section);
        log_message(LOG_LEVEL_DEBUG, "\t%s:%d:%d error:%s\n",
                    punica->settings->coap.database_file, error.line, error.column,
                    error.text);

        return -1;
    }

    if (!json_is_array(j_devices))
    {
        log_message(LOG_LEVEL_INFO,
                    "%s Database file must contain a JSON array.\n",
                    logging_section);
        linked_list_delete(devices);

        json_decref(j_devices);
        return -1;
    }

    if (json_array_size(j_devices) == 0)
    {
        /* empty array, must be populated with /devices REST API */
        log_message(LOG_LEVEL_INFO,
                    "%s Created empty devices database file.\n",
                    logging_section);
        json_decref(j_devices);
        return 0;
    }

    json_array_foreach(j_devices, index, j_device)
    {
        if (devices_database_entry_validate(j_device))
        {
            log_message(LOG_LEVEL_INFO,
                        "%s Found error(s) in devices database entry no. %ld\n",
                        logging_section, index);
            continue;
        }

        device = malloc(sizeof(database_entry_t));
        if (device == NULL)
        {
            log_message(LOG_LEVEL_ERROR,
                        "%s Failed to allocate device entry memory at %s.\n",
                        logging_section, __func__);
            json_decref(j_devices);
            return -1;
        }

        if (devices_database_entry_from_json(j_device, device) == 0)
        {
            linked_list_add(devices, (void *) device);
        }
        else
        {
            log_message(LOG_LEVEL_INFO,
                        "%s Failed to parse entry to JSON.\n",
                        logging_section);
            devices_database_entry_free(device);
        }
    }

    json_decref(j_devices);
    return 0;
}

int devices_database_to_file(linked_list_t *devices, const char *file_name)
{
    json_t *j_devices = json_array();

    if (file_name == NULL)
    {
        return 0;
    }

    if (!json_is_array(j_devices))
    {
        return -1;
    }

    if (devices_database_to_json(devices, j_devices) != 0)
    {
        json_decref(j_devices);
        return -1;
    }

    if (json_dump_file(j_devices, file_name, 0) != 0)
    {
        json_decref(j_devices);
        return -1;
    }

    json_decref(j_devices);
    return 0;
}