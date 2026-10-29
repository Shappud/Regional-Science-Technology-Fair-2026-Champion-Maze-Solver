// ====================== FULL ARDUINO SKETCH ======================
// Goes straight unless a wall is detected.
// When a wall is detected -> performs turning sequence in order.
// Sequence repeats forever.

// Motor pins
#define PWM1_PIN            5
#define PWM2_PIN            6      

// 74HCT595N Chip pins
#define SHCP_PIN            2
#define EN_PIN              7
#define DATA_PIN            8
#define STCP_PIN            4

// Ultrasonic pins
#define TRIG_PIN            12
#define ECHO_PIN            13

// Motor movement codes
const int Forward       = 163;
const int Clockwise     = 172;
const int Contrarotate  = 83;
const int Stop          = 0;

// Speed & distances
const int Speed = 255;
const int WallDistance = 7;    // cm - wall threshold

// LEFT = -1 
// RIGHT = 1

//TURNDURATION = MEASURED IN MILLISECONDS

// Turn sequence:
// left, right, right, left, left, left, left, right
int turnSequence[]     = { -1,  1,  1, -1, -1, -1, -1,  1 };
int turnDurationsMs[]  = { 550, 540, 600, 520, 590, 570, 540, 550 };
int sequenceStep = 0;
int sequenceLen = sizeof(turnSequence) / sizeof(turnSequence[0]);

// -----------------------------------------------------------------
void setup() {
    Serial.begin(9600);

    pinMode(SHCP_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(STCP_PIN, OUTPUT);
    pinMode(PWM1_PIN, OUTPUT);
    pinMode(PWM2_PIN, OUTPUT);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    Motor(Stop, 0);
    Serial.println("Robot Ready.");
}

void loop() {

    Motor(Forward, Speed);

    float dist = checkDistance();
    if (dist > WallDistance) {
        // Path clear → keep going straight
        delay(20);
        return;
    }

    // ==============================================================
    // ------------ TURN SEQUENCE (happens when wall hit) -----------
    // ==============================================================

    // Restart sequence when finished
    if (sequenceStep >= sequenceLen) {
        sequenceStep = 0;
    }

    

    int turn = turnSequence[sequenceStep];
    int dur  = turnDurationsMs[sequenceStep];
    sequenceStep++;

    if (turn == -1) {
        Serial.print("Turning LEFT for "); Serial.print(dur); Serial.println("ms");
        Motor(Contrarotate, Speed);
        delay(dur);
    } else {
        Serial.print("Turning RIGHT for "); Serial.print(dur); Serial.println("ms");
        Motor(Clockwise, Speed);
        delay(dur);
    }




}

// ---------------------------------------------------------
float checkDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    if (duration == 0) return 999.0;

    return duration / 58.0;
}

// ---------------------------------------------------------
void Motor(int Dir, int spd) {
    digitalWrite(EN_PIN, LOW);
    analogWrite(PWM1_PIN, spd);
    analogWrite(PWM2_PIN, spd);

    digitalWrite(STCP_PIN, LOW);
    shiftOut(DATA_PIN, SHCP_PIN, MSBFIRST, Dir);
    digitalWrite(STCP_PIN, HIGH);
}
