/*
 * Password functions
 *
 * Copyright (C) 2011, Google Inc.
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined( _LIBBDE_PASSWORD_H )
#define _LIBBDE_PASSWORD_H

#include <common.h>
#include <types.h>

#include <liberror.h>

#include "libbde_libhmac.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libbde_password_key_data libbde_password_key_data_t;

struct libbde_password_key_data
{
	/* The salt
	 */
	uint8_t salt[ 16 ];

	/* The SHA256 hash
	 * This value contains the SHA256 hash calculated in the previous key iteration
	 */
	uint8_t sha256_hash[ LIBHMAC_SHA256_HASH_SIZE ];
};

int libbde_utf8_password_calculate_hash(
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     uint8_t password_hash[ 32 ],
     liberror_error_t **error );

int libbde_utf16_password_calculate_hash(
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     uint8_t password_hash[ 32 ],
     liberror_error_t **error );

int libbde_password_calculate_key(
     const uint8_t password_hash[ 32 ],
     const uint8_t salt[ 16 ],
     uint8_t key[ 32 ],
     liberror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif
