# include "Field.h"
  
Field::Field(int st) {
	state = st;
} 

void Field::set_state(int st) {
	state = st;
}

int Field::get_state() {
	return state;
}