
//Motor1(forward & backware) enable signal
#define D_PIN5_M1_EN  5

//Motor2(forward & backward) enable signal
#define D_PIN6_M2_EN  6

// forward(4) &  backward(7)      
#define D_PIN4_M1  4
#define D_PIN7_M1  7

// left(8) & right(10)
#define D_PIN8_M2  8
#define D_PIN10_M2  10

// Test Pin
#define D_PIN5 5
int order = 0;
int velocity = 0;
int32_t frequency = 33000;

// RC car default speed
int speed = 120;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (! Serial);

  // Arduino timer frequency change
  TCCR0B = TCCR0B & B11111000 | B00000001;
 
  // Function to determine to how to use the pin
  pinMode(D_PIN5_M1_EN, OUTPUT);
  pinMode(D_PIN6_M2_EN, OUTPUT);

  pinMode(D_PIN4_M1, OUTPUT);
  pinMode(D_PIN7_M1, OUTPUT);

  pinMode(D_PIN8_M2, OUTPUT);
  pinMode(D_PIN10_M2, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available())
  {
    order = Serial.read();
	
	// without Enter(\n)
    Serial.print("I reveived: ");
	// with Enter(\n)
    Serial.println(order, DEC);
   
	// 스마트폰에서 120에서 200 사이 값을 전달 받으면 그 값은 속도 기본 값 
    if((order > 119) && (order < 201))
    {
      Serial.print("velocity : ");
      Serial.println(order, DEC);
      velocity = order;
    }
	
	// 속도를 4 단계로 나눔 
    if (order || 97 || 98 || 99 || 100)
    {
      if (order == 97)
      {
        velocity = speed;
      }
      else if (order == 98)
      {
        velocity = speed + 15;
      }
      else if (order == 99)
      {
        velocity = speed + 30;
      }
      else if (order == 100)
      {
        velocity = speed + 50;
      }
      else {}
    }

    if (order == 49) //Go 1
    {
      //Serial.print("Order : ");
      //Serial.println(order, DEC);
      //analogWrite(D_PIN5_M1_EN, velocity);
      digitalWrite(D_PIN5_M1_EN, HIGH);
      digitalWrite(D_PIN4_M1, HIGH);
      digitalWrite(D_PIN7_M1, LOW);
    }
    else if (order == 50) // Back 2
    {
      //Serial.print("Order : ");
      //Serial.println(order, DEC);
      //analogWrite(D_PIN5_M1_EN, velocity);
      digitalWrite(D_PIN5_M1_EN, HIGH);
      digitalWrite(D_PIN4_M1, LOW);
      digitalWrite(D_PIN7_M1, HIGH);
    }
    else if (order == 56) // GBStop 8
    {
      //Serial.print("Order : ");
      //Serial.println(order, DEC);
      digitalWrite(D_PIN5_M1_EN, LOW);
      digitalWrite(D_PIN4_M1, LOW);
      digitalWrite(D_PIN7_M1, LOW);
    }
    else {}

    if (order == 51) // LLeft 3
    {
      //Serial.print("Order : ");
      //Serial.println(order, DEC);
      //analogWrite(D_PIN6_M2_EN, 130);
      digitalWrite(D_PIN6_M2_EN, HIGH);
      digitalWrite(D_PIN8_M2, LOW);
      digitalWrite(D_PIN10_M2, HIGH);

    }
    else if (order == 52) // HLeft 4
    {
      //Serial.print("Order : ");
      //Serial.println(order, DEC);
      //analogWrite(D_PIN6_M2_EN, 255);
      digitalWrite(D_PIN6_M2_EN, HIGH);
      digitalWrite(D_PIN8_M2, LOW);
      digitalWrite(D_PIN10_M2, HIGH);
    }
    else if (order == 53) // LRight 5
    {
      //Serial.print("Order : ");
      //Serial.println(order, DEC);
      //analogWrite(D_PIN6_M2_EN, 130);
      digitalWrite(D_PIN6_M2_EN, HIGH);
      digitalWrite(D_PIN8_M2, HIGH);
      digitalWrite(D_PIN10_M2, LOW);
    }
    else if (order == 54) // HRight 6
    { 
      //Serial.print("Order : ");
      //Serial.println(order, DEC);
      //analogWrite(D_PIN6_M2_EN, 255);
      digitalWrite(D_PIN6_M2_EN, HIGH);
      digitalWrite(D_PIN8_M2, HIGH);
      digitalWrite(D_PIN10_M2, LOW);
    }
    else if (order == 55) // LRStop 7
    {
      //Serial.print("Order : ");
      //Serial.println(order, DEC);
      digitalWrite(D_PIN6_M2_EN, LOW);
      digitalWrite(D_PIN8_M2, LOW);
      digitalWrite(D_PIN10_M2, LOW);
    }
    else {}

  }


}
