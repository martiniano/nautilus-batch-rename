/*
 *  nautilus-batch-rename-extension.h
 * 
 *  Copyright (C) 2010 Anderson Martiniano
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Author: Anderson Martiniano <andersonmartiniano@gmail.com>
 * 
 */

#ifndef NAUTILUS_BATCH_RENAME_EXTENSION_H
#define NAUTILUS_BATCH_RENAME_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

/* Declarations for the open terminal extension object.  This object will be
 * instantiated by nautilus.  It implements the GInterfaces 
 * exported by libnautilus. */


#define NAUTILUS_TYPE_BATCH_RENAME_EXTENSION	  (nautilus_batch_rename_extension_get_type ())
#define NAUTILUS_BATCH_RENAME_EXTENSION(o)		  (G_TYPE_CHECK_INSTANCE_CAST ((o), NAUTILUS_TYPE_BATCH_RENAME_EXTENSION, NautilusBatchRenameExtension))
#define NAUTILUS_IS_BATCH_RENAME_EXTENSION(o)	  (G_TYPE_CHECK_INSTANCE_TYPE ((o), NAUTILUS_TYPE_BATCH_RENAME_EXTENSION))
typedef struct _NautilusBatchRenameExtension	  NautilusBatchRenameExtension;
typedef struct _NautilusBatchRenameExtensionClass	  NautilusBatchRenameExtensionClass;

struct _NautilusBatchRenameExtension {
	GObject parent_slot;
};

struct _NautilusBatchRenameExtensionClass {
	GObjectClass parent_slot;
};

GType nautilus_batch_rename_extension_get_type      (void);
void  nautilus_batch_rename_extension_register_type (GTypeModule *module);

G_END_DECLS

#endif
