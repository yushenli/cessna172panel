#define IS_DEBUG                 0

#define UPDATE_STATE_LENGTH_US             2000000
#define CHECK_INTERVAL_US_UPDATE_STATE         100
#define CHECK_INTERVAL_US_NON_UPDATE_STATE    5000  // 5 miliseconds
#define MIN_LOOPS_BETWEEN_UPDATES                5

#define ALTIMETER                0
#define PIN_ALTIMETER_CLK        23
#define PIN_ALTIMETER_DT         22

class Knob {
    public:
        Knob(const char* knobName, int pinClk, int pinDt, int pulseDegree);
        bool Update();
    private:
        const char* knobName;
        int pinClk;
        int pinDt;
        int pulseDegree;  // Number of degrees turned for one pulse
        int lastClk;
        int degree;
        unsigned int loopsSinceLastUpdate = 0;
};

Knob::Knob(const char* knobName, int pinClk, int pinDt, int pulseDegree) {
    this->knobName = knobName;
    this->pinClk = pinClk;
    this->pinDt = pinDt;
    this->pulseDegree = pulseDegree;

    pinMode(this->pinClk, INPUT_PULLUP);
    pinMode(this->pinDt, INPUT_PULLUP);
    this->lastClk = digitalRead(this->pinClk);
    this->degree = 0;

    Serial.print("Initialized knob ");
    Serial.print(this->knobName);
    Serial.print(" CLK=");
    Serial.print(this->pinClk);
    Serial.print(" DT=");
    Serial.println(this->pinDt);
}

bool Knob::Update() {
    loopsSinceLastUpdate++;
    int valClk = digitalRead(pinClk);
    int valDt = digitalRead(pinDt);
    if (valClk != lastClk) {
        lastClk = valClk;
        if (loopsSinceLastUpdate < MIN_LOOPS_BETWEEN_UPDATES) {
            // With CHECK_INTERVAL_US_NON_UPDATE_STATE = 5000 and CHECK_INTERVAL_US_UPDATE_STATE = 100,
            // we check the encoder pins between 200 and 10k Hz. If two updates were observed too close
            // to each other, there has likely been a bounce, or a loop may be delayed by serial
            // operation or interrupts. In both cases, we want to ignore the update.
            return false;
        }

        if (IS_DEBUG) {
            Serial.print("reading pin ");
            Serial.print(pinClk);
            Serial.print(" last ");
            Serial.print(lastClk);
            Serial.print(" now ");
            Serial.println(valClk);
        }

        if (IS_DEBUG) {
            Serial.print("Knob ");
            Serial.print(knobName);
        }
        if (valClk != valDt) {
            // clockwise
            if (IS_DEBUG) {
                Serial.print(" turned clockwise to ");
            }
            degree = (degree + pulseDegree) % 360;
        }
        else {
            // counter-clockwise
            if (IS_DEBUG) {
                Serial.print(" turned counter-clockwise to ");
            }
            degree = (degree - pulseDegree + 360) % 360;
        }
        if (IS_DEBUG) {
            Serial.println(degree);
        }
        loopsSinceLastUpdate = 0;
        return true;
    }
    return false;
}

Knob* knobs[1];
int loopIntervalUs;
int updateStateEndsInChecks;

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(9600);

    loopIntervalUs = 5000;
    updateStateEndsInChecks = 0;
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    knobs[ALTIMETER] = new Knob("ALTIMETER", PIN_ALTIMETER_CLK, PIN_ALTIMETER_DT, 12);
}

// the loop function runs over and over again forever
void loop() {
    bool updated = false;
    for (int i = 0; i < sizeof(knobs) / sizeof(Knob*); i++) {
        updated |= knobs[i]->Update();
    }

    if (updated) {
        if (updateStateEndsInChecks <= 0) {
            digitalWrite(LED_BUILTIN, HIGH);
            loopIntervalUs = CHECK_INTERVAL_US_UPDATE_STATE;
        }
        updateStateEndsInChecks = UPDATE_STATE_LENGTH_US / CHECK_INTERVAL_US_UPDATE_STATE;
    }
    else {
        updateStateEndsInChecks--;
        if (updateStateEndsInChecks <= 0) {
            digitalWrite(LED_BUILTIN, LOW);
            loopIntervalUs = CHECK_INTERVAL_US_NON_UPDATE_STATE;
        }
    }

    delayMicroseconds(loopIntervalUs);
}
