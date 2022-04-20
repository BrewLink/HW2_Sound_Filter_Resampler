//
//  main.c
//  readWavFile
//
//  Created by ehsan on 4/19/22.
//

#include <stdio.h>
#include <sndfile.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "bandpass_filter_coeff.h"

// Defines -----------------------------------------------------------------
#define PI                     (3.14)
#define SAMPLE_RATE            (48000)          // Sample rate           48kHz
#define LENGTH                 (1.0)            // Sound length          (1.0) Second
#define AMPLITUDE_PART1        (0.25)           // Sound Amplitude       (1/4) amp
#define AMPLITUDE_PART2        (0.50)           // Sound Amplitude       (1/2) amp
#define CLIPPING_POINT_PART1   (1.0)            // clip on               100%  (no clipping)
#define CLIPPING_POINT_PART2   (0.25)           // clip on               25.0% (inclusive)
#define FREQUENCY              (440)            // Sound frequency       440Hz
// Portaudio defines
#define  WAVE_LENGTH           (SAMPLE_RATE*1)
#define  MSG_SIZE              (20)
#define FRAMES_PER_BUFFER      (64)
#define NUM_SECONDS            (1)
// Audio Player (REKO) States
#define REKO_OK                (0)
#define REKO_FAILED_INIT       (-1)
#define REKO_FAILED_GENERAL    (-2)
#define REKO_FAILED_NO_DEVICE  (-3)

// Argument management defines
#define FINE_NAME_SIZE 74

static char fn_buf[FINE_NAME_SIZE];
const char * path_to_wav;
static const char path_sine_wav[]    = "./gc.wav"; // Default path
static const char * path_r_wav;
static void mod_file_name(const char in[], char  *const fn_buf, int size);
static void test_via_buffer_dump(bool, long long, short*);
static short * half_resampler(short *buf, long long size);
static void bandPassFilter(short*, long long size);


int main(int argc, const char * argv[]) {
    
    // Check for the input arguments:
    printf("Number of argumetns: %d \n", argc);
    if(argc > 2)
    {
        printf("Error! Number of comand-line arguments must be 1 \n");
        printf("Please provide exact full path to the wav file. Ex. /home/tmp/mywav.wav \n");
        exit(2);
    }
    else
    {
        if(argc ==2)
        {
            printf("Input wav file: %s \n", argv[1]);
            path_to_wav = argv[1];
            int size =  (int)strlen(argv[1]);
            
            // Mod the name according to the input
            mod_file_name( path_to_wav,fn_buf,size);
            path_r_wav = fn_buf;
            
        }
        else
        {
            printf("Error! No arguments. Allowing exception - Using defaults values... \n");
            path_to_wav =path_sine_wav;
        }
    }

    // Read Wav file
    
    // Local Vars
    SF_INFO sf_info;
    SNDFILE *fp;
    
    // Open the wav file
    fp = sf_open( path_to_wav, SFM_READ, &sf_info);
    if(fp)
        printf(" File read: %s \nNumber of frames: %lld \n",  path_to_wav, sf_info.frames);
    
    // Read the wav file into a local buffer
    short * intbuf_ptr = (short*)(malloc(sf_info.frames*sizeof(short)));
    if(sf_read_short(fp, intbuf_ptr,   sf_info.frames) != sf_info.frames)
       puts(sf_strerror (fp));
    sf_close(fp);
       
    // Test the read file
    test_via_buffer_dump(false, sf_info.frames, intbuf_ptr);
    
    // Downsample // TDOD auto resize
    bandPassFilter(intbuf_ptr, sf_info.frames);
    intbuf_ptr = half_resampler(intbuf_ptr, sf_info.frames);
    
    
    // Write to a file
    // Local auto variablesz
    SF_INFO sfinfo;
    
    sfinfo.samplerate = (sf_info.samplerate/2);
    sfinfo.frames     =  (sf_info.frames/2);
    sfinfo.channels   =  1;
    sfinfo.format     =  (SF_FORMAT_WAV | SF_FORMAT_PCM_16);
    
    if(!(fp = sf_open(path_r_wav, SFM_WRITE, &sfinfo))) // TODO
        return ( (void)(printf("Error : Couldn't open - .wav \n")), -1 );
    
    if(sf_write_short(fp, intbuf_ptr, sfinfo.channels * (sf_info.frames/2)) != sfinfo.channels * sf_info.frames/2)
        puts(sf_strerror (fp));
    sf_close(fp); // clipped.wav
    
    return 0;
}



// Helper functions
static void test_via_buffer_dump(bool enable, long long num_frames, short * buf)
{
    if(enable == true)
        // Test function
        for(long long i = 0; i < num_frames; i++)
            printf("i=%lld data=%d \n", i, buf[i]);
}


// ----------  Resampler Helper funciton ----------------------------------------------------------
static short * half_resampler(short * buf, long long size)
{
    short * buf_new = (short*) malloc( (size/2)*sizeof(short) );
    for(int i = 0; i < (size/2); i++)
    {
        buf_new[i] = buf[i*2];
    }
    
    free(buf);
    return buf_new;
}


// Bandpass Filter with given coffieents ----------------------------------------------------------
static void bandPassFilter(short * buf, long long size)
{
    // Make a VLA Buffer on stack
    // This allows for easier buffer handling and debugging
    short ta[size+ NUMBER_OF_COEFFS];

    
    for(register int i = 0; i < size+NUMBER_OF_COEFFS; i++)
    {
        if( i < NUMBER_OF_COEFFS)
            ta[i] = 0;
        else
            ta[i] = buf[i-91];
    }
    
    
    for(register int i = 91; i < size+NUMBER_OF_COEFFS; i++)
    {
        double y = 0;
        for(int j = 0; j< (sizeof(coeffs)/sizeof(double)); j++)
        {
            y += coeffs[j]*(ta[i-j]);
        }
        //printf("i: %d y: %f buf=%d ta=%d \n",i,  y, buf[i], ta[i]);
        ta[i-NUMBER_OF_COEFFS] = (short)(y);
    }
    
    for(register int i = 0; i < size; i++)
    {
        buf[i] = ta[i];
        //printf("i: %dbuf=%d ta=%d \n",i,  buf[i], ta[i]);
    }
    
    
}

// Helpfer function to modify the input name
static void mod_file_name(const char in[], char *const fn_buf, int size)
{
    
    int in_size = size - 1;
    char cur =   in[in_size];
    int size_of_name = 0;
    int oversize =FINE_NAME_SIZE;
    
    
    if(size > FINE_NAME_SIZE)
    {
        printf("Buffer is too small for argument input \n");
        exit(4);
    }
    
    while( cur != '/' && oversize)
    {
        size_of_name++;
        in_size--;
        cur =   in[in_size];
        --oversize;
    }
    
    if(oversize <= 0)
    {
        printf("Fine name is too big or incorrect! Reason: \'\\\' not found! \n");
        exit(3);
    }
    
    
    bool added_r = false;
    
    for(int i = 0; i < size+1; i++)
    {
        
        if(i != in_size+1)
        {
            if(added_r == true)
                fn_buf[i] = in[i-1];
            else
                fn_buf[i] = in[i];
        }
        else
        {
            fn_buf[i] = 'r';
            added_r = true;
        }
    }
}

