#include <stdbool.h>
#include <stddef.h>

#define NUM_FLOORS 10
#define DOOR_TIME = 2
#define MOVE_TIME = 3

typedef enum {
	IDLE,
	MOVING_UP,
	MOVING_DOWN,
	DOOR_OPEN
} ElevatorState;

typedef enum {
	INPUT_NONE,
	INPUT_CALL_BUTTON,
	INPUT_FLOOR_BUTTON,
	INPUT_DOOR_OPEN,
	INPUT_DOOR_CLOSE
} InputEvent;

typedef enum {
	OUTPUT_NOTHING,
	OUTPUT_OPEN_DOOR,
	OUTPUT_CLOSE_DOOR,
	OUTPUT_MOVING_UP,
	OUTPUT_MOVING_DOWN
} OutputEvent;

struct ElevatorData {
	int currentFloor;
	int targetFloor;
	ElevatorState state;
	bool requests[NUM_FLOORS];
	int timer;
	bool unhandledDoorOpen;
	bool unhandledDoorClose;
};

struct Elevator {
	ElevatorData* data;
};

OutputEvent run(struct Elevator* e, InputEvent input, int inputFloor) {
	OutputEvent toReturn = OUTPUT_NOTHING;
	handleInput(e->data, input, inputFloor);
	if(e->data->timer > 0) e->data->timer--;
	switch (e->data->state)
	{
	case IDLE:
		if(e->data->unhandledDoorOpen) {
			toReturn = OUTPUT_OPEN_DOOR;
			e->data->timer = DOOR_TIME;
			e->data->unhandledDoorOpen = false;
			break;
		}
		if(hasPendingRequests(e->data)) {
			int next = selectNextFloor(e->data);
			if(next == -1) break; // should never happen
			e->data->targetFloor = next;

			// if the request is on the floor we're on
			if(next == e->data->currentFloor) {
				// if no timer set, open the door
				if(e->data->timer == 0){
					toReturn = OUTPUT_OPEN_DOOR;
					e->data->state = DOOR_OPEN;
					e->data->timer = DOOR_TIME;
				} else {
					// if timer reaches zero, close the door
					toReturn = e->data->timer == 0 ? OUTPUT_CLOSE_DOOR : OUTPUT_NOTHING;
					e->data->state = IDLE;
					removeRequest(e->data, currentFloor);
				}
				
			} else if(next < e->data->currentFloor) {
				// start moving down
				toReturn = OUTPUT_MOVING_DOWN;
				e->data->targetFloor = next;
				e->data->timer = MOVE_TIME * (e->data->currentFloor - next);
			} else if(next > e->data->currentFloor) {
				// start moving up
				toReturn = OUTPUT_MOVING_UP;
				e->data->targetFloor = next;
				e->data->timer = MOVE_TIME * (next - e->data->currentFloor);
			}
		}
		break;

	case MOVING_UP:
	case MOVING_DOWN:
		break;

	case DOOR_OPEN:
		if(e->data->unhandledDoorClose || --e->data->timer <= 0) {
			toReturn = OUTPUT_CLOSE_DOOR;
			e->data->state = IDLE;
		}
	
	default:
		break;
	}
}



int main(void) {

}