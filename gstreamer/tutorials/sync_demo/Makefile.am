bin_PROGRAMS = audio_leader audio_follower

audio_leader_SOURCES = leader.c

AM_CPPFLAGS = $(GSTREAMER1_CFLAGS)
audio_leader_LDADD = $(GSTREAMER1_LIBS)

audio_follower_SOURCES = follower.c

audio_follower_LDADD = $(GSTREAMER1_LIBS)
