#include <stdlib.h>
#include <stdio.h>

struct queue
{
    int* items;
    int capacity;
};


struct reno
{
    struct queue* rtts;
};

struct reno* newQueue(int capacity) {
    struct reno *q = malloc(sizeof(struct reno));
    q->rtts = malloc(sizeof(struct queue));
    q->rtts->items = malloc(capacity * sizeof(int));

    q->rtts->capacity = 5;

    return q;
}


int main() {
    
    // struct reno* ca = newQueue(10);

    // printf("cap: %d\n", ca->rtts->capacity);

    int a[] = {1,2,3,4,5};

    int sum = 0;
    int index = 0;
	//int curr_rtt = ca->rtts[ca->front];
	int prev_avg;

    int i = 0;
    while (i < 5-1) {
        sum += a[index];
        index++;
        //if (index < 0) index = ca->size-1;
		i++;
    }

	prev_avg = sum / (5-1);
    printf("SUM: %d\nPREV_AVG: %d\n", sum, prev_avg);

    printf("WEIGHTED: %d\n", prev_avg * 80 / 100 + 5 * 20 / 100);

    return(0);
}