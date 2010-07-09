/*
 *  nautilus-batch-rename.c
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

#include "nautilus-batch-rename.h"

#include <string.h>
#include <errno.h>

#include <glib/gi18n.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#include <libnautilus-extension/nautilus-file-info.h>
 
#define PKGDATADIR DATADIR "/" PACKAGE

typedef struct _NautilusBatchRenamePrivate NautilusBatchRenamePrivate;

struct _NautilusBatchRenamePrivate {
	GList *files;
	
	gchar *suffix;
	gchar *last_suffix;
	
	gint files_renamed;
	gint files_total;
	long increment;
	gboolean cancelled;
	
	gint order;
	char cstart;
	long lstart;

	GtkDialog *rename_dialog;
	GtkComboBox *order_combobox;
	GtkEntry *name_entry;
	GtkEntry *start_entry;
	GtkEntry *increment_entry;
};

#define NAUTILUS_BATCH_RENAME_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), NAUTILUS_TYPE_BATCH_RENAME, NautilusBatchRenamePrivate))

G_DEFINE_TYPE (NautilusBatchRename, nautilus_batch_rename, G_TYPE_OBJECT)

enum {
	PROP_FILES = 1,
};

typedef enum {
	/* Place Signal Types Here */
	SIGNAL_TYPE_EXAMPLE,
	LAST_SIGNAL
} NautilusBatchRenameSignalType;

static void
nautilus_batch_rename_finalize(GObject *object)
{
	NautilusBatchRename *dialog = NAUTILUS_BATCH_RENAME (object);
	NautilusBatchRenamePrivate *priv = NAUTILUS_BATCH_RENAME_GET_PRIVATE (dialog);
	
	g_free (priv->suffix);
		
	G_OBJECT_CLASS(nautilus_batch_rename_parent_class)->finalize(object);
}

static void
nautilus_batch_rename_set_property (GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
	NautilusBatchRename *dialog = NAUTILUS_BATCH_RENAME (object);
	NautilusBatchRenamePrivate *priv = NAUTILUS_BATCH_RENAME_GET_PRIVATE (dialog);

	switch (property_id) {
	case PROP_FILES:
		priv->files = g_value_get_pointer (value);
		priv->files_total = g_list_length (priv->files);
		break;
	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
		break;
	}
}

static void
nautilus_batch_rename_get_property (GObject      *object,
                        guint         property_id,
                        GValue       *value,
                        GParamSpec   *pspec)
{
	NautilusBatchRename *dialog = NAUTILUS_BATCH_RENAME (object);
	NautilusBatchRenamePrivate *priv = NAUTILUS_BATCH_RENAME_GET_PRIVATE (dialog);

	switch (property_id) {
	case PROP_FILES:
		g_value_set_pointer (value, priv->files);
		break;
	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
		break;
	}
}

