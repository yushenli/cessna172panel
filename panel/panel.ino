#define IS_DEBUG                 0

#define UPDATE_STATE_LENGTH_US             2000000
#define CHECK_INTERVAL_US_UPDATE_STATE         100
#define CHECK_INTERVAL_US_NON_UPDATE_STATE    5000  // 5 miliseconds

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
};

Knob::Knob(const char* knobName, int pinClk, int pinDt, int pulseDegree) {
    this->knobName = knobName;
    this->pinClk = pinClk;
    this->pinDt = pinDt;
    this->pulseDegree = pulseDegree;

    pinMode(this->pinClk, INPUT);
    pinMode(this->pinDt, INPUT);
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
    int valClk = digitalRead(pinClk);
    int valDt = digitalRead(pinDt);
    if (valClk != lastClk) {
        if (IS_DEBUG) {
            Serial.print("reading pin ");
            Serial.print(pinClk);
            Serial.print(" last ");
            Serial.print(lastClk);
            Serial.print(" now ");
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
        lastClk = valClk;
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
