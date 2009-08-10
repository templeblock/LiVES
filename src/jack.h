// jack.h
// LiVES (lives-exe)
// (c) G. Finch 2005 - 2009
// Released under the GPL 3 or later
// see file ../COPYING for licensing details

#ifndef _HAS_LIVES_JACK_H
#define _HAS_LIVES_JACK_H

#ifdef ENABLE_JACK

/////////////////////////////////////////////////////////////////
// Transport
#include <jack/jack.h>
#include <jack/transport.h>

void lives_jack_init (void); /* start up server on LiVES init */
gboolean lives_jack_poll(gpointer data); /* poll function to check transport state */
void lives_jack_end (void);

int lives_start_ready_callback (jack_transport_state_t state, jack_position_t *pos, void *arg);

void jack_pb_start (void); /* start playback transport master */
void jack_pb_stop (void); /* pause playback transport master */


////////////////////////////////////////////////////////////////////////////
// Audio


#define JACK_MAX_OUTPUT_PORTS 10
#define JACK_MAX_INPUT_PORTS 10

#define ERR_PORT_NOT_FOUND 10

typedef struct {
  guchar* data;                 /* pointer to the audio data */
  size_t  size;                /* number of bytes pointed to by data */
} audio_buffer_t;

typedef jack_nframes_t nframes_t;


#define JACK_CMD_PROCESSED 0
#define JACK_CMD_FILE_OPEN 1
#define JACK_CMD_FILE_CLOSE 2
#define JACK_CMD_FILE_SEEK 3

/* message passing structure */
typedef struct _jack_message_t {
  gint command;
  gchar *data;
  struct _jack_message_t *next;
} jack_message_t;

// let's hope these are well above the standard jack transport states...
#define JackTClosed 1024
#define JackTReset 1025
#define JackTStopped 1026

typedef struct {
  gint      dev_idx;                      /* id of this device ??? */ 
  glong     sample_out_rate;                   /* samples(frames) per second */
  glong     sample_in_rate;                   /* samples(frames) per second */
  gulong    num_input_channels;            /* number of input channels(1 is mono, 2 stereo etc..) */
  gulong    num_output_channels;           /* number of output channels(1 is mono, 2 stereo etc..) */
  gulong    bytes_per_channel;

  gulong    latencyMS;                     /* latency in ms between writing and actual audio output of the written data */

  gulong    buffer_size;                   /* number of bytes in the buffer allocated for processing data in process_audio() */

  gchar*    sound_buffer;

  gulong    num_calls;                     /* count of process_audio() calls */
  gulong    chunk_size;

  jack_port_t*     output_port[JACK_MAX_OUTPUT_PORTS]; /* output ports */
  jack_port_t*     input_port[JACK_MAX_INPUT_PORTS]; /* input ports */
  jack_client_t*   client;                        /* pointer to jack client */

  gchar             **jack_port_name;              /* user given strings for the port names, can be NULL */
  unsigned int     jack_port_name_count;          /* the number of port names given */
  gulong    jack_port_flags;               /* flags to be passed to jack when opening the output ports */

  audio_buffer_t*    aPlayPtr;                      /* pointer to the current audio file */
  gint loop;

#define JACK_LOOP_NONE 0
#define JACK_LOOP_FORWARD 1
#define JACK_LOOP_PINGPONG 2

  jack_transport_state_t state;

  float     volume[JACK_MAX_OUTPUT_PORTS];      // amount volume, 1.0 is full volume

  gboolean          in_use;                        /* true if this device is currently in use */
  gboolean mute;

  volatile jack_message_t   *msgq;          /* linked list of messages we are sending to the callback process */

  int fd;                  /* if >0 we are playing from a file */
  gboolean is_opening; // TRUE if file is opening (audiodump.pcm)
  off_t seek_pos;
  off_t seek_end;
  gboolean usigned;
  gboolean reverse_endian;

  gshort *whentostop; // pointer to mainw->whentostop
  volatile gint *cancelled; // pointer to mainw->cancelled

  /* variables used for trying to restart the connection to jack */
  gboolean             jackd_died;                    /* true if jackd has died and we should try to restart it */
  struct timeval   last_reconnect_attempt;

  gboolean play_when_stopped; // if we should play audio even when jack transport is stopped
  gint64 audio_ticks; // ticks when we did the last seek, used to calculate current ticks from audio
  gulong frames_written;

  gint out_chans_available;
  gint in_chans_available;

  gboolean is_paused;

  gboolean is_output; // is output FROM host to jack

  gboolean is_silent;

  gint playing_file;

  /* next are not used yet */
#ifdef JACK_BUFF
  guchar *in_buf1;
  guchar *in_buf2;

  off_t in_buf1_start; // input buffer 1 start offset in file
  off_t in_buf2_start; // input buffer 2 start offset in file

  gulong in_buf1_size; // total size of buffer 1
  gulong in_buf2_size; // total size of buffer 2

  gulong in_buf1_used; // used bytes in buffer 1
  gulong in_buf2_used; // used bytes in buffer 2

  gint current_buffer; // 1 or 2 
#endif

  volatile float jack_pulse[1024];


  lives_audio_buf_t **abufs;
  volatile gint read_abuf;

} jack_driver_t;


#define JACK_MAX_OUTDEVICES 10
#define JACK_MAX_INDEVICES 10


////////////////////////////////////////////////////////////////////////////

jack_driver_t *jack_get_driver(gint dev_idx, gboolean is_output); // get driver

int jack_audio_init(void); // init jack for host output
int jack_audio_read_init(void); // init jack for host input

int jack_open_device(jack_driver_t *jackd); // open device for host output
int jack_open_device_read(jack_driver_t *jackd); // open device for host input

int jack_driver_activate (jack_driver_t *jackd); // activate for host playback
int jack_read_driver_activate (jack_driver_t *jackd); // activate for host recording

void jack_close_device(jack_driver_t* jackd, gboolean close_client);


// utils
volatile jack_message_t *jack_get_msgq(jack_driver_t *jackd); // pull last msg from msgq, or return NULL
gint64 lives_jack_get_time(jack_driver_t *jackd); // get time from jack, in 10^-8 seconds
void audio_seek_frame (jack_driver_t *jackd, gint frame); // seek to (video) frame
long audio_seek_bytes (jack_driver_t *jackd, long bytes); // seek to byte position

void get_rec_avals(jack_driver_t *jackd);

#endif


#endif
