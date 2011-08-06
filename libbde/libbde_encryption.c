/*
 * Encryption functions
 *
 * Copyright (C) 2011, Joachim Metz <jbmetz@users.sourceforge.net>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <memory.h>
#include <types.h>

#include <liberror.h>
#include <libnotify.h>

#include "libbde_aes.h"
#include "libbde_definitions.h"
#include "libbde_diffuser.h"
#include "libbde_encryption.h"

/* Initialize an encryption context
 * Make sure the value encryption context is pointing to is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libbde_encryption_initialize(
     libbde_encryption_context_t **context,
     uint32_t method,
     liberror_error_t **error )
{
	static char *function = "libbde_encryption_initialize";

	if( context == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}
	if( *context == NULL )
	{
		*context = memory_allocate_structure(
		            libbde_encryption_context_t );

		if( *context == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create context.",
			 function );

			goto on_error;
		}
		if( memory_set(
		     *context,
		     0,
		     sizeof( libbde_encryption_context_t ) ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear context.",
			 function );

			memory_free(
			 *context );

			*context = NULL;

			return( -1 );
		}
		if( libbde_aes_initialize(
		     &( ( *context )->fvek_decryption_context ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable initialize FVEK decryption context.",
			 function );

			goto on_error;
		}
		if( libbde_aes_initialize(
		     &( ( *context )->fvek_encryption_context ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable initialize FVEK encryption context.",
			 function );

			goto on_error;
		}
		if( libbde_aes_initialize(
		     &( ( *context )->tweak_decryption_context ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable initialize TWEAK decryption context.",
			 function );

			goto on_error;
		}
		if( libbde_aes_initialize(
		     &( ( *context )->tweak_encryption_context ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable initialize TWEAK encryption context.",
			 function );

			goto on_error;
		}
		( *context )->method = method;
	}
	return( 1 );

on_error:
	if( *context != NULL )
	{
		if( ( *context )->tweak_decryption_context != NULL )
		{
			libbde_aes_free(
			 &( ( *context )->tweak_decryption_context ),
			 NULL );
		}
		if( ( *context )->fvek_encryption_context != NULL )
		{
			libbde_aes_free(
			 &( ( *context )->fvek_encryption_context ),
			 NULL );
		}
		if( ( *context )->fvek_decryption_context != NULL )
		{
			libbde_aes_free(
			 &( ( *context )->fvek_decryption_context ),
			 NULL );
		}
		memory_free(
		 *context );

		*context = NULL;
	}
	return( -1 );
}

/* Frees an encryption context
 * Returns 1 if successful or -1 on error
 */
