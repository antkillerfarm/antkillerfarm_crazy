#include <stdio.h>
#include "aubio/aubio.h"

FILE *fout, *fout0, *fout1;
const char_t *hdr = "";
void logging(int level, const char_t *message, void *data)
{
  if ((level >= 0) && (data != NULL))
  {
    fprintf(fout, "%s", (const char_t *)data);
  }
  fprintf(fout, "%s", message);
}

int main (void)
{
  uint_t samplerate = 0;
  uint_t hop_size = 160;
  uint_t n_frames = 0, read = 0;
  uint_t win_s = 320;

  aubio_source_t* s = new_aubio_source("../1188-133604-0025.flac.wav",
    samplerate, hop_size);
  fvec_t * vec = new_fvec(hop_size);
  cvec_t * fftgrain = new_cvec (win_s); // fft norm and phase
  fvec_t * out = new_fvec (hop_size + 1); // output buffer

  // allocate fft and other memory space
  aubio_pvoc_t * pv = new_aubio_pvoc(win_s, hop_size);

  //set log fun
  fout0 = fopen("log_0.txt","w");
  fout1 = fopen("log_1.txt","w");
  aubio_log_set_function(logging, (void *)hdr);


  if (samplerate == 0)
  {
    samplerate = aubio_source_get_samplerate(s);
    printf("samplerate:%d\n", samplerate);
  }

  do
  {
    aubio_source_do(s, vec, &read);
    fout = fout0;
    fvec_print (vec);

    aubio_pvoc_do (pv, vec, fftgrain);
    fout = fout1;
    cvec_print (fftgrain);

    n_frames += read;
  } while ( read == hop_size );

  // clean up
  del_cvec(fftgrain);
  del_fvec(out);
  del_fvec (vec);
  del_aubio_pvoc(pv);
  del_aubio_source (s);
  fclose(fout0);
  fclose(fout1);

  return 0;
}
