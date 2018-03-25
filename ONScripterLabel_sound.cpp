/* -*- C++ -*-
 *
 *  ONScripterLabel_sound.cpp - Methods for playing sound for ONScripter-EN
 *
 *  Copyright (c) 2001-2011 Ogapee. All rights reserved.
 *  (original ONScripter, of which this is a fork).
 *
 *  ogapee@aqua.dti2.ne.jp
 *
 *  Copyright (c) 2008-2011 "Uncle" Mion Sonozaki
 *
 *  UncleMion@gmail.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>
 *  or write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Modified by Haeleth, Autumn 2006, to better support OS X/Linux packaging.

// Modified by Mion, April 2009, to update from
// Ogapee's 20090331 release source code.

// Modified by Mion, November 2009, to update from
// Ogapee's 20091115 release source code.

#ifdef _MSC_VER
#pragma warning(disable:4244)
#pragma warning(disable:4717)
#endif

#include "ONScripterLabel.h"
#include <new>
#ifdef LINUX
#include <signal.h>
#endif

#ifdef USE_AVIFILE
#include "AVIWrapper.h"
#endif

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

struct WAVE_HEADER{
    char chunk_riff[4];
    char riff_length[4];
    char fmt_id[8];
    char fmt_size[4];
    char data_fmt[2];
    char channels[2];
    char frequency[4];
    char byte_size[4];
    char sample_byte_size[2];
    char sample_bit_size[2];

    char chunk_id[4];
    char data_length[4];
} header;

static inline void clearTimer(SDL_TimerID &timer_id)
{
    clearTimer( timer_id );
}

extern bool ext_music_play_once_flag;

extern "C"{
    extern void mp3callback( void *userdata, Uint8 *stream, int len );
    extern void oggcallback( void *userdata, Uint8 *stream, int len );
    extern Uint32 SDLCALL cdaudioCallback( Uint32 interval, void *param );
    extern Uint32 SDLCALL silentmovieCallback( Uint32 interval, void *param );
#if defined(MACOSX) //insani
    extern Uint32 SDLCALL seqmusicSDLCallback( Uint32 interval, void *param );
#endif
}
extern void seqmusicCallback( int sig );
extern void musicCallback( int sig );
extern SDL_TimerID timer_cdaudio_id;
extern SDL_TimerID timer_silentmovie_id;

#if defined(MACOSX) //insani
extern SDL_TimerID timer_seqmusic_id;
#endif

#define TMP_SEQMUSIC_FILE "tmp.mus"
#define TMP_MUSIC_FILE "tmp.mus"

#define SWAP_SHORT_BYTES(sptr){          \
            Uint8 *bptr = (Uint8 *)sptr; \
            Uint8 tmpb = *bptr;          \
            *bptr = *(bptr+1);           \
            *(bptr+1) = tmpb;            \
        }

//WMA header format
#define IS_ASF_HDR(buf)                           \
         ((buf[0] == 0x30) && (buf[1] == 0x26) && \
          (buf[2] == 0xb2) && (buf[3] == 0x75) && \
          (buf[4] == 0x8e) && (buf[5] == 0x66) && \
          (buf[6] == 0xcf) && (buf[7] == 0x11))

//AVI header format
#define IS_AVI_HDR(buf)                         \
         ((buf[0] == 'R') && (buf[1] == 'I') && \
          (buf[2] == 'F') && (buf[3] == 'F') && \
          (buf[8] == 'A') && (buf[9] == 'V') && \
          (buf[10] == 'I'))

extern long decodeOggVorbis(ONScripterLabel::MusicStruct *music_struct, Uint8 *buf_dst, long len, bool do_rate_conversion)
{
	long total_len = 0;

    return total_len;
}

int ONScripterLabel::playSound(const char *filename, int format, bool loop_flag, int channel)
{
    if ( !audio_open_flag ) return SOUND_NONE;

    long length = script_h.cBR->getFileLength( filename );
    if (length == 0) return SOUND_NONE;

    //Mion: account for mode_wave_demo setting
    //(i.e. if not set, then don't play non-bgm wave/ogg during skip mode)
    if (!mode_wave_demo_flag &&
        ( (skip_mode & SKIP_NORMAL) || ctrl_pressed_status )) {
        if ((format & (SOUND_OGG | SOUND_WAVE)) &&
            ((channel < ONS_MIX_CHANNELS) || (channel == MIX_WAVE_CHANNEL) ||
             (channel == MIX_CLICKVOICE_CHANNEL)))
            return SOUND_NONE;
    }

    unsigned char *buffer;

    if ((format & (SOUND_MP3 | SOUND_OGG_STREAMING)) && 
        (length == music_buffer_length) &&
        music_buffer ){
        buffer = music_buffer;
    }
    else{
        buffer = new(std::nothrow) unsigned char[length];
        if (buffer == NULL) {
            snprintf(script_h.errbuf, MAX_ERRBUF_LEN,
                     "failed to load sound file [%s] (%lu bytes)",
                     filename, length);
            errorAndCont( script_h.errbuf, "unable to allocate buffer", "Memory Issue" );
            return SOUND_NONE;
        }
        script_h.cBR->getFile( filename, buffer );
    }

    if (format & (SOUND_OGG | SOUND_OGG_STREAMING)){
        int ret = playOGG(format, buffer, length, loop_flag, channel);
        if (ret & (SOUND_OGG | SOUND_OGG_STREAMING)) return ret;
    }

    /* check for WMA (i.e. ASF header format) */
    if ( IS_ASF_HDR(buffer) ){
        snprintf(script_h.errbuf, MAX_ERRBUF_LEN,
        "sound file '%s' is in WMA format, skipping", filename);
        errorAndCont(script_h.errbuf);
        delete[] buffer;
        return SOUND_OTHER;
    }

    /* check for AVI header format */
    if ( IS_AVI_HDR(buffer) ){
        snprintf(script_h.errbuf, MAX_ERRBUF_LEN,
        "sound file '%s' is in AVI format, skipping", filename);
        errorAndCont(script_h.errbuf);
        delete[] buffer;
        return SOUND_OTHER;
    }

    if (format & SOUND_WAVE){
        if (strncmp((char*) buffer, "RIFF", 4) != 0) {
            // bad (encrypted?) header; need to recreate
            // assume the first 64 bytes are bad (encrypted)
            int channels, rate, bits;
            char *fmtname = new char[strlen(filename) + strlen(".fmt") + 1];
            sprintf(fmtname, "%s.fmt", filename);
            unsigned int fmtlen = script_h.cBR->getFileLength( fmtname );
            if ( fmtlen > 0) {
                // a file called filename + ".fmt" exists; read fmt info
                unsigned char *buffer2 = new unsigned char[fmtlen];
                script_h.cBR->getFile( fmtname, buffer2 );
                channels = buffer2[0];
                rate = 0;
                for (int i=5; i>1; i--) {
                    rate = (rate << 8) + buffer2[i];
                }
                bits = buffer2[6];
                delete[] buffer2;
#ifdef _MSC_VER
				{
#endif
                for (int i=0; i<64; i++) {
                    buffer[i] = 0;
                }
#ifdef _MSC_VER
				}
#endif
                setupWaveHeader(buffer, channels, rate, bits,
                    length - sizeof(WAVE_HEADER));
            }
            delete[] fmtname;
        }
    }

    if (format & SOUND_MP3){
        if (music_cmd){
            FILE *fp;
            if ( (fp = fopen(TMP_MUSIC_FILE, "wb", true)) == NULL){
                snprintf(script_h.errbuf, MAX_ERRBUF_LEN,
                         "can't open temporary music file %s", TMP_MUSIC_FILE);
                errorAndCont(script_h.errbuf);
            }
            else{
                if (fwrite(buffer, 1, length, fp) != (size_t)length){
                    snprintf(script_h.errbuf, MAX_ERRBUF_LEN,
                             "can't write to temporary music file %s", TMP_MUSIC_FILE);
                    errorAndCont(script_h.errbuf);
                }
                fclose( fp );
                ext_music_play_once_flag = !loop_flag;
                if (playExternalMusic(loop_flag) == 0){
                    music_buffer = buffer;
                    music_buffer_length = length;
                    return SOUND_MP3;
                }
            }
        }
    }

    if (format & SOUND_SEQMUSIC){
        FILE *fp;
        if ( (fp = fopen(TMP_SEQMUSIC_FILE, "wb", true)) == NULL){
            snprintf(script_h.errbuf, MAX_ERRBUF_LEN,
                     "can't open temporary music file %s", TMP_SEQMUSIC_FILE);
            errorAndCont(script_h.errbuf);
        }
        else{
            if (fwrite(buffer, 1, length, fp) != (size_t)length){
                snprintf(script_h.errbuf, MAX_ERRBUF_LEN,
                         "can't write to temporary music file %s",
                         TMP_SEQMUSIC_FILE);
                errorAndCont(script_h.errbuf);
            }
            fclose( fp );
            ext_music_play_once_flag = !loop_flag;
            if (playSequencedMusic(loop_flag) == 0){
                delete[] buffer;
                return SOUND_SEQMUSIC;
            }
        }
    }

    delete[] buffer;

    return SOUND_OTHER;
}

