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

#ifndef PUNICA_REST_REQUEST_H
#define PUNICA_REST_REQUEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct CRequest;
typedef struct CRequest CRequest;

void delete_Request(CRequest *c_request);
char *Request_getPath(CRequest *c_request);
char *Request_getMethod(CRequest *c_request);
char *Request_getHeader(CRequest *c_request, const char *c_header);
uint8_t *Request_getBody(CRequest *c_request);

#ifdef __cplusplus
}
#endif

#endif // PUNICA_REST_REQUEST_H