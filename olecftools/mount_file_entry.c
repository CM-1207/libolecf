/*
 * Mount file entry
 *
 * Copyright (C) 2008-2018, Joachim Metz <joachim.metz@gmail.com>
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
#include <narrow_string.h>
#include <system_string.h>
#include <types.h>
#include <wide_string.h>

#if defined( HAVE_SYS_STAT_H )
#include <sys/stat.h>
#endif

#include "mount_file_entry.h"
#include "mount_file_system.h"
#include "olecftools_libcerror.h"
#include "olecftools_libolecf.h"

#if !defined( S_IFDIR )
#define S_IFDIR 0x4000
#endif

#if !defined( S_IFREG )
#define S_IFREG 0x8000
#endif

/* Creates a file entry
 * Make sure the value file_entry is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_initialize(
     mount_file_entry_t **file_entry,
     mount_file_system_t *file_system,
     const system_character_t *name,
     libolecf_item_t *item,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_initialize";
	size_t name_length    = 0;

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( *file_entry != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid file entry value already set.",
		 function );

		return( -1 );
	}
	if( file_system == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file system.",
		 function );

		return( -1 );
	}
	*file_entry = memory_allocate_structure(
	               mount_file_entry_t );

	if( *file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create file entry.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *file_entry,
	     0,
	     sizeof( mount_file_entry_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear file entry.",
		 function );

		memory_free(
		 *file_entry );

		*file_entry = NULL;

		return( -1 );
	}
	( *file_entry )->file_system = file_system;

	if( name != NULL )
	{
		name_length = system_string_length(
		               name );

		( *file_entry )->name = system_string_allocate(
		                         name_length + 1 );

		if( ( *file_entry )->name == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create name string.",
			 function );

			goto on_error;
		}
		if( system_string_copy(
		     ( *file_entry )->name,
		     name,
		     name_length ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy name.",
			 function );

			goto on_error;
		}
		( *file_entry )->name[ name_length ] = 0;

		( *file_entry )->name_size = name_length + 1;
	}
	( *file_entry )->item = item;

	return( 1 );

on_error:
	if( *file_entry != NULL )
	{
		if( ( *file_entry )->name != NULL )
		{
			memory_free(
			 ( *file_entry )->name );
		}
		memory_free(
		 *file_entry );

		*file_entry = NULL;
	}
	return( -1 );
}

/* Frees a file entry
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_free(
     mount_file_entry_t **file_entry,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_free";
	int result            = 1;

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( *file_entry != NULL )
	{
		if( ( *file_entry )->name != NULL )
		{
			memory_free(
			 ( *file_entry )->name );
		}
		if( libolecf_item_free(
		     &( ( *file_entry )->item ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free item.",
			 function );

			result = -1;
		}
		memory_free(
		 *file_entry );

		*file_entry = NULL;
	}
	return( result );
}

/* Retrieves the parent file entry
 * Returns 1 if successful, 0 if no such file entry or -1 on error
 */
int mount_file_entry_get_parent_file_entry(
     mount_file_entry_t *file_entry,
     mount_file_entry_t **parent_file_entry,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_get_parent_file_entry";

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( parent_file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid parent file entry.",
		 function );

		return( -1 );
	}
	if( *parent_file_entry != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid parent file entry value already set.",
		 function );

		return( -1 );
	}
/* TODO implement libolecf_item_get_parent function */
	return( 1 );
}

/* Retrieves the creation date and time
 * On Windows the timestamp is an unsigned 64-bit FILETIME timestamp
 * otherwise the timestamp is a signed 64-bit POSIX date and time value in number of nanoseconds
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_get_creation_time(
     mount_file_entry_t *file_entry,
     uint64_t *creation_time,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_get_creation_time";
	uint64_t filetime     = 0;

#if !defined( WINAPI )
	int64_t posix_time    = 0;
#endif

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( creation_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid creation time.",
		 function );

		return( -1 );
	}
	if( libolecf_item_get_creation_time(
	     file_entry->item,
	     &filetime,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve creation time from item.",
		 function );

		return( -1 );
	}
#if defined( WINAPI )
	*creation_time = filetime;
#else
	if( filetime != 0 )
	{
		/* Convert the FILETIME timestamp into a POSIX nanoseconds timestamp
		 */
		posix_time = ( (int64_t) filetime - 116444736000000000L ) * 100;
	}
	*creation_time = (uint64_t) posix_time;
