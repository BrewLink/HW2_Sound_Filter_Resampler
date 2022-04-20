CC=GCC
LDLIBS= -lsndfile

halfrate: halfrate.c bandpass_filter_coeff.c

clean:
	rm halfrate 
