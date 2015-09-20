from gi.repository import GObject
from gi.repository import GLib
from gi.repository import Gtk
from gi.repository import Gst
from ctypes import *

class PlaybackInterface:

    def __init__(self):
        self.playing = False
        self.ispaused = False
        self.isforward = False

        # A free example sound track
        self.src = "/home/tj/my/crazy/python/2012.avi"
        self.uri = "file://" + self.src

        # GTK window and widgets
        self.window = Gtk.Window()
        self.window.set_size_request(300,50)
        self.window.set_title("My Gstreamer Player V1.0")

        vbox = Gtk.Box(Gtk.Orientation.HORIZONTAL, 0)
        vbox.set_margin_top(3)
        vbox.set_margin_bottom(3)
        self.window.add(vbox)

        self.playButtonImage = Gtk.Image()
        self.playButtonImage.set_from_icon_name("media-playback-start", Gtk.IconSize.BUTTON)
        self.playButton = Gtk.Button()
        self.playButton.add(self.playButtonImage)
        self.playButton.connect("clicked", self.playToggled)
        Gtk.Box.pack_start(vbox, self.playButton, False, False, 0)

        self.pauseButtonImage = Gtk.Image()
        self.pauseButtonImage.set_from_icon_name("media-playback-pause", Gtk.IconSize.BUTTON)
        self.pauseButton = Gtk.Button()
        self.pauseButton.add(self.pauseButtonImage)
        self.pauseButton.connect("clicked", self.pauseToggled)
        self.pauseButton.set_sensitive(False)
        Gtk.Box.pack_start(vbox, self.pauseButton, False, False, 0)

        self.forwardButtonImage = Gtk.Image()
        self.forwardButtonImage.set_from_icon_name("media-seek-forward", Gtk.IconSize.BUTTON)
        self.forwardButton = Gtk.Button()
        self.forwardButton.add(self.forwardButtonImage)
        self.forwardButton.connect("clicked", self.forwardToggled)
        self.forwardButton.set_sensitive(False)
        Gtk.Box.pack_start(vbox, self.forwardButton, False, False, 0)

        self.slider = Gtk.HScale()
        self.slider.set_margin_left(6)
        self.slider.set_margin_right(6)
        self.slider.set_draw_value(False)
        self.slider.set_range(0, 100)
        self.slider.set_increments(1, 10)
        #self.slider.connect("value-changed", self.sliderValueChanged)
        self.slider.connect("change-value", self.sliderChangeValue)
        Gtk.Box.pack_start(vbox, self.slider, True, True, 0)

        self.label = Gtk.Label(label='0:00')
        self.label.set_margin_left(6)
        self.label.set_margin_right(6)
        Gtk.Box.pack_start(vbox, self.label, False, False, 0)

        self.window.connect("destroy", self.playerQuit)
        self.window.show_all()

        # GStreamer Setup
        Gst.init_check(None)
        self.IS_GST010 = Gst.version()[0] == 0

##        self.player = Gst.ElementFactory.make("playbin", "player")
##        fakesink = Gst.ElementFactory.make("fakesink", "fakesink")
##        self.player.set_property("video-sink", fakesink)

##        self.player = Gst.parse_launch("filesrc location="+self.src+\
##                                       " ! decodebin name=dmux dmux. ! \
##                                       queue ! audioconvert ! autoaudiosink \
##                                       dmux. ! queue ! videoconvert ! \
##                                       ximagesink")

        self.player = Gst.Pipeline.new("my_pippeline")
        source = Gst.ElementFactory.make("filesrc", "source");
        decoder = Gst.ElementFactory.make("decodebin", "decoder");
        audio_queue = Gst.ElementFactory.make("queue", "audio_queue")
        audio_convert = Gst.ElementFactory.make("audioconvert", "audio_convert")
        audio_sink = Gst.ElementFactory.make("autoaudiosink", "audio_sink")
        video_queue = Gst.ElementFactory.make("queue", "video_queue")
        video_convert = Gst.ElementFactory.make("videoconvert", "video_convert")
        video_sink = Gst.ElementFactory.make("ximagesink", "video_sink")
        if (not self.player or not source or not decoder or not audio_queue\
            or not audio_convert or not audio_sink or not video_queue \
            or not video_convert or not video_sink):
            print "Not all elements could be created."
            exit(-1)

        self.player.add(source, decoder, audio_queue, audio_convert, \
                          audio_sink, video_queue, video_convert, video_sink)
        res0 = source.link(decoder)
        res1 = audio_queue.link(audio_convert)
        res2 = audio_convert.link(audio_sink)
        res3 = video_queue.link(video_convert)
        res4 = video_convert.link(video_sink)
        if (not res0 or not res1 or not res2 or not res3 or not res4):
            print "Elements could not be linked."
            exit(-1)

