#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pinos dos LEDs dos Carros (3 LEDs)
const int CAR_RED    = 12;
const int CAR_YELLOW = 11;
const int CAR_GREEN  = 10;

// Pinos dos LEDs dos Pedestres (2 LEDs)
const int PED_RED   = 7;
const int PED_GREEN = 6;

// Botão Único e Buzzer
const int BUTTON_PIN = 2;
const int BUZZER_PIN = 4;

// TEMPOS CONFIGURADOS (TODOS EM 30 SEGUNDOS)
const unsigned long TEMPO_PREPARO          = 30000; // 30s de retenção/segurança pós-clique
const unsigned long TEMPO_AMARELO_CARROS   = 30000; // 30s de sinal amarelo
const unsigned long TEMPO_TRAVESSIA_VERDE  = 30000; // 30s de travessia verde
const unsigned long TEMPO_TRAVESSIA_ALERTA = 10000; // 10s de pisca alerta (Total travessia = 40s)
const unsigned long TEMPO_COOLDOWN_CARROS  = 30000; // 30s mínimos de trânsito livre para os carros

bool pedidoTravessia = false;
unsigned long tempoInicioEstado = 0;
unsigned long tempoUltimaTravessia = 0;

enum EstadoSemaforo {
  CARROS_VERDE,
  AGUARDANDO_RETENCAO,
  CARROS_AMARELO,
  PEDESTRES_PASSA,
  PEDESTRES_ALERTA
};

EstadoSemaforo estadoAtual = CARROS_VERDE;

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(CAR_RED, OUTPUT);
  pinMode(CAR_YELLOW, OUTPUT);
  pinMode(CAR_GREEN, OUTPUT);
  
  pinMode(PED_RED, OUTPUT);
  pinMode(PED_GREEN, OUTPUT);
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Estado Inicial: Carros VERDE | Pedestres VERMELHO
  atualizarLuzes(false, false, true, true, false);
}

void loop() {
  unsigned long tempoAtual = millis();

  // Leitura do Botão: Aceita solicitação se estiver no Verde e após o cooldown de 30s dos carros
  if (digitalRead(BUTTON_PIN) == LOW && estadoAtual == CARROS_VERDE && !pedidoTravessia) {
    if (tempoAtual - tempoUltimaTravessia >= TEMPO_COOLDOWN_CARROS || tempoUltimaTravessia == 0) {
      pedidoTravessia = true;
      estadoAtual = AGUARDANDO_RETENCAO;
      tempoInicioEstado = tempoAtual;
      
      tone(BUZZER_PIN, 1000, 100);
    }
  }

  // Máquina de Estados Principal
  switch (estadoAtual) {

    case CARROS_VERDE:
      lcd.setCursor(0, 0);
      lcd.print("CARROS: LIBERADO");
      lcd.setCursor(0, 1);
      lcd.print("PEDESTRE: APERTE");
      break;

    case AGUARDANDO_RETENCAO:
      {
        unsigned long decorrido = tempoAtual - tempoInicioEstado;
        int rest = (TEMPO_PREPARO - decorrido) / 1000;

        lcd.setCursor(0, 0);
        lcd.print("SOLICITACAO OK! ");
        lcd.setCursor(0, 1);
        lcd.print("PREPARANDO: ");
        if (rest < 9) lcd.print("0");
        lcd.print(rest + 1);
        lcd.print("s ");

        if (decorrido >= TEMPO_PREPARO) {
          estadoAtual = CARROS_AMARELO;
          tempoInicioEstado = tempoAtual;
          atualizarLuzes(false, true, false, true, false);
        }
      }
      break;

    case CARROS_AMARELO:
      {
        unsigned long decorrido = tempoAtual - tempoInicioEstado;
        int rest = (TEMPO_AMARELO_CARROS - decorrido) / 1000;

        lcd.setCursor(0, 0);
        lcd.print("CARROS: AMARELO ");
        lcd.setCursor(0, 1);
        lcd.print("PARAR EM: ");
        if (rest < 9) lcd.print("0");
        lcd.print(rest + 1);
        lcd.print("s   ");

        if (decorrido >= TEMPO_AMARELO_CARROS) {
          estadoAtual = PEDESTRES_PASSA;
          tempoInicioEstado = tempoAtual;
          atualizarLuzes(true, false, false, false, true);
        }
      }
      break;

    case PEDESTRES_PASSA:
      {
        unsigned long decorrido = tempoAtual - tempoInicioEstado;
        int rest = ((TEMPO_TRAVESSIA_VERDE + TEMPO_TRAVESSIA_ALERTA) - decorrido) / 1000;

        lcd.setCursor(0, 0);
        lcd.print("PEDESTRE: PASSE!");
        lcd.setCursor(0, 1);
        lcd.print("FECHA EM: ");
        if (rest < 9) lcd.print("0");
        lcd.print(rest + 1);
        lcd.print("s   ");

        if ((tempoAtual / 600) % 2 == 0) {
          tone(BUZZER_PIN, 700, 100);
        }

        if (decorrido >= TEMPO_TRAVESSIA_VERDE) {
          estadoAtual = PEDESTRES_ALERTA;
          tempoInicioEstado = tempoAtual;
        }
      }
      break;

    case PEDESTRES_ALERTA:
      {
        unsigned long decorrido = tempoAtual - tempoInicioEstado;
        int rest = (TEMPO_TRAVESSIA_ALERTA - decorrido) / 1000;

        lcd.setCursor(0, 0);
        lcd.print("ATENCAO: FECHANDO");
        lcd.setCursor(0, 1);
        lcd.print("TEMPO: ");
        if (rest < 9) lcd.print("0");
        lcd.print(rest + 1);
        lcd.print("s   ");

        if ((tempoAtual / 200) % 2 == 0) {
          digitalWrite(PED_GREEN, HIGH);
          tone(BUZZER_PIN, 1200, 80);
        } else {
          digitalWrite(PED_GREEN, LOW);
        }

        if (decorrido >= TEMPO_TRAVESSIA_ALERTA) {
          pedidoTravessia = false;
          tempoUltimaTravessia = millis();
          estadoAtual = CARROS_VERDE;
          atualizarLuzes(false, false, true, true, false);
        }
      }
      break;
  }
}

void atualizarLuzes(bool cRed, bool cYel, bool cGrn, bool pRed, bool pGrn) {
  digitalWrite(CAR_RED, cRed);
  digitalWrite(CAR_YELLOW, cYel);
  digitalWrite(CAR_GREEN, cGrn);
  
  digitalWrite(PED_RED, pRed);
  digitalWrite(PED_GREEN, pGrn);
}