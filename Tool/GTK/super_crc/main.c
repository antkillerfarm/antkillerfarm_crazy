#include <gtk/gtk.h>
#include <string.h>
#include "types.h"
#include "nmea_crc.h"
#include "crc16.h"
#include "crc24q.h"

enum{
INPUT_TYPE_TEXT = 0,
INPUT_TYPE_FILE
};

enum{
CON_TYPE_ASCII = 0,
CON_TYPE_HEX, //hex data on ascii form
CON_TYPE_BIN
};

enum{
CRC_TYPE_NMEA = 0,
CRC_TYPE_CRC16,
CRC_TYPE_CRC24Q
};

typedef struct{
	GtkEntry *entry_con;
	GtkEntry *entry_crc;
	GtkButton *btn_choose_file;
}MainWindowSubWidget;

typedef struct{
	gint input_type;
	gint context_type;
	gint crc_type;
}CRCStatus;

MainWindowSubWidget main_window_sub_widget;
CRCStatus crc_status = {CON_TYPE_ASCII, CRC_TYPE_NMEA};

G_MODULE_EXPORT void do_toggled_rb_input_t0(GtkToggleButton *togglebutton, gpointer data)
{
	crc_status.input_type = INPUT_TYPE_TEXT;
	gtk_widget_set_sensitive(GTK_WIDGET(main_window_sub_widget.btn_choose_file), FALSE);
}

G_MODULE_EXPORT void do_toggled_rb_input_t1(GtkToggleButton *togglebutton, gpointer data)
{
	crc_status.input_type = INPUT_TYPE_FILE;
	gtk_widget_set_sensitive(GTK_WIDGET(main_window_sub_widget.btn_choose_file), TRUE);
}

G_MODULE_EXPORT void do_toggled_rb_con_t0(GtkToggleButton *togglebutton, gpointer data)
{
	crc_status.context_type = CON_TYPE_ASCII;
}

G_MODULE_EXPORT void do_toggled_rb_con_t1(GtkToggleButton *togglebutton, gpointer data)
{
	crc_status.context_type = CON_TYPE_HEX;
}

G_MODULE_EXPORT void do_toggled_rb_con_t2(GtkToggleButton *togglebutton, gpointer data)
{
	crc_status.context_type = CON_TYPE_BIN;
}

G_MODULE_EXPORT void do_toggled_rb_crc_t0(GtkToggleButton *togglebutton, gpointer data)
{
	crc_status.crc_type = CRC_TYPE_NMEA;
}

G_MODULE_EXPORT void do_toggled_rb_crc_t1(GtkToggleButton *togglebutton, gpointer data)
{
	crc_status.crc_type = CRC_TYPE_CRC16;
}

G_MODULE_EXPORT void do_toggled_rb_crc_t2(GtkToggleButton *togglebutton, gpointer data)
{
	crc_status.crc_type = CRC_TYPE_CRC24Q;
}

G_MODULE_EXPORT void do_choose_file_button_clicked(GtkButton *button, gpointer data)
{
	GtkWidget *dialog;
	GtkWindow *window = (GtkWindow *)data;

	dialog = gtk_file_chooser_dialog_new ("Open File",
					      window,
					      GTK_FILE_CHOOSER_ACTION_OPEN,
					      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		gchar *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gtk_entry_set_text(main_window_sub_widget.entry_con, filename);
		g_print("IPhone 5S!%d/%d/%d/%s\r\n",
			crc_status.input_type,
			crc_status.context_type,
			crc_status.crc_type,
			filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);

}

G_MODULE_EXPORT void do_calc_button_clicked(GtkButton *button, gpointer data)
{
	const gchar * content = gtk_entry_get_text(main_window_sub_widget.entry_con);
	gchar buf[2046];
        gchar *buf_in;
	gchar *buf_out;
	int32u len;

	if (crc_status.input_type == INPUT_TYPE_TEXT)
	{
		buf_in = g_strdup(content);
	}
	else
	{
		GFile *file = g_file_new_for_path(content);
		GFileInputStream *f_in_stream = g_file_read(file, NULL, NULL);
		gint file_len;
		if (f_in_stream != NULL)
		{
			
			g_input_stream_read_all(G_INPUT_STREAM(f_in_stream), buf, 
						2046, &file_len, NULL, NULL);
			buf_in = buf;
			len = file_len;
			g_print("Jobs!%d/%s\r\n", len, buf_in);
			g_input_stream_close(G_INPUT_STREAM(f_in_stream), NULL, NULL);
			g_object_unref(f_in_stream);
		}
		g_object_unref(file);
	}

        if (crc_status.context_type == CON_TYPE_HEX)
	{
	        int32u i, j = 0;
		int8u data, flag = 0;
		len = strlen(buf_in);
		g_print("Woz!%d\r\n", len);
		for (i = 0; i < len; i++)
		{
			if (g_ascii_isxdigit(buf_in[i]))
			{
				if (flag == 0)
				{
					data = 0;
					data = (g_ascii_xdigit_value(buf_in[i])) << 4;
					flag = 1;
				}
				else
				{
					data |= g_ascii_xdigit_value(buf_in[i]);
					buf_in[j] = data;
					j++;
					flag = 0;
				}
			}
		}
		len = j;
	}
	else if (crc_status.context_type == CON_TYPE_ASCII)
	{
		len = strlen(buf_in);
	}

	if (crc_status.crc_type == CRC_TYPE_NMEA)
	{
		int32u crc = _LibNMEA_Xor(buf_in);
		buf_out = g_strdup_printf("%02X", crc);
	}
	else if (crc_status.crc_type == CRC_TYPE_CRC16)
	{
		int16u crc = Block_Crc16(buf_in, len);
		buf_out = g_strdup_printf("%04X", crc);
	}
	else
	{
		int32u crc = CRC_24Q(buf_in, len);
		buf_out = g_strdup_printf("%06X", crc);
	}
	
	gtk_entry_set_text(main_window_sub_widget.entry_crc, buf_out);

	g_free(buf_in);
	g_free(buf_out);
}

int main(int argc, char *argv[])
{
	GtkWidget *window = NULL;
	GtkBuilder *builder;
	GError *err = NULL;
	GtkEntry *entry;
	GtkButton *button;

	gtk_init(NULL, NULL);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "super_crc.glade", &err);

	gtk_builder_connect_signals(builder, NULL);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

	entry = GTK_ENTRY(gtk_builder_get_object(builder, "entry_con"));
	main_window_sub_widget.entry_con = entry;
	entry = GTK_ENTRY(gtk_builder_get_object(builder, "entry_crc"));
	main_window_sub_widget.entry_crc = entry;
	button = GTK_BUTTON(gtk_builder_get_object(builder, "btn_choose_file"));
	main_window_sub_widget.btn_choose_file = button;

	g_signal_connect(window, "destroy",
			 G_CALLBACK (gtk_main_quit), &window);

	g_object_unref(G_OBJECT(builder));

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
