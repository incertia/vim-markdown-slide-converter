#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRESENT_STRING  "_final.md"
#define CLEAN_STRING    "_clean.md"
#define INIT_SIZE       0x10

typedef struct slideinfo_t {
  size_t n;
  char *s;
  char m;
} slideinfo_t;

char *xbasename(const char *fname){
  char *bn = strdup(fname);
  size_t l = strlen(bn);

  while(l--) if(bn[l] == '.') bn[l] = '\0';

  return bn;
}

static void generateslides(FILE *f, char **lb, size_t n);

int main(int argc, char **argv){
  int i = 1;

  /* check arguments */
  if(argc <= 1){
    fprintf(stderr, "usage: %s [slides.md]\n", argv[0]);
    return 1;
  }

  for(; i < argc; i++){
    char *slidesfile = argv[i];
    char *xbn = xbasename(slidesfile);
    char *slidesoutput = malloc(strlen(xbn) + strlen(PRESENT_STRING) + 1);
    char *slidesclean = malloc(strlen(xbn) + strlen(CLEAN_STRING) + 1);
    FILE *f;
    char **linebuf = NULL;
    size_t bsize = 0, bmax = 0, _;
    ssize_t bytes_read = 0;

    *slidesoutput = *slidesclean = '\0';

    /* generate output fnames */
    strcat(slidesoutput, xbn);
    strcat(slidesoutput, PRESENT_STRING);
    strcat(slidesclean , xbn);
    strcat(slidesclean, CLEAN_STRING);

    /* read infile */
    f = fopen(slidesfile, "r");
    if (!f) {
      fprintf(stderr, "could not open: %s\n", slidesfile);
      return 1;
    }
    while(1) {
      /* resize linebuf */
      if(bsize == bmax){
        if(bmax == 0) bmax = INIT_SIZE;
        else bmax *= 2;

        linebuf = realloc(linebuf, bmax * sizeof(char *));
      }

      linebuf[bsize] = NULL;
      _ = 0;
      bytes_read = getline(&linebuf[bsize], &_, f);
      if(bytes_read < 0) break;
      bsize++;
    }
    fclose(f);

    /* write the slides */
    f = fopen(slidesoutput, "w");
    generateslides(f, linebuf, bsize);
    fclose(f);

    /* write clean version of slides */
    f = fopen(slidesclean, "w");
    for(_ = 0; _ < bsize; _++){
      char *l = linebuf[_];
      if(strlen(l) >= 4) fprintf(f, "%s", l[4] == '\0' ? "\n" : l + 4);
    }
    fclose(f);

    /* clean up linebuf */
    for(_ = 0; _ < bsize; _++) free(linebuf[_]);
    free(linebuf);

    free(slidesoutput);
    free(slidesclean);
    free(xbn);
  }

  return 0;
}

/* deal with complexity */
static void generateslide(FILE *f, slideinfo_t *sb, size_t n, size_t m){
  size_t i = 0;
  /* print current slides */
  for(i = 0; i <= m; i++){
    size_t j = 0;
    int *redact = malloc((m + 1) * sizeof(int));
    memset(redact, 0, (m + 1) * sizeof(int));

    /* go only up to the current slide number */
    int skip = 0;
    for(; j < n; j++){
      slideinfo_t s = sb[j];
      if(s.n <= i){
        if(s.m == '>' || s.n == i){
          while(skip--) fprintf(f, "\n");
          fprintf(f, "%s", s.s);
          skip = 0;
        } else if(s.m == ']' && !redact[s.n]){
          /* search for indent */
          size_t k = 0;
          for(; isspace(s.s[k]); k++) fprintf(f, "%c", s.s[k]);
          fprintf(f, "[redacted]\n"), redact[s.n] = 1;
        } else if(redact[s.n]){
          /* do nothing */
        } else {
          fprintf(stderr, "error: unexpected mode '%c'\n", s.m);
        }
      } else {
        // if(skip == 0) fprintf(f, "skip\n");
        skip++;
      }
    }
    free(redact);

    fprintf(f, "\n");
  }
}

/* further deal with complexity */
static void generateslides(FILE *f, char **lb, size_t n){
  size_t _, ssize = 0, smax = 0, m = 0;
  slideinfo_t *slidebuf = NULL;
  for(_ = 0; _ < n + 1; _++){
    if(_ < n && strlen(lb[_]) >= 4){
      char *l = lb[_];
      size_t n = (l[0] - '0') * 10 + (l[1] - '0');
      char *s = strdup(*(l + 4) == '\0' ? "\n" : l + 4);
      if(ssize == smax){
        if(smax == 0) smax = INIT_SIZE;
        else smax *= 2;
        slidebuf = realloc(slidebuf, smax * sizeof(slideinfo_t));
      }

      slidebuf[ssize].n = n;
      slidebuf[ssize].s = s;
      slidebuf[ssize].m = l[2];
      ssize++;

      /* set slide count */
      if(n > m) m = n;
    } else {
      size_t i = 0;
      /* print current slides */
      generateslide(f, slidebuf, ssize, m);

      /* reset slidebuf for next iteration */
      for(i = 0; i < ssize; i++) free(slidebuf[i].s);
      free(slidebuf);
      slidebuf = NULL;
      m = smax = ssize = 0;
    }
  }
}
