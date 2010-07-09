/*
 *  nautilus-batch-rename.h
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

#ifndef NAUTILUS_BATCH_RENAME_H
#define NAUTILUS_BATCH_RENAME_H

#include <glib-object.h>

G_BEGIN_DECLS

#define NAUTILUS_TYPE_BATCH_RENAME         (nautilus_batch_rename_get_type ())
#define NAUTILUS_BATCH_RENAME(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), NAUTILUS_TYPE_BATCH_RENAME, NautilusBatchRename))
#define NAUTILUS_BATCH_RENAME_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), NAUTILUS_TYPE_BATCH_RENAME, NautilusBatchRenameClass))
#define NAUTILUS_IS_BATCH_RENAME(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), NAUTILUS_TYPE_BATCH_RENAME))
#define NAUTILUS_IS_BATCH_RENAME_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), NAUTILUS_TYPE_BATCH_RENAME))
#define NAUTILUS_BATCH_RENAME_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), NAUTILUS_TYPE_BATCH_RENAME, NautilusBatchRenameClass))

typedef struct _NautilusBatchRename NautilusBatchRename;
typedef struct _NautilusBatchRenameClass NautilusBatchRenameClass;

struct _NautilusBatchRename {
	GObject parent;
};

struct _NautilusBatchRenameClass {
	GObjectClass parent_class;
	/* Add Signal Functions Here */
};

GType nautilus_batch_rename_get_type ();
NautilusBatchRename *nautilus_batch_rename_new (GList *files);
void nautilus_batch_rename_show_dialog (NautilusBatchRename *dialog);

G_END_DECLS

#endif /* NAUTILUS_BATCH_RENAME_H */