int libbde_encryption_free(
     libbde_encryption_context_t **context,
     liberror_error_t **error )
{
	static char *function = "libbde_encryption_free";
	int result            = 1;

	if( context == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}
	if( *context != NULL )
	{
		if( libbde_aes_free(
		     &( ( *context )->fvek_decryption_context ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable free FVEK decryption context.",
			 function );

			result = -1;
		}
		if( libbde_aes_free(
		     &( ( *context )->fvek_encryption_context ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable free FVEK encryption context.",
			 function );

			result = -1;
		}
		if( libbde_aes_free(
		     &( ( *context )->tweak_decryption_context ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable free TWEAK decryption context.",
			 function );

			result = -1;
		}
		if( libbde_aes_free(
		     &( ( *context )->tweak_encryption_context ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable free TWEAK encryption context.",
			 function );

			result = -1;
		}
		memory_free(
		 *context );

		*context = NULL;
	}
	return( result );
}

/* Sets the de- and encryption keys
 * Returns 1 if successful or -1 on error
 */
int libbde_encryption_set_keys(
     libbde_encryption_context_t *context,
     uint8_t full_volume_encryption_key[ 32 ],
     uint8_t tweak_key[ 32 ],
     liberror_error_t **error )
{
	static char *function = "libbde_encryption_set_keys";
	size_t key_bit_size   = 0;

	if( context == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}
	if( ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_128_CBC )
	 || ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_128_CBC_DIFFUSER ) )
	{
		key_bit_size = 128;
	}
	else if( ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_256_CBC )
	      || ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_256_CBC_DIFFUSER ) )
	{
		key_bit_size = 256;
	}
	/* The volume master key is always 256-bit in size
	 */
	if( libbde_aes_set_decryption_key(
	     context->fvek_decryption_context,
	     full_volume_encryption_key,
	     key_bit_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set full volume encryption key in decryption context.",
		 function );

		return( -1 );
	}
	if( libbde_aes_set_encryption_key(
	     context->fvek_encryption_context,
	     full_volume_encryption_key,
	     key_bit_size,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set full volume encryption key in encryption context.",
		 function );

		return( -1 );
	}
	/* The TWEAK key is only used with diffuser
	 */
	if( ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_128_CBC_DIFFUSER )
	 || ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_256_CBC_DIFFUSER ) )
	{
		if( libbde_aes_set_decryption_key(
		     context->tweak_decryption_context,
		     tweak_key,
		     key_bit_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set tweak key in decryption context.",
			 function );

			return( -1 );
		}
		if( libbde_aes_set_encryption_key(
		     context->tweak_encryption_context,
		     tweak_key,
		     key_bit_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set tweak key in encryption context.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* De- or encrypts a block of data
 * Returns 1 if successful or -1 on error
 */
int libbde_encryption_crypt(
     libbde_encryption_context_t *context,
     int mode,
     const uint8_t *input_data,
     size_t input_data_size,
     uint8_t *output_data,
     size_t output_data_size,
     uint64_t block_key,
     liberror_error_t **error )
{
	uint8_t block_key_data[ 16 ];
	uint8_t initialization_vector[ 16 ];
	uint8_t xor_data[ 32 ];

	static char *function = "libbde_encryption_crypt";
	size_t data_index     = 0;

	if( context == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}
	if( ( mode != LIBBDE_ENCYPTION_CRYPT_MODE_DECRYPT )
	 && ( mode != LIBBDE_ENCYPTION_CRYPT_MODE_ENCRYPT ) )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported mode.",
		 function );

		return( -1 );
	}
	if( memory_set(
	     initialization_vector,
	     0,
	     16 ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear initialization vector.",
		 function );

		return( -1 );
	}
	if( ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_128_CBC_DIFFUSER )
	 || ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_256_CBC_DIFFUSER ) )
	{
		if( memory_set(
		     block_key_data,
		     0,
		     16 ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear block key data.",
			 function );

			return( -1 );
		}
		if( memory_set(
		     xor_data,
		     0,
		     32 ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear XOR data.",
			 function );

			return( -1 );
		}
		byte_stream_copy_to_uint64_little_endian(
		 block_key_data,
		 block_key );

		/* The block key for the initialization vector is encrypted
		 * with the FVEK
		 */
		if( libbde_aes_ecb_crypt(
		     context->fvek_encryption_context,
		     LIBBDE_AES_CRYPT_MODE_ENCRYPT,
		     block_key_data,
		     16,
		     initialization_vector,
		     16,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBERROR_ENCRYPTION_ERROR_GENERIC,
			 "%s: unable to encrypt initialization vector.",
			 function );

			return( -1 );
		}
		/* The block key for the XOR data is encrypted
		 * with the TWEAK key
		 */
		if( libbde_aes_ecb_crypt(
		     context->tweak_encryption_context,
		     LIBBDE_AES_CRYPT_MODE_ENCRYPT,
		     block_key_data,
		     16,
		     xor_data,
		     16,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBERROR_ENCRYPTION_ERROR_GENERIC,
			 "%s: unable to encrypt XOR data.",
			 function );

			return( -1 );
		}
		/* Set the last byte to contain 0x80 (128)
		 */
		block_key_data[ 15 ] = 0x80;

		if( libbde_aes_ecb_crypt(
		     context->tweak_encryption_context,
		     LIBBDE_AES_CRYPT_MODE_ENCRYPT,
		     block_key_data,
		     16,
		     &( xor_data[ 16 ] ),
		     16,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBERROR_ENCRYPTION_ERROR_GENERIC,
			 "%s: unable to encrypt XOR data.",
			 function );

			return( -1 );
		}
	}
	if( mode == LIBBDE_ENCYPTION_CRYPT_MODE_ENCRYPT )
	{
/* TODO safe guard input data ? */
		if( ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_128_CBC_DIFFUSER )
		 || ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_256_CBC_DIFFUSER ) )
		{
			for( data_index = 0;
			     data_index < input_data_size;
			     data_index++ )
			{
				/* value & 0x1f = value % 32
				 */
				output_data[ data_index ] ^= xor_data[ data_index & 0x1f ];
			}
/* TODO create diffuser encryption functons
			if( libbde_diffuser_a(
			     output_data,
			     output_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ENCRYPTION,
				 LIBERROR_ENCRYPTION_ERROR_GENERIC,
				 "%s: unable to Diffuser-A output data.",
				 function );

				return( -1 );
			}
			if( libbde_diffuser_b(
			     output_data,
			     output_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ENCRYPTION,
				 LIBERROR_ENCRYPTION_ERROR_GENERIC,
				 "%s: unable to Diffuser-B output data.",
				 function );

				return( -1 );
			}
*/
		}
		if( libbde_aes_cbc_crypt(
		     context->fvek_encryption_context,
		     LIBBDE_AES_CRYPT_MODE_ENCRYPT,
		     initialization_vector,
		     input_data,
		     input_data_size,
		     output_data,
		     output_data_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBERROR_ENCRYPTION_ERROR_GENERIC,
			 "%s: unable to AES-CBC encrypt output data.",
			 function );

			return( -1 );
		}
	}
	else
	{
		if( libbde_aes_cbc_crypt(
		     context->fvek_decryption_context,
		     LIBBDE_AES_CRYPT_MODE_DECRYPT,
		     initialization_vector,
		     input_data,
		     input_data_size,
		     output_data,
		     output_data_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBERROR_ENCRYPTION_ERROR_GENERIC,
			 "%s: unable to AES-CBC decrypt output data.",
			 function );

			return( -1 );
		}
		if( ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_128_CBC_DIFFUSER )
		 || ( context->method == LIBBDE_ENCRYPTION_METHOD_AES_256_CBC_DIFFUSER ) )
		{
			libnotify_print_data(
			 output_data,
			 output_data_size );

			if( libbde_diffuser_b(
			     output_data,
			     output_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ENCRYPTION,
				 LIBERROR_ENCRYPTION_ERROR_GENERIC,
				 "%s: unable to Diffuser-B output data.",
				 function );

				return( -1 );
			}
			libnotify_print_data(
			 output_data,
			 output_data_size );

			Diffuser_A_Decrypt(
			 output_data,
			 output_data_size );

/* TODO
			if( libbde_diffuser_a(
			     output_data,
			     output_data_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_ENCRYPTION,
				 LIBERROR_ENCRYPTION_ERROR_GENERIC,
				 "%s: unable to Diffuser-A output data.",
				 function );

				return( -1 );
			}
*/
			libnotify_print_data(
			 output_data,
			 output_data_size );

			for( data_index = 0;
			     data_index < input_data_size;
			     data_index++ )
			{
				/* value & 0x1f = value % 32
				 */
				output_data[ data_index ] ^= xor_data[ data_index & 0x1f ];
			}
		}
	}
	return( 1 );
}
