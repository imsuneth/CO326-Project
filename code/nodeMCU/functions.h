int vehicle_count[]={0,0};
int reedPins[] = {16, 5, 4, 0, 2, 14};
int prev_reading[4];
void pin_setup() {
  //initialize reed switches

  int i;
  for (i = 0; i < 6; i++) {
    pinMode(reedPins[i], INPUT);
  }
  for (i = 0; i < 4; i++) {
    prev_reading[i] = LOW;
  }


}

boolean hasVehicle(int i) {
  int reading = digitalRead(reedPins[i]);

  boolean ret = false;
  if (reading == HIGH) {
    if (prev_reading[i] == LOW) {
      ret = true;
    }
  }
  prev_reading[i] = reading;
  return ret;
}


//int reedPins[] = {16, 5, 4, 0, 2, 14};
void get_count() {
  int i = 0;
  while (i <= 2) {
      if (hasVehicle(i)) {
        vehicle_count[i] += 1;
      }
      if (hasVehicle(i + 1)) {
        vehicle_count[i] -= 1;
      }
      i=i+2;
  }
}
