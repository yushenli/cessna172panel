#define IS_DEBUG                 0

#define UPDATE_STATE_LENGTH_US             2000000
#define CHECK_INTERVAL_US_UPDATE_STATE         100
#define CHECK_INTERVAL_US_NON_UPDATE_STATE    1000  // 5 miliseconds

// Only if the input from CLK pin remains the same value STABLE_LENGTH times in a row,
// before it is considered the new CLK value.
#define STABLE_LENGTH            10

#define ALTIMETER                0
#define PIN_ALTIMETER_CLK        23
#define PIN_ALTIMETER_DT         22

class Knob {
    public:
        Knob(const char* knobName, int pinClk, int pinDt, int pulseDegree);
        bool Update();
    private:
        int updateAndGetLastClk(int newClk);
        int updateAndGetLastDt(int newDt);
        const char* knobName;
        int pinClk;
        int pinDt;
        int pulseDegree;  // Number of degrees turned for one pulse
        int lastClkIndex = 0;  // The index where lastClk was last updated in the previous loop.
        int lastClk[STABLE_LENGTH];
        int lastStableClk;
        int lastDtIndex = 0;  // The index where lastClk was last updated in the previous loop.
        int lastDt[STABLE_LENGTH];
        int degree;
};

Knob::Knob(const char* knobName, int pinClk, int pinDt, int pulseDegree) {
    this->knobName = knobName;
    this->pinClk = pinClk;
    this->pinDt = pinDt;
    this->pulseDegree = pulseDegree;

    pinMode(this->pinClk, INPUT);
    pinMode(this->pinDt, INPUT);
    lastStableClk = digitalRead(this->pinClk);
    for (int i = 0; i < STABLE_LENGTH; i++) {
        lastClk[i] = lastStableClk;
    }
    this->degree = 0;

    Serial.print("Initialized knob ");
    Serial.print(this->knobName);
    Serial.print(" CLK=");
    Serial.print(this->pinClk);
    Serial.print(" DT=");
    Serial.println(this->pinDt);
}

bool Knob::Update() {
    int valClk = updateAndGetLastClk(digitalRead(pinClk));
    int valDt = updateAndGetLastDt(digitalRead(pinDt));
    if (valClk == -1 || valDt == -1) {
        return false;
    }

    if (valClk != lastStableClk) {
        if (IS_DEBUG) {
            Serial.print("reading pin ");
            Serial.print(pinClk);
            Serial.print(" new value ");
            Serial.println(valClk);
        }

        Serial.print("Knob ");
        Serial.print(knobName);
        if (valClk != valDt) {
            // clockwise
            Serial.print(" turned clockwise to ");
            degree = (degree + pulseDegree) % 360;
        }
        else {
            // counter-clockwise
            Serial.print(" turned counter-clockwise to ");
            degree = (degree - pulseDegree + 360) % 360;
        }
        Serial.println(degree);

        lastStableClk = valClk;
        return true;
    }
    return false;
}

// Update the lastClk array using the newClk value. If all the recent STABLE_LENGTH values are the same,
// return that value, otherwise return -1.
int Knob::updateAndGetLastClk(int newClk) {
    lastClkIndex = (lastClkIndex + 1) % STABLE_LENGTH;
    lastClk[lastClkIndex] = newClk;
    for (int i = (lastClkIndex + STABLE_LENGTH - 1) % STABLE_LENGTH;
            i != lastClkIndex;
            i = (i + STABLE_LENGTH - 1) % STABLE_LENGTH) {
        if (lastClk[i] != newClk) {
            return -1;
        }
    }
    return newClk;
}

// Update the lastDt array using the newDt value. If all the recent STABLE_LENGTH values are the same,
// return that value, otherwise return -1.
int Knob::updateAndGetLastDt(int newDt) {
    lastDtIndex = (lastDtIndex + 1) % STABLE_LENGTH;
    lastDt[lastDtIndex] = newDt;
    for (int i = (lastDtIndex + STABLE_LENGTH - 1) % STABLE_LENGTH;
            i != lastDtIndex;
            i = (i + STABLE_LENGTH - 1) % STABLE_LENGTH) {
        if (lastDt[i] != newDt) {
            return -1;
        }
    }
    return newDt;
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