static void
nautilus_batch_rename_class_init(NautilusBatchRenameClass *klass)
{
	g_type_class_add_private (klass, sizeof (NautilusBatchRenamePrivate));

	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	GParamSpec *files_param_spec;

	object_class->finalize = nautilus_batch_rename_finalize;
	object_class->set_property = nautilus_batch_rename_set_property;
	object_class->get_property = nautilus_batch_rename_get_property;

	files_param_spec = g_param_spec_pointer ("files",
	"Files",
	"Set selected files",
	G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_property (object_class,
	PROP_FILES,
	files_param_spec);
}

static void run_op (NautilusBatchRename *rename);

static gchar* 
incremente(gchar *string, int pos, long increment){
	int i, n;
	if(string[pos] == 'Z' || string[pos] == 'z'){
		char nchar = (string[pos] == 'Z') ? 'A' : 'a';		
		string[pos] = nchar;
		if(pos == 0){
			sprintf(string,"%c%s",nchar,string);
		}else{
			string = incremente(string,pos-1,increment);
		}		
	}else{
		char c = string[pos];
		char* substr = strdup(string); 
		substr[pos] = substr[pos] + increment;		
		string = (char *) malloc(strlen(substr));
		strcpy(string,substr);
	}
	return string;
}

static GFile *
nautilus_batch_rename_transform_filename (NautilusBatchRename *rename, GFile *orig_file)
{
	NautilusBatchRenamePrivate *priv = NAUTILUS_BATCH_RENAME_GET_PRIVATE (rename);

	GFile *parent_file, *new_file;
	char *basename, *extension, *new_path, *new_basename, *order;
	gchar** psuffix;
	
	g_return_val_if_fail (G_IS_FILE (orig_file), NULL);

	parent_file = g_file_get_parent (orig_file);

	basename = g_strdup (g_file_get_basename (orig_file));
	
	extension = g_strdup (strrchr (basename, '.'));
	if (extension != NULL)
		basename[strlen (basename) - strlen (extension)] = '\0';

	psuffix = g_strsplit(priv->suffix,"#",2);
		
	if(priv->order == 0){
		order = g_strdup_printf("%ld",priv->lstart);
	}else if(priv->order == 1){ //a= 97 and z = 122
		order = g_strdup_printf("%s",priv->last_suffix);
	}
		
	new_basename = g_strdup_printf ("%s%s%s%s", 
		psuffix[0] == NULL ? "" : psuffix[0],
		order,
		psuffix[1] == NULL ? "" : psuffix[1],
		extension == NULL ? "" : extension);
	g_free (basename);
	g_free (extension);
	g_free (order);

	new_file = g_file_get_child (parent_file, new_basename);

	g_object_unref (parent_file);
	g_free (new_basename);

	return new_file;
}

static void
nautilus_batch_rename_cancel_cb (GtkDialog *dialog, gint response_id, gpointer user_data)
{
	NautilusBatchRename *rename = NAUTILUS_BATCH_RENAME (user_data);
	NautilusBatchRenamePrivate *priv = NAUTILUS_BATCH_RENAME_GET_PRIVATE (rename);
	
	priv->cancelled = TRUE;
	gtk_dialog_set_response_sensitive (dialog, GTK_RESPONSE_CANCEL, FALSE);
}

static void
run_op (NautilusBatchRename *rename)
{
	NautilusBatchRenamePrivate *priv = NAUTILUS_BATCH_RENAME_GET_PRIVATE (rename);
	GList *file;
	char *filename;
	GError *err = NULL;
	
	g_return_if_fail (priv->files != NULL);

	for (file = priv->files; file != NULL; file = file->next) {
		NautilusFileInfo *nfile = NAUTILUS_FILE_INFO (file->data);

		GFile *orig_location = nautilus_file_info_get_location (nfile);
		GFile *new_location = nautilus_batch_rename_transform_filename (rename, orig_location);

		filename = g_strdup (nautilus_file_info_get_name(nfile));

		if(g_file_move (orig_location,new_location, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &err)){
			g_object_unref (orig_location);
			g_object_unref (new_location);
		
			if(priv->order == 0){
				priv->lstart = priv->lstart + priv->increment;
			}else if(priv->order == 1){
				priv->last_suffix = incremente(priv->last_suffix,strlen(priv->last_suffix)-1,priv->increment);
				//priv->cstart++;
			}
		}else{
			if(err){	
				GtkWidget *msg_dialog = gtk_message_dialog_new (GTK_WINDOW (priv->rename_dialog),
					GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
					GTK_BUTTONS_OK,"File '%s': %s", filename, err->message);
				gtk_dialog_run (GTK_DIALOG (msg_dialog));
				gtk_widget_destroy (msg_dialog);	
			}

			err = NULL;
		}
		

	}
	g_free(file);
	g_free(filename);		
	g_free(err);
	/* terminate operation */
}

static long
is_numeric(gchar *start){
	char *endptr;
    long val;

	errno = 0;    /* To distinguish success/failure after call */
    val = strtol(start, &endptr, 10);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		return 0;
	}

	return val;
}