void ONScripterLabel::playCDAudio()
{
    if (!audio_open_flag) return;

    if ( cdaudio_flag ){
        if ( cdrom_info ){
            int length = cdrom_info->track[current_cd_track - 1].length / 75;
            SDL_CDPlayTracks( cdrom_info, current_cd_track - 1, 0, 1, 0 );
            timer_cdaudio_id = SDL_AddTimer( length * 1000, cdaudioCallback, NULL );
        }
    }
    else{
        //if CD audio is not available, search the "cd" subfolder
        //for a file named "track01.mp3" or similar, depending on the
        //track number; check for mp3, ogg and wav files
        char filename[256];
        sprintf( filename, "cd\\track%2.2d.mp3", current_cd_track );
        int ret = playSound( filename, SOUND_MP3, cd_play_loop_flag );
        if (ret == SOUND_MP3) return;

        sprintf( filename, "cd\\track%2.2d.ogg", current_cd_track );
        ret = playSound( filename, SOUND_OGG_STREAMING, cd_play_loop_flag );
        if (ret == SOUND_OGG_STREAMING) return;

        sprintf( filename, "cd\\track%2.2d.wav", current_cd_track );
        ret = playSound( filename, SOUND_WAVE, cd_play_loop_flag, MIX_BGM_CHANNEL );
    }
}

