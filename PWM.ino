const int ledPin = 7;

int pwm_period = 0;
int pwm_duty_percent = 0;

void setup(){
  pinMode(ledPin, OUTPUT);
  set_period(10); 
}

void loop(){
  for(int duty = 0; duty <= 100; duty++){
    set_duty(duty);
    run_pwm_cycle();
  }

  for(int duty = 100; duty >= 0; duty--){
    set_duty(duty);
    run_pwm_cycle();
  }
}

void set_period(int period){
  pwm_period = period;
}

void set_duty(int duty){
  pwm_duty_percent = duty;
}

void run_pwm_cycle(){
  long hightime = (long)pwm_period * pwm_duty_percent / 100;
  long lowtime = pwm_period - hightime;
  
  digitalWrite(ledPin, HIGH);
  delayMicroseconds(hightime);
  digitalWrite(ledPin, LOW);
  delayMicroseconds(lowtime);
}