static void
nautilus_batch_rename_response_cb (GtkDialog *dialog, gint response_id, gpointer user_data)
{
	NautilusBatchRename *rename = NAUTILUS_BATCH_RENAME (user_data);
	NautilusBatchRenamePrivate *priv = NAUTILUS_BATCH_RENAME_GET_PRIVATE (rename);
	gchar *start, *suffix, *increment;

	if (response_id == GTK_RESPONSE_OK) {
		suffix =  g_strdup(gtk_entry_get_text (priv->name_entry));
		if (strlen (suffix) == 0) {
			GtkWidget *msg_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog),
				GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK, _("Please enter a valid filename suffix!"));
			gtk_dialog_run (GTK_DIALOG (msg_dialog));
			gtk_widget_destroy (msg_dialog);
			g_free(suffix);
			return;
		}

		if (strlen (g_strchomp(suffix)) == 0) {
			GtkWidget *msg_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog),
				GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK, _("Please enter a valid start string!"));
			gtk_dialog_run (GTK_DIALOG (msg_dialog));
			gtk_widget_destroy (msg_dialog);
			g_free(suffix);			
			return;
		}

		if (strrchr (suffix,'#') == NULL) {
			GtkWidget *msg_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog),
				GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK, _("Please, the filename must have a mask `#`"));
			gtk_dialog_run (GTK_DIALOG (msg_dialog));
			gtk_widget_destroy (msg_dialog);
			g_free(suffix);			
			return;
		}

		priv->suffix =  suffix;
		priv->order  =  gtk_combo_box_get_active (priv->order_combobox);
		start 		 =  g_strdup(gtk_entry_get_text (priv->start_entry));
		start		 =  g_strchomp(start);
 
		if(priv->order == 0){ //priv->start must be a number
	
			priv->lstart = is_numeric(start);

			if (errno != 0) {
				GtkWidget *msg_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog),
					GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
					GTK_BUTTONS_OK, _("Please, the start must have a integer number"));
				gtk_dialog_run (GTK_DIALOG (msg_dialog));
				gtk_widget_destroy (msg_dialog);
				return;
			}		
		}else if(priv->order == 1){ //priv->start must be a alpha
			if (!g_ascii_isalpha(start[0]) || strlen(start) != 1) {
				GtkWidget *msg_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog),
					GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
					GTK_BUTTONS_OK, _("Please, the start must have only an alphabetic (i.e: a,b,c....)"));
				gtk_dialog_run (GTK_DIALOG (msg_dialog));
				gtk_widget_destroy (msg_dialog);
				return;
			}

			priv->cstart = start[0];
			priv->last_suffix = start;
		}

		increment	 =  g_strdup(gtk_entry_get_text (priv->increment_entry));
		increment	 =  g_strchomp(increment);

		priv->increment = is_numeric(increment);

		if (priv->increment == 0 || errno != 0) {
			GtkWidget *msg_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog),
				GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK, _("Please, the increment must have a non zero integer number"));
			gtk_dialog_run (GTK_DIALOG (msg_dialog));
			gtk_widget_destroy (msg_dialog);
			return;
		}
		
		run_op (rename);
	}
	gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
order_combo_box_changed(GtkWidget *widget, gpointer user_data)
{
	NautilusBatchRename *rename = NAUTILUS_BATCH_RENAME (user_data);
	NautilusBatchRenamePrivate *priv = NAUTILUS_BATCH_RENAME_GET_PRIVATE (rename);
	gint order =  gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
	if(order == 0){
		gtk_entry_set_text(priv->start_entry, "0");
	}else{
		gtk_entry_set_text(priv->start_entry, "a");
	}
}

static void
nautilus_batch_rename_init(NautilusBatchRename *rename)
{
	NautilusBatchRenamePrivate *priv = NAUTILUS_BATCH_RENAME_GET_PRIVATE (rename);

	GladeXML *xml_dialog;

	xml_dialog = glade_xml_new (PKGDATADIR "/nautilus-batch-rename.glade",
					  NULL, GETTEXT_PACKAGE);

	priv->rename_dialog = GTK_DIALOG (glade_xml_get_widget (xml_dialog, "rename_dialog"));
	priv->order_combobox = GTK_COMBO_BOX (glade_xml_get_widget (xml_dialog, "order_combobox"));
	priv->name_entry = GTK_ENTRY (glade_xml_get_widget (xml_dialog, "name_entry"));	
	priv->start_entry = GTK_ENTRY (glade_xml_get_widget (xml_dialog, "start_entry"));
	priv->increment_entry = GTK_ENTRY (glade_xml_get_widget (xml_dialog, "increment_entry"));

	g_signal_connect (G_OBJECT (priv->rename_dialog), "response", G_CALLBACK(nautilus_batch_rename_response_cb), rename);
	g_signal_connect(G_OBJECT(priv->order_combobox), "changed", G_CALLBACK(order_combo_box_changed), rename);

}


NautilusBatchRename *
nautilus_batch_rename_new (GList *files)
{
	return g_object_new (NAUTILUS_TYPE_BATCH_RENAME, "files", files, NULL);
}

void
nautilus_batch_rename_show_dialog (NautilusBatchRename *rename)
{
	NautilusBatchRenamePrivate *priv = NAUTILUS_BATCH_RENAME_GET_PRIVATE (rename);

	gtk_widget_show (GTK_WIDGET (priv->rename_dialog));

	char *basename, *extension;
	NautilusFileInfo *file = NAUTILUS_FILE_INFO (priv->files->data);
	basename = g_strdup (nautilus_file_info_get_name(file));
	
	extension = g_strdup (strrchr (basename, '.'));
	if (extension != NULL){
		basename[strlen (basename) - strlen (extension)] = '#';
		basename[strlen (basename) - strlen (extension) + 1] = '\0';
	}else{
		g_sprintf(basename,"%s%s",basename,"#");
	}

	gtk_entry_set_text(priv->name_entry,basename);
	gtk_combo_box_set_active(priv->order_combobox,0);
	gtk_entry_set_text(priv->start_entry,"0");
	gtk_entry_set_text(priv->increment_entry,"1");
}
