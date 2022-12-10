//////////////////////////////////////////////////////
//  ELE3000 - Projet personnel en génie électrique  //
//  - Automne 2022 - * Plantation *                 //
//                                                  //
//  Auteur :                                        //
//  Boizi Jean-Marc Kadjo       Matricule:          //
//                                                  //
//  Date: 2022-11-23                                //
//////////////////////////////////////////////////////

#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>          // Librairie du bluetooth

const uint16_t PIN_BT_RX = 3;
const uint16_t PIN_BT_TX = 2;

const uint16_t PIN_PUMP = 9;
const uint16_t PIN_PUMP_EN = 10;

const uint16_t PIN_FAN = 6;
const uint16_t PIN_FAN_EN = 8;

const uint16_t PIN_LED = 5;
const uint16_t PIN_LED_EN = 7;

const uint16_t PIN_BUZZER = 4;
const uint16_t PIN_SOL = A0;

const uint16_t PWM_MAX = 255;
const uint16_t PWM_MOITIE = 127;

const uint16_t BAUD_RATE = 9600;

const String TEXTE_MODE_X[] = { "Pump:", "Fan:", "Stop" };
const String TEXTE_MODE_Y[] = { "LED:", "Buzzer", "Stop" };
const String TEXTE_MODE_HOLD[] = { "Hold", "Change" };

const uint16_t TEMPS_DELAI = 100;  // ms

enum
{
    PUMP, FAN, STOP
};

enum
{
    LED, BUZZER
};

struct Joystick
{
    uint16_t xValeur = PWM_MOITIE;
    uint16_t yValeur = PWM_MOITIE;
    uint16_t xMode = STOP;
    uint16_t yMode = STOP;
};

SoftwareSerial BTSerial(PIN_BT_RX, PIN_BT_TX);     // RX, TX  // pins utilisés sur la boite arduino
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x3F for a 16 chars and 2 line display

void setup()
{
    lcd.init();
    lcd.clear();
    lcd.backlight();  // Make sure backlight is on

    BTSerial.begin(BAUD_RATE); // Controleur
    Serial.begin(BAUD_RATE);   // Moniteur PC

    pinMode(PIN_PUMP_EN, OUTPUT); // mode output
    pinMode(PIN_LED_EN, OUTPUT); // mode output
    pinMode(PIN_FAN_EN, OUTPUT); //  mode output

    digitalWrite(PIN_PUMP_EN, HIGH); // pin a ON
    digitalWrite(PIN_LED_EN, HIGH); // pin a ON
    digitalWrite(PIN_FAN_EN, HIGH); // pin a ON

    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, HIGH);  // HIGH for silence
}

void loop()
{
    struct Joystick actuel;
    uint8_t flagPresse;

    String buf;

    if (BTSerial.available())
    {
        // Lire valeurs du controleur
        buf = BTSerial.readStringUntil('\n');  // readString lisait seulement tous les données par '\n'
        buf.trim();                            // efface les données en arrière 

        actuel.xMode = buf.substring(0, 1).toInt();    // change valeur en string en int
        actuel.xValeur = buf.substring(1, 4).toInt();   // change valeur en string
        actuel.yMode = buf.substring(4, 5).toInt();    // change valeur en string
        actuel.yValeur = buf.substring(5, 8).toInt();  // change valeur en string

        flagPresse = buf.substring(8, 9).toInt();  // change valeur en string

        if (actuel.xMode == PUMP)  // Left - Pump
        {
            analogWrite(PIN_PUMP, actuel.xValeur);
            analogWrite(PIN_FAN, 0);
        }
        else if (actuel.xMode == FAN)  // Right - Fan
        {
            analogWrite(PIN_PUMP, 0);
            analogWrite(PIN_FAN, actuel.xValeur);
        }
        else  // actuel.xMode ==  STOP // Center - Stop
        {
            analogWrite(PIN_PUMP, 0);
            analogWrite(PIN_FAN, 0);
        }

        if (actuel.yMode == LED)  // Up - LED
        {
            analogWrite(PIN_LED, actuel.yValeur);
            digitalWrite(PIN_BUZZER, HIGH); // HIGH pour silence
        }
        else if (actuel.yMode == BUZZER)  // Down - Buzzer
        {
            analogWrite(PIN_LED, 0);
            digitalWrite(PIN_BUZZER, LOW); // LOW pour jouer
        }
        else  // actuel.yMode == STOP  // Center - Arret
        {
            analogWrite(PIN_LED, 0);
            digitalWrite(PIN_BUZZER, HIGH); // HIGH pour silence
        }
    
    
    uint16_t humidite = (PWM_MAX - (analogRead(PIN_SOL) >> 2)) * 100 / PWM_MAX;

    char buf2[5];
    sprintf(buf2, "%02u", humidite);
    BTSerial.println(String(buf2));
    
    // Ecrire valeurs au moniteur PC
  
    char buf3[15];
    sprintf(buf3, "%01u%03u%01u%03u%01u%02u", actuel.xMode, actuel.xValeur,
          actuel.yMode, actuel.yValeur, flagPresse, humidite);
    
    Serial.println(String(buf3));

    /* Pour debogage
    lcd.setCursor(0, 0);
    lcd.print(TEXTE_MODE_X[actuel.xMode]);
    if (actuel.xMode != STOP)
    {
        lcd.print(actuel.xValeur);
    }
    lcd.print(" ");
    lcd.print(TEXTE_MODE_HOLD[flagPresse]);
    lcd.print("       ");

    lcd.setCursor(0, 1);
    lcd.print(TEXTE_MODE_Y[actuel.yMode]);
    if ((actuel.yMode != STOP) && (actuel.yMode != BUZZER))
    {
        lcd.print(actuel.yValeur);
    }
    lcd.print("       ");

    lcd.setCursor(11, 1);
    lcd.print("M:");
    lcd.print(buf2);
    lcd.print("%");
    */
    }
    //delay(TEMPS_DELAI);
}
