CFLAGS = -mms-bitfields -ID:/gtk-bundle_3.6.4-20130921_win32/include/gtk-3.0 -ID:/gtk-bundle_3.6.4-20130921_win32/include/cairo -ID:/gtk-bundle_3.6.4-20130921_win32/include/pango-1.0 -ID:/gtk-bundle_3.6.4-20130921_win32/include/atk-1.0 -ID:/gtk-bundle_3.6.4-20130921_win32/include/cairo -ID:/gtk-bundle_3.6.4-20130921_win32/include/pixman-1 -ID:/gtk-bundle_3.6.4-20130921_win32/include -ID:/gtk-bundle_3.6.4-20130921_win32/include/freetype2 -ID:/gtk-bundle_3.6.4-20130921_win32/include -ID:/gtk-bundle_3.6.4-20130921_win32/include/libpng15 -ID:/gtk-bundle_3.6.4-20130921_win32/include/gdk-pixbuf-2.0 -ID:/gtk-bundle_3.6.4-20130921_win32/include/libpng15 -ID:/gtk-bundle_3.6.4-20130921_win32/include/glib-2.0 -ID:/gtk-bundle_3.6.4-20130921_win32/lib/glib-2.0/include -LD:/gtk-bundle_3.6.4-20130921_win32/lib -lgtk-3 -lgdk-3 -lgdi32 -limm32 -lshell32 -lole32 -Wl,-luuid -lpangocairo-1.0 -lpangoft2-1.0 -lfreetype -lfontconfig -lpangowin32-1.0 -lgdi32 -lpango-1.0 -lm -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lintl

gtk3demo.exe : gtk3.c
	gcc -o gtk3demo.exe gtk3.c $(CFLAGS) -mwindows

clean:
	del gtk3demo.exe
