/*
 *  nautilus-batch-rename-extension.c
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

#ifdef HAVE_CONFIG_H
 #include <config.h> /* for GETTEXT_PACKAGE */
#endif

#include "nautilus-batch-rename-extension.h"
#include "nautilus-batch-rename.h"

#include <libnautilus-extension/nautilus-menu-provider.h>

#include <glib/gi18n-lib.h>
#include <gtk/gtkicontheme.h>
#include <gtk/gtkwidget.h>

#include <string.h> /* for strcmp */

static GType type_list[1];

static void nautilus_batch_rename_extension_instance_init (NautilusBatchRenameExtension      *rename);
static void nautilus_batch_rename_extension_class_init    (NautilusBatchRenameExtensionClass *class);

static GType batch_rename_extension_type = 0;

void
nautilus_module_initialize (GTypeModule *module)
{
	g_print ("Initializing nautilus-batch-rename extension\n");

	nautilus_batch_rename_extension_register_type (module);
	type_list[0] = NAUTILUS_TYPE_BATCH_RENAME_EXTENSION;

	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
}

void
nautilus_module_shutdown (void)
{
	g_print ("Shutting down nautilus-batch-rename extension\n");
}

void 
nautilus_module_list_types (const GType **types,
			    int          *num_types)
{
	*types = type_list;
	*num_types = G_N_ELEMENTS (type_list);
}

static void
batch_rename_callback (NautilusMenuItem *item,
			GList *files)
{
	NautilusBatchRename *rename = nautilus_batch_rename_new (files);
	nautilus_batch_rename_show_dialog (rename);
}

static GList *
nautilus_batch_rename_extension_get_background_items (NautilusMenuProvider *provider,
					     GtkWidget		  *window,
					     NautilusFileInfo	  *file_info)
{
	return NULL;
}

GList *
nautilus_batch_rename_extension_get_file_items (NautilusMenuProvider *provider,
				       GtkWidget            *window,
				       GList                *files)
{
	NautilusMenuItem *item;
	GList *file;
	GList *items = NULL;
	
	if(g_list_length (files) > 1){
			item = nautilus_menu_item_new ("NautilusBatchRenameExtension::rename",
				        _("_Rename Files..."),
				        _("Rename each selected file"),
				       "stock_position-size");
			g_signal_connect (item, "activate",
					  G_CALLBACK (batch_rename_callback),
					  nautilus_file_info_list_copy (files));
					
			items = g_list_prepend (items, item);
			
			items = g_list_reverse (items);

			return items;
	}
	
	return NULL;
}

static void
nautilus_batch_rename_extension_menu_provider_iface_init (NautilusMenuProviderIface *iface)
{
	iface->get_background_items = nautilus_batch_rename_extension_get_background_items;
	iface->get_file_items = nautilus_batch_rename_extension_get_file_items;
}

static void 
nautilus_batch_rename_extension_instance_init (NautilusBatchRenameExtension *rename)
{
}

static void
nautilus_batch_rename_extension_class_init (NautilusBatchRenameExtensionClass *class)
{
}

GType
nautilus_batch_rename_extension_get_type (void) 
{
	return batch_rename_extension_type;
}

void
nautilus_batch_rename_extension_register_type (GTypeModule *module)
{
	static const GTypeInfo info = {
		sizeof (NautilusBatchRenameExtensionClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) nautilus_batch_rename_extension_class_init,
		NULL, 
		NULL,
		sizeof (NautilusBatchRenameExtension),
		0,
		(GInstanceInitFunc) nautilus_batch_rename_extension_instance_init,
	};

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) nautilus_batch_rename_extension_menu_provider_iface_init,
		NULL,
		NULL
	};

	batch_rename_extension_type = g_type_module_register_type (module,
						     G_TYPE_OBJECT,
						     "NautilusBatchRenameExtension",
						     &info, 0);

	g_type_module_add_interface (module,
				     batch_rename_extension_type,
				     NAUTILUS_TYPE_MENU_PROVIDER,
				     &menu_provider_iface_info);
}
