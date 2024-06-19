#ifndef _CAM_WEBM_
#define _CAM_WEBM_

#include <stdio.h>
#include <gst/gst.h>

/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData
{
  GstElement *playbin;          /* Our one and only element */

  gint n_video;                 /* Number of embedded video streams */
  gint n_audio;                 /* Number of embedded audio streams */
  gint n_text;                  /* Number of embedded subtitle streams */

  gint current_video;           /* Currently playing video stream */
  gint current_audio;           /* Currently playing audio stream */
  gint current_text;            /* Currently playing subtitle stream */

  GMainLoop *main_loop;         /* GLib's Main Loop */
} CustomData;

/* playbin flags */
typedef enum
{
  GST_PLAY_FLAG_VIDEO = (1 << 0),       /* We want video output */
  GST_PLAY_FLAG_AUDIO = (1 << 1),       /* We want audio output */
  GST_PLAY_FLAG_TEXT = (1 << 2) /* We want subtitle output */
} GstPlayFlags;

#endif