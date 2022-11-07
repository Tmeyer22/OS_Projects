#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pzip.h"

/**
 * pzip() - zip an array of characters in parallel
 *
 * Inputs:
 * @n_threads:		   The number of threads to use in pzip
 * @input_chars:		   The input characters (a-z) to be zipped
 * @input_chars_size:	   The number of characaters in the input file
 *
 * Outputs:
 * @zipped_chars:       The array of zipped_char structs
 * @zipped_chars_count:   The total count of inserted elements into the zippedChars array.
 * @char_frequency[26]: Total number of occurences
 *
 * NOTE: All outputs are already allocated. DO NOT MALLOC or REASSIGN THEM !!!
 *
 */

static int portionLength;

pthread_barrier_t barrier;

void *counter(void *tData);
void *counter(void *tData)
{
	char *portion;
	portion = (char *)tData;

	fprintf(stderr, "adjusted array: ");
	for (size_t i = 0; i < portionLength; i++) {
		fprintf(stderr, "%c", portion[i]);
	}
	fprintf(stderr, "\n");

	//after all local things have been calculated
	pthread_barrier_wait(&barrier);

	return portion;
}

void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
	portionLength = input_chars_size / n_threads;

	pthread_barrier_init(&barrier, NULL, n_threads + 1);

	for (int i = 0; i < n_threads; i++) {
		char *temp = input_chars + (i * portionLength);

		pthread_t tid;

		pthread_create(&tid, NULL, counter, (void *)temp);
	}
	pthread_barrier_wait(&barrier);

	pthread_barrier_destroy(&barrier);

	exit(1);
}