##        audio_pad = decoder.get_request_pad("src_%u")
##        queue_audio_pad = audio_queue.get_static_pad("sink")
##        video_pad = decoder.get_request_pad("src_%u")
##        queue_video_pad = video_queue.get_static_pad("sink")
##        if (audio_pad.link(queue_audio_pad) != Gst.PadLinkReturn.OK or
##            video_pad.link(queue_video_pad) != Gst.PadLinkReturn.OK):
##            print "Decoder could not be linked."
##            exit(-1)
        decoder.connect("pad-added", self.pad_added_handler, 0)
        
        bus = self.player.get_bus()
        #bus.add_signal_watch_full()
        bus.connect("message", self.on_message)
##        self.player.connect("about-to-finish",  self.on_finished)

    def on_message(self, bus, message):
        t = message.type
        if t == Gst.Message.EOS:
                self.player.set_state(Gst.State.NULL)
                self.playing = False
        elif t == Gst.Message.ERROR:
                self.player.set_state(Gst.State.NULL)
                err, debug = message.parse_error()
                print "Error: %s" % err, debug
                self.playing = False

        self.updateButtons()

    def on_finished(self, player):
        self.playing = False
        self.slider.set_value(0)
        self.label.set_text("0:00")
        self.updateButtons()

    def pad_added_handler(src, new_pad, data):
        print "Received new pad '%s' from '%s':"%( new_pad.get_name(), \
                                                   src.get_name())

    def play(self):
##        self.player.set_property("uri", self.uri)
##        self.player.set_property("location", self.src)
        self.player.set_state(Gst.State.PLAYING)
        GObject.timeout_add(1000, self.updateSlider)

    def stop(self):
        self.player.set_state(Gst.State.NULL)
        
    def playToggled(self, w):
        self.slider.set_value(0)
        self.label.set_text("0:00")

        if(self.playing == False):
                self.play()
        else:
                self.stop()

        self.playing=not(self.playing)
        self.updateButtons()

    def pauseToggled(self, w):
        if(self.ispaused == False):
           self.player.set_state(Gst.State.PAUSED)
        else:
           self.player.set_state(Gst.State.PLAYING)
        self.ispaused = not(self.ispaused)

    def forwardToggled(self, w):
        if self.IS_GST010:
            nanosecs = self.player.query_position(Gst.Format.TIME)[2]
        else:
            nanosecs = self.player.query_position(Gst.Format.TIME)[1]

        if(self.isforward == False):
           self.player.seek(0.5, Gst.Format.TIME, Gst.SeekFlags.FLUSH, Gst.SeekType.SET,\
                         nanosecs, Gst.SeekType.NONE, c_long(Gst.CLOCK_TIME_NONE).value)
        else:
           self.player.seek(2.0, Gst.Format.TIME, Gst.SeekFlags.FLUSH, Gst.SeekType.SET,\
                         nanosecs, Gst.SeekType.NONE, c_long(Gst.CLOCK_TIME_NONE).value)
        self.isforward = not(self.isforward)

    def sliderValueChanged(self, w):
        print w.get_value()

    def sliderChangeValue(self, w, scroll, value):
        print value
        if value>=0:
            self.player.seek(1.0, Gst.Format.TIME, Gst.SeekFlags.FLUSH, Gst.SeekType.SET,\
                             value * Gst.SECOND, Gst.SeekType.NONE, \
                             c_long(Gst.CLOCK_TIME_NONE).value)

    def updateSlider(self):
        if(self.playing == False):
           return False # cancel timeout

        try:
           if self.IS_GST010:
              nanosecs = self.player.query_position(Gst.Format.TIME)[2]
              duration_nanosecs = self.player.query_duration(Gst.Format.TIME)[2]
           else:
              nanosecs = self.player.query_position(Gst.Format.TIME)[1]
              duration_nanosecs = self.player.query_duration(Gst.Format.TIME)[1]

           # block seek handler so we don't seek when we set_value()
           # self.slider.handler_block_by_func(self.on_slider_change)

           duration = float(duration_nanosecs) / Gst.SECOND
           position = float(nanosecs) / Gst.SECOND
           self.slider.set_range(0, duration)
           self.slider.set_value(position)
           self.label.set_text ("%d" % (position / 60) + ":%02d" % (position % 60))

           #self.slider.handler_unblock_by_func(self.on_slider_change)
        
        except Exception as e:
                # pipeline must not be ready and does not know position
                print e
                pass

        return True

    def updateButtons(self):
        if(self.playing == False):
           self.playButtonImage.set_from_icon_name("media-playback-start", Gtk.IconSize.BUTTON)
           self.pauseButton.set_sensitive(False)
           self.forwardButton.set_sensitive(False)
        else:
           self.playButtonImage.set_from_icon_name("media-playback-stop", Gtk.IconSize.BUTTON)
           self.pauseButton.set_sensitive(True)
           self.forwardButton.set_sensitive(True)

    def playerQuit(self, w):
        myplayer.player.set_state(Gst.State.NULL)
        Gtk.main_quit()

if __name__ == "__main__":
        myplayer = PlaybackInterface()
        Gtk.main()
