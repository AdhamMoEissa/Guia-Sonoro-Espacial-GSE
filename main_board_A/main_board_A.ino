
#define trigPin 2
#define echoPin 3
#define trigPin2 5
#define echoPin2 6

unsigned long previousTime = 0;

long measureDistance() {
    long duration, distance;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration / 58;

    return distance;
}

long measureDistance2() {
    long duration2, distance2;

    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH);
    distance2 = duration2 / 58;

    return distance2;
}

void setup() {
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
}

void loop() {
    unsigned long currentTime = millis();

    if (currentTime - previousTime >= 600) {
        previousTime = currentTime;

        long distance = measureDistance();
        long distance2 = measureDistance2();

        Serial.print("Distancia: ");
        Serial.print(distance);
        Serial.println(" cm");
        Serial.print("Distancia2: ");
        Serial.print(distance2);
        Serial.println(" cm");
    }
}