#endif
	return( 1 );
}

/* Retrieves the access date and time
 * On Windows the timestamp is an unsigned 64-bit FILETIME timestamp
 * otherwise the timestamp is a signed 64-bit POSIX date and time value in number of nanoseconds
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_get_access_time(
     mount_file_entry_t *file_entry,
     uint64_t *access_time,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_get_access_time";

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( mount_file_system_get_mounted_timestamp(
	     file_entry->file_system,
	     access_time,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve mounted timestamp.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the modification date and time
 * On Windows the timestamp is an unsigned 64-bit FILETIME timestamp
 * otherwise the timestamp is a signed 64-bit POSIX date and time value in number of nanoseconds
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_get_modification_time(
     mount_file_entry_t *file_entry,
     uint64_t *modification_time,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_get_modification_time";
	uint64_t filetime     = 0;

#if !defined( WINAPI )
	int64_t posix_time    = 0;
#endif

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( modification_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid modification time.",
		 function );

		return( -1 );
	}
	if( libolecf_item_get_modification_time(
	     file_entry->item,
	     &filetime,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve modification time from item.",
		 function );

		return( -1 );
	}
#if defined( WINAPI )
	*modification_time = filetime;
#else
	if( filetime != 0 )
	{
		/* Convert the FILETIME timestamp into a POSIX nanoseconds timestamp
		 */
		posix_time = ( (int64_t) filetime - 116444736000000000L ) * 100;
	}
	*modification_time = (uint64_t) posix_time;
#endif
	return( 1 );
}

/* Retrieves the inode change date and time
 * On Windows the timestamp is an unsigned 64-bit FILETIME timestamp
 * otherwise the timestamp is a signed 64-bit POSIX date and time value in number of nanoseconds
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_get_inode_change_time(
     mount_file_entry_t *file_entry,
     uint64_t *inode_change_time,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_get_inode_change_time";

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( mount_file_system_get_mounted_timestamp(
	     file_entry->file_system,
	     inode_change_time,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve mounted timestamp.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the file mode
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_get_file_mode(
     mount_file_entry_t *file_entry,
     uint16_t *file_mode,
     libcerror_error_t **error )
{
	static char *function   = "mount_file_entry_get_file_mode";
	int number_of_sub_items = 0;

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( file_mode == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file mode.",
		 function );

		return( -1 );
	}
	if( libolecf_item_get_number_of_sub_items(
	     file_entry->item,
	     &number_of_sub_items,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of sub items.",
		 function );

		return( -1 );
	}
	if( number_of_sub_items > 0 )
	{
		*file_mode = S_IFDIR | 0555;
	}
	else
	{
		*file_mode = S_IFREG | 0444;
	}
	return( 1 );
}

/* Retrieves the size of the name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_get_name_size(
     mount_file_entry_t *file_entry,
     size_t *string_size,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_get_name_size";

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( string_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid string size.",
		 function );

		return( -1 );
	}
	*string_size = file_entry->name_size;

	return( 1 );
}

/* Retrieves the name
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_get_name(
     mount_file_entry_t *file_entry,
     system_character_t *string,
     size_t string_size,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_get_name";

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( file_entry->name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid file entry - missing name.",
		 function );

		return( -1 );
	}
	if( string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid string.",
		 function );

		return( -1 );
	}
	if( string_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( string_size < file_entry->name_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: invalid string size value too small.",
		 function );

		return( -1 );
	}
	if( system_string_copy(
	     string,
	     file_entry->name,
	     file_entry->name_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy name.",
		 function );

		return( -1 );
	}
	string[ file_entry->name_size - 1 ] = 0;

	return( 1 );
}

/* Retrieves the number of sub file entries
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_get_number_of_sub_file_entries(
     mount_file_entry_t *file_entry,
     int *number_of_sub_file_entries,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_get_number_of_sub_file_entries";

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( libolecf_item_get_number_of_sub_items(
	     file_entry->item,
	     number_of_sub_file_entries,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of items.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the sub file entry for the specific index
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_get_sub_file_entry_by_index(
     mount_file_entry_t *file_entry,
     int sub_file_entry_index,
     mount_file_entry_t **sub_file_entry,
     libcerror_error_t **error )
{
	libolecf_item_t *sub_item         = NULL;
	system_character_t *name          = NULL;
	system_character_t *sub_item_name = NULL;
	static char *function             = "mount_file_entry_get_sub_file_entry_by_index";
	size_t name_size                  = 0; 
	size_t sub_item_name_size         = 0; 
	int result                        = 0;

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( sub_file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub file entry.",
		 function );

		return( -1 );
	}
	if( *sub_file_entry != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid sub file entry value already set.",
		 function );

		return( -1 );
	}
/* TODO refactor to libolecf_item_get_sub_item_by_index */
	if( libolecf_item_get_sub_item(
	     file_entry->item,
	     sub_file_entry_index,
	     &sub_item,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub item: %d.",
		 function,
		 sub_file_entry_index );

		goto on_error;
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libolecf_item_get_utf16_name_size(
	          sub_item,
	          &sub_item_name_size,
	          error );
#else
	result = libolecf_item_get_utf8_name_size(
	          sub_item,
	          &sub_item_name_size,
	          error );
#endif
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub item name size.",
		 function );

		goto on_error;
	}
	sub_item_name = system_string_allocate(
	                 sub_item_name_size );

	if( sub_item_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create sub item name string.",
		 function );

		goto on_error;
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libolecf_item_get_utf16_name(
	          sub_item,
	          (uint16_t *) sub_item_name,
	          sub_item_name_size,
	          error );
