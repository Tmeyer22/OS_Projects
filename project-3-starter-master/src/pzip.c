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

//global vars for threads
static int portionLength;
pthread_barrier_t barrier;
struct zipped_char *glob_zipchars;
int *num_structsMade;
pthread_t *globTids;
int numThreads;
pthread_mutex_t *mutexArr;
int *global_charFreq;
int *global_zipChars_Count;
pthread_mutex_t zipChar_Count_Mutex;

void *counter(void *tData);
void *counter(void *tData)
{
	char *portion;
	portion = (char *)tData;

	struct zipped_char *charStructArr =
		calloc(portionLength, sizeof(struct zipped_char));

	char tempChar = portion[0];
	int currentStructPlace = 0;
	charStructArr[currentStructPlace].character = portion[0];

	for (int i = 0; i < portionLength; i++) {
		if (tempChar == portion[i]) {
			charStructArr[currentStructPlace].occurence++;
		} else {
			tempChar = portion[i];
			currentStructPlace++;
			charStructArr[currentStructPlace].character =
				portion[i];
			charStructArr[currentStructPlace].occurence++;
		}
	}

	//after all local things have been calculated
	int currentTid = 0;

	for (int i = 0; i < numThreads; i++) {
		if (pthread_self() == globTids[i]) {
			num_structsMade[i] = currentStructPlace + 1;
			currentTid = i;
			pthread_mutex_lock(&zipChar_Count_Mutex);
			*global_zipChars_Count += num_structsMade[i];
			pthread_mutex_unlock(&zipChar_Count_Mutex);
			break;
		}
	}

	pthread_barrier_wait(&barrier);

	int zipCharsArrayStart = 0;
	for (int i = 0; i < currentTid; i++) {
		zipCharsArrayStart += num_structsMade[i];
	}

	for (int i = zipCharsArrayStart;
	     i < zipCharsArrayStart + currentStructPlace + 1; i++) {
		glob_zipchars[i].character =
			charStructArr[i - zipCharsArrayStart].character;
		glob_zipchars[i].occurence =
			charStructArr[i - zipCharsArrayStart].occurence;

		pthread_mutex_lock(&mutexArr[glob_zipchars[i].character - 97]);
		global_charFreq[glob_zipchars[i].character - 97] +=
			glob_zipchars[i].occurence;
		pthread_mutex_unlock(
			&mutexArr[glob_zipchars[i].character - 97]);
	}
	free(charStructArr);

	return portion;
}

void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
	//initialize global variables
	portionLength = input_chars_size / n_threads;
	glob_zipchars = zipped_chars;
	global_charFreq = char_frequency;
	global_zipChars_Count = zipped_chars_count;

	//initialize mutex array
	pthread_mutex_t tempMutexArr[26];
	mutexArr = tempMutexArr;
	for (int i = 0; i < 26; i++) {
		pthread_mutex_init(&mutexArr[i], NULL);
	}
	pthread_mutex_init(&zipChar_Count_Mutex, NULL);

	num_structsMade = calloc(input_chars_size, sizeof(int));
	numThreads = n_threads;

	pthread_barrier_init(&barrier, NULL, n_threads + 1);
	pthread_t tids[n_threads];
	globTids = tids;

	for (int i = 0; i < n_threads; i++) {
		char *temp = input_chars + (i * portionLength);
		pthread_create(&tids[i], NULL, counter, (void *)temp);
	}

	pthread_barrier_wait(&barrier);
	pthread_barrier_destroy(&barrier);

	for (int i = 0; i < 26; i++) {
		pthread_mutex_destroy(&mutexArr[i]);
	}
	pthread_mutex_destroy(&zipChar_Count_Mutex);

	for (int i = 0; i < n_threads; i++) {
		if (pthread_join(tids[i], NULL)) {
			perror("threadJoin failed");
			exit(-1);
		}
	}

	free(num_structsMade);
}