int ONScripterLabel::playMP3()
{
    return 0;
}

int ONScripterLabel::playOGG(int format, unsigned char *buffer, long length, bool loop_flag, int channel)
{
	return SOUND_OTHER;
}

int ONScripterLabel::playExternalMusic(bool loop_flag)
{
    int music_looping = loop_flag ? -1 : 0;
#ifdef LINUX
    signal(SIGCHLD, musicCallback);
    if (music_cmd) music_looping = 0;
#endif

    char music_filename[256];
    sprintf(music_filename, "%s%s", script_h.save_path, TMP_MUSIC_FILE);

    // Mix_VolumeMusic( music_volume );

    return 0;
}

int ONScripterLabel::playSequencedMusic(bool loop_flag)
{
    char seqmusic_filename[256];
    sprintf(seqmusic_filename, "%s%s", script_h.save_path, TMP_SEQMUSIC_FILE);
    int seqmusic_looping = loop_flag ? -1 : 0;

#ifdef LINUX
    signal(SIGCHLD, seqmusicCallback);
    if (seqmusic_cmd) seqmusic_looping = 0;
#endif

#if defined(MACOSX) //insani
    // Emulate looping on MacOS ourselves to work around bug in SDL_Mixer
    seqmusic_looping = 0;
    timer_seqmusic_id = SDL_AddTimer(1000, seqmusicSDLCallback, NULL);
#else
#endif
    current_cd_track = -2;

    return 0;
}

int ONScripterLabel::playingMusic()
{
	return 0;
}

int ONScripterLabel::setCurMusicVolume( int volume )
{
    if (!audio_open_flag) return 0;

    return 0;
}

int ONScripterLabel::setVolumeMute( bool do_mute )
{
    if (!audio_open_flag) return 0;

    int music_vol = music_volume;

    return 0;
}

int ONScripterLabel::playMPEG( const char *filename, bool async_flag, bool use_pos, int xpos, int ypos, int width, int height )
{
    int ret = 0;

    return ret;
}

int ONScripterLabel::playAVI( const char *filename, bool click_flag )
{
#ifdef USE_AVIFILE
    char *absolute_filename = new char[ strlen(archive_path) + strlen(filename) + 1 ];
    sprintf( absolute_filename, "%s%s", archive_path, filename );
    for ( unsigned int i=0 ; i<strlen( absolute_filename ) ; i++ )
        if ( absolute_filename[i] == '/' ||
             absolute_filename[i] == '\\' )
            absolute_filename[i] = DELIMITER;

    AVIWrapper *avi = new AVIWrapper();
    if ( avi->init( absolute_filename, false ) == 0 &&
         avi->initAV( screen_surface, audio_open_flag ) == 0 ){
        if (avi->play( click_flag )) return 1;
    }
    delete avi;
    delete[] absolute_filename;

    if ( audio_open_flag ){
    }
#else
    errorAndCont( "avi: avi video playback is disabled." );
#endif

    return 0;
}