#else
	result = libolecf_item_get_utf8_name(
	          sub_item,
	          (uint8_t *) sub_item_name,
	          sub_item_name_size,
	          error );
#endif
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub item name.",
		 function );

		goto on_error;
	}
	if( mount_file_system_get_sanitized_filename(
	     file_entry->file_system,
	     sub_item_name,
	     sub_item_name_size - 1,
	     &name,
	     &name_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub file entry name.",
		 function );

		goto on_error;
	}
	memory_free(
	 sub_item_name );

	sub_item_name = NULL;

	if( mount_file_entry_initialize(
	     sub_file_entry,
	     file_entry->file_system,
	     name,
	     sub_item,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize sub file entry: %d.",
		 function,
		 sub_file_entry_index );

		goto on_error;
	}
	memory_free(
	 name );

	return( 1 );

on_error:
	if( sub_item_name != NULL )
	{
		memory_free(
		 sub_item_name );
	}
	if( name != NULL )
	{
		memory_free(
		 name );
	}
	if( sub_item != NULL )
	{
		libolecf_item_free(
		 &sub_item,
		 NULL );
	}
	return( -1 );
}

/* Reads data at a specific offset
 * Returns the number of bytes read or -1 on error
 */
ssize_t mount_file_entry_read_buffer_at_offset(
         mount_file_entry_t *file_entry,
         void *buffer,
         size_t buffer_size,
         off64_t offset,
         libcerror_error_t **error )
{
	static char *function = "mount_file_entry_read_buffer_at_offset";
	ssize_t read_count    = 0;
	uint8_t item_type     = 0;

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( libolecf_item_get_type(
	     file_entry->item,
	     &item_type,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve type from item.",
		 function );

		return( -1 );
	}
	if( item_type != LIBOLECF_ITEM_TYPE_STREAM )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported item type.",
		 function );

		return( -1 );
	}
	read_count = libolecf_stream_read_buffer_at_offset(
	              file_entry->item,
	              buffer,
	              buffer_size,
	              offset,
	              error );

	if( read_count < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read buffer at offset: %" PRIi64 " (0x%08" PRIx64 ") from item.",
		 function,
		 offset,
		 offset );

		return( -1 );
	}
	return( read_count );
}

/* Retrieves the size
 * Returns 1 if successful or -1 on error
 */
int mount_file_entry_get_size(
     mount_file_entry_t *file_entry,
     size64_t *size,
     libcerror_error_t **error )
{
	static char *function = "mount_file_entry_get_size";
	uint32_t item_size    = 0;

	if( file_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file entry.",
		 function );

		return( -1 );
	}
	if( size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid size.",
		 function );

		return( -1 );
	}
	if( libolecf_item_get_size(
	     file_entry->item,
	     &item_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve size from item.",
		 function );

		return( -1 );
	}
	*size = (size64_t) item_size;

	return( 1 );
}