void ONScripterLabel::stopBGM( bool continue_flag )
{
    if ( cdaudio_flag && cdrom_info ){
        extern SDL_TimerID timer_cdaudio_id;

        clearTimer( timer_cdaudio_id );
        if (SDL_CDStatus( cdrom_info ) >= CD_PLAYING )
            SDL_CDStop( cdrom_info );
    }

    if ( !continue_flag ){
        setStr( &music_file_name, NULL );
        music_play_loop_flag = false;
        if ( music_buffer ){
            delete[] music_buffer;
            music_buffer = NULL;
        }
    }

    if ( !continue_flag ){
        setStr( &seqmusic_file_name, NULL );
        seqmusic_play_loop_flag = false;
    }

    if ( !continue_flag ) current_cd_track = -1;
}

void ONScripterLabel::stopAllDWAVE()
{
    if (!audio_open_flag) return;

    // just in case the bgm was turned down for the voice channel,
    // set the bgm volume back to normal
    if (bgmdownmode_flag)
        setCurMusicVolume( music_volume );
}

void ONScripterLabel::playClickVoice()
{
    if      ( clickstr_state == CLICK_NEWPAGE ){
        if ( clickvoice_file_name[CLICKVOICE_NEWPAGE] )
            playSound(clickvoice_file_name[CLICKVOICE_NEWPAGE],
                      SOUND_WAVE|SOUND_OGG, false, MIX_CLICKVOICE_CHANNEL);
    }
    else if ( clickstr_state == CLICK_WAIT ){
        if ( clickvoice_file_name[CLICKVOICE_NORMAL] )
            playSound(clickvoice_file_name[CLICKVOICE_NORMAL],
                      SOUND_WAVE|SOUND_OGG, false, MIX_CLICKVOICE_CHANNEL);
    }
}

void ONScripterLabel::setupWaveHeader( unsigned char *buffer, int channels, int rate, int bits, unsigned long data_length )
{
    memcpy( header.chunk_riff, "RIFF", 4 );
    int riff_length = sizeof(WAVE_HEADER) + data_length - 8;
    header.riff_length[0] = riff_length & 0xff;
    header.riff_length[1] = (riff_length >> 8) & 0xff;
    header.riff_length[2] = (riff_length >> 16) & 0xff;
    header.riff_length[3] = (riff_length >> 24) & 0xff;
    memcpy( header.fmt_id, "WAVEfmt ", 8 );
    header.fmt_size[0] = 0x10;
    header.fmt_size[1] = header.fmt_size[2] = header.fmt_size[3] = 0;
    header.data_fmt[0] = 1; header.data_fmt[1] = 0; // PCM format
    header.channels[0] = channels; header.channels[1] = 0;
    header.frequency[0] = rate & 0xff;
    header.frequency[1] = (rate >> 8) & 0xff;
    header.frequency[2] = (rate >> 16) & 0xff;
    header.frequency[3] = (rate >> 24) & 0xff;

    int sample_byte_size = channels * bits / 8;
    int byte_size = sample_byte_size * rate;
    header.byte_size[0] = byte_size & 0xff;
    header.byte_size[1] = (byte_size >> 8) & 0xff;
    header.byte_size[2] = (byte_size >> 16) & 0xff;
    header.byte_size[3] = (byte_size >> 24) & 0xff;
    header.sample_byte_size[0] = sample_byte_size;
    header.sample_byte_size[1] = 0;
    header.sample_bit_size[0] = bits;
    header.sample_bit_size[1] = 0;

    memcpy( header.chunk_id, "data", 4 );
    header.data_length[0] = (char)(data_length & 0xff);
    header.data_length[1] = (char)((data_length >> 8) & 0xff);
    header.data_length[2] = (char)((data_length >> 16) & 0xff);
    header.data_length[3] = (char)((data_length >> 24) & 0xff);

    memcpy( buffer, &header, sizeof(header) );
}



OVInfo *ONScripterLabel::openOggVorbis( unsigned char *buf, long len, int &channels, int &rate )
{
    OVInfo *ovi = NULL;

    return ovi;
}

int ONScripterLabel::closeOggVorbis(OVInfo *ovi)
{
    if (ovi->buf){
        ovi->buf = NULL;
    }
    if (ovi->cvt.buf){
        delete[] ovi->cvt.buf;
        ovi->cvt.buf = NULL;
        ovi->cvt_len = 0;
    }
    delete ovi;

    return 0;
}
